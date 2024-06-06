#ifndef COURSE_PROJECT_DATABASE_H
#define COURSE_PROJECT_DATABASE_H

#include "../collection/collection.h"
#include "../scheme/scheme.h"
#include "../customer/customer.h"
#include "../pool/pool.h"

namespace database
{

    enum class mode
    {
        IN_MEMORY_CACHE,
        FILE_SYSTEM
    };

    enum class key_type
    {
        ID,
        NAME, 
        BALANCE
    };

    template<typename tkey_containers, typename tkey_id, typename tvalue>
    class database final:
        protected allocator_guardant,
        protected logger_guardant
    {

    private:

        container<tkey_containers, container<tkey_containers, container<tkey_containers, container<tkey_id, tvalue>*> * > *>  * _database;

        size_t _t;

        logger * _logger;

        allocator * _allocator;

        cntr::allocator_type _type;

        std::function<int(tkey_containers const &, tkey_containers const &)> _containers_keys_comparer;

        std::function<int(tkey_id const &, tkey_id const &)> _id_keys_comparer;

        mode _mode;

        key_type _key_type;

    public:

        key_type const & get_key_type()
        {
            return _key_type;
        }

        void set_mode(mode const & mode)
        {
            _mode = mode;
        }

        mode const & get_mode()
        {
            return _mode;
        }

    public:

        explicit database(
            size_t t,
            key_type the_key_type,
            std::function<int(tkey_containers const &, tkey_containers const &)> containers_keys_comparer = std::less<tkey_containers>(),
            std::function<int(tkey_id const &, tkey_id const &)> id_keys_comparer = std::less<tkey_id>(),
            allocator *allocator = nullptr, 
            allocator_type type = allocator_type::Boundary_Tags,
            logger *logger = nullptr);

        ~database();

        database(database const & other);

        database &operator=(database const & other);

        database(database && other) noexcept;

        database &operator=(database && other) noexcept;

    public:

        void add_value(
            tkey_containers const & pool_name, 
            tkey_containers const & scheme_name, 
            tkey_containers const & collection_name, 
            tkey_id const & customer_key, 
            uint const & customer_id,
            std::string const & customer_name, 
            uint const & customer_money);

        tvalue &read_value(tkey_containers const & pool_name, 
            tkey_containers const & scheme_name, 
            tkey_containers const & collection_name, 
            tkey_id const & customer_key);

        std::vector<typename associative_container<tkey_id, tvalue>::key_value_pair> read_value_between(tkey_containers const & pool_name, 
            tkey_containers const & scheme_name, 
            tkey_containers const & collection_name, 
            tkey_id const & min_customer_key,
            tkey_id const & max_customer_key);

        void update_value(
            tkey_containers const & pool_name, 
            tkey_containers const & scheme_name, 
            tkey_containers const & collection_name, 
            tkey_id const & customer_key,
            std::string const & new_customer_name, 
            uint const & new_customer_money);

        tvalue const delete_value(tkey_containers const & pool_name, 
            tkey_containers const & scheme_name, 
            tkey_containers const & collection_name, 
            tkey_id const & customer_key);

        void add_pool(
            tkey_containers const & pool_name, 
            allocator * _allocator = nullptr,
            cntr::allocator_type allct_type = cntr::allocator_type::Boundary_Tags,
            logger * logger = nullptr);
        
        void delete_pool(
            tkey_containers const & pool_name);
        
        void add_scheme(
            tkey_containers const & pool_name, 
            tkey_containers const & scheme_name, 
            allocator * _allocator = nullptr,
            cntr::allocator_type allct_type = cntr::allocator_type::Boundary_Tags,
            logger * logger = nullptr);

        void delete_scheme(
            tkey_containers const & pool_name, 
            tkey_containers const & scheme_name);

        void add_collection(
            tkey_containers const & pool_name, 
            tkey_containers const & scheme_name, 
            tkey_containers const & collection_name,
            allocator * _allocator = nullptr,
            cntr::allocator_type allct_type = cntr::allocator_type::Boundary_Tags,
            logger * logger = nullptr);

        void delete_collection(
            tkey_containers const & pool_name, 
            tkey_containers const & scheme_name, 
            tkey_containers const & collection_name);

    private:
        
        [[nodiscard]] inline virtual allocator *get_allocator() const;
        inline virtual logger *get_logger() const;

    };

