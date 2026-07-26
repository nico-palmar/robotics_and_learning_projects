from abc import ABC, abstractmethod

import torch
import torch.nn as nn
from torch.distributions import (
    Categorical,
    Independent,
    Normal,
    TransformedDistribution,
)
from torch.distributions.transforms import AffineTransform

from torch.distributions.transforms import TanhTransform
# try:
#     from torch.distributions.transforms import TanhTransform
# except ImportError:
#     import math
#     import torch.nn.functional as F
#     from torch.distributions import constraints
#     from torch.distributions.transforms import Transform
#     # implemented by chat to account for my old openAI spinup env...
#     class TanhTransform(Transform):
#         domain = constraints.real
#         codomain = constraints.interval(-1.0, 1.0)
#         bijective = True
#         sign = +1

#         def __init__(self, cache_size=1):
#             super().__init__(cache_size=cache_size)

#         def __eq__(self, other):
#             return isinstance(other, TanhTransform)

#         def _call(self, x):
#             return x.tanh()

#         def _inverse(self, y):
#             eps = torch.finfo(y.dtype).eps
#             y = y.clamp(min=-1.0 + eps, max=1.0 - eps)
#             return 0.5 * (torch.log1p(y) - torch.log1p(-y))

#         def log_abs_det_jacobian(self, x, y):
#             return 2.0 * (math.log(2.0) - x - F.softplus(-2.0 * x))


def mlp(sizes, activation=nn.ReLU, output_activation=nn.Identity):
    # Build a feedforward neural network.
    layers = []
    for j in range(len(sizes)-1):
        act = activation if j < len(sizes)-2 else output_activation
        layers += [nn.Linear(sizes[j], sizes[j+1]), act()]
    return nn.Sequential(*layers)


class RLPolicyNet(nn.Module, ABC):
    """Interface for policy networks used throughout the RL codebase."""

    @abstractmethod
    def sample_action(self, obs: torch.Tensor):
        """Sample an action for a single observation."""

    @abstractmethod
    def get_probs(self, obs: torch.Tensor):
        """Return a Torch distribution representing the policy."""


class CategoricalNet(RLPolicyNet):
    def __init__(self, obs_dim, act_dim, hidden_sizes,
                 activation=nn.ReLU, output_activation=nn.Identity):
        super().__init__()
        sizes = [obs_dim] + hidden_sizes + [act_dim]
        self.logits_net = mlp(sizes, activation=activation,
                              output_activation=output_activation)

    def forward(self, obs):
        return self.logits_net(obs)

    def get_probs(self, obs):
        logits = self.forward(obs)
        return Categorical(logits=logits)

    def sample_action(self, obs):
        action = self.get_probs(obs).sample()
        return action.item() if action.ndim == 0 else action


class DiagonalGaussianNet(RLPolicyNet):
    def __init__(self, obs_dim, act_dim, act_low, act_high,
                 hidden_sizes, activation=nn.ReLU,
                 output_activation=nn.Identity, log_std_init=-0.5):
        """
        Squashed diagonal Gaussian policy with a state-independent log_std.
        Maps observations -> actions ∈ [act_low, act_high].
        """
        super().__init__()
        network_sizes = [obs_dim] + hidden_sizes + [act_dim]
        self.mean_net = mlp(network_sizes, activation=activation,
                            output_activation=output_activation)
        self.log_std = nn.Parameter(torch.ones(act_dim) * log_std_init)
        # register these params as buffers to signal that they are not part of the learnable params
        self.register_buffer("act_low", torch.as_tensor(act_low, dtype=torch.float32))
        self.register_buffer("act_high", torch.as_tensor(act_high, dtype=torch.float32))
        self.register_buffer("act_mid", (self.act_high + self.act_low) / 2.0)
        self.register_buffer("act_scale", (self.act_high - self.act_low) / 2.0)

    def forward(self, obs):
        return self.mean_net(obs)

    def _base_distribution(self, obs):
        mu = self.forward(obs)
        std = torch.exp(self.log_std)
        # use Independent signals that Normal is multivariate; on calling log_prob it
        #  sums over all dims and returns a scalar automatically
        base = Independent(Normal(mu, std), 1)
        return base

    def get_probs(self, obs):
        base = self._base_distribution(obs)
        # apply tanh 'squashing' between [-1, 1]
        # then apply scaling to the low and high of the action space
        transforms = [
            TanhTransform(cache_size=1),
            AffineTransform(loc=self.act_mid, scale=self.act_scale),
        ]
        # the trasnformed distribution object applies this pipeline of transformations
        # when calling log_prob on this, it automatically includes the Jacobian term 
        # required to account for the transformations on the network outputs (handles it all under the hood)
        return TransformedDistribution(base, transforms)

    def sample_action(self, obs):
        action = self.get_probs(obs).rsample()
        return action.detach().cpu().numpy()
