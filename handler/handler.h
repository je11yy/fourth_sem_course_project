#ifndef COURSE_PROJECT_HADNLER_H
#define COURSE_PROJECT_HADNLER_H

#include "../containers/database/database.h"

#include <string>
#include <fstream>
#include <iostream>
#include <boost/filesystem.hpp>
#include <ctime>

#include "../editions/validation/validation.h"
#include "../dialog/dialog.h"
#include "../chain_of_responsibility/handler.h"

namespace program
{

    void program_in_memory_cache(std::string const & file_name, database::key_type type, bool dialog);

    void program_in_file_system(std::string const & file_name, database::key_type type, bool dialog);

    void program(std::string const & file_name, database::mode const & mode, database::key_type type, bool dialog);

    namespace comparison
    {

        class int_comparer final
        {

        public:

            int operator()(
                uint const &left,
                uint const &right) const noexcept
            {
                return left - right;
            }

        };

        class stdstring_comparer final
        {

        public:

            int operator()(
                std::string const &first,
                std::string const &second) const noexcept
            {
                if (first == second) return 0;
                if (first > second) return 1;
                return -1;
            }

        };

        class ac_kvp_int_stdstring_comparer final
        {

        public:

            int operator()(
                typename associative_container<int, std::string>::key_value_pair const &first,
                typename associative_container<int, std::string>::key_value_pair const &second)
            {
                auto keys_comparison_result = int_comparer()(first.key, second.key);
                if (keys_comparison_result != 0) return keys_comparison_result;
                return stdstring_comparer()(first.value, second.value);
            }

        };

    };

    namespace handler
    {

        template<typename tvalue>
        class handler final
        {

        private:

            database::database<std::string, std::string, tvalue> _database;
        
        protected:

            chain_of_responsibility::request_with_command_chain _chain;

            database::mode _mode;
            
            database::key_type _type = database::key_type::ID;

        public:

            void set_key_type(database::key_type const & type)
            {
                _type = type;
            }

            database::key_type const &get_key_type()
            {
                return _type;
            }

            void set_mode(database::mode const & mode)
            {
                _mode = mode;
            }

        public:

            explicit handler(
                size_t t,
                database::key_type the_key_type = database::key_type::ID,
                database::mode mode = database::mode::IN_MEMORY_CACHE,
                std::uint64_t add_value_time = 0,
                std::uint64_t delete_value_time = 0,
                std::uint64_t read_value_time = 0,
                std::uint64_t update_value_time = 0,
                std::uint64_t read_between_value_time = 0,
                allocator * allocator = nullptr,
                cntr::allocator_type type = allocator_type::Boundary_Tags,
                logger * logger = nullptr) : 
                _database(t, the_key_type, comparison::stdstring_comparer(), comparison::stdstring_comparer(), allocator, type, logger),
                _mode(mode),
                _type(the_key_type)
            {
                _chain
                .add_handler(new command_pattern::add_pool(&_database))
                .add_handler(new command_pattern::add_scheme(&_database))
                .add_handler(new command_pattern::add_collection(&_database))
                .add_handler(new command_pattern::add_value(&_database, add_value_time))
                .add_handler(new command_pattern::update_value(&_database, update_value_time))
                .add_handler(new command_pattern::read_value(&_database))
                .add_handler(new command_pattern::read_between_value(&_database, read_between_value_time))
                .add_handler(new command_pattern::delete_value(&_database, delete_value_time))
                .add_handler(new command_pattern::delete_collection(&_database))
                .add_handler(new command_pattern::delete_scheme(&_database))
                .add_handler(new command_pattern::delete_pool(&_database));
            }

            handler(handler const & other) = default;

            handler &operator=(handler const & other) = default;

            handler(handler && other) noexcept = default;

            handler &operator=(handler && other) noexcept = default;

            ~handler()
            {
                _chain.~request_with_command_chain();
            }
        public:

            void handling_file(std::string file_name)
            {
                std::ifstream file(file_name);
                if (!file.is_open()) throw std::logic_error("Wrong input file\n");
                std::string command_string;
                while (true)
                {
                    if (!std::getline(file, command_string)) break;
                    if (command_string[command_string.length() - 1] == '\n') command_string.erase(command_string.length() - 1);
                    if (command_string.empty()) continue;

                    std::int64_t current_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

                    while (!_chain.handle(command_string, current_time))
                    {
                        current_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                    }
                }
            }

            void handling_dialog()
            {
                std::string command_string;
                std::cout << dialog::start;
                dialog::interactive_dialog();

                while (true)
                {
                    std::cin.sync();
                    std::cout << "INPUT THE COMMAND >> ";
                    if (!std::getline(std::cin, command_string)) break;
                    if (command_string.empty()) if (!std::getline(std::cin, command_string)) break;

                    bool is_insert = false;
                    
                    while (command_string == "INSERTFILE")
                    {
                        is_insert = true;
                        try
                        {
                            handling_file(dialog::parse_file());
                        }
                        catch(const std::logic_error& e)
                        {
                            std::cerr << e.what() << '\n';
                            continue;
                        }
                        break;
                        
                    }
                    if (command_string == "QUIT") break;
                    if (is_insert) continue;

                    bool is_command = false;

                    if (command_string[command_string.length() - 1] == '\n') command_string.erase(command_string.length() - 1);

                    for (auto & item : dialog::dialog_commands)
                    {
                        if (item == command_string)
                        {
                            is_command = true;
                            break;
                        }
                    } 

                    if (!is_command)
                    {
                        std::cout << "Wrong command\n";
                        continue;
                    }

                    std::int64_t current_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                    
                    while (!_chain.handle(command_string, current_time))
                    {
                        current_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                    }
                }
            }

        };
    };
        

    void program_in_memory_cache(std::string const & file_name, database::key_type type, bool dialog)
    {
        handler::handler<customer> * _handler = new handler::handler<customer>(3, type, database::mode::IN_MEMORY_CACHE);

        if (dialog)
        {
            _handler->handling_dialog();
            delete _handler;
            return;
        }
        
        _handler->handling_file(file_name);
        
        delete _handler;
    }

    void program_in_file_system(std::string const & file_name, database::key_type type, bool dialog)
    {
        handler::handler<customer> * _handler = new handler::handler<customer>(3, type, database::mode::FILE_SYSTEM);

        // создаем папочку, где все будет храниться
        boost::filesystem::create_directories("database"); 

        if (dialog)
        {
            _handler->handling_dialog();
            delete _handler;
            return;
        }

        _handler->handling_file(file_name);
        
        delete _handler;
    }

    void program(std::string const & file_name, database::mode const & mode, database::key_type type, bool dialog)
    {
        switch (mode)
        {
        case database::mode::IN_MEMORY_CACHE:
            program_in_memory_cache(file_name, type, dialog);
            break;

        case database::mode::FILE_SYSTEM:
            program_in_file_system(file_name, type, dialog);
            break;
        
        default:
            break;
        }
    }

};

#endif