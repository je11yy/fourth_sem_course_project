#ifndef COURSE_PROJECT_POOL_H
#define COURSE_PROJECT_POOL_H

#include "../container/container.h"

using namespace cntr;

template<typename tkey, typename tvalue>
class pool final : public container<tkey, tvalue>
{

public:

    explicit pool(
        size_t t,
        std::function<int(tkey const &, tkey const &)> keys_comparer = std::less<tkey>(),
        allocator *allocator = nullptr, 
        allocator_type type = allocator_type::Boundary_Tags,
        logger *logger = nullptr) :
            container<tkey, tvalue>(t, keys_comparer, allocator, type, logger) {}

public:

    std::string get_typename()
    {
        return "pool";
    }

    ~pool()
    {
        auto it = this->begin_infix();
        while (it != this->end_infix())
        {
            delete std::get<3>(*it);
            it++;
        }
    }

    pool(pool const & other) = default;

    pool &operator=(pool const & other) = default;

    pool(pool && other) noexcept = default;

    pool &operator=(pool && other) noexcept = default;

};

#endif