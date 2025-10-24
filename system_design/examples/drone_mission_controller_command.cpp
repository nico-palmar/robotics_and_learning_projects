// implement a drone mission controller
// this should use the command design chain, so that the mission controller can queue and execute commands in sequence
// but each command should act on the drone (reciever) -> takeoff, land, moveTo, hover, etc.
// should be able to add new command w/o changing existing classes

// how will the class strucuture look like using the command pattern?
// first we will have the reciever; the drone, which actually implements some functionality
// we will then have an interface for a drone command, which has an execute method
// this interface will be implemented by many concrete commands (takeoff, land, moveTo)
//    which holds reference to the drone reciever and runs some drone commands
// finally, there is an invoker which is the drone mission controller
// this invoker must have a queue vector, which executes the commands in sequence

#include <iostream>
#include <memory>
#include <vector>

class Drone
{
public:
    bool takeOff() const
    {
        std::cout << "This is the drone taking off" << std::endl;
        return true;
    }

    bool land() const
    {
        std::cout << "This is the drone landing" << std::endl;
        return true;
    }

    bool moveTo() const
    {
        // This function should have possitional args... keeping it simple
        std::cout << "This is the drone moving to" << std::endl;
        return true;
    }
};

class DroneCommand
{
public:
    DroneCommand(std::shared_ptr<Drone> drone): drone_(drone) {}
    virtual bool execute() const = 0;
    // this is good practice, but generally not needed
    virtual ~DroneCommand() = default;
protected:
    // can either have a reference or shared pointer to the drone
    // yep a reference is nicer since each command has a drone
    // weaker ownership here would be better
    std::shared_ptr<Drone> drone_;
};

class DroneTakeOffCommand: public DroneCommand
{
public:
    using DroneCommand::DroneCommand;
    bool execute() const override
    {
        return drone_->takeOff();
    }
};

class DroneLandCommand: public DroneCommand
{
public:
    using DroneCommand::DroneCommand;
    bool execute() const override
    {
        return drone_->land();
    }
};

class DroneMoveToCommand: public DroneCommand
{
public:
    using DroneCommand::DroneCommand;
    bool execute() const override
    {
        return drone_->moveTo();
    }
};

class DroneMissionController
{
public:
    void addCommand(std::unique_ptr<DroneCommand> command)
    {
        commands_to_execute_.push_back(std::move(command));
    }

    bool runCommands() const
    {
        for (std::size_t i = 0; i < commands_to_execute_.size(); i++)
        {
            const auto ret = commands_to_execute_[i]->execute();
            if (ret == false)
            {
                // immitate something failing
                return false;
            }
        }
        return true;
    }

private:
    std::vector<std::unique_ptr<DroneCommand>> commands_to_execute_;
};

int main()
{
    auto drone = std::make_shared<Drone>();
    auto c1 = std::make_unique<DroneTakeOffCommand>(drone);
    auto c2 = std::make_unique<DroneMoveToCommand>(drone);
    auto c3 = std::make_unique<DroneLandCommand>(drone);

    DroneMissionController mission_controller;
    mission_controller.addCommand(std::move(c1));
    mission_controller.addCommand(std::move(c2));
    mission_controller.addCommand(std::move(c3));

    mission_controller.runCommands();
}