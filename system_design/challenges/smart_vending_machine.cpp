
// must be able to hold multiple items
// must be able to accept coins/bills for payment
// must be able to cancel a transaction midway
// maybe we want to extend to other, different payment options
// must have an interface for a user to interact with (select, pay, dispense)
// provide change
// keep track of inventory

// class ideas:
// item class: item name, cost, and ID
// payment abstract class. Use a strategy method, has a pay method, and a cancel method
// implement one version of pay which accepts an item and some money (coins/dollars)
// it should keep track of how much has been paid, and any change to give back

// actually no... these are just methods of the vending machine, keep it that way
// Select a vending machine interface, which contains functionality and state machine
// the select operation should be able to get an ID and select an item (do first)
// pay should be able to use the payment implementation to do a payment (do second)
// dispense should check for payment complete in state machine (do third, and reset)

// finally, there is a vending machine class, which will have a bunch of items (unique ptrs)
// a single vending machine interface
// it will also have method to keep track of inventory (which are items)
// and may be the thing that a client actually interacts with
#include <string>
#include <vector>
#include <memory>

class Item
{
public:
    Item(const std::string name, const float cost): name_(name), cost_(cost) {}

    std::string getName() const;
    float getCost() const;

private:
    std::string name_;
    float cost_;
};

enum class PaymentResponse
{
    PaidFull,
    PaidPartial,
    PaidAndRequiresChange
};

class PaymentMethod
{
public:
    virtual PaymentResponse pay(const Item& item, const float& amount) = 0;
    virtual bool cancel() = 0;
};

class CashPayment: public PaymentMethod
{
public:
    PaymentResponse pay(const Item& item, const float& amount) override
    {
        if (item.getCost() > (amount + paid_so_far_))
        {
            paid_so_far_ += amount;
            return PaymentResponse::PaidPartial;
        }
        else
        {
            PaymentResponse resp = PaymentResponse::PaidFull;
            if (item.getCost() != (amount + paid_so_far_))
            {
                change_ = amount + paid_so_far_ - item.getCost();
                resp = PaymentResponse::PaidAndRequiresChange;
            }
            paid_so_far_ = 0;
            return resp;
        }
    }

    bool cancel() override;

private:
    float paid_so_far_ { 0.0f };
    float change_ { 0.0f }; 
};

class CoinPayment: public PaymentMethod
{
public:
    PaymentResponse pay(const Item& item, const float& amount) override;

    bool cancel() override;

private:
    float paid_so_far_ { 0.0f };
};

enum class VendingMachineState
{
    Idle,
    SelectingItem,
    Paying,
    DispensingItem,
};

// define a spot class which might have an item, but might be empty
// also holds a quantity
// and ID
class VendingMachineSpot;

class VendingMachine
{
public:
    void setPaymenthMethod(std::unique_ptr<PaymentMethod> method)
    {
        payment_method_ = std::move(method);
    }

    void stockMachine(const std::vector<Item>& items);

    // note that these methods should not be implemented in the state machine
    // we might want another interface which runs the state machine, calls actions, and handles state
    void selectItem(const int id)
    {
        for (const auto& spot: spots_)
        {
            if (spot->getID() == id)
            {
                curr_item_ = spot.getItem();
            }
        }
    }

    void pay(const float amount)
    {
        payment_method_->pay(curr_item_, amount);
    }


    void dispense();

    void cancelTransation();

private:
    std::vector<std::unique_ptr<VendingMachineSpot>> spots_;
    Item& curr_item_;
    std::unique_ptr<PaymentMethod> payment_method_;
    VendingMachineState state_ { VendingMachineState::Idle };
};