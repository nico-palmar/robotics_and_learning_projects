// implement a path tracking system that can switch between different control strategies at runtime

// note that in this case, the input to the controllers is the same (the path, maybe robot state)
// The switching ability can be done by a strategy pattern

// we will have 1 controller base class
// multiple control strategies
// then we will have a Path follower, which has a controller and uses it (while having the ability to swap it out)

#include <vector>
#include <iostream>
#include <memory>

struct Pose2D
{
    float x;
    float y; 
    float yaw;
};

class BaseController
{
public:
    virtual void track(const std::vector<Pose2D>& path) const = 0;
    virtual ~BaseController() = default;
};

class PIDController: public BaseController
{
public:
    void track(const std::vector<Pose2D>& path) const override
    {
        std::cout << "Using PID controller" << std::endl;
    }
};

class StanleyController: public BaseController
{
public:
    void track(const std::vector<Pose2D>& path) const override
    {
        std::cout << "Using stanley controller" << std::endl;
    }
};

class PathFollower
{
public:
    void setController(std::unique_ptr<BaseController> controller)
    {
        controller_ = std::move(controller);
    }

    void follow_path(const std::vector<Pose2D>& path) const
    {
        if (!controller_)
        {
            throw std::runtime_error("Controller not given to the path follower");
        }
        controller_->track(path);
    }

private:
    std::unique_ptr<BaseController> controller_;
};

int main()
{
    std::vector<Pose2D> path {{1, 1, 3.15}, {2, 2, 1.0}};
    const auto follower = std::make_unique<PathFollower>();
    // follower->follow_path(path);
    follower->setController(std::make_unique<PIDController>());

    follower->follow_path(path);

    follower->setController(std::make_unique<StanleyController>());
    follower->follow_path(path);
}