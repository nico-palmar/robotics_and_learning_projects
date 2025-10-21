#include <vector>
#include <memory>

struct pos2D
{
    float x;
    float y;
    float theta;
};

// SRP
class Planner
{
public:
    virtual std::vector<pos2D> plan(const pos2D& start, const pos2D& goal) = 0;
};

// OCP => this becomes extensible since we can make multiple interfaces
// no need to change internal planner code (or anywhere else)
class AStarPlanner: public Planner
{
public:
    std::vector<pos2D> plan(const pos2D& start, const pos2D& goal) override
    {
        std::vector<pos2D> path;
        // implement
        return path;
    }
};

// DIP => motor controller is abstracted as an interface
// higher level modules don't need to look low level
class IMotorController
{
public:
    virtual void setSpeed(double v = 0) = 0;
};

class DCMotor: public IMotorController
{
public:
    void setSpeed(double v = 0) override
    {
        return;
    }
};

// can repeat the same with a simulated motor controller

// SRP
class Controller
{
public:
    Controller(std::unique_ptr<IMotorController> motor): motor_(std::move(motor)) {}
    virtual void follow_path(std::vector<pos2D> path) = 0;

private:
    std::unique_ptr<IMotorController> motor_;
};

class SimpleController: public Controller
{
public:
    SimpleController(std::unique_ptr<IMotorController> motor): Controller(std::move(motor)) {}
    void follow_path(std::vector<pos2D> path) override
    {
        // do some path following
        // use the motor -> 
        return;
    }
};

// SRP: the navigation manager is using the objects in this case
// also depedency-inversion; lower level classes are not deep baked in here
class NavigationManager
{
public:
    NavigationManager(std::unique_ptr<Planner> planner, std::unique_ptr<Controller> controller): planner_(std::move(planner)), controller_(std::move(controller))
    {}

    void navigate(const pos2D& goal) const
    {
        const auto start = pos2D();
        const auto path =  planner_->plan(start, goal);
        controller_->follow_path(path);
    }

private:
std::unique_ptr<Planner> planner_;
std::unique_ptr<Controller> controller_;

};

int main()
{
    auto motor = std::make_unique<DCMotor>();
    auto planner = std::make_unique<AStarPlanner>();
    auto controller = std::make_unique<SimpleController>(std::move(motor));
    const auto manager = NavigationManager(std::move(planner), std::move(controller));
    const auto goal = pos2D();
    manager.navigate(goal);
}