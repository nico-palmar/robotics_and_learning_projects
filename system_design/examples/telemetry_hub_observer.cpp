// need to implement a telemetry hub for a robotic system
// should use the observer design pattern

// the subject is some telemetry hub, which publishes updates on robot state variables
// the observers are different monitoring components (BatteryDisplay, CoolingSystem, etc)

// how would the design work?
// need an observer interface which will readData from the telemetry station
// the different observers will read the data and decide what they want from it

// the subject would have some interface for attaching/detaching observers, and notifying them
// the telemetry hub then extends the subject. It has a bunch of weak pointers to observers (so it can keep track if they don't exist). Weak has_a relationship.
#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>

struct Pose2D
{
    float x;
    float y;
    float yaw;
};

struct RobotStateData
{
    Pose2D pose;
    float temp;
    int battery_level;
};

// TODO: consider renaming
class SubsystemObserver
{
public:
    virtual void getData(const RobotStateData& data) = 0;
    virtual ~SubsystemObserver() = default;
};

// these could probably just have a subsystem observer, and then call the getData method?
// I guess it's because it'll broadcast to observers, so they need to extend to get the message

class BatteryDisplay: public SubsystemObserver  
{
public:
    void getData(const RobotStateData& data) override
    {
        battery_level_ = data.battery_level;
        std::cout << "Read back battery data as " << battery_level_ << std::endl;
    }

private:
    int battery_level_;
};

class CoolingSystem: public SubsystemObserver  
{
public:
    void getData(const RobotStateData& data) override
    {
        temp_ = data.temp;
        std::cout << "Read back temperature data as " << temp_ << std::endl;
    }

private:
    int temp_;
};

// now implement the subject interface/telemetry hub
class RobotDataPublisher
{
public:
    // note: pass these are reference since their count isn't actually here
    // we don't want to increase the ref count on pass by value
    virtual void attach(const std::shared_ptr<SubsystemObserver>& subscriber) = 0;

    virtual void detach(const std::shared_ptr<SubsystemObserver>& subscriber) = 0;

    virtual void publish(const RobotStateData& data) = 0;

protected:
    // make the data publisher have a weak pointer to subscribers; prevent cyclic pointers
    std::vector<std::weak_ptr<SubsystemObserver>> subscribers_;
};

class TelemetryHub: public RobotDataPublisher
{
public:
    void attach(const std::shared_ptr<SubsystemObserver>& subscriber) override
    {
        subscribers_.push_back(subscriber);
    }

    void detach(const std::shared_ptr<SubsystemObserver>& subscriber) override
    {
        // implement safe detaching (if subscriber doesn't exist)
        // could just loop through, get the lock for weak pointer, and if null call erase on the vector
        for (auto it = subscribers_.begin(); it != subscribers_.end();)
        {
            if (it->expired() || it->lock() == subscriber)
            {
                it = subscribers_.erase(it);
            }
            else
            {
                it++;
            }
        }

        // there is a better way using more 'idiomatic C++'
        // subscribers_.erase(
        //     std::remove_if(
        //         subscribers_.begin(), subscribers_.end(),
        //         [&](const std::weak_ptr<SubsystemObserver>& w)
        //         {
        //             return w.expired() || w.lock() == subscriber;
        //         }
        //     ),
        //     subscribers_.end()
        // );
    }

    void publish(const RobotStateData& data) override
    {
        // loop through all the subscribers
        // check if they exist via lock on weak pointer
        // if they do, call their getData methods to send over the data
        for (auto it = subscribers_.begin(); it != subscribers_.end(); )
        {
            // get a shared pointer, if it still exists
            const auto obs = it->lock();
            if (obs)
            {
                obs->getData(data);
                it++;
            }
            else
            {
                // remove a dead subscriber
                // note that erase gives you the next valid iterator position
                it = subscribers_.erase(it);
            }
        }
    }
};

int main()
{
    const auto cooling_sys = std::make_shared<CoolingSystem>();

    const auto telemetry = std::make_unique<TelemetryHub>();
    RobotStateData data { {1, 1, 1}, 20, 99 };
    {
        const auto battery_sys = std::make_shared<BatteryDisplay>();

        telemetry->attach(cooling_sys);
        telemetry->attach(battery_sys);

        std::cout << "Publish 1" << std::endl;
        telemetry->publish(data);
    }
    std::cout << "Publish 2" << std::endl;
    telemetry->publish(data);
    telemetry->detach(cooling_sys);
    std::cout << "Publish 3" << std::endl;
    telemetry->publish(data);
}