    template <typename tkey_containers, typename tkey_id, typename tvalue>
    inline database<tkey_containers, tkey_id, tvalue>::database(
        size_t t,
        key_type the_key_type,
        std::function<int(tkey_containers const &, tkey_containers const &)> containers_keys_comparer,
        std::function<int(tkey_id const &, tkey_id const &)> id_keys_comparer,
        allocator *allocator, 
        allocator_type type,
        logger *logger) : _type(type), _key_type(the_key_type), _allocator(allocator), _logger(logger), _t(t), _id_keys_comparer(id_keys_comparer), _containers_keys_comparer(containers_keys_comparer)
    {
        _database = reinterpret_cast<container<tkey_containers, container<tkey_containers, container<tkey_containers, container<tkey_id, tvalue>*> *> *>  *>(allocate_with_guard(sizeof(container<tkey_containers, container<tkey_containers, scheme<tkey_containers, collection<tkey_id, tvalue>*> *> *> ), 1));
        allocator::construct(_database, t, containers_keys_comparer, allocator, type, logger);
    }

    template <typename tkey_containers, typename tkey_id, typename tvalue>
    inline database<tkey_containers, tkey_id, tvalue>::~database()
    {
        auto it = _database->begin_infix();
        while (it != _database->end_infix())
        {
            delete std::get<3>(*it);
            it++;
        }
        allocator::destruct(_database);
        deallocate_with_guard(_database);
    }

    template <typename tkey_containers, typename tkey_id, typename tvalue>
    inline database<tkey_containers, tkey_id, tvalue>::database(database const &other) : _database(other._database), _allocator(other._allocator), _type(other._type)
    {
        _containers_keys_comparer = other._containers_keys_comparer;
        _id_keys_comparer = other._id_keys_comparer;
    }

    template <typename tkey_containers, typename tkey_id, typename tvalue>
    inline database<tkey_containers, tkey_id, tvalue> &database<tkey_containers, tkey_id, tvalue>::operator=(database<tkey_containers, tkey_id, tvalue> const &other)
    {
        if (this == &other) return *this;

        _database = other._database;
        _allocator = other._allocator;
        _type = other._type;
        _containers_keys_comparer = other._containers_keys_comparer;
        _id_keys_comparer = other._id_keys_comparer;

        return *this;
    }

    template <typename tkey_containers, typename tkey_id, typename tvalue>
    inline database<tkey_containers, tkey_id, tvalue>::database(database &&other) noexcept : _database(std::move(other._database)), _allocator(std::move(other._allocator)), _type(other._type)
    {
        _containers_keys_comparer = std::move(other._containers_keys_comparer);
        _id_keys_comparer = std::move(other._id_keys_comparer);
    }

    template <typename tkey_containers, typename tkey_id, typename tvalue>
    inline database<tkey_containers, tkey_id, tvalue> &database<tkey_containers, tkey_id, tvalue>::operator=(database<tkey_containers, tkey_id, tvalue> &&other) noexcept
    {
        if (this == &other) return *this;

        _database = std::move(other._database);
        _allocator = std::move(other._allocator);
        _type = other._type;
        _containers_keys_comparer = std::move(other._containers_keys_comparer);
        _id_keys_comparer = std::move(other._id_keys_comparer);

        return *this;
    }

    template <typename tkey_containers, typename tkey_id, typename tvalue>
    inline void database<tkey_containers, tkey_id, tvalue>::add_value(
        tkey_containers const &pool_name, 
        tkey_containers const &scheme_name, 
        tkey_containers const &collection_name, 
        tkey_id const & customer_key, 
        uint const & customer_id,
        std::string const &customer_name, 
        uint const &customer_money)
    {
        container<tkey_containers, container<tkey_containers, container<tkey_id, tvalue> *> *> * the_pool;
        container<tkey_containers, container<tkey_id, tvalue> *> * the_scheme;
        container<tkey_id, tvalue> * the_collection;
        try
        {
            the_pool = _database->obtain(pool_name);
        }
        catch(const std::logic_error& e)
        {
            std::string error = "Pool with name - " + pool_name + " doesn't exist\n";
            throw std::logic_error(error);
        }
        try
        {
            the_scheme = the_pool->obtain(scheme_name);
        }
        catch(const std::logic_error& e)
        {
            std::string error = "Scheme with name - " + pool_name + " " + scheme_name + " doesn't exist\n";
            throw std::logic_error(error);
        }
        try
        {
            the_collection = the_scheme->obtain(collection_name);
        }
        catch(const std::logic_error& e)
        {
            std::string error = "Collection with name - " + pool_name + " " + scheme_name + " " + collection_name + " doesn't exist\n";
            throw std::logic_error(error);
        }
        try
        {
            the_collection->insert(customer_key, customer(customer_id, customer_name, customer_money));
        }
        catch(const std::logic_error& e)
        {
            std::string error = "Customer " + std::to_string(customer_id) + " " + customer_name + " " + std::to_string(customer_money) + " already exist\n";
            throw std::logic_error(error);
        }
    }

