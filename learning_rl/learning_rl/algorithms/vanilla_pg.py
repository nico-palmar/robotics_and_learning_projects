from learning_rl.helpers.networks import mlp
import gym
from gym.spaces import Discrete, Box
from torch.optim import Adam
from torch.distributions.categorical import Categorical
import torch
import numpy as np

def reset_episode(env, episode_rewards):
    # pass by reference; overwrite the list
    episode_rewards.clear()
    obs = env.reset()
    return obs

def reward_to_go(episode_rewards):
    raise NotImplementedError("Have not implemented rtg yet")

def estimate_advantage_function_values():
    raise NotImplementedError("Have not implemented advantage func estimation")

def train(lr=1e-1, n_epochs = 20, batch_size=5000, env_name='CartPole-v0'):
    # 1. Setup things global to entire training session
    # implement version of vanilla PG that works with discrete action spaces
    # TODO (npalmar): make version that works for both discrete and continuous action spaces
    env = gym.make(env_name)

    # TODO (npalmar): tune these networks
    policy_net = mlp(sizes=[env.observation_space.shape[0], 32, env.action_space.n])
    # estimates the return (value function) given some starting observation
    value_func_net = mlp(sizes=[env.observation_space.shape[0], 32, 1])

    for epoch in range(n_epochs):
        # 2. Setup things global to an epoch
        episode_rewards = []
        epoch_returns = []
        epoch_observations = []
        epoch_actions = []
        epoch_advantage_function_vals = []
        episode_lengths = []

        # start off with some intial state
        obs = reset_episode(env, episode_rewards)

        # run for the entire epoch, not just one episode
        while True:
            # sample for some trajectory
            epoch_observations.append(obs)

            # TODO: make the network take some action based on obs
            action = None
            obs, reward, done, _ = env.step(action)
            # action and reward associated with observation at current time step
            episode_rewards.append(reward)
            epoch_actions.append(action)

            if done:
                returns = reward_to_go(episode_rewards)
                epoch_returns += returns
                episode_lengths.append(len(returns))

                # TODO: find out how to compute advantage function estimate, A_hat
                # based on value function net. Use simple method first
                advantages = estimate_advantage_function_values()
                epoch_advantage_function_vals.append(advantages)

                if sum(episode_lengths) > batch_size:
                    # break out, we have all data from this batch
                    break

                # same epoch, just reset the episode
                obs = reset_episode(env, episode_rewards)

        # have all trajectories, RTG, and advantage function values for all timesteps
        
        # TODO: Compute loss (gradient of expected reward), take a step

        # TODO: Compute updated advantage function estimate via loss, take a step
    
    else:
        # close the environment on last for loop
        env.close()

if __name__ == "__main__":
    train()
    print("TRAINED VANILLA PG")