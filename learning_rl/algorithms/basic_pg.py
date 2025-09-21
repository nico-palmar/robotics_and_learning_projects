from ..helpers.networks import mlp
import gym
from gym.spaces import Discrete, Box
from torch.optim import Adam
from torch.distributions.categorical import Categorical
import torch
import numpy as np


# 2 uses for the neural net
# 1: Get the action we will need for training
# 2: Get the loss
# Both of these need to make use of the policy (though one will sample, other won't)

# this is for prob(a | s)
def get_policy(net, obs):
    logits = net(obs)
    # the categorical allows us to get softmax outputs/log prob (log softmax) outputs
    return Categorical(logits=logits)

def get_action(net, obs):
    # sample the logits using the network (probabilistically) for a single item
    return get_policy(net, obs).sample().item()

def compute_loss(net, obs, acts, returns):
    # compute log probs, mult by episode returns (recall returns = sum(rewards) in finite horizon undiscounted returns)
    # note log prob (a | s) => net(s).log_prob(index by a) ... indexing the a'th action's log prob
    log_prob_occured = get_policy(net, obs).log_prob(acts) # get the log prob of actions that happened in this episode
    # the policy grad is just grad (mean (log probs * returns)); take -ve for gradient ascent
    print(log_prob_occured.shape, returns.shape)
    return - (log_prob_occured * returns).mean()


def perform_opt(net, obs, acts, returns, optim):
    torch_obs = torch.tensor(obs, dtype=torch.float32)
    torch_acts = torch.tensor(acts, dtype=torch.int32)
    torch_returns = torch.tensor(returns, dtype=torch.float32)

    loss = compute_loss(net, torch_obs, torch_acts, torch_returns)
    # reset the gradient
    optim.zero_grad()
    # take the gradient with a backwards pass (on the loss function)
    # this is doing grad(-J(pi)); gradient of objective function
    loss.backward()
    optim.step()
    return loss

def reset_episode(env, episode_rewards):
    # pass by reference; overwrite the list
    episode_rewards = []
    obs = env.reset()
    return obs

# start with the code to set up the neural net, optimizer, and training environment
def train(lr=1e-2, n_epochs = 10, batch_size=1000):
    env = gym.make("CartPole-v0")
    assert isinstance(env.observation_space, Box), \
        "This example only works for envs with continuous state spaces."
    assert isinstance(env.action_space, Discrete), \
        "This example only works for envs with discrete action spaces."
    
    # get in input (observation) and output (action) sizes
    obs_size = env.observation_space.shape[0]
    n_actions = env.action_space.n
    hidden_dim = 36 # choose arbitrarily

    # create policy network
    feedforward_net = mlp([obs_size, hidden_dim, n_actions])

    optim = Adam(feedforward_net.parameters(), lr=lr)

    for epoch in range(n_epochs):
        # setup structs for training in this epoch
        batch_obs = []
        batch_returns = []
        batch_actions = []

        # store an extra list for rewards
        # return in the end is the sum of the rewards (finite horizon)
        episode_rewards = []
        obs = reset_episode(env, episode_rewards)

        while True:
            # first copy in the observations
            # adds initial observation, ignore final one (good)
            batch_obs.append(obs.copy())

            # get corresponding action
            action = get_action(feedforward_net, torch.tensor(obs, dtype=torch.float32))

            # get state update from env (new obs, actions, rewards)
            obs, reward, done, _ = env.step(action)

            # note: we record batch actions as a_t; action at some time
            batch_actions.append(action)

            # note: unlike batch actions, we do not record the reward for the batch
            # the batch records the return R(tau) = sum(rewards) for a finite horizon
            # we keep track of the rewards so we can sum them later and include in batch
            episode_rewards.append(reward)

            # check if the current episode is done
            if done:
                # get the return in this case
                episode_return = sum(episode_rewards)
                episode_len = len(episode_rewards)
                batch_returns += [episode_return] * episode_len
                if len(batch_obs) > batch_size:
                    # done enough, no need to run more episodes
                    break

                # not done enough, need to reset things and continue on this batch
                obs = reset_episode(env, episode_rewards)

        # done one epoch
        loss = perform_opt(feedforward_net, batch_obs, batch_actions, batch_returns, optim)
        # print some epoch stats here
        print(f"Finish epoch {epoch}: Loss = {loss} | Returns = {np.mean(list(set(batch_returns)))}")


if __name__ == "__main__":
    train()
    print("WE STARTED DEEP RL, woohoo")