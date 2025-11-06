#include <string>
#include <iostream>
#include <vector>
#include <memory>

class RobotTask
{
public:
    virtual void execute() = 0;
    virtual std::string name() const = 0;
    virtual ~RobotTask() = default;
};

class NavigationTask: public RobotTask
{
public:
    void execute() override
    {
        std::cout << "Executing nav task" << std::endl;
    }

    std::string name() const override
    {
        return name_;
    }
private:
    std::string name_ { "Nav Task" };
};

class PickupTask: public RobotTask
{
public:
    void execute() override
    {
        std::cout << "Picking up item" << std::endl;
    }

    std::string name() const override
    {
        return name_;
    }
private:
    std::string name_ { "Pickup Task" };
};


class RechargeTask: public RobotTask
{
public:
    void execute() override
    {
        std::cout << "Executing recharge task" << std::endl;
    }

    std::string name() const override
    {
        return name_;
    }
private:
    std::string name_ { "Recharge Task" };
};

class RobotController
{
public:
    void addTask(std::unique_ptr<RobotTask> task)
    {
        tasks_.push_back(std::move(task));
    }

    void printNames() const
    {
        for (const auto& task: tasks_)
        {
            std::cout << "Task name: " << task->name() << std::endl;
        }
    }

    void runTasks()
    {
        if (tasks_.empty())
        {
            return;
        }

        for (auto it = tasks_.begin(); it != tasks_.end(); )
        {
            (*it)->execute();
            it = tasks_.erase(it);
        }
    }

private:
    std::vector<std::unique_ptr<RobotTask>> tasks_;
};

int main()
{

    auto controller = RobotController();

    auto task_1 = std::make_unique<NavigationTask>();
    auto task_2 = std::make_unique<PickupTask>();
    auto task_3 = std::make_unique<RechargeTask>();

    controller.addTask(std::move(task_1));
    controller.addTask(std::move(task_2));
    controller.addTask(std::move(task_3));

    controller.printNames();
    controller.runTasks();

    return 0;
}