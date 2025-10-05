from learning_rl.helpers.networks import mlp
import gym
from gym.spaces import Discrete, Box
from torch.optim import Adam
from torch.distributions.categorical import Categorical
import torch
import numpy as np

get_probs = lambda net, obs : Categorical(logits=net(obs))

get_action = lambda net, obs: get_probs(net, obs).sample().item()

# watch the np.mean for categorical vs continous action spaces; depending on what log_probs returns
# NOTE: watch sign on loss
compute_policy_loss = lambda net, obs, actions, advantages: - (get_probs(net, obs).log_prob(actions) * advantages).mean()

compute_value_loss = lambda net, obs, returns: ((returns - net(obs)) ** 2).mean()

def reset_episode(env, episode_rewards):
    # pass by reference; overwrite the list
    episode_rewards.clear()
    obs = env.reset()
    return obs

def reward_to_go(episode_rewards):
    rtg = [0] * len(episode_rewards)
    for i in range(len(episode_rewards)-1, -1, -1):
        if (i == len(episode_rewards) - 1):
            rtg[i] = episode_rewards[i]
        else:
            rtg[i] = episode_rewards[i] + rtg[i+1]
    return rtg

def train(lr=1e-2, n_epochs = 20, batch_size=5000, env_name='CartPole-v1', render=True):
    # 1. Setup things global to entire training session
    # implement version of vanilla PG that works with discrete action spaces
    # TODO (npalmar): make version that works for both discrete and continuous action spaces
    env = gym.make(env_name)

    # TODO (npalmar): tune these networks
    policy_net = mlp(sizes=[env.observation_space.shape[0], 32, env.action_space.n])
    # estimates the return (value function) given some starting observation
    value_func_net = mlp(sizes=[env.observation_space.shape[0], 32, 1])

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

        curr_render = True if render else False

        # start off with some intial state
        obs = reset_episode(env, episode_rewards)

        # run for the entire epoch, not just one episode
        while True:
            if curr_render:
                env.render()

            # sample for some trajectory
            epoch_observations.append(obs)

            torch_obs = torch.tensor(obs, dtype=torch.float32)
            action = get_action(policy_net, torch_obs)
            obs, reward, done, _ = env.step(action)
            value_estimates = value_func_net(torch_obs)
            # action and reward associated with observation at current time step
            episode_rewards.append(reward)
            epoch_actions.append(action)
            epoch_value_estimates.append(value_estimates)

            if done:
                returns = reward_to_go(episode_rewards)
                epoch_returns += returns
                episode_lengths.append(len(returns))

                # TODO: look into general advantage functions
                # understand bias vs variance trade off
                # implement a few different advantage function methods
                returns_tensor = torch.tensor(returns, dtype=torch.float32)
                episode_value_estimates = torch.tensor(epoch_value_estimates[len(epoch_value_estimates) - len(returns):], dtype=torch.float32)
                advantages = returns_tensor - episode_value_estimates
                epoch_advantage_function_vals += advantages.tolist()

                if sum(episode_lengths) > batch_size:
                    # break out, we have all data from this batch
                    break

                # same epoch, just reset the episode
                obs = reset_episode(env, episode_rewards)
                curr_render = False

        # have all trajectories, RTG, and advantage function values for all timesteps
        epoch_observations = torch.tensor(epoch_observations, dtype=torch.float32)
        epoch_actions = torch.tensor(epoch_actions, dtype=torch.int32)
        epoch_advantage_function_vals = torch.tensor(epoch_advantage_function_vals, dtype=torch.float32)
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
        policy_optim.step()

        print(f"Epoch {epoch}: Loss = {policy_loss.item():.3f} | Mean Return = {epoch_returns.mean().item():.3f} | Std Return = {epoch_returns.std().item():.3f} | Avg Ep Length = {episode_lengths.median()}")

    else:
        # close the environment on last for loop
        env.close()

if __name__ == "__main__":
    train()
    print("TRAINED VANILLA PG")