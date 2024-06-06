#ifndef COURSE_PROJECT_COMMAND_H
#define COURSE_PROJECT_COMMAND_H

#include <string>
#include <iostream>
#include <sstream>

#include "../containers/database/database.h"
#include "../containers/customer/customer.h"
#include "../editions/validation/validation.h"
#include "boost/filesystem.hpp"

namespace command_pattern
{
    customer read_customer_from_file(std::string file_name)
    {
        std::ifstream file;
        file.open(file_name);
        std::string name;
        uint money, id;
        file >> id >> name >> money;
        return customer(id, name, money);
    }

    class command
    {

    protected:
        
        database::database<std::string, std::string, customer> * _database;

        std::uint64_t _time_activity_should_be_finished;

    public:

        [[nodiscard]] std::int64_t get_time_and_date() const
        {
            return _time_activity_should_be_finished;
        }
    
    public:

        // передаем указатель на объект, который содержит саму базу данных (он уже должен быть создан)
        command(database::database<std::string, std::string, customer> * database, std::uint64_t const & _time = 0)
        {
            _time_activity_should_be_finished = _time;
            _database = database;
        }

    private:

        void clear()
        {
            _database = nullptr;
        }

    public:

        ~command()
        {
            clear();
        }

        command(command const & other)
        {
            clear();
            _database = other._database;
        }

        command &operator=(command const & other)
        {
            if (this != &other)
            {
                clear();
                _database = other._database;
            }
            return *this;
        }

        command(command && other) noexcept
        {
            clear();
            _database = other._database;
            other.clear();
        }

        command &operator=(command && other)
        {
            if (this != &other)
            {
                clear();
                _database = other._database;
                other.clear();
            }
            return *this;
        }

    public:

        virtual bool can_execute(std::string const&, std::uint64_t const & current_time = 0) = 0;

        virtual void execute(std::string const&, std::uint64_t const & current_time = 0) = 0;
    };

    class add_pool final:
        public command
    {

    private:

        std::string _pool_name;

    public:

        add_pool(database::database<std::string, std::string, customer> * database) : command(database)
        {}

    public:

        bool can_execute(std::string const & request, std::uint64_t const & current_time = 0) override
        {
            std::istringstream string_with_commands(request);
            std::string command;
            string_with_commands >> command;

            if (command == "ADDPOOL")
            {
                std::string pool_name;
                string_with_commands >> pool_name;
                if (!validation::is_name(pool_name)) return false;
                _pool_name = pool_name;
                return true;
            }
            return false;
        }

        void execute(std::string const& request, std::uint64_t const & current_time = 0)  override
        {
            if (_database->get_mode() == database::mode::IN_MEMORY_CACHE)
            {
                _database->add_pool(_pool_name);
                return;
            }

            std::string new_dir = "database/" + _pool_name;
            boost::filesystem::path path(new_dir);
            if (boost::filesystem::exists(path))
            {
                std::string error = "Pool with name - " + _pool_name + " already exists\n";
                throw std::logic_error(error);
            }

            boost::filesystem::create_directories(new_dir);
        }
    };

    class delete_pool final:
        public command
    {

    private:

        std::string _pool_name;

    public:

        delete_pool(database::database<std::string, std::string, customer> * database) : command(database)
        {}

    public:

        bool can_execute(std::string const & request, std::uint64_t const & current_time = 0)  override
        {
            std::istringstream string_with_commands(request);
            std::string command;
            string_with_commands >> command;

            if (command == "DELETEPOOL")
            {
                std::string pool_name;
                string_with_commands >> pool_name;
                if (!validation::is_name(pool_name)) return false;
                _pool_name = pool_name;
                return true;
            }
            return false;
        }

        void execute(std::string const& request, std::uint64_t const & current_time = 0)  override
        {
            if (_database->get_mode() == database::mode::IN_MEMORY_CACHE)
            {
                _database->delete_pool(_pool_name);
                return;
            }

            std::string pool_str = "database/" + _pool_name;
            boost::filesystem::path pool_path(pool_str);
            if (!boost::filesystem::exists(pool_path))
            {
                std::string error = "Pool with name - " + _pool_name + " doesn't exist\n";
                throw std::logic_error(error);
            }

            boost::filesystem::remove_all(pool_str);
        }
    };


