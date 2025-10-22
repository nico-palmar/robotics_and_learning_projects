## Review of State Space from ECE488: Multivariable Control Systems

#### What is the state space framework?
A collection of first order DEs that describe the dynamics of a system.
In general, x = [x1 ... xn], y = [y1 ... yp], u = [u1 ... um] and
x_dot = Ax + Bu, y = Cx + Du

For an LTI system, eig(A) = poles of system.
You can also use the rosenbrock (I think that's his name) matrix, and check for what values it falls below full rank for the zeros of a system.

#### Interconnecting SS Systems:
Can use the SS model, deriving the equations of a system, then put them together in matrix-variable form to get the final state space equation of the entire system (for systems of state-space equations).

#### Understanding CODS

Informal def'n of **observability**: Modes that show up in output, y(t) are said to be observable. All modes show up => SS is observable; else it's unobservable. Only way to become unobservable is through p-z cancellation.Formally, it means we can figure out the state using the output, and control inputs (by solving the DE).

Informal def'n of **controllability**: Modes that are effected by the input are said to be controllable. All modes controllable => SS controllable. If no p-z cancellations, then shou;d be controllable. Formally, it means that we can start at any state and reach any final state over time (full control over our state). (A, B) controllable PBH test exists to check for controllability.

Limitations exists for uncontrollable/unobservable, similar to p-z cancellation (like how we can never stabilize a system with unstable p-z cancellation).

Def'n of **stabilizablity**: All unstable modes are said to be controllable => no unstable p-z cancellations.

Def'n of **detectability**: All unstable modes are said observable => no unstable p/z cancellations.


#### Plane State Feedback

Assume the entire state can be measured. 
Add r as a reference on the input signal, u; u = -kx. Then constructing state space, we find:
A_cls = (A-Bk). Want to place the poles to be on OLHP... CL poles are eig(A-Bk). We can then choose to place the poles in OLHP by adjusting K, vector.

Theorem: Closed loop poles eig(A-Bk) can be moved arbitrarily <=> (A,B) is controllable. Gain K can to chosen to stabilize system <=> (A, B) is observable.

#### Observer Based State Feedback

Want to estimate unmeasured state components (we don't have the full state vector).
We use a state estimator as our observer.

The only thing that changes here is that rather than measuring the state, x, right from some plant, we take the input to the plant, output from the plant, and make an 'observer' to measure x_hat. We then apply plain state feedback.

We use a luenberger observer, which has a special form. The key thing to note is that it has an observer gain matrix which tries to make y - y_hat error 0 (so that the observer mimics the plant).

By analyzing the error dynamics... e = x - x_hat, we get e_dot = (A - HC)e.

Note that for e(t) -> 0 for any e(0) <=> eig(A-HC) in OLHP. With this, if (A,C) is observable => error dynamics go to 0 for the observer.

If you put this all together, you reazlie the A_cls matrix is upper trianglular, with A_cls for plane state feedback on one diagonal and the error term for the observer on the bottom right. So the eigen values are the union of these two. Implication: We can design controllers for the state feedback controller separately, then put them together (separation principle).

Then the algorithm:
1. Check (A, B) controllable
2. Check (A, C) observable
3. Design controllers by placin poles of eig(A-BK) in OLHP, eig(A-HC) in OLHP

Rule of thumb: Place observer poles 2x as fast as controller poles; observer is non-dominant.


#### Integral Control

Recall good strategy to get signal to 0 -> feed it into integrator; stabilize overall system

Leads us to...

**Plain State Feedback Controller w Integral Action**

Essentially, we have the plant, with a reference on the input, and then feed that into a bank of integrals to drive error to 0. Think of that as an augmented plant.

We can derive some state equations for this plant with intergrator model (state space of the augmented plant model). Putting it in state space for, we can make 2 alias A_tilda and B_tilda, substitude u = -k*x_tilda. We end up with something resembling plain state feedback... just check (A_tilda, B_tilda) are controllable, and then we can arbitrarily place the poles.


**Observer-Based State-Feedback w/ Integral Action**

Same idea as before. Only difference is that the block diagram isn't getting the state, x, directly. It gets a state estimate from an observer. Now, the state vector becomes (x, z, e)... where we have the extra e term. We derive the state equation by tracing out the block diagram (manipulation 3). 

We should note that by the separation principle, the A_cls should be upper triangular => design plain state feedback controller and observer separately. Just check (A_tilda, B_tilda). Then places the poles for the plain state feedback. Check for observabiliy on (A, C) and then we can arbitrarily place the H poles to control error dynamics. 

#### Optimal State-Feedback + Optimal Observers

Before, we have said the we arbitrarily choose poles, which set values for K and H matricies. However, if we have something like 12 poles to be placed, this all becomes more complicated. That's part of the reasoning behind optimal control -> optimally place poles for state-feedback controllers and observers. 


**MIMO LQR**

Given some plant in state space form, D = 0, and x(0) = x0

Objective: Find controller that minimizes scalar cost:
J = integral(0, inf)[x(t)' * Q * x(t) + u(t)' * R * u(t)]. Q, R >= 0 are symmetric matricies.

Assume: (A, B) stabilizable, (A, Q) is detectable.

Result: Controller that minimizes J is stabilizing linear state-feedback controller, u = -K * x... K  is computed by algebraic Riccati equation (though that's more of a detail). These use the A and B matricies (different from observer error dynamics, described below).

The whole idea here is that J penalizes both state and control input (and to what degree). Rather than tweaking poles, we can tweak Q, R as knobs; more meaningfully than choosing poles. 

Commonly, Q is diagonal. Sometimes use Q = C' * C = C_transpose * C. This yields norm(y)^2.... penalizing size of plant output.

R is also diagnonal. If we want the ith control channel to be large, we make r_ii large. Usually solved via iteration; calculators for solving LQR readily available. 


Some obervations of LQR on a spring damper system: With a single input, u, if we just make the R vector (single value; saying that control signal is cheap, small cost) small, we expect to give less weight to the input. Rather than both poles going to very small negative values, only one pole does. Why? Because our Q matrix is identity => equally penalize position and velocity. Small R entry => we want both small position and small velocity, but this contradicts. Now, if we set the Q entry on velocity to 0, then we ignore the velocity and poles go off to negative infinity. 

If control becomes expensive (set R to some large value), then poles approach the open loop poles.

Note that LQR also works with the error dynamics, except we use a trick and end up having to check (A', C') is stabilizeable and (A', Q) is detectable. In LQR, we end up using A` and C' matricies.


#### Constrained Optimal Control

There are many practical scenarios where signals get saturated (especially control signals). Linear techniques cannot handles this... it is non-linear.

Researchers essentially found ways to modify LQR st it handles saturation.

The problem is essentailly the same as LQR, except we constrain mag(u_i(t)) <= alpha_i for all time.

There are no analytical solutions to this problem; only efficient optimization algorithms to solve the problem numerically (to determine u(t) for time instant t). Rather than solving the integral to infinity, we often also choose some finite bound; we optimize over some time horizon.

This constrained technique is known as MPC.