#ifndef COURSE_PROJECT_SCHEME_H
#define COURSE_PROJECT_SCHEME_H

#include "../container/container.h"

using namespace cntr;

template<typename tkey, typename tvalue>
class scheme final : public container<tkey, tvalue>
{

public:

    explicit scheme(
        size_t t,
        std::function<int(tkey const &, tkey const &)> keys_comparer = std::less<tkey>(),
        allocator *allocator = nullptr, 
        allocator_type type = allocator_type::Boundary_Tags,
        logger *logger = nullptr) : 
            container<tkey, tvalue>(t, keys_comparer, allocator, type, logger)
    {}

public:

    ~scheme()
    {
        auto it = this->begin_infix();
        while (it != this->end_infix())
        {
            delete std::get<3>(*it);
            it++;
        }
    }

    scheme(scheme const & other) = default;

    scheme &operator=(scheme const & other) = default;

    scheme(scheme && other) noexcept = default;

    scheme &operator=(scheme && other) noexcept = default;

    std::string get_typename()
    {
        return "scheme";
    }

};

#endif
