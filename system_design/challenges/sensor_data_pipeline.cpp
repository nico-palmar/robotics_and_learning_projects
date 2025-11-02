
// have a sensor data pipeline for the different sensors
// have a base sensor interface, which had a base datatype for sensor data
// also has an error enum

// there should be some driver interface, which specifies how to read a packet
// then specalized drivers actually do the reading
// The driver as the lowest level lib will throw to propagate up

// finally, there will be one controller which puts everything together, and does error handling
#include <chrono>
#include <vector>
#include <expected>
#include <memory>
#include <string>
#include <exception>
#include <stdexcept>

enum class SensorError
{
    DataCorrupted,
    TimedOut,
    DataEmpty
};

struct Point3D
{
    float x;
    float y;
    float z;
};

struct State2D
{
    float x;
    float y;
    float yaw;
};

struct SensorPacket
{
    std::vector<uint8_t> data;
};

struct SensorData
{
    std::chrono::time_point<std::chrono::steady_clock> timestamp;
    virtual ~SensorData() = default;
};

class SensorDriver
{
public:
    virtual SensorPacket read() = 0;
};

class Sensor
{
public:
    virtual std::expected<std::unique_ptr<SensorData>, SensorError> getData() = 0;
    virtual ~Sensor() = default;

    virtual SensorData packetToData(const SensorPacket& packet);

    virtual void accumulateState(State2D& state) = 0;

    virtual void start() const = 0;
    virtual void stop() const = 0;
protected:
    std::unique_ptr<SensorDriver> driver_;
};

struct LidarData: public SensorData
{
    std::vector<Point3D> scan;
};

class LidarDriver: public SensorDriver
{
public:
    SensorPacket read() override
    {
        std::string data = "";

        if (data.length() == 0)
        {
            return SensorPacket();
        }

        if (data.find("#") != std::string::npos)
        {
            throw std::runtime_error("Corrupted driver!");
        }

        auto packet = SensorPacket();
        packet.data.push_back(5);
        return packet;
    }
};

class Lidar: public Sensor
{
public:
    // initialize sensor driver
    Lidar();

    std::expected<std::unique_ptr<SensorData>, SensorError> getData() override
    {
        auto packet = driver_->read();
        *data_ = packetToData(packet);
        auto sim_timeout = false;
        
        if (data_->scan.empty())
        {
            return std::unexpected(SensorError::DataEmpty);
        }
        if (sim_timeout)
        {
            return std::unexpected(SensorError::TimedOut);
        }

        return data_;
    }

    void start() const override;

    virtual void stop() const override;

private:
    std::unique_ptr<LidarData> data_;
};


class StateEstimator2D
{
public:
    void addSensor(const std::shared_ptr<Sensor> sensor)
    {
        sensors_.push_back(std::move(sensor));
    }

    void updateState()
    {
        try
        {
            for (const auto& sensor: sensors_)
            {
                auto res = sensor->getData();
                if (!res)
                {
                    switch (res.error())
                    {
                        case SensorError::DataCorrupted:
                            break;
                        // add more cases
                        default:
                            break;
                    }
                }
                sensor->accumulateState(state_);
            }
        }
        catch(const std::runtime_error& e)
        {
            // should catch these excpetions earlier on potentially? No need at the controller level
            // driver error, handle it
            for (const auto& sensor: sensors_)
            {
                sensor->stop();
            }
            std::cerr << e.what() << '\n';
        }
        
    }

private:
    State2D state_;
    std::vector<std::shared_ptr<Sensor>> sensors_;
};