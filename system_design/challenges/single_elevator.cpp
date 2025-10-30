
// single elevator in a multi-floor building
// building has N floors
// has starting state: idle, floor 1

#include <queue>
#include <vector>
#include <functional>
#include <memory>
#include <optional>
#include <iostream>

enum class ElevatorState
{
    idle,
    movingUp,
    movingDown,
    doorOpen,
};

enum class ElevatorDirection
{
    up,
    down
};

// elevator should prioritize requests efficiently
// what should the design be?
// should have some interface for handling the request - we can change how it works
// we should have an elevator controller what aggregates the state information and transitions
// we can think out later splitting the elevator controller into a state pattern, but will keep as a state machine inside for now

class RequestHanlder
{
public:
    virtual void outside_request(const int& floor, const ElevatorDirection& direction) = 0;
    virtual void inside_request(const int& floor) = 0;
    // uses up/down knowledge to get the next floor from the heap
    virtual std::optional<int> getNextFloor() = 0;
    virtual void setElevatorDirection(const ElevatorDirection& direction) = 0;
};

// no knowledge of the state machine, just interface to communicate with 
class BasicRequestHanlder: public RequestHanlder
{
public:
    void outside_request(const int& floor, const ElevatorDirection& direction) override
    {
        if (direction == ElevatorDirection::up)
        {
            up_heap_.push(floor);
        }
        else
        {
            down_heap_.push(floor);
        }
    }

    void inside_request(const int& floor)
    {
        if (active_direction_ == ElevatorDirection::up)
        {
            up_heap_.push(floor);
        }
        else
        {
            down_heap_.push(floor);
        }
    }

private:
    std::priority_queue<int> down_heap_;
    std::priority_queue<int, std::vector<int>, std::greater<int>> up_heap_;
    ElevatorDirection active_direction_ { ElevatorDirection::up };
};


class ElevatorController
{
public:
    void outside_request(const int& floor, const ElevatorDirection& direction)
    {
        handler_->outside_request(floor, direction);
    }

    void inside_request(const int& floor)
    {
        handler_->inside_request(floor);
    }

    void run()
    {
        switch (state_)
        {
            case ElevatorState::idle:
                handleIdleState();
                break;
            case ElevatorState::movingUp:
                handleUpState();
                break;
            case ElevatorState::movingDown: 
                handleDownState();
                break;
            case ElevatorState::doorOpen:
                handleOpenState();
                break;
            default:
                throw std::runtime_error("State not recognized!");
        }
    }

private:

    void handleIdleState()
    {
        // go to the next floor
        auto next = handler_->getNextFloor();
        if (next.has_value())
        {
            target_floor_ = next.value();
            if (target_floor_ > curr_floor_)
            {
                state_ = ElevatorState::movingUp;
                handler_->setElevatorDirection(ElevatorDirection::up);
                std::cout << "Moving up!" << std::endl;
            }
            else if (target_floor_ < curr_floor_)
            {
                state_ = ElevatorState::movingDown;
                handler_->setElevatorDirection(ElevatorDirection::down);
                std::cout << "Moving Down!" << std::endl;
            }
            else
            {
                // requested floor is current, just stay
                std::cout << "Staying in idle state" << std::endl;
            }
            
        }
        else
        {
            std::cout << "Staying in idle state" << std::endl;
        }
    }

    void handleUpState();

    void handleDownState();

    void handleOpenState();

    std::unique_ptr<RequestHanlder> handler_;
    ElevatorState state_ { ElevatorState::idle };
    int curr_floor_ { 1 };
    int target_floor_ { 1 };
};