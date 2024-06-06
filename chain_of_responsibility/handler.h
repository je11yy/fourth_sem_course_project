#ifndef COURSE_PROJECT_CHAIN_OF_RESPONIBILITY_H
#define COURSE_PROJECT_CHAIN_OF_RESPONIBILITY_H

#include "../command/command.h"
#include <ctime>

namespace chain_of_responsibility
{
    class handler
    {

    public:

        handler * _next_handler;

    public:

        handler() : _next_handler(nullptr)
        {}

        virtual ~handler() = default;

    public:

        virtual bool handle(std::string const &request, std::int64_t data_and_time) = 0;
    };

    class request_with_command : public handler
    {

    private:

        command_pattern::command * _target_action;

    public:

        explicit request_with_command(command_pattern::command * target_action) :
                _target_action(target_action)
        {}

        ~request_with_command() override
        {
            delete _target_action;
        }

        request_with_command(request_with_command const& other) = delete;

        request_with_command(request_with_command&& other) = delete;

        request_with_command& operator=(request_with_command const& other) = delete;

        request_with_command& operator=(request_with_command const&& other) = delete;

    public:

        bool handle(std::string const& request, std::int64_t current_time) final
        {
            if (current_time < _target_action->get_time_and_date()) return false;

            if (!_target_action->can_execute(request))
            {
                if (!this->_next_handler) return false;
                return this->_next_handler->handle(request, current_time);
            }

            _target_action->execute(request);

            return true;
        }

    // public:

        
    //     void write_time_to_file(std::string const& request) const
    //     {
    //         std::string first_word;
    //         std::istringstream iss(request);
    //         iss >> first_word;

    //         std::time_t time = _date_time_activity_started / 1000;
    //         std::tm* tm_info = std::localtime(&time);
    //         char buffer[80];
    //         std::strftime(buffer, 80, "[%Y-%m-%d %H:%M:%S]", tm_info);
    //         std::string formatted_time(buffer);

    //         std::cout << first_word << std::endl;

    //         std::ofstream file("time_log.txt", std::ios::app);

    //         if (first_word == "ADD" || first_word == "UPDATE" || first_word == "READVALUE" ||
    //             first_word == "READBETWEEN")
    //         {
    //             file << formatted_time << " " << request << std::endl;
    //         }

    //         file.close();
    //     }
    };

    class request_with_command_chain
    {

    private:

        handler *_first_handler;
        handler *_last_handler;

    public:

        request_with_command_chain() :
                _first_handler(nullptr), _last_handler(nullptr)
        {}

        ~request_with_command_chain()
        {
            while (_first_handler)
            {
                _last_handler = _first_handler;
                _first_handler = _first_handler->_next_handler;
                delete _last_handler;
            }
        }

        request_with_command_chain(request_with_command_chain const & other) = delete;

        request_with_command_chain(request_with_command_chain && other) = delete;

        request_with_command_chain& operator=(request_with_command_chain const & other) = delete;

        request_with_command_chain& operator=(request_with_command_chain && other) = delete;

    public:

        [[nodiscard]] bool handle(std::string const& request, std::int64_t date_time_target) const
        {
            if (!_first_handler) return false;
            
            return _first_handler->handle(request, date_time_target);
        }

    public:

        request_with_command_chain& add_handler(command_pattern::command * command)
        {
            _last_handler = !_first_handler
                            ? _first_handler = new request_with_command(command)
                            : _last_handler->_next_handler = new request_with_command(command);

            return *this;
        }
    };
};

#endif