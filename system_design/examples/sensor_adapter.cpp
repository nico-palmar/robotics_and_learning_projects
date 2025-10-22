#include <memory>
#include <iostream>

class RangeSensor
{
public:
    virtual float getDistanceMeters() const = 0;
    virtual ~RangeSensor() = default;
};

class UltrasonicSensorLegacy
{
public:
    UltrasonicSensorLegacy(const int& dummy_dist): dummy_read_dist_(dummy_dist) {}
    int readDistanceCM() const
    {
        // simulate reading from a legacy snesor
        return dummy_read_dist_;
    }
private:
    int dummy_read_dist_ { 10 };
};

// create an adapter class
// will be a range sensor
// and provide composition with an ultrasonic sensor
class UltrasonicSensorAdapter: public RangeSensor
{
public:
    UltrasonicSensorAdapter(std::shared_ptr<UltrasonicSensorLegacy> sensor): ultrasonic_sensor_(sensor) {}

    float getDistanceMeters() const override
    {
        const int distance_cm = ultrasonic_sensor_->readDistanceCM();
        const float distance_m = static_cast<float>(distance_cm) / 100.0f;
        return distance_m;
    }
private:
    const std::shared_ptr<UltrasonicSensorLegacy> ultrasonic_sensor_;
};

int main()
{
    // client code want to use a range sensor
    auto ultrasonic_sensor = std::make_shared<UltrasonicSensorLegacy>(94);
    auto range_sensor = std::make_unique<UltrasonicSensorAdapter>(ultrasonic_sensor);

    // client code can now use the range sensor
    const auto close_distance_m = 0.95f;
    if (range_sensor->getDistanceMeters() > close_distance_m)
    {
        std::cout << "The sensor reading is far" << std::endl;
    }
    else
    {
        std::cout << "The sensor reading is CLOSE!" << std::endl;
    }
}