    template <typename tkey_containers, typename tkey_id, typename tvalue>
    inline tvalue &database<tkey_containers, tkey_id, tvalue>::read_value(
        tkey_containers const &pool_name, 
        tkey_containers const &scheme_name, 
        tkey_containers const &collection_name, 
        tkey_id const &customer_key)
    {
        container<tkey_containers, container<tkey_containers, container<tkey_id, tvalue> *> *> * the_pool;
        container<tkey_containers, container<tkey_id, tvalue> *> * the_scheme;
        container<tkey_id, tvalue> * the_collection;
        try
        {
            the_pool = _database->obtain(pool_name);
        }
        catch(const std::logic_error& e)
        {
            std::string error = "Pool with name - " + pool_name + " doesn't exist\n";
            throw std::logic_error(error);
        }
        try
        {
            the_scheme = the_pool->obtain(scheme_name);
        }
        catch(const std::logic_error& e)
        {
            std::string error = "Scheme with name - " + pool_name + " " + scheme_name + " doesn't exist\n";
            throw std::logic_error(error);
        }
        try
        {
            the_collection = the_scheme->obtain(collection_name);
        }
        catch(const std::logic_error& e)
        {
            std::string error = "Collection with name - " + pool_name + " " + scheme_name + " " + collection_name + " doesn't exist\n";
            throw std::logic_error(error);
        }
        try
        {
            return the_collection->obtain(customer_key);
        }
        catch(const std::logic_error& e)
        {
            std::string error = "Customer with key " + customer_key + " doesn't exist\n";
            throw std::logic_error(error);
        }
    }

    template <typename tkey_containers, typename tkey_id, typename tvalue>
    inline std::vector<typename associative_container<tkey_id, tvalue>::key_value_pair> database<tkey_containers, tkey_id, tvalue>::read_value_between(
        tkey_containers const &pool_name, 
        tkey_containers const &scheme_name, 
        tkey_containers const &collection_name, 
        tkey_id const &min_customer_id, 
        tkey_id const &max_customer_id)
    {
        container<tkey_containers, container<tkey_containers, container<tkey_id, tvalue> *> *> * the_pool;
        container<tkey_containers, container<tkey_id, tvalue> *> * the_scheme;
        container<tkey_id, tvalue> * the_collection;
        try
        {
            the_pool = _database->obtain(pool_name);
        }
        catch(const std::logic_error& e)
        {
            std::string error = "Pool with name - " + pool_name + " doesn't exist\n";
            throw std::logic_error(error);
        }
        try
        {
            the_scheme = the_pool->obtain(scheme_name);
        }
        catch(const std::logic_error& e)
        {
            std::string error = "Scheme with name - " + pool_name + " " + scheme_name + " doesn't exist\n";
            throw std::logic_error(error);
        }
        try
        {
            the_collection = the_scheme->obtain(collection_name);
        }
        catch(const std::logic_error& e)
        {
            std::string error = "Collection with name - " + pool_name + " " + scheme_name + " " + collection_name + " doesn't exist\n";
            throw std::logic_error(error);
        }

        return the_collection->obtain_between(min_customer_id, max_customer_id, true, true);
    }

    template <typename tkey_containers, typename tkey_id, typename tvalue>
    inline void database<tkey_containers, tkey_id, tvalue>::update_value(
        tkey_containers const &pool_name, 
        tkey_containers const &scheme_name, 
        tkey_containers const &collection_name,
        tkey_id const &customer_key,
        std::string const &new_customer_name, 
        uint const &new_customer_money)
    {
        container<tkey_containers, container<tkey_containers, container<tkey_id, tvalue> *> *> * the_pool;
        container<tkey_containers, container<tkey_id, tvalue> *> * the_scheme;
        container<tkey_id, tvalue> * the_collection;
        try
        {
            the_pool = _database->obtain(pool_name);
        }
        catch(const std::logic_error& e)
        {
            std::string error = "Pool with name - " + pool_name + " doesn't exist\n";
            throw std::logic_error(error);
        }
        try
        {
            the_scheme = the_pool->obtain(scheme_name);
        }
        catch(const std::logic_error& e)
        {
            std::string error = "Scheme with name - " + pool_name + " " + scheme_name + " doesn't exist\n";
            throw std::logic_error(error);
        }
        try
        {
            the_collection = the_scheme->obtain(collection_name);
        }
        catch(const std::logic_error& e)
        {
            std::string error = "Collection with name - "  + pool_name + " " + scheme_name + " " + collection_name + " doesn't exist\n";
            throw std::logic_error(error);
        }
        try
        {
            customer the_customer = the_collection->obtain(customer_key);
            the_collection->insert(customer_key, the_customer.get_id(), new_customer_name, new_customer_money);
        }
        catch(const std::logic_error& e)
        {
            std::string error = "Customer with key " + customer_key + " doesn't exist\n";
            throw std::logic_error(error);
        }
    }

