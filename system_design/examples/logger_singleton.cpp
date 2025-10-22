
// create a single logger
// thread safe, globally accessible

// the logger needs to have a mutex on it's write operation (one writer at a time)
// no exposed constructor
// exposed static getter method

#include <iostream>
#include <mutex>

class Logger
{
public:
    static Logger& getInstance()
    {
        // statically create a single instance of a logger
        static Logger logger;
        return logger;
    }

    void log(const int robot_id) const
    {
        // aquire lock so that only single robot can log at a time
        std::lock_guard<std::mutex> lock(mutex_);
        std::cout << "[" << robot_id << "]" << " message coming from robot" << std::endl;
    }

    // delete all copy methods
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

private:
    Logger() = default;
    mutable std::mutex mutex_;
};

int main()
{
    Logger::getInstance().log(5);
    Logger::getInstance().log(7);
}