import torch
import torch.nn as nn
from torch.distributions import Normal

def mlp(sizes, activation=nn.Tanh, output_activation=nn.Identity):
    # Build a feedforward neural network.
    layers = []
    for j in range(len(sizes)-1):
        act = activation if j < len(sizes)-2 else output_activation
        layers += [nn.Linear(sizes[j], sizes[j+1]), act()]
    return nn.Sequential(*layers)

class DiagonalGaussianPolicy(nn.Module):
    def __init__(self, obs_dim, act_dim, act_low, act_high,
                 hidden_sizes=(64, 64), log_std_init=-0.5):
        """
        Squashed Gaussian policy with a state-independent log_std.
        Maps observations -> actions ∈ [act_low, act_high].
        """
        super().__init__()
        self.act_low = torch.as_tensor(act_low, dtype=torch.float32)
        self.act_high = torch.as_tensor(act_high, dtype=torch.float32)

        # --- Mean network ---
        layers = []
        last_size = obs_dim
        for h in hidden_sizes:
            layers += [nn.Linear(last_size, h), nn.ReLU()]
            last_size = h
        layers += [nn.Linear(last_size, act_dim)]
        self.mean_net = nn.Sequential(*layers)
        # diagonal gaussian policy implemented in a state independant manner
        self.log_std = nn.Parameter(torch.ones(act_dim) * log_std_init)

    def forward(self, obs):
        mu = self.mean_net(obs)
        std = torch.exp(self.log_std)
        dist = Normal(mu, std)
        return dist

    def sample_action(self, obs):
        dist = self.forward(obs)
        raw_action = dist.rsample() # reparameterize in order to make differentiable wrt network params

        # need to clip the distribution to the limits of the space
        squashed = torch.tanh(raw_action)

        # Rescale to environment's [low, high]
        scaled_action = self.act_low + 0.5 * (squashed + 1.0) * (self.act_high - self.act_low)

        # Compute log-prob correction for tanh squashing
        # this is done by computing a jacobian of the tanh as a result of the changing in volume (squishing of the space)
        # note that nothing special is done for the scaled action, since this does not directly alter the paramters (only changes them by some scalar value)
        log_prob = dist.log_prob(raw_action).sum(axis=-1)
        log_prob -= torch.sum(torch.log(1 - torch.tanh(raw_action).pow(2) + 1e-6), dim=-1)

        return scaled_action, log_prob