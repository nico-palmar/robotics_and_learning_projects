#include <iostream>

enum class TrafficLightStates
{
    Red,
    Yellow,
    Green,
};

class TrafficController
{
public:
    void run()
    {
        tick_ += 1;
        curr_tick_ += 1;
        std::cout << "Tick " << tick_ << ": ";
        switch(state_)
        {
            case TrafficLightStates::Red:
                if (curr_tick_ < 5)
                {
                    std::cout << "Red";
                    break;
                }
                state_ = TrafficLightStates::Green;
                std::cout << "Red -> Green";
                curr_tick_ = 0;
                break;
            case TrafficLightStates::Yellow:
                if (curr_tick_ < 2)
                {
                    std::cout << "Yellow";
                    break;
                }
                state_ = TrafficLightStates::Red;
                std::cout << "Yellow -> Red";
                curr_tick_ = 0;
                break;
            case TrafficLightStates::Green:
                if (curr_tick_ < 5)
                {
                    std::cout << "Green";
                    break;
                }
                state_ = TrafficLightStates::Yellow;
                std::cout << "Green -> Yellow";
                curr_tick_ = 0;
                break;
            default:
                throw std::runtime_error("State not defined!");
        }

        std::cout << "" << std::endl;

    }

    int getTick()
    {
        return tick_;
    }

private:
    int tick_ { 0 };
    int curr_tick_ { 0 };
    TrafficLightStates state_ { TrafficLightStates::Red };
};

int main()
{
    TrafficController controller;

    while (controller.getTick() < 50)
    {
        controller.run();
    }
}

// how would I change this into a state design pattern?
// have a base interface for traffic light state. It should have update, enter, and exit
// the update method should take a reference to the upper controller class, so that we can change the state
// implement all 3 states
// then, in the controller, just call updates on the state
// in each update call, if there is a state transition occuring, call the controller class state transition
// the controller class state transition will then call exit on the old state (if there is one)
// use the enter/exit to reset things related to the state cleanup/entry for the state.
// then swap to the new state, then call enter on the new state.

