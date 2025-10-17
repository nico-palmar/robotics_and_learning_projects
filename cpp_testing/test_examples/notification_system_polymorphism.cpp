#include <string>
#include <iostream>
#include <vector>
#include <memory>

class Notification
{
public:
    Notification(const std::string& data): message_{data} {}
    virtual void send() const = 0;

protected:
    std::string getMessage() const
    {
        return message_;
    }

private:
    std::string message_;
};

class EmailNotification: public Notification
{
public:
    EmailNotification(const std::string& message): Notification(message) {}

    void send() const override final
    {
        std::cout << "Sending over notif by email..." << std::endl;
        std::cout << getMessage() << std::endl;
    }
};

class SMSNotification: public Notification
{
public:
    SMSNotification(const std::string& message): Notification(message) {}

    void send() const override final
    {
        std::cout << "Sending over notif by SMS..." << std::endl;
        std::cout << getMessage() << std::endl;
    }
};

class PushNotification: public Notification
{
public:

    PushNotification(const std::string& message): Notification(message) {}

    void send() const override final
    {
        std::cout << "Sending over notif by Push..." << std::endl;
        std::cout << getMessage() << std::endl;
    }
};

int main()
{
    std::vector<std::unique_ptr<Notification>> notifications;

    // same as push back... no performance diff. Both just move the pointer
    notifications.emplace_back(std::make_unique<EmailNotification>("Hello Nico,"));
    notifications.emplace_back(std::make_unique<SMSNotification>("AMBER ALERT"));
    notifications.emplace_back(std::make_unique<PushNotification>("Bank accout push"));

    for (const auto& notif: notifications)
    {
        // all notifications sent using a common interface
        notif->send();
    }
}