
// skip vehicle class for now.. might be as simple as an enum class
// need to create rider and driver classes
// There also needs to be a ride class, which will have multiple states
// a central system manages the whole thing
#include <string>
#include <memory>
#include <vector>

struct GPSLocation
{
    float lat;
    float longitude;
};


class Rider
{
public:
    Rider(const std::string& name, const GPSLocation& pickup, const GPSLocation& dropoff)
    {
        name_ = name;
        pickup_spot_ = pickup;
        dropoff_spot_ = dropoff;
    }

    GPSLocation getPickup() const
    {
        return pickup_spot_;
    }

    GPSLocation getDropoff() const
    {
        return dropoff_spot_;
    }

    std::string getName() const;

private:
    std::string name_;
    GPSLocation pickup_spot_;
    GPSLocation dropoff_spot_;
};

class Driver
{
public:
    Driver(const std::string& name, const float& rating): name_(name), rating_(rating) {}

    bool isFree();

    GPSLocation getCurrLoc();

private:
    std::string name_;
    float rating_;
    GPSLocation curr_loc_;
    GPSLocation next_loc_;
    bool free_ { true };
};

enum class RideState
{
    finding_driver,
    driver_arriving,
    driving,
    complete
};

class Ride
{
public:
    Ride(const std::shared_ptr<Rider> rider): rider_(std::move(rider)) {}

    // implement methods here for getting a ride
    void assignDriver(const std::shared_ptr<Driver> driver)
    {
        driver_ = std::move(driver);
        state_ = RideState::driver_arriving;
    }

    void startRide()
    {
        state_ = RideState::driving;
    }

    void finishRide();

    RideState getState();

private:
    RideState state_ { RideState::finding_driver };
    std::shared_ptr<Driver> driver_;
    std::shared_ptr<Rider> rider_;
};

class RideMatcher
{
public:
    virtual std::shared_ptr<Driver> match(const std::shared_ptr<Rider>& rider) = 0;
    void addDriver(const std::shared_ptr<Driver> driver);
protected:
    // needs to know the drivers for ride matching
    std::vector<std::shared_ptr<Driver>> drivers_;
};

class BasicMatcher: public RideMatcher
{
public:

    std::shared_ptr<Driver> match(const std::shared_ptr<Rider>& rider) override
    {
        for (auto& driver: drivers_)
        {
            if (dist(driver->getCurrLoc(), rider->getPickup()) <= max_dist_km_)
            {
                return driver;
            }
        }
        // no driver found
        return nullptr;
    }

private:
    float max_dist_km_ { 30 };
};


class RideManager
{
public:
    RideManager(std::unique_ptr<RideMatcher> matcher): matcher_(std::move(matcher)) {}

    void addRider(const std::shared_ptr<Rider> rider)
    {
        riders_.push_back(std::move(rider));
    }

    void addDriver(const std::shared_ptr<Driver> driver)
    {
        matcher_->addDriver(driver);
        drivers_.push_back(std::move(driver));
    }

    void createRide(const std::string& name)
    {
        std::shared_ptr<Rider> current_rider = nullptr;
        for (auto& rider: riders_)
        {
            if (rider->getName() == name)
            {
                current_rider = rider;
            }
        }

        if (current_rider == nullptr)
        {
            throw std::runtime_error("The rider doesn't exist");
        }

        Ride ride(current_rider);
        auto driver = matcher_->match(current_rider);
        ride.assignDriver(driver);
        rides_.push_back(ride);
    }
    // consider adding more methods of picking up passenger, and dropping off passenger
    // to actually enforce ride transitions

private:
    std::unique_ptr<RideMatcher> matcher_;
    std::vector<std::shared_ptr<Rider>> riders_;
    std::vector<std::shared_ptr<Driver>> drivers_;
    // TODO: make this dyanmic, shared pointer because rides can be used in other parts of the system
    std::vector<Ride> rides_; // track ride creation and handling
};