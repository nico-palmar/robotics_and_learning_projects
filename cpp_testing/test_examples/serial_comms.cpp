#include <optional>
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <memory>

class SerialInterface {
public:
    virtual void openPort(const std::string& port_name) = 0;
    virtual std::optional<std::string> readCommand() = 0;  // return a string if data available
    virtual void closePort() = 0;
};


class USBInterface: public SerialInterface
{
public:
    USBInterface(const std::vector<std::string>& valid_names): valid_names_(valid_names) {}

    void openPort(const std::string& port_name) override
    {
        // check if a valid port to open exists
        if (std::count(valid_names_.begin(), valid_names_.end(), port_name) <= 0)
        {
            throw std::runtime_error("Port not found!");
        }

        // check if already open
        if (open_)
        {
            throw std::logic_error("Port already open!");
        }

        open_ = true;
    }

    void closePort() override
    {
        if (!open_)
        {
            throw std::logic_error("Port closed already!");
        }

        open_ = false;
    }

    std::optional<std::string> readCommand() override
    {
        // simulate some data
        const std::string data = "yes";

        if (data.length() == 0)
        {
            return std::nullopt;
        }
        if (data.find('#') != std::string::npos)
        {
            throw std::invalid_argument("Malformed packet");
        }

        std::string ret = "MOVECMD:10";
        return ret;
    }


private:
    bool open_ { false };
    std::vector<std::string> valid_names_;
};

int main()
{
    const std::unique_ptr<SerialInterface> serial = std::make_unique<USBInterface>(std::vector<std::string> { "COM1", "/dev/ttyUSB0"});
    serial->openPort("/dev/ttyUSB0");
    auto cmd = serial->readCommand();
    if (cmd.has_value())
        std::cout << "Received: " << *cmd << "\n";
    else
        std::cout << "No command available\n";
    serial->closePort();

    // serial->openPort("/dev/ttyBAD"); // should throw runtime_error

    // general idea... low level libs should only catch to clean up it's state, then rethrow
    // mid level modules should catch more specific errors and translate that. Then retry/reset/enter degraded
    // finally, the top level application should catch (mostly everything), shutdown gracefully

    return 0;
}
