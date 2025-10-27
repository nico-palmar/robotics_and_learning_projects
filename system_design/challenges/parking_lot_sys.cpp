
// design classes for parking lot system
// handle vehicles entering, parking, exiting, and tracking of available spots

// concepts: levels, parking spots, one vehicle per spot, vehicle types, some spots for specific types (motorcycle, disabled),
// vehcile entry assigns tickets with some spots
// vehicle exit charges someone based on time, and spot is freed


// operations:
// park(Vehicle v), unpark(Ticket t), displayFreeSpots(), calculateFee(Ticket t)

// think about the class structures

// there will need to be a base interface for vehicles, containing vehicle data
// there will be multiple types of vehicles which extend the vehicle

// now for the parking infra -> this is what is in charge of parking a vehicle and will also handle unparking
// there needs to be a class for spots (what type of vehicle, free/full, ID)
// there then needs to be a class for parking level, which contains all spots on that level
// finally there should be a parking lot, which contains all levels possible

// now we need to think of payment methods
// there should be a payment machine class. This can handle payment methods, and give tickets
// this means that there's also a payment method class (for keeping track of different payments)
// there's also ticket class, for keeping track of entrance time, vehicle type, and spot
#include <string>
#include <vector>
#include <memory>

enum class VehicleType
{
    motorcycle,
    car,
    truck,
};

// leave like this for now, we can see if subclassing is necessary
class Vehicle
{
public:
    Vehicle(const VehicleType& type, const std::string& plate): type_(type), plate_(plate) {}
    VehicleType getType()
    {
        return type_;
    }

    std::string getPlate()
    {
        return plate_;
    }

protected:
    VehicleType type_;
    std::string plate_;
};

enum class ParkingLevelType
{
    level_0,
    level_1,
    level_2,
    level_3
};

class Spot
{
public:
    Spot(const int& id, const ParkingLevelType& level): id_(id), level_(level) {}

    bool assignVehicle(const std::shared_ptr<Vehicle> vehicle)
    {
        if (occupied_)
        {
            // TODO: consider throwing here instead
            return false;
        }
        vehicle_ = std::move(vehicle);
        occupied_ = true;
        return true;
    }

    bool removeVehicle()
    {
        if (!occupied_)
        {
            return false;
        }
        occupied_ = false;
        return true;
    }

// private:
    std::shared_ptr<Vehicle> vehicle_;
    bool occupied_ { false };
    int id_;
    ParkingLevelType level_;
};

// class ParkingLevel
// {
// public:
//     ParkingLevel(std::vector<Spot>& spots)
//     {
//         // make sure no copy here
//         spots_ = std::move(spots);
//     }

    // this should just be responsbile for finding a spot

// private:
//     std::vector<Spot> spots_;
// };

class ParkingLot
{
public:
    ParkingLot(std::vector<Spot>& spots): spots_(std::move(spots)) {}

    bool assignVehicle(const int& spot_id, const std::shared_ptr<Vehicle>& vehicle)
    {
        for (auto& vect_spot: spots_)
        {
            if (vect_spot.id_ == spot_id)
            {
                vect_spot.assignVehicle(std::move(vehicle));
            }
        }
    }

    bool removeVehicle(Spot& spot)
    {
        return spot.removeVehicle();
    }

    std::vector<Spot> getAvailableSpots()
    {
        std::vector<Spot> free_spots;
        for (const auto spot: spots_)
        {
            if (!spot.occupied_)
            {
                free_spots.push_back(spot);
            }
        }
        return free_spots;
    }

private:
    // consider making this a map from id -> spot
    // also these should be shared ptrs
    std::vector<Spot> spots_;
};

class Ticket
{
public:
    Ticket(const std::string& start_time, const Spot& spot, const ParkingLevelType level, const Vehicle& vehicle): start_time_(start_time), spot_(spot), level_(level), vehicle_(vehicle) {}

    void pay()
    {
        paid_ = true;
    }

    // add an is paid method

private:
    std::string start_time_;
    // has spot and level to uniquely identify the location
    Spot& spot_; // todo make shared ptr
    ParkingLevelType level_;
    // as a nice to have, also has a vehicle
    Vehicle& vehicle_; // todo make shared ptr
    bool paid_ {false};
};

// to pay using a machine, need differrent types of payment methods
// use the factory method to enable a common payment interface
// essentially, will have one payment interface, which declare pay
// will be multiple payment methods. Have a factory which creates these

class PaymentMethod
{
public:
    virtual void pay(Ticket& ticket) = 0;
    virtual ~PaymentMethod() = default;
};

class CardPayment: public PaymentMethod
{
public:
    void pay(Ticket& ticket) override
    {
        ticket.pay();
    }
};

class PaymentFactory
{
public:
    virtual std::unique_ptr<PaymentMethod> createPaymentMethod() = 0;
};

class CardPaymentFactory: PaymentFactory
{
public:
    std::unique_ptr<PaymentMethod> createPaymentMethod() override
    {
        return std::make_unique<CardPayment>();
    }
};

// ticket machine handles payments, and gives tickets/spots
class TicketMachine
{
public:
    TicketMachine(ParkingLot& lot): lot_(lot), factory_(std::move(std::make_unique<PaymentFactory>())) {}

    // issue a ticket, which assigns a spot
    // gets a spot using the lot, which should be a shared pointer
    // populate all ticket fields

    Spot assignSpot()
    {
        // get first free spot
        auto spot = lot_.getAvailableSpots()[0];
        return spot;
    }

    // handle payment
    // should have a ticket
    // from the ticket, can check if paid or not. If not...
    // create a payment method and paaya




private:
    std::unique_ptr<PaymentFactory> factory_;
    ParkingLot& lot_;
};