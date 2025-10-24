
// there is a MotionCommand which is meant to generate movement for a motion planner
// we want to conditionally add runtime features to the motion command

// the design will use decorators to add conditional runtime changes
// there will be an interface to use for the motion command pipeline
// Then the main motion planner, along with a number of decorators

class MotionCommand
{
public:
    virtual void moveToPose() = 0;
};

// make a MotionCommandImplementor, which "moves"

// then make a decorator, like saftey decorator, which inherits MotionCommander and has a unique pointer to a MotionCommand
// then, it implements the moveToPose method, adding it's own functionality and calling the unique pointer's method too


// in creation, will need to create each decorator node in order, and pass it in through the constructor
// then we call the decorator's moveToPose method