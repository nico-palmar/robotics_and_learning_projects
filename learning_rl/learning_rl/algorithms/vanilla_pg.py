from learning_rl.helpers.networks import CategoricalNet, DiagonalGaussianNet, mlp
import gymnasium as gym
from gymnasium.spaces import Discrete, Box
from torch.optim import Adam
import torch
import numpy as np
import types

# watch the np.mean for categorical vs continous action spaces; depending on what log_probs returns
# NOTE: watch sign on loss
compute_policy_loss = lambda net, obs, actions, advantages: - (net.get_probs(obs).log_prob(actions) * advantages).mean()

compute_value_loss = lambda net, obs, returns: ((returns - net(obs)) ** 2).mean()

# paper said: γ ∈ [0.96, 0.99] and λ ∈ [0.92, 0.99].
# for cart pole
def compute_GAE(rewards, values, gamma=0.99, lam_bda=0.95):
    advantages = [0] * len(rewards)

    for t in reversed(range(len(rewards))):
        # base case, last reward
        if t == (len(rewards) - 1):
            advantages[t] = rewards[t] - values[t]
            continue
        
        # otherwise, compute delta for this time step
        delta = rewards[t] + gamma * values[t+1] - values[t]

        advantages[t] = delta + (gamma * lam_bda * advantages[t+1])
    
    return advantages


def reset_episode(env, episode_rewards):
    # pass by reference; overwrite the list
    episode_rewards.clear()
    obs, _ = env.reset()
    return obs

def reward_to_go(episode_rewards):
    rtg = [0] * len(episode_rewards)
    for i in range(len(episode_rewards)-1, -1, -1):
        if (i == len(episode_rewards) - 1):
            rtg[i] = episode_rewards[i]
        else:
            rtg[i] = episode_rewards[i] + rtg[i+1]
    return rtg

def train(lr=1e-3, n_epochs = 20, batch_size=10000, env_name='CartPole-v1', render=True):
    # 1. Setup things global to entire training session
    render_mode = "human" if render else None
    env = gym.make(env_name, render_mode=render_mode)
    base_env = env.unwrapped

    # if render:
    #     original_render = base_env.render

    #     def _safe_render(self, *args, **kwargs):
    #         if hasattr(self, "last_u"):
    #             self.last_u = float(np.asarray(self.last_u).squeeze())
    #         return original_render(*args, **kwargs)

    #     base_env.render = types.MethodType(_safe_render, base_env)

    obs_dim = env.observation_space.shape[0]
    if isinstance(env.action_space, Discrete):
        policy_net = CategoricalNet(obs_dim=obs_dim,
                                    act_dim=env.action_space.n,
                                    hidden_sizes=[64, 64])
        action_dtype = torch.int64
    elif isinstance(env.action_space, Box):
        act_dim = env.action_space.shape[0]
        policy_net = DiagonalGaussianNet(
            obs_dim=obs_dim,
            act_dim=act_dim,
            act_low=env.action_space.low,
            act_high=env.action_space.high,
            hidden_sizes=[64, 64],
        )
        action_dtype = torch.float32
    else:
        raise NotImplementedError(f"Unsupported action space: {env.action_space}")

    # estimates the return (value function) given some starting observation
    value_func_net = mlp(sizes=[obs_dim, 32, 32, 1])

    policy_optim = Adam(policy_net.parameters(), lr=lr)
    value_func_optim = Adam(value_func_net.parameters(), lr=lr)

    for epoch in range(n_epochs):
        # 2. Setup things global to an epoch
        episode_rewards = []
        epoch_returns = []
        epoch_observations = []
        epoch_actions = []
        epoch_advantage_function_vals = []
        epoch_value_estimates = []
        episode_lengths = []

        render_this_episode = render
        if render:
            base_env.render_mode = "human"

        # start off with some initial state
        obs = reset_episode(env, episode_rewards)

        # run for the entire epoch, not just one episode
        while True:
            # sample for some trajectory
            epoch_observations.append(obs)

            torch_obs = torch.tensor(obs, dtype=torch.float32)
            action = policy_net.sample_action(torch_obs)
            obs, reward, terminated, truncated, _ = env.step(action)
            done = terminated or truncated
            value_estimates = value_func_net(torch_obs)
            # action and reward associated with observation at current time step
            episode_rewards.append(reward)
            epoch_actions.append(action)
            epoch_value_estimates.append(value_estimates.item())
            # print(value_estimates.shape)

            if done:
                returns = reward_to_go(episode_rewards)
                epoch_returns += returns
                episode_lengths.append(len(returns))

                episode_value_estimates = epoch_value_estimates[len(epoch_value_estimates) - len(returns):]
                # TODO: investigate the effects of different param values below
                advantages = compute_GAE(episode_rewards, episode_value_estimates)
                # TODO: compare returns as returns = advantages + values, and see how the compare to the returns by RTG 
                epoch_advantage_function_vals += advantages

                if sum(episode_lengths) > batch_size:
                    # break out, we have all data from this batch
                    break

                # same epoch, just reset the episode
                obs = reset_episode(env, episode_rewards)
                if render_this_episode:
                    render_this_episode = False
                    base_env.render_mode = None

        # have all trajectories, RTG, and advantage function values for all timesteps
        epoch_observations = torch.tensor(epoch_observations, dtype=torch.float32)
        epoch_actions = torch.tensor(epoch_actions, dtype=action_dtype)
        epoch_advantage_function_vals = torch.tensor(epoch_advantage_function_vals, dtype=torch.float32)
        adv_std = epoch_advantage_function_vals.std()
        # substracting a constant baseline here (no issue)
        # even after using GAE, scale can vary across episodes in batches
        # we centre and scale to keep policy gradient well conditioned
        epoch_advantage_function_vals = (
            epoch_advantage_function_vals - epoch_advantage_function_vals.mean()
        ) / (adv_std + 1e-8)
        epoch_returns = torch.tensor(epoch_returns, dtype=torch.float32)
        epoch_value_estimates = torch.tensor(epoch_value_estimates, dtype=torch.float32)
        episode_lengths = torch.tensor(episode_lengths, dtype=torch.int32)
        
        # Compute loss (gradient of expected reward), take a step
        # print(epoch_observations.shape, epoch_actions.shape, epoch_advantage_function_vals.shape)
        policy_loss = compute_policy_loss(policy_net, epoch_observations, epoch_actions, epoch_advantage_function_vals)
        policy_optim.zero_grad()
        policy_loss.backward()
        policy_optim.step()

        # Compute updated advantage function estimate via loss, take a step
        value_loss = compute_value_loss(value_func_net, epoch_observations, epoch_returns)
        value_func_optim.zero_grad()
        # print(policy_loss.shape, policy_loss)
        value_loss.backward()
        value_func_optim.step()

        print(f"Epoch {epoch}: Loss = {policy_loss.item():.3f} | Mean Return = {epoch_returns.mean().item():.3f} | Std Return = {epoch_returns.std().item():.3f} | Avg Ep Length = {episode_lengths.median()} | Advantage Mean = {epoch_advantage_function_vals.mean()} | Advantage Std = {epoch_advantage_function_vals.std()}")

    else:
        # close the environment on last for loop
        env.close()

if __name__ == "__main__":
    train(env_name="Pendulum-v1", render=False)
    print("TRAINED VANILLA PG")
