// need to design a system to mix and match
// there will be mobile robots (wheeled, legged)
// and control algos (PID, MPC, RL)
// should be able to mix and match

// the mixing and matching of the 2 different 'types' opens us up to a bridge design
// in this case, the higher level concept is the robot type (abstraction).
// there can be many different robot types defined
// the lower level this is the controller (implementation)
// there can be many different controllers, which define their own control mechanisms

#include <memory>
#include <iostream>


class Controller
{
public:
    virtual void track() = 0;
    virtual ~Controller() = default;
};

class MPCController: public Controller
{
public:
    void track() override
    {
        std::cout << "Simulate tracking with an MPC controller" << std::endl;
    }
};

class Robot
{
public:
    Robot(Controller& controller): controller_(controller) {}
    virtual void move() const = 0;
    virtual ~Robot() = default;
protected:
    // should use some implementation
    // const std::shared_ptr<Controller> controller_;
    Controller& controller_;
    // note: In my mind, more implementations can be defined here using an interface
    // that way, the underlying implementation can change; we could have planners here and more stuff
};

class WheeledRobot: public Robot
{
public:
    // use base class' constructor
    using Robot::Robot;

    void move() const override
    {
        std::cout << "Movement for a wheeled robot" << std::endl;
        controller_.track();
    }
};


class LeggedRobot: public Robot
{
public:
    using Robot::Robot;

    void move() const override
    {
        std::cout << "Movement for a legged robot" << std::endl;
        controller_.track();
    }
};

int main()
{
    auto controller = MPCController();
    // NOTE: when working with objects, they can only be defined const if the methods
    // that are called by them are also const
    // otherwise, they can't be called
    const auto wheeled_robot = WheeledRobot(controller);
    const auto legged_robot = LeggedRobot(controller);

    wheeled_robot.move();
    legged_robot.move();
}