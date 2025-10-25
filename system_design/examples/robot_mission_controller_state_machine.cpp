
// implement a robot mission state machine, using the state pattern
// there will be multiple states (Idle, takeoff, navigate, and land)

// for the design, we need to think of the different components for the state pattern
// first, there will be a mission state interface. Each mission state will have an execute command
// then, there will be concrete states, which implement the execute method for the state. They will then transition to the correct state
// finally, there will be a mission controller, which has a state and polimorphically calls execute

#include <memory>
#include <iostream>

class MissionController;

class MissionState
{
public:
    // these methods should mimic the set of actions that the state can perform
    virtual void progress(MissionController& mission_controller) = 0;
    virtual void reset(MissionController& mission_controller) = 0;
    virtual ~MissionState() = default;
};

class MissionController
{
public:
    MissionController(std::unique_ptr<MissionState> starting_state): state_(std::move(starting_state)) {}

    void setState(std::unique_ptr<MissionState> state)
    {
        state_ = std::move(state);
    }

    void update()
    {
        state_->progress(*this);
    }

    void reset()
    {
        state_->reset(*this);
    }

private:
    std::unique_ptr<MissionState> state_;
};

class IdleState: public MissionState
{
public:
    // declare this up here
    void progress(MissionController& mission_controller) override;
    void reset(MissionController& mission_controller) override;
};

class LandingState: public MissionState
{
public:
    void progress(MissionController& mission_controller) override
    {
        std::cout << "In Landing state, moving to idle" << std::endl;
        mission_controller.setState(std::make_unique<IdleState>());
    }

    void reset(MissionController& mission_controller) override
    {
        progress(mission_controller);
    }
};

class NavigateState: public MissionState
{
public:
    void progress(MissionController& mission_controller) override
    {
        std::cout << "In Navigate state, moving to landing" << std::endl;
        mission_controller.setState(std::make_unique<LandingState>());
    }

    void reset(MissionController& mission_controller) override
    {
        progress(mission_controller);
    }
};

class TakeoffState: public MissionState
{
public:
    void progress(MissionController& mission_controller) override
    {
        std::cout << "In TAKEOFF state, moving to Navigating" << std::endl;
        mission_controller.setState(std::make_unique<NavigateState>());
    }

    void reset(MissionController& mission_controller) override
    {
        std::cout << "Resetting takeoff state to landing" << std::endl;
        mission_controller.setState(std::make_unique<LandingState>());
    }
};

void IdleState::progress(MissionController& mission_controller)
{
    std::cout << "In IDLE state, moving to takeoff" << std::endl;
    mission_controller.setState(std::make_unique<TakeoffState>());
}

void IdleState::reset(MissionController& mission_controller)
{
    std::cout << "Already IDLE, stay here for reset" << std::endl;
}


int main()
{
    MissionController mission_controller(std::make_unique<IdleState>());

    mission_controller.update();
    mission_controller.reset();

    mission_controller.reset();
    mission_controller.update();
    mission_controller.update();
    mission_controller.update();
}