    class add_scheme final:
        public command
    {

    private:

        std::string _pool_name;
        
        std::string _scheme_name;

    public:

        add_scheme(database::database<std::string, std::string, customer> * database) : command(database)
        {}

    public:

        bool can_execute(std::string const & request, std::uint64_t const & current_time = 0)  override
        {
            std::istringstream string_with_commands(request);
            std::string command;
            string_with_commands >> command;

            if (command == "ADDSCHEME")
            {
                std::string pool_name;
                std::string scheme_name;
                string_with_commands >> pool_name >> scheme_name;
                if (!validation::is_name(pool_name) || !validation::is_name(scheme_name)) return false;
                _pool_name = pool_name;
                _scheme_name = scheme_name;
                return true;
            }
            return false;
        }

        void execute(std::string const& request, std::uint64_t const & current_time = 0)  override
        {
            if (_database->get_mode() == database::mode::IN_MEMORY_CACHE)
            {
                _database->add_scheme(_pool_name, _scheme_name);
                return;
            }

            std::string pool_path_str = "database/" + _pool_name;
            boost::filesystem::path pool_path(pool_path_str);
            if (!boost::filesystem::exists(pool_path))
            {
                std::string error = "Pool with name - " + _pool_name + " doesn't exist\n";
                throw std::logic_error(error);
            }

            std::string scheme_path_str = pool_path_str + "/" + _scheme_name;
            boost::filesystem::path scheme_path(scheme_path_str);
            if (boost::filesystem::exists(scheme_path))
            {
                std::string error = "Scheme with name - " + _pool_name + " " + _scheme_name + " already exists\n";
                throw std::logic_error(error);
            }

            boost::filesystem::create_directories(scheme_path_str);
        }
    };

    class delete_scheme final:
        public command
    {

    private:

        std::string _pool_name;
        
        std::string _scheme_name;

    public:

        delete_scheme(database::database<std::string, std::string, customer> * database) : command(database)
        {}

    public:

        bool can_execute(std::string const & request, std::uint64_t const & current_time = 0)  override
        {
            std::istringstream string_with_commands(request);
            std::string command;
            string_with_commands >> command;

            if (command == "DELETESCHEME")
            {
                std::string pool_name;
                std::string scheme_name;
                string_with_commands >> pool_name >> scheme_name;
                if (!validation::is_name(pool_name) || !validation::is_name(scheme_name)) return false;
                _pool_name = pool_name;
                _scheme_name = scheme_name;
                return true;
            }
            return false;
        }

        void execute(std::string const& request, std::uint64_t const & current_time = 0)  override
        {
            if (_database->get_mode() == database::mode::IN_MEMORY_CACHE)
            {
                _database->delete_scheme(_pool_name, _scheme_name);
                return;
            }

            std::string pool_path_str = "database/" + _pool_name;
            boost::filesystem::path pool_path(pool_path_str);
            if (!boost::filesystem::exists(pool_path))
            {
                std::string error = "Pool with name - " + _pool_name + " doesn't exist\n";
                throw std::logic_error(error);
            }

            std::string scheme_path_str = pool_path_str + "/" + _scheme_name;
            boost::filesystem::path scheme_path(scheme_path_str);
            if (~boost::filesystem::exists(scheme_path))
            {
                std::string error = "Scheme with name - " + _pool_name + " " + _scheme_name + " doesn't exist\n";
                throw std::logic_error(error);
            }

            boost::filesystem::remove_all(scheme_path_str);
        }
    };

