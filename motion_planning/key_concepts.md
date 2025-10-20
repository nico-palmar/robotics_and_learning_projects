## Motion Planning Review

### 1. C-Space

Configuration: Specs on all possible positions of the robot

C-space: Space of all configurations.

Generally, we care about the C-space of the robot because we want to do planning in the C-space. It's much harder to represent collisons and geometry in the work space mathematially. Instead, we find a plan that is C-free for the robot.

Note that the C-space for a planar mobile robot would be (x, y, yaw), while that of a robot arm would be it's joint angles.

There's a dimensionality vs complexity trade-off here. The higher the dim of the c-space, the exponentially more branching and search we need to perform. However, doing search in a low dim C-space may lead to intractable paths. Sometimes, we can constrain the planner under certain conditions and in a lower C-space to be more computationally efficient. 

For rigid robots (in 2D/3D), we generally construct the C-space by taking the robot as a point, and using minowski sum to inflate the obstacles to account for the robot's geometry.

With manipulators, this doesn't happen due to high dimensionality. Instead, you implicitly do collision checking with obstacles (as in sample-based planning methods... which will be reviewed next).


### 2. Sample-Based Planning

- RRT (Rapidly-exploring Random Tree)

RRT works by creating a random tree, that goes out from the C-space at the start (randomly), checks for collisions at every point, continually building a tree with nodes/edges, and finishes when the end point is close enough to the goal.

The pseudo code looks something like this:

```
RRT(q_start, q_goal):
    Tree T = initalizeTree(q_start) // initalize tree only with initial C-space
    for i in range(N): // loop through for N iterations
        q_rand = RandomCSpace() // get some random pt in C space
        q_nearest = closest(T, q_rand) // find closest point to random C-space point in tree
        q_new = direct(q_nearest, q_rand, step_size) // direct us towards this random point
        if collision(q_nearest, q_new):
            continue // not possible; in C-space this generates collision
        T.addEdge(q_nearest, q_new)
        T.addVertex(q_new)

        if distance(q_new, q_goal) < goal_thresh:
            return Path(T, q_start, q_new)
    
    return Failure // took to many iterations
```

Note that the collision function is not trivial. We need to discretely sample some points in between this edge (get a list of points in C-space). Then we use forward kinematics to convert points in C-space to actual geometric workspace of robot. Finally, we can use some collision checker in geometric space to see if there is any collision.

Note that RRT is quite suboptimal and leads to paths that are a bit.. random `:)`. This can be fixed by using RRT*. It's pretty close to RRT, but keep a cost for the distance to each node, and if a shorter cost is found, it decreases the cost to arrive to nodes before (similar to djikstras).

```
RRT*(q_start, q_goal):
    Tree T = initializeTree(q_start) // also tag along a cost.. say path length
    for i in range(N):
        q_rand = RandomCSpace()
        q_nearest = closest(T, q_rand)
        q_new = direct(q_nearest, q_new, step_size)

        if collision(q_nearest, q_new):
            continue // could probably skip this, but keep it for simplicity sake

        q_parent = q_nearest
        q_neighbours = nearby(T, q_nearest, radius)
        min_cost = cost(q_nearest) + distance(q_nearest, q_new)

        // find the most optimal neighbour in some neighbourhood
        for q_neighbour in q_neighbours:
            if collision(q_neighbour, q_new):
                // record this in some collision cache
                continue

            if cost(q_neighbour) + distance(q_parent, q_new) < min_cost:
                min_cost = cost(q_neighbour) + distance(q_parent, q_new)
                q_parent = q_neighbour // keep track of which neighbour has the shortest non-collision path
        
        T.addEdge(q_parent, q_new)
        T.addVertex(q_new)


        // rewire the tree to go through this neighbour, if possible
        for q_neighbour in q_neighbours:
            if collisionByCahcedCheck(q_neighbour, q_new): // no recompute
                continue
            
            if cost(q_neighbour) + distance(q_neighbour, q_new) < cost(q_neighbour):
                // rewrite the tree
                neighbour_parent = T.get_parent(q_neighbour)
                T.remove_edge(neighbour_parent, q_neighbour)
                T.add_edge(neighbour_parent, q_new)
                T.add_edge(q_new, q_neighbour)
        
        // goal check
        nearest_to_goal = closest(T, q_goal)

        if distance(nearest_to_goal, q_goal) < thresh:
            return path(T, q_start, nearest_to_goal)
    
    return Failure
```

RRT* can generate much more straight and optimal paths by checking for the lowest cost neighbour and updating the shortest path with the new nodes. It is significantly more expensive though, as it does many more collision checks in the optimization process.

- PRM (Probabilistic Roadmap)

The concept of PRM is essentially to build a graph to represent the environment, excluding paths that collide with obstacles in the C-space. Then, add the start and end nodes to the graph in some fashion, and find the shortest path in the graph using A*, djikstras, etc. Works if you really only have static obstacles. Also works if you need to plan many paths quickly.

```
PRM_build(N):
    V = {}
    E = {} // data structurs for constructing a graph

    for i in range(N):
        q_rand = randomCSpace()
        if inCollision(q_rand):
            continue
        V.add(q_rand)

    // now construct edges in nearby nodes
    for q_curr in V: 
        q_neighbours = findKNearNodes(V, q_curr, radius, K)

        for neighbour in q_neighbours:
            if inCollision(q_curr, neighbour):
                continue
            
            E.add(q_curr, neighbour)
    
    return (V, E) // returns a preconstructed graph of edges and verticies

PRM_run(q_start, q_goal, V, E):
    if inCollision(q_start) or inCollision(q_goal):
        return Failure
    
    V += { q_start, q_goal }

    near_start = findKNearNodes(V, q_start, radius, K)
    for q_near_start in near_start:
        if inCollision(q_start, q_near_start):
            return Failure
        E.add(q_start, q_near_start)
    
    // do the same process for the goal node

    return shortest_path(q_start, q_goal, V, E)
```

### 3. Kinodynamic Planning

Kinodynamic planning takes everything a step further. Not only do  we consider C-space/kinematics of the robot, but we also take into account the dynamic constraints. This means, the robot must be able to generate feasible trajectories. We must get some feasible control inputs and propagate them forward in time (using dynamics equations/simulation) and ensure these paths are collision free. The edges in the graphs are no longer straight lines, they are dynamically feasible movements.

Let's take a look at the simplest one:

```
// NOTE: in this case we use, x, the state.
// We are not using q for C-space
// we check everything in state space, including collisons
kinodynamicRRT(x_start, x_goal):
    Tree T = initializeTree(x_start)

    for i in range(N):
        x_rand = randomStateSpace()
        x_nearest = findNearest(T, x_rand)
        u_rand = chooseControlU(x_nearest, x_rand) // generate dynamically feasible control input.. attempt to do this at least, to get close
        x_next = dynamics(x_nearest, u_rand, delta_t) // simulate the dynamics

        if collision_free_path_between(x_nearest ... x_next): // the entire traced out trajectory is collision free. Not only the straight line in C-space.
            continue
        
        T.addEdge(q_nearest, q_next, u_rand)
        T.addNode(q_next)

        // goal check
        if distance(q_next, q_goal) < thresh:
            return Path(q_start, q_next)
```

### 4. Optimization-Based/Trajectory Planning

### 5. Brief: Practice MoveIT ideas