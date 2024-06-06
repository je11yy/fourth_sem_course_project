#ifndef COURSE_PROJECT_SHOP_H
#define COURSE_PROJECT_SHOP_H

#include <string>

class customer
{ 

private:

    class wallet
    {
    
    public:

        enum currency_type
        {
            rubles,
            dollars,
            euro
        };

    private:

        uint _money;

        currency_type _type;

    public:

        uint get_money() const noexcept
        {
            return _money;
        }

        std::string get_currency() const noexcept
        {
            switch (_type)
            {
                case rubles:
                    return "rubles";
                case dollars:
                    return "dollars";
                case euro:
                    return "euro";
            }
        }
    
    public:

        wallet(uint money, currency_type type = rubles) : _money(money), _type(type) {}

        ~wallet() {}

        wallet(wallet const & other) : _money(other._money), _type(other._type) {}

        wallet &operator=(wallet const & other)
        {
            if (this == &other) return *this;
            _money = other._money;
            _type = other._type;
            return *this;
        }

        wallet(wallet && other) noexcept  : _money(std::move(other._money)), _type(std::move(other._type)) {}

        wallet &operator=(wallet && other) noexcept 
        {
            if (this == &other) return *this;
            _money = std::move(other._money);
            _type = std::move(other._type);
            return *this;
        }
    };

private:

    uint _id;

    std::string _name;

    wallet _money;

public:

    uint get_id() const noexcept
    {
        return _id;
    }

    std::string get_name() const noexcept
    {
        return _name;
    }

    uint get_money() const noexcept
    {
        return _money.get_money();
    }

    std::string get_currency() const noexcept
    {
        return _money.get_currency();
    }
    
public:

    customer(uint const & id, std::string const & name, uint const & money = 0, wallet::currency_type type = wallet::currency_type::rubles):
        _money(money, type), _name(name), _id(id)
    {}

    ~customer() {}

    customer(customer const & other) : _name(other._name), _money(other._money), _id(other._id) {}

    customer &operator=(customer const & other)
    {
        if (this == &other) return *this;
        _name = other._name;
        _money = other._money;
        _id = other._id;
        return *this;
    }

    customer(customer && other) noexcept  : _name(std::move(other._name)), _money(std::move(other._money)), _id(other._id) {}

    customer &operator=(customer && other) noexcept
    {
        if (this == &other) return *this;
        _name = std::move(other._name);
        _money = std::move(other._money);
        _id = other._id;
        return *this;
    }

};

#endif