    class add_collection final:
        public command
    {

    private:

        std::string _pool_name;
        
        std::string _scheme_name;

        std::string _collection_name;

    public:

        add_collection(database::database<std::string, std::string, customer> * database) : command(database)
        {}

    public:

        bool can_execute(std::string const & request, std::uint64_t const & current_time = 0)  override
        {
            std::istringstream string_with_commands(request);
            std::string command;
            string_with_commands >> command;

            if (command == "ADDCOLLECTION")
            {
                std::string pool_name;
                std::string scheme_name;
                std::string collection_name;
                string_with_commands >> pool_name >> scheme_name >> collection_name;
                if (!validation::is_name(pool_name) || !validation::is_name(scheme_name) || !validation::is_name(collection_name)) return false;
                _pool_name = pool_name;
                _scheme_name = scheme_name;
                _collection_name = collection_name;
                return true;
            }
            return false;
        }

        void execute(std::string const& request, std::uint64_t const & current_time = 0)  override
        {
            if (_database->get_mode() == database::mode::IN_MEMORY_CACHE)
            {
                _database->add_collection(_pool_name, _scheme_name, _collection_name);
                return;
            }
            
            std::string pool_path_str = "database/" + _pool_name;
            boost::filesystem::path pool_path(pool_path_str);
            if (!boost::filesystem::exists(pool_path))
            {
                std::string error = "Pool with name - " + _pool_name + " doesn't exist\n";
                throw std::logic_error(error);
            }

            std::string scheme_path_str = pool_path_str + "/" + _scheme_name;
            boost::filesystem::path scheme_path(scheme_path_str);
            if (!boost::filesystem::exists(scheme_path))
            {
                std::string error = "Scheme with name - " + _pool_name + " " + _scheme_name + " doesn't exist\n";
                throw std::logic_error(error);
            }

            std::string collection_path_str = scheme_path_str + "/" + _collection_name;
            boost::filesystem::path collection_path(collection_path_str);
            if (boost::filesystem::exists(collection_path))
            {
                std::string error = "Collection with name - " + _pool_name + " " + _scheme_name + " " + _collection_name + " already exist\n";
                throw std::logic_error(error);
            }

            boost::filesystem::create_directories(collection_path_str);
        }
    };

    class delete_collection final:
        public command
    {

    private:

        std::string _pool_name;
        
        std::string _scheme_name;

        std::string _collection_name;

    public:

        delete_collection(database::database<std::string, std::string, customer> * database) : command(database)
        {}

    public:

        bool can_execute(std::string const & request, std::uint64_t const & current_time = 0)  override
        {
            std::istringstream string_with_commands(request);
            std::string command;
            string_with_commands >> command;

            if (command == "DELETECOLLECTION")
            {
                std::string pool_name;
                std::string scheme_name;
                std::string collection_name;
                string_with_commands >> pool_name >> scheme_name >> collection_name;
                if (!validation::is_name(pool_name) || !validation::is_name(scheme_name) || !validation::is_name(collection_name)) return false;
                _pool_name = pool_name;
                _scheme_name = scheme_name;
                _collection_name = collection_name;
                return true;
            }
            return false;
        }

        void execute(std::string const& request, std::uint64_t const & current_time = 0)  override
        {
            if (_database->get_mode() == database::mode::IN_MEMORY_CACHE)
            {
                _database->delete_collection(_pool_name, _scheme_name, _collection_name);
                return;
            }
            
            std::string pool_path_str = "database/" + _pool_name;
            boost::filesystem::path pool_path(pool_path_str);
            if (!boost::filesystem::exists(pool_path))
            {
                std::string error = "Pool with name - " + _pool_name + " doesn't exist\n";
                throw std::logic_error(error);
            }

            std::string scheme_path_str = pool_path_str + "/" + _scheme_name;
            boost::filesystem::path scheme_path(scheme_path_str);
            if (!boost::filesystem::exists(scheme_path))
            {
                std::string error = "Scheme with name - " + _pool_name + " " + _scheme_name + " doesn't exist\n";
                throw std::logic_error(error);
            }

            std::string collection_path_str = scheme_path_str + "/" + _collection_name;
            boost::filesystem::path collection_path(collection_path_str);
            if (!boost::filesystem::exists(collection_path))
            {
                std::string error = "Collection with name - " + _pool_name + " " + _scheme_name + " " + _collection_name + " doesn't exist\n";
                throw std::logic_error(error);
            }

            boost::filesystem::remove_all(collection_path_str);
        }
    };