    template <typename tkey_containers, typename tkey_id, typename tvalue>
    inline tvalue const database<tkey_containers, tkey_id, tvalue>::delete_value(
        tkey_containers const &pool_name, 
        tkey_containers const &scheme_name, 
        tkey_containers const &collection_name, 
        tkey_id const &customer_id)
    {
        container<tkey_containers, container<tkey_containers, container<tkey_id, tvalue> *> *> * the_pool;
        container<tkey_containers, container<tkey_id, tvalue> *> * the_scheme;
        container<tkey_id, tvalue> * the_collection;
        try
        {
            the_pool = _database->obtain(pool_name);
        }
        catch(const std::logic_error& e)
        {
            std::string error = "Pool with name - " + pool_name + " doesn't exist\n";
            throw std::logic_error(error);
        }
        try
        {
            the_scheme = the_pool->obtain(scheme_name);
        }
        catch(const std::logic_error& e)
        {
            std::string error = "Scheme with name - " + pool_name + " " + scheme_name + " doesn't exist\n";
            throw std::logic_error(error);
        }
        try
        {
            the_collection = the_scheme->obtain(collection_name);
        }
        catch(const std::logic_error& e)
        {
            std::string error = "Collection with name - " + pool_name + " " + scheme_name + " " + collection_name + " doesn't exist\n";
            throw std::logic_error(error);
        }
        try
        {
            return the_collection->dispose(customer_id);
        }
        catch(const std::logic_error& e)
        {
            std::string error = "Customer with key - " + customer_id + " doesn't exist\n";
            throw std::logic_error(error);
        }
    }

    template <typename tkey_containers, typename tkey_id, typename tvalue>
    inline void database<tkey_containers, tkey_id, tvalue>::add_pool(tkey_containers const &pool_name, allocator * allocator, cntr::allocator_type allct_type, logger * logger)
    {
        container<tkey_containers, container<tkey_containers, container<tkey_id, tvalue> *> *> * new_pool = reinterpret_cast<container<tkey_containers, container<tkey_containers, container<tkey_id, tvalue> *> *> *>(allocate_with_guard(sizeof(pool<tkey_containers, container<tkey_containers, container<tkey_id, tvalue> *> *>), 1));
        allocator::construct(static_cast<pool<tkey_containers, container<tkey_containers, container<tkey_id, tvalue> *> *> *>(new_pool), _t, _containers_keys_comparer, allocator, allct_type, logger);

        _database->insert(pool_name, new_pool);
    }

    template <typename tkey_containers, typename tkey_id, typename tvalue>
    inline void database<tkey_containers, tkey_id, tvalue>::delete_pool(tkey_containers const &pool_name)
    {
        container<tkey_containers, container<tkey_containers, container<tkey_id, tvalue> *> *> * the_pool;
        try
        {
            the_pool = _database->dispose(pool_name);
        }
        catch(const std::logic_error& e)
        {
            std::string error = "Pool with name - " + pool_name + " doesn't exist\n";
            throw std::logic_error(error);
        }
        

        allocator::destruct(static_cast<pool<tkey_containers, container<tkey_containers, container<tkey_id, tvalue> *> *> *>(the_pool));
        deallocate_with_guard(the_pool);
    }

    template <typename tkey_containers, typename tkey_id, typename tvalue>
    inline void database<tkey_containers, tkey_id, tvalue>::add_scheme(tkey_containers const &pool_name, tkey_containers const &scheme_name, allocator * allocator, cntr::allocator_type allct_type, logger *logger)
    {
        container<tkey_containers, container<tkey_containers, container<tkey_id, tvalue> *> *> * the_pool;
        try
        {
            the_pool = _database->obtain(pool_name);
        }
        catch(const std::logic_error& e)
        {
            std::string error = "Pool with name - " + pool_name + " doesn't exist\n";
            throw std::logic_error(error);
        }

        container<tkey_containers, container<tkey_id, tvalue> *> * new_scheme = reinterpret_cast<container<tkey_containers, container<tkey_id, tvalue> *> *>(allocate_with_guard(sizeof(scheme<tkey_containers, container<tkey_id, tvalue> *>), 1));
        allocator::construct(static_cast<scheme<tkey_containers, container<tkey_id, tvalue> *> *>(new_scheme), _t, _containers_keys_comparer, allocator, allct_type, logger);

        the_pool->insert(scheme_name, new_scheme);
    }

    template <typename tkey_containers, typename tkey_id, typename tvalue>
    inline void database<tkey_containers, tkey_id, tvalue>::delete_scheme(tkey_containers const &pool_name, tkey_containers const &scheme_name)
    {
        container<tkey_containers, container<tkey_containers, container<tkey_id, tvalue> *> *> * the_pool;
        container<tkey_containers, container<tkey_id, tvalue> *> * the_scheme;
        try
        {
            the_pool = _database->obtain(pool_name);
        }
        catch(const std::logic_error& e)
        {
            std::string error = "Pool with name - " + pool_name + " doesn't exist\n";
            throw std::logic_error(error);
        }
        try
        {
            the_scheme = the_pool->dispose(scheme_name);
        }
        catch(const std::exception& e)
        {
            std::string error = "Scheme with name - " + pool_name + " " + scheme_name + " doesn't exist\n";
            throw std::logic_error(error);
        }

        allocator::destruct(static_cast<scheme<tkey_containers, container<tkey_id, tvalue> *> *>(the_scheme));
        deallocate_with_guard(the_scheme);
    }

    template <typename tkey_containers, typename tkey_id, typename tvalue>
    inline void database<tkey_containers, tkey_id, tvalue>::add_collection(tkey_containers const &pool_name, tkey_containers const &scheme_name, tkey_containers const &collection_name, allocator * allocator, cntr::allocator_type allct_type, logger *logger)
    {
        container<tkey_containers, container<tkey_containers, container<tkey_id, tvalue> *> *> * the_pool;
        container<tkey_containers, container<tkey_id, tvalue> *> * the_scheme;
        try
        {
            the_pool = _database->obtain(pool_name);
        }
        catch(const std::logic_error& e)
        {
            std::string error = "Pool with name - " + pool_name + " doesn't exist\n";
            throw std::logic_error(error);
        }
        try
        {
            the_scheme = the_pool->obtain(scheme_name);
        }
        catch(const std::exception& e)
        {
            std::string error = "Scheme with name - " + pool_name + " " + scheme_name + " doesn't exist\n";
            throw std::logic_error(error);
        }

        container<tkey_id, tvalue> * new_collection = reinterpret_cast<container<tkey_id, tvalue> *>(allocate_with_guard(sizeof(collection<tkey_id, tvalue>), 1));
        allocator::construct(static_cast<collection<tkey_id, tvalue> *>(new_collection), _t, _id_keys_comparer, allocator, allct_type, logger);

        the_scheme->insert(collection_name, new_collection);
    }

    template <typename tkey_containers, typename tkey_id, typename tvalue>
    inline void database<tkey_containers, tkey_id, tvalue>::delete_collection(tkey_containers const &pool_name, tkey_containers const &scheme_name, tkey_containers const &collection_name)
    {
        container<tkey_containers, container<tkey_containers, container<tkey_id, tvalue> *> *> * the_pool;
        container<tkey_containers, container<tkey_id, tvalue> *> * the_scheme;
        container<tkey_id, tvalue> * the_collection;
        try
        {
            the_pool = _database->obtain(pool_name);
        }
        catch(const std::logic_error& e)
        {
            std::string error = "Pool with name - " + pool_name + " doesn't exist\n";
            throw std::logic_error(error);
        }
        try
        {
            the_scheme = the_pool->obtain(scheme_name);
        }
        catch(const std::exception& e)
        {
            std::string error = "Scheme with name - " + pool_name + " " + scheme_name + " doesn't exist\n";
            throw std::logic_error(error);
        }
        try
        {
            the_collection = the_scheme->dispose(collection_name);
        }
        catch(const std::exception& e)
        {
            std::string error = "Collection with name - " + pool_name + " " + scheme_name + " " + collection_name + " doesn't exist\n";
            throw std::logic_error(error);
        }
        

        allocator::destruct(static_cast<collection<tkey_id, tvalue> *>(the_collection));
        deallocate_with_guard(the_collection);
    }

    template <typename tkey_containers, typename tkey_id, typename tvalue>
    inline allocator *database<tkey_containers, tkey_id, tvalue>::get_allocator() const
    {
        return _allocator;
    }

    template <typename tkey_containers, typename tkey_id, typename tvalue>
    inline logger *database<tkey_containers, tkey_id, tvalue>::get_logger() const
    {
        return _logger;
    }
};

#endif
