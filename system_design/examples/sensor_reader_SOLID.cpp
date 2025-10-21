// design a sensor reading application with SOLID principles
// sensor reads in data
// processor handles it with some operations
// logger displays the data 

// easily support new sensor methods
// easily swap real/simulated sensors
// note the pipeline: sensor -> processor -> logger

// design ideas:
// Have a sensor interface which can just read some data (single method). Should be extensible for diff sensors or even sim
// how to design the processor if we don't know the operations?
// thought: have a processor interface with is composed of a sensor. Should also be an interface with a process method (single method) returns generic sensor data. 
// then, implement a derived class which uses any sensor and does a specific process method
// finally the logger
// the logger needs to deal with a processor. It should interact with the processor interface.
// it should then have a logging interface that has a log method
// we can create a logging derived class, which can output the processed sensor data in certain fashion


#include <memory>
#include <vector>
#include <string>
#include <iostream>

class ISensorReader
{
public:
    ISensorReader(const std::string& name): name_(name){}
    virtual ~ISensorReader() = default; // virtual destructor for deletion of derived classes through base pointers
    virtual float read() = 0;

    std::string get_name() // add const correcteness
    {
        return name_;
    }


private:
    std::string name_;
};

class TemperatureSensor: public ISensorReader
{
public:
    TemperatureSensor(const std::string& name): ISensorReader(name) {}

    float read() override
    {
        // simulate reading from a temp sensor
        return 10.0;
    }
};

class ISensorProcessor
{
public:
    ISensorProcessor(std::unique_ptr<ISensorReader> sensor): sensor_(std::move(sensor))
    {
        // make the vector 100 element long
        recent_readings_.reserve(100);
    }

    virtual float process() = 0;

    float get_last()
    {
        return last_val_;
    }

    std::string get_sensor_name()
    {
        return sensor_->get_name();
    }

protected:
    std::unique_ptr<ISensorReader> sensor_;
    std::vector<float> recent_readings_;
    float last_val_ { 0.0 };
};

class GainProcessor: public ISensorProcessor
{
public:
    GainProcessor(std::unique_ptr<ISensorReader> sensor, const int gain): ISensorProcessor(std::move(sensor)), gain_(gain) {}

    float process() override
    {
        const auto sensor_reading = sensor_->read();
        last_val_ = sensor_reading * gain_;
        return last_val_;
    }
private:
    int gain_ { 2 };
};

class ILogger
{
public:
    ILogger(std::shared_ptr<ISensorProcessor> processor): processor_(processor) {}
    virtual void log() = 0;
protected:
    std::shared_ptr<ISensorProcessor> processor_;
};

class ConsoleLogger: public ILogger
{
public:
    ConsoleLogger(std::shared_ptr<ISensorProcessor> processor): ILogger(std::move(processor)) {}
    void log() override
    {
        std::cout << "The " << processor_->get_sensor_name() << " sensor last logged a value of " << processor_->get_last() << std::endl;
    }
};

int main()
{
    auto temp_sensor = std::make_unique<TemperatureSensor>("Temperature Sensor");
    const int gain = 2;
    auto temp_gain_processor = std::make_shared<GainProcessor>(std::move(temp_sensor), gain);
    auto logger = std::make_unique<ConsoleLogger>(temp_gain_processor);

    temp_gain_processor->process();
    logger->log();
}