    class add_value final:
        public command
    {

    private:

        std::string _pool_name;
        
        std::string _scheme_name;

        std::string _collection_name;

        uint _id;

        std::string _name;

        uint _money;

        std::string _key;

    public:

        add_value(database::database<std::string, std::string, customer> * database, std::uint64_t const & _time = 0) : command(database, _time)
        {}

    public:

        bool can_execute(std::string const & request, std::uint64_t const & current_time = 0)  override
        {
            std::istringstream string_with_commands(request);
            std::string command;
            string_with_commands >> command;

            if (command == "ADD")
            {
                if (current_time != 0 && this->get_time_and_date() < current_time) return false;
                std::string pool_name;
                std::string scheme_name;
                std::string collection_name;
                std::string id_str, name, money_str;
                uint id, money;
                string_with_commands >> pool_name >> scheme_name >> collection_name >> id_str >> name >> money_str;
                if (!validation::is_name(pool_name) || !validation::is_name(scheme_name) || !validation::is_name(collection_name)) return false;
                if (!validation::is_id(id_str, id) || !validation::is_name(name) || !validation::is_id(money_str, money)) return false;
                _pool_name = pool_name;
                _scheme_name = scheme_name;
                _collection_name = collection_name;
                _id = id;
                _money = money;
                _name = name;
                switch (_database->get_key_type())
                {
                case database::key_type::ID:
                    _key = id_str;
                    break;

                case database::key_type::BALANCE:
                    _key = money_str;
                    break;

                case database::key_type::NAME:
                    _key = name;
                    break;
                
                default:
                    break;
                }
                return true;
            }
            return false;
        }

        void execute(std::string const& request, std::uint64_t const & current_time = 0)  override
        {
            if (_database->get_mode() == database::mode::IN_MEMORY_CACHE)
            {
                _database->add_value(_pool_name, _scheme_name, _collection_name, _key, _id, _name, _money);
                return;
            }
            
            std::string pool_path_str = "database/" + _pool_name;
            boost::filesystem::path pool_path(pool_path_str);
            if (!boost::filesystem::exists(pool_path))
            {
                std::string error = "Pool with name - " + _pool_name + " doesn't exist\n";
                throw std::logic_error(error);
            }

            std::string scheme_path_str = pool_path_str + "/" + _scheme_name;
            boost::filesystem::path scheme_path(scheme_path_str);
            if (!boost::filesystem::exists(scheme_path))
            {
                std::string error = "Scheme with name - " + _pool_name + " " + _scheme_name + " doesn't exist\n";
                throw std::logic_error(error);
            }

            std::string collection_path_str = scheme_path_str + "/" + _collection_name;
            boost::filesystem::path collection_path(collection_path_str);
            if (!boost::filesystem::exists(collection_path))
            {
                std::string error = "Collection with name - " + _pool_name + " " + _scheme_name + " " + _collection_name + " doesn't exist\n";
                throw std::logic_error(error);
            }

            std::string customer_path_str = scheme_path_str + "/" + _key;
            boost::filesystem::path customer_path(collection_path_str);
            if (boost::filesystem::exists(customer_path))
            {
                std::string error = "Customer in " + _pool_name + " " + _scheme_name + " " + _collection_name + " with key - " + _key +  "already exists\n";
                throw std::logic_error(error);
            }

            boost::filesystem::create_directory(customer_path_str);
            std::ofstream file(customer_path_str);
            file << _id << " " << _name << " " << _money;
        }
    };

    class delete_value final:
        public command
    {

    private:

        std::string _pool_name;
        
        std::string _scheme_name;

        std::string _collection_name;

        std::string _key;

    public:

        delete_value(database::database<std::string, std::string, customer> * database, std::uint64_t const & _time = 0) : command(database, _time)
        {}

    public:

        bool can_execute(std::string const & request, std::uint64_t const & current_time = 0)  override
        {
            std::istringstream string_with_commands(request);
            std::string command;
            string_with_commands >> command;

            if (command == "DELETE")
            {
                if (current_time != 0 && this->get_time_and_date() < current_time) return false;
                std::string pool_name;
                std::string scheme_name;
                std::string collection_name;
                std::string key;
                uint tmp;
                string_with_commands >> pool_name >> scheme_name >> collection_name >> key;
                if (!validation::is_name(pool_name) || !validation::is_name(scheme_name) || !validation::is_name(collection_name)) return false;
                switch (_database->get_key_type())
                {
                case database::key_type::ID:
                    if (!validation::is_id(key, tmp)) return false;
                    break;

                case database::key_type::BALANCE:
                    if (!validation::is_id(key, tmp)) return false;
                    break;

                case database::key_type::NAME:
                    if (!validation::is_name(key)) return false;
                    break;
                
                default:
                    break;
                }
                _pool_name = pool_name;
                _scheme_name = scheme_name;
                _collection_name = collection_name;
                _key = key;
                return true;
            }
            return false;
        }

        void execute(std::string const& request, std::uint64_t const & current_time = 0)  override
        {
            if (_database->get_mode() == database::mode::IN_MEMORY_CACHE)
            {
                _database->delete_value(_pool_name, _scheme_name, _collection_name, _key);
                return;
            }
            
            std::string pool_path_str = "database/" + _pool_name;
            boost::filesystem::path pool_path(pool_path_str);
            if (!boost::filesystem::exists(pool_path))
            {
                std::string error = "Pool with name - " + _pool_name + " doesn't exist\n";
                throw std::logic_error(error);
            }

            std::string scheme_path_str = pool_path_str + "/" + _scheme_name;
            boost::filesystem::path scheme_path(scheme_path_str);
            if (!boost::filesystem::exists(scheme_path))
            {
                std::string error = "Scheme with name - " + _pool_name + " " + _scheme_name + " doesn't exist\n";
                throw std::logic_error(error);
            }

            std::string collection_path_str = scheme_path_str + "/" + _collection_name;
            boost::filesystem::path collection_path(collection_path_str);
            if (!boost::filesystem::exists(collection_path))
            {
                std::string error = "Collection with name - " + _pool_name + " " + _scheme_name + " " + _collection_name + " doesn't exist\n";
                throw std::logic_error(error);
            }

            std::string customer_path_str = scheme_path_str + "/" + _key;
            boost::filesystem::path customer_path(collection_path_str);
            if (!boost::filesystem::exists(customer_path))
            {
                std::string error = "Customer in " + _pool_name + " " + _scheme_name + " " + _collection_name + " with key - " + _key +  "doesn't exist\n";
                throw std::logic_error(error);
            }

            boost::filesystem::create_directory(customer_path_str);
        }
    };

    class read_between_value final:
        public command
    {

    private:

        std::string _pool_name;
        
        std::string _scheme_name;

        std::string _collection_name;

        std::string _min_key;

        std::string _max_key;

    public:

        read_between_value(database::database<std::string, std::string, customer> * database, std::uint64_t const & _time = 0) : command(database, _time)
        {}

    public:

        bool can_execute(std::string const & request, std::uint64_t const & current_time = 0)  override
        {
            std::istringstream string_with_commands(request);
            std::string command;
            string_with_commands >> command;

            if (command == "READBETWEEN")
            {
                if (current_time != 0 && this->get_time_and_date() < current_time) return false;
                std::string pool_name;
                std::string scheme_name;
                std::string collection_name;
                std::string min_key, max_key;
                uint tmp;
                string_with_commands >> pool_name >> scheme_name >> collection_name >> min_key >> max_key;
                if (!validation::is_name(pool_name) || !validation::is_name(scheme_name) || !validation::is_name(collection_name)) return false;
                switch (_database->get_key_type())
                {
                case database::key_type::ID:
                    if (!validation::is_id(min_key, tmp)) return false;
                    if (!validation::is_id(max_key, tmp)) return false;
                    break;

                case database::key_type::BALANCE:
                    if (!validation::is_id(min_key, tmp)) return false;
                    if (!validation::is_id(max_key, tmp)) return false;
                    break;

                case database::key_type::NAME:
                    if (!validation::is_name(min_key)) return false;
                    if (!validation::is_name(max_key)) return false;
                    break;
                
                default:
                    break;
                }
                _pool_name = pool_name;
                _scheme_name = scheme_name;
                _collection_name = collection_name;
                _min_key = min_key;
                _max_key = max_key;
                return true;
            }
            return false;
        }

        void execute(std::string const& request, std::uint64_t const & current_time = 0)  override
        {
            if (_database->get_mode() == database::mode::IN_MEMORY_CACHE)
            {
                auto res = _database->read_value_between(_pool_name, _scheme_name, _collection_name, _min_key, _max_key);
                std::cout << "\nValues with key from " << _min_key << " to " << _max_key << std::endl;
                for (auto & item : res) std::cout << "id: " << item.value.get_id() << ", name: " << item.value.get_name() << ", balance: " << item.value.get_money() << std::endl;
                return;
            }
            
            std::string pool_path_str = "database/" + _pool_name;
            boost::filesystem::path pool_path(pool_path_str);
            if (!boost::filesystem::exists(pool_path))
            {
                std::string error = "Pool with name - " + _pool_name + " doesn't exist\n";
                throw std::logic_error(error);
            }

            std::string scheme_path_str = pool_path_str + "/" + _scheme_name;
            boost::filesystem::path scheme_path(scheme_path_str);
            if (!boost::filesystem::exists(scheme_path))
            {
                std::string error = "Scheme with name - " + _pool_name + " " + _scheme_name + " doesn't exist\n";
                throw std::logic_error(error);
            }

            std::string collection_path_str = scheme_path_str + "/" + _collection_name;
            boost::filesystem::path collection_path(collection_path_str);
            if (!boost::filesystem::exists(collection_path))
            {
                std::string error = "Collection with name - " + _pool_name + " " + _scheme_name + " " + _collection_name + " doesn't exist\n";
                throw std::logic_error(error);
            }

            boost::filesystem::directory_iterator begin("collection_path");
            boost::filesystem::directory_iterator end; 
            std::cout << "\nValues with key from " << _min_key << " to " << _max_key << std::endl;
            for (; begin != end; ++ begin)
            {
                if (begin->path().string() >= (_min_key + ".txt") && 
                    begin->path().string() <=  (_max_key + ".txt")) 
                {
                    std::string file_name = (begin->path()).string();
                    customer the_customer = read_customer_from_file(file_name);
                    std::cout << "id: " << the_customer.get_id() << ", name: " << the_customer.get_name() << ", balance: " << the_customer.get_money() << std::endl;
                }
            }
        }
    };

    class read_value final:
        public command
    {

    private:

        std::string _pool_name;
        
        std::string _scheme_name;

        std::string _collection_name;

        std::string _key;

    public:

        read_value(database::database<std::string, std::string, customer> * database, std::uint64_t const & _time = 0) : command(database, _time)
        {}

    public:

        bool can_execute(std::string const & request, std::uint64_t const & current_time = 0)  override
        {
            std::istringstream string_with_commands(request);
            std::string command;
            string_with_commands >> command;

            if (command == "READ")
            {
                if (current_time != 0 && this->get_time_and_date() < current_time) return false;
                std::string pool_name;
                std::string scheme_name;
                std::string collection_name;
                std::string key;
                uint tmp;
                string_with_commands >> pool_name >> scheme_name >> collection_name >> key;
                if (!validation::is_name(pool_name) || !validation::is_name(scheme_name) || !validation::is_name(collection_name)) return false;
                switch (_database->get_key_type())
                {
                case database::key_type::ID:
                    if (!validation::is_id(key, tmp)) return false;
                    break;

                case database::key_type::BALANCE:
                    if (!validation::is_id(key, tmp)) return false;
                    break;

                case database::key_type::NAME:
                    if (!validation::is_name(key)) return false;
                    break;
                
                default:
                    break;
                }
                _pool_name = pool_name;
                _scheme_name = scheme_name;
                _collection_name = collection_name;
                _key = key;
                return true;
            }
            return false;
        }

        void execute(std::string const& request, std::uint64_t const & current_time = 0)  override
        {
            if (_database->get_mode() == database::mode::IN_MEMORY_CACHE)
            {
                auto res = _database->read_value(_pool_name, _scheme_name, _collection_name, _key);
                std::cout << "\nValue with key  " << _key << std::endl;
                std::cout << "id: " << res.get_id() << ", name: " << res.get_name() << ", balance: " << res.get_money() << std::endl;
                return;
            }
            
            std::string pool_path_str = "database/" + _pool_name;
            boost::filesystem::path pool_path(pool_path_str);
            if (!boost::filesystem::exists(pool_path))
            {
                std::string error = "Pool with name - " + _pool_name + " doesn't exist\n";
                throw std::logic_error(error);
            }

            std::string scheme_path_str = pool_path_str + "/" + _scheme_name;
            boost::filesystem::path scheme_path(scheme_path_str);
            if (!boost::filesystem::exists(scheme_path))
            {
                std::string error = "Scheme with name - " + _pool_name + " " + _scheme_name + " doesn't exist\n";
                throw std::logic_error(error);
            }

            std::string collection_path_str = scheme_path_str + "/" + _collection_name;
            boost::filesystem::path collection_path(collection_path_str);
            if (!boost::filesystem::exists(collection_path))
            {
                std::string error = "Collection with name - " + _pool_name + " " + _scheme_name + " " + _collection_name + " doesn't exist\n";
                throw std::logic_error(error);
            }

            std::string file_name = collection_path_str + "/" + _key + ".txt";
            boost::filesystem::path file_path(file_name);
            if (!boost::filesystem::exists(file_path))
            {
                std::string error = "Customer with key - " + _key + " doesn't exist\n";
                throw std::logic_error(error);
            }
            std::cout << "\nValue with key  " << _key << std::endl;
            customer the_customer = read_customer_from_file(file_name);
            std::cout << "id: " << the_customer.get_id() << ", name: " << the_customer.get_name() << ", balance: " << the_customer.get_money() << std::endl;
        }
    };

    class update_value final:
        public command
    {

    private:

        std::string _pool_name;
        
        std::string _scheme_name;

        std::string _collection_name;

        std::string _key;

        std::string _new_name;

        std::string _new_balance_str;

        uint _new_balance;

    public:

        update_value(database::database<std::string, std::string, customer> * database, std::uint64_t const & _time = 0) : command(database, _time)
        {}

    public:

        bool can_execute(std::string const & request, std::uint64_t const & current_time = 0) override
        {
            std::istringstream string_with_commands(request);
            std::string command;
            string_with_commands >> command;

            if (command == "UPDATE")
            {
                if (current_time != 0 && this->get_time_and_date() < current_time) return false;
                std::string pool_name;
                std::string scheme_name;
                std::string collection_name;
                std::string key;
                std::string new_name, new_balance;
                uint tmp, balance;
                string_with_commands >> pool_name >> scheme_name >> collection_name >> key >> new_name >> new_balance;
                if (!validation::is_name(pool_name) || !validation::is_name(scheme_name) || !validation::is_name(collection_name)) return false;
                if (!validation::is_name(new_name) || !validation::is_id(new_balance, balance)) return false;

                switch (_database->get_key_type())
                {
                case database::key_type::ID:
                    if (!validation::is_id(key, tmp)) return false;
                    break;

                case database::key_type::BALANCE:
                    if (!validation::is_id(key, tmp)) return false;
                    break;

                case database::key_type::NAME:
                    if (!validation::is_name(key)) return false;
                    break;
                
                default:
                    break;
                }
                _pool_name = pool_name;
                _scheme_name = scheme_name;
                _collection_name = collection_name;
                _key = key;
                _new_name = new_name;
                _new_balance_str = new_balance;
                _new_balance = balance;
                return true;
            }
            return false;
        }

        void execute(std::string const& request, std::uint64_t const & current_time = 0) override
        {
            if (_database->get_mode() == database::mode::IN_MEMORY_CACHE)
            {
                auto res = _database->delete_value(_pool_name, _scheme_name, _collection_name, _key);
                switch (_database->get_key_type())
                {
                case database::key_type::BALANCE:
                    _key = _new_balance_str;
                    break;

                case database::key_type::NAME:
                    _key = _new_name;
                    break;
                
                default:
                    break;

                }
               _database->add_value(_pool_name, _scheme_name, _collection_name, _key, res.get_id(), _new_name, _new_balance);
                return;
            }
            
            std::string pool_path_str = "database/" + _pool_name;
            boost::filesystem::path pool_path(pool_path_str);
            if (!boost::filesystem::exists(pool_path))
            {
                std::string error = "Pool with name - " + _pool_name + " doesn't exist\n";
                throw std::logic_error(error);
            }

            std::string scheme_path_str = pool_path_str + "/" + _scheme_name;
            boost::filesystem::path scheme_path(scheme_path_str);
            if (!boost::filesystem::exists(scheme_path))
            {
                std::string error = "Scheme with name - " + _pool_name + " " + _scheme_name + " doesn't exist\n";
                throw std::logic_error(error);
            }

            std::string collection_path_str = scheme_path_str + "/" + _collection_name;
            boost::filesystem::path collection_path(collection_path_str);
            if (!boost::filesystem::exists(collection_path))
            {
                std::string error = "Collection with name - " + _pool_name + " " + _scheme_name + " " + _collection_name + " doesn't exist\n";
                throw std::logic_error(error);
            }

            std::string file_name = collection_path_str + "/" + _key + ".txt";
            boost::filesystem::path file_path(file_name);
            if (!boost::filesystem::exists(file_path))
            {
                std::string error = "Customer with key - " + _key + " doesn't exist\n";
                throw std::logic_error(error);
            }
            auto res = read_customer_from_file(file_name);
            boost::filesystem::remove_all(file_path);
            switch (_database->get_key_type())
            {
            case database::key_type::BALANCE:
                _key = _new_balance_str;
                break;

            case database::key_type::NAME:
                _key = _new_name;
                break;
            
            default:
                break;
            }

            file_name = collection_path_str + "/" + _key + ".txt";
            boost::filesystem::path new_file_path(file_name);

            boost::filesystem::create_directory(new_file_path);
            std::ofstream file(file_name);
            file << res.get_id() << " " << _new_name << " " << _new_balance;
        }
    };

    
};

#endif