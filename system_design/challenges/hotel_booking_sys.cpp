
// high level classes
// room
// hotel
// booking
// booking system (consider hotel manager and booking manager separately)
#include <map>
#include <vector>
#include <utility>
#include <memory>
#include <string>

class Room
{
public:
    Room(const float& price, const std::size_t& people, const int& id)
    {
        price_ = price;
        n_people_ = people;
        id_ = id;
    }

    // return availability internvals in C++
    std::vector<std::pair<int, int>> getAvailability() const
    {
        std::vector<std::pair<int, int>> available;
        int start_free { - 1 };
        int end_free;
        // iterate through entire range in map
        for (auto it = availability_.begin(); it != availability_.end(); it++)
        {
            if (it->second)
            {
                // free set the starting free day
                if (start_free == -1)
                {
                    start_free = it->first;
                }
            }
            else
            {
                // not free
                if (start_free != -1)
                {
                    end_free = it->first;
                    available.emplace_back(std::make_pair(start_free, end_free));
                    start_free = -1;
                }
            }
        }
        return available;
    }

    void setAvailability(std::pair<int, int> range)
    {
        for (auto it = availability_.lower_bound(range.first); it != availability_.upper_bound(range.second); it++)
        {
            it->second = false;
        }
    }

private:
    float price_;
    // maps, in temporal order, day as an index (to keep it simple) -> availability
    // this is probably better as a vector, but that's fine for now
    // vector would just have idx as days
    std::map<int, bool> availability_;
    std::size_t n_people_;
    int id_;
};

class Hotel
{
public:
    void addRoom(std::shared_ptr<Room> room)
    {
        rooms_.push_back(std::move(room));
    }

    // find rooms in a hotel free between certain ranges
    std::vector<std::shared_ptr<Room>> findAvailableRooms(std::pair<int, int> range) const
    {
        std::vector<std::shared_ptr<Room>> rooms;
        for (auto& room: rooms_)
        {
            auto availability = room->getAvailability();
            // write an algorithm to find availability, assume they match up
            for (const auto& single_range: availability)
            {
                if (single_range.first <= range.first && single_range.second >= range.second)
                {
                    rooms.push_back(room);
                }
            }
        }
        return rooms;
    }

private:
    std::vector<std::shared_ptr<Room>> rooms_;
};

// could use a command pattern here to handle reverting
// would have a concrete booking (as is here); can book or cancel
// then an interface to a booking command, which can execute
// implementations for booking command and cancelling command
// the booking system would have a vector of commands, and be able to handle reverting... won't implement right now
class Booking
{
public:
    Booking(const int& start, const int& end, const std::string& name);

    void book(std::shared_ptr<Room> room)
    {
        room_ = std::move(room);
        room_->setAvailability(std::make_pair(start_day_, end_day_));
    }

private:
    std::shared_ptr<Room> room_;
    int start_day_;
    int end_day_;
    std::string name_; // an identifier to the person
};

// make a single booking system for simplicity
// the booking system could use a facade
// the booking system delegates all logic to booking and hotels
class BookingSystem
{
public:
    void addHotel(std::shared_ptr<Hotel> hotel);

    // find rooms in some range
    // could use strategy pattern here to abstract the search algorithm
    std::vector<std::shared_ptr<Room>> findAvailableRooms(std::pair<int, int> range) const;
    // this should find rooms ina hotel, and copy them into a vector, returning the vector
    // can use .insert to insert at the end of the result vector the entire hotel room vector

    // selects a room from a hotel and makes a booking
    void book();
private:
    std::vector<std::shared_ptr<Hotel>> hotels_;
    std::vector<std::unique_ptr<Booking>> bookings_;
};