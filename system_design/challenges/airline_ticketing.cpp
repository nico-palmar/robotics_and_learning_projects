
// design an airline ticketing system
// assume there is some existing passenger class, so we don't have to write that (to get details on passenger)

// then, we must design:
// Flight: departing/arrival, array of seats (with unique IDs), unique ID
// FlightManager -> can follow a strategy pattern for finding flights
// FlightFinder interface, with a find flights method
// single implementation, which flight manager owns + uses
// Booking
#include <vector>
#include <string>
#include <memory>

struct Seat
{
    int id;
    bool occupied;
};

class Flight
{
public:
    Flight(const std::vector<std::shared_ptr<Seat>>& seats, const std::string& source_, const std::string& dest, const std::string& flight_id, const int& flight_day);

    std::pair<std::string, std::string> getRoute() const;

    int getDay() const;

    std::vector<std::shared_ptr<Seat>> getFreeSeats() const
    {
        std::vector<std::shared_ptr<Seat>> free_seats_;
        free_seats_.reserve(seats_.size() / 2);
        for (const auto& seat: seats_)
        {
            if (!seat->occupied)
            {
                free_seats_.push_back(seat);
            }
        }
        return free_seats_;
    }

private:
    // will share seats with bookings!
    std::vector<std::shared_ptr<Seat>> seats_;
    std::string source_;
    std::string dest_;
    std::string flight_id_;
    int flight_day_;
};

class FlightFinder
{
public:
    FlightFinder(const std::vector<std::shared_ptr<Flight>>& flights);

    virtual std::vector<std::shared_ptr<Flight>> findFlights(const std::pair<std::string, std::string>& route, const int& day) const = 0;
protected:
    std::vector<std::shared_ptr<Flight>> flights_;
};

class BasicFinder: public FlightFinder
{
public:
    using FlightFinder::FlightFinder;
    std::vector<std::shared_ptr<Flight>> findFlights(const std::pair<std::string, std::string>& route, const int& day) const override
    {
        std::vector<std::shared_ptr<Flight>> flights;
        for (const auto& flight: flights_)
        {
            if (flight->getRoute() == route && flight->getDay() == day)
            {
                flights.push_back(flight);
            }
        }
        return flights;
    }
};

class FlightBooking
{
public:
    // constructor is pretty much a book flight method
    FlightBooking(const std::shared_ptr<Passenger>& passenger, const std::shared_ptr<Flight>& flight, const std::shared_ptr<Seat>& seat);

    void cancelBooking()
    {
        cancelled_ = true;
        seat_->occupied = false;
    }

private:
    std::shared_ptr<Passenger> passenger_;
    std::shared_ptr<Flight> flight_;
    std::shared_ptr<Seat> seat_;
    bool cancelled_ { false };
};

class FlightManager
{
public:
    FlightManager(const std::vector<std::shared_ptr<FlightBooking>>& bookings_, std::unique_ptr<FlightFinder> flight_finder_);

    std::vector<std::shared_ptr<Flight>> findFlights(const std::pair<std::string, std::string>& route, const int& day) const;

    void creatBooking(std::shared_ptr<Passenger> passenger, std::shared_ptr<Flight> flight, std::shared_ptr<Seat> seat)
    {
        auto booking = std::make_shared<FlightBooking>(passenger, flight, seat);
        bookings_.push_back(booking);
    }

    void cancelBooking(std::shared_ptr<FlightBooking> booking)
    {
        booking->cancelBooking();
    }

private:
    std::vector<std::shared_ptr<FlightBooking>> bookings_;
    std::unique_ptr<FlightFinder> flight_finder_;
};