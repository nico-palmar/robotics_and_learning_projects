
// design a system for robot manipulators
// should all have an activate methods
// manipulators consist of: arm -> end effector -> joints. 

// should use composite structure, where joint is the smallest component
#include <iostream>
#include <memory>
#include <vector>

class RobotComponent
{
public:
    virtual void activate() const = 0;
    virtual ~RobotComponent() = default;
};


class Joint: public RobotComponent
{
public:
    Joint(const std::size_t id): joint_id_(id) {}
    void activate() const override
    {
        std::cout << "Activating robot joint " << joint_id_ << std::endl;
    }
private:
    std::size_t joint_id_;
};


class RobotSubsystem: public RobotComponent
{
public:
    RobotSubsystem(const std::string& name): name_(name) {}

    void add(const std::shared_ptr<RobotComponent> component)
    {   
        effector_joints_.push_back(component);
    }

    void activate() const override
    {
        std::cout << "Activating the " << name_ << " subsystem" << std::endl;
        for (const auto joint&: effector_joints_)
        {
            joint->activate();
        }
    }

private:
    // TODO: consider swapping these to unique pointers
    std::vector<std::shared_ptr<RobotComponent>> effector_joints_;
    std::string name_;
};

int main()
{
    const auto joint_0 = std::make_shared<Joint>(0);
    const auto joint_2 = std::make_shared<Joint>(2);
    const auto joint_3 = std::make_shared<Joint>(3);
    const auto joint_4 = std::make_shared<Joint>(4);

    const auto end_effector = std::make_shared<RobotSubsystem>("End Effector");
    end_effector->add(joint_0);
    end_effector->add(joint_2);

    const auto arm = std::make_shared<RobotSubsystem>("Arm");
    arm->add(end_effector);
    arm->add(joint_3);
    arm->add(joint_4);

    arm->activate();
}
