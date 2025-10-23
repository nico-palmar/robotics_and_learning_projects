import numpy as np
import cvxpy as cp
from scipy.signal import cont2discrete
import matplotlib.pyplot as plt

# Write down the equations for a double integrator
# we have x1_dot = x2, x2_dot = u. Then x1 = position, x2 = velocity, u = acceleration.
A = np.array(
    [
        [0, 1],
        [0, 0]
    ]
)

B = np.array(
    [
        [0],
        [1]
    ]
)

# let's assume the output is the position; x1
C = np.array(
    [1, 0]
)

D = np.array(
    [0]
)

# Convert the state space double integrator into discrete version -> use ZOH method
dt = 0.1
discrete_sys = cont2discrete((A, B, C, D), dt, method='zoh')
A_d, B_d, C_d, D_d, _ = discrete_sys

# Setup MPC params
N = 10
# set the velocity as same weight to position
# behaves nicest this way; otherwise it won't converge in time or position seems to be overshooting at the end
Q = np.diag([1, 1]) # start with identity knobs, then tweak them
# making R small => smaller cost on control signal => vary quickly
# making R large => larger cost => control signal only changes slowly and doesn't even converge in time
R = np.array([[1e-1]])
x0 = np.array([-10, 0]) # starting state at position 2 with 0 velocity
# set limits on accel input
u_max = 1
u_min = -1

def solve_MPC(x0):
    # define the shapes of variables to optimize over in the time horizon
    # note that we optimize up to N variables, but also predict the N+1 var for x
    # due to the discrete state dynamics
    x = cp.Variable((2, N+1))
    # TODO: see what happens if you remove the 1 below, just make it N long
    u = cp.Variable((1, N))

    # setup all the constraints
    # 1. Know the starting state of x; x_0
    # 2. Know the dynamics equations as the step updates
    # 3. Constraints on the control var
    constraints = [x[:, 0] == x0]
    cost = 0 # J term in MPC
    for k in range(N):
        # dynamics eq'n: x_k+1 = A * x_k + B * u_k
        constraints += [x[:, k+1] == A_d @ x[:, k] + B_d @ u[:, k]]
        # input constraints
        constraints += [u_min <= u[:, k], u_max >= u[:, k]]
        
        # now apply MPC for each step; integral for a single horizon
        cost += cp.quad_form(x[:, k], Q) + cp.quad_form(u[:, k], R)

    # assume we are ignoring the cost to go term
    problem = cp.Problem(cp.Minimize(cost), constraints)
    problem.solve()

    # get optimal control sequence for horizon and state values
    return (x.value, u.value)

# now to solve the full MPC problem, simulate solving MPC for multiple time steps at different parts in the horizon
sim_steps = 100
x_history = [x0]
u_history = []

x_prev = x0

for i in range(sim_steps):
    x_opt, u_opt = solve_MPC(x_prev)

    # take the first control input
    u_apply = u_opt[:, 0]

    # simulate running that input
    x_next = A_d @ x_prev + B_d @ u_apply

    x_history.append(x_next)
    u_history.append(u_apply)
    x_prev = x_next

# Q: How does MPC work?
# Start with some state, use Q, R and knobs to tweak the cost that is used in the optimization
# have some starting state
# In this case its...
# start with some input, run MPC to optmize over some finite horizon
# then we should take the first control input and use that for our plant
# we can then read back the state of our plant (or use some state esimator). Incorporate that into MPC
# in tracking MPC, we would compute the error and use that as first part in the cost (rather than just x; e = x - x_ref)
# run another step of MPC with the updated state for some more time steps
# MPC will again give us optimal control inputs over a time horizon. Iterate and continue


# plot
x_history = np.array(x_history)
u_history = np.array(u_history)

plt.figure(figsize=(10,4))
plt.subplot(2,1,1)
plt.plot(x_history[:,0], label='Position')
plt.plot(x_history[:,1], label='Velocity')
plt.xlabel('Time step')
plt.ylabel('States')
plt.legend()
plt.grid(True)

plt.subplot(2,1,2)
plt.step(range(len(u_history)), u_history, where='post', label='Control')
plt.xlabel('Time step')
plt.ylabel('Control input')
plt.legend()
plt.grid(True)

plt.tight_layout()
plt.show()

# understand what control law is being implemented; describe it