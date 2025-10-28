
// design a stock notifying system
// key componets: interface for a user
// concrete users
// interface for a notifier
// concrete stock notifiers
// a facade/single controller which managers all stock notifiers
#include <string>
#include <memory>
#include <vector>

class User
{
public:
    virtual void handleUpdate(const std::string& symbol, const float& price) = 0;
    virtual ~User() = default;
};

class CasualUser: public User
{
public:
    void handleUpdate(const std::string& symbol, const float& price) override
    {
        // imiate a print happening here
    }
};

class Stock
{
public:
    Stock(const std::string& symbol): symbol_(symbol) {}
    void setPrice(const float& price);
    float getPrice() const;
    std::string getSymbol() const;
private:
    std::string symbol_;
    float price_;
};

class StockNotifier
{
public:
    StockNotifier(const std::string& symbol): stock_(symbol) {}
    virtual void attach(std::shared_ptr<User> user)
    {
        users_.push_back(user);
    }

    virtual void remove(std::shared_ptr<User> user) = 0;
    virtual void notify(const float& price)
    {
        if (price == stock_.getPrice())
        {
            return;
        }
        stock_.setPrice(price);
        for (auto it = users_.begin(); it != users_.end();)
        {
            if (it->expired())
            {
                it++;
                continue;
            }
            auto user = it->lock();
            user->handleUpdate(stock_.getSymbol(), stock_.getPrice());
            it++;
        }
    }

    std::string getStockSymbol()
    {
        return stock_.getSymbol();
    }

protected:
    // weak pointer to prevent cyclic references
    std::vector<std::weak_ptr<User>> users_;
    // an alternative would be to just have a dictionary in the StockManager
    // this way, we simplify the pattern a bit, we can create multiple notifier objects in the dictionary
    // for different stocks
    Stock stock_;
};

class VFVNotifier: public StockNotifier
{
public:
    using StockNotifier::StockNotifier;
    void remove(std::shared_ptr<User> user) override;
};

class StockManager
{
public:
    void updatePrice(const std::string& symbol, float new_price)
    {
        for (const auto& notifier: stock_notifiers_)
        {
            if (notifier->getStockSymbol() == symbol)
            {
                notifier->notify(new_price);
            }
        }
    }

    void subscribeUser(const std::string& symbol, std::shared_ptr<User>& user);
    // the mutex would happen here on the subscribers

private:
    std::vector<std::unique_ptr<StockNotifier>> stock_notifiers_;
    // include a mutex here to lock subscriptions for a user
    std::mutex mutex_;
};