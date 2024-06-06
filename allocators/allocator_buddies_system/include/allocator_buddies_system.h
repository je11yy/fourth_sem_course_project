#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_ALLOCATOR_ALLOCATOR_BUDDIES_SYSTEM_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_ALLOCATOR_ALLOCATOR_BUDDIES_SYSTEM_H

#include "../../allocator/include/allocator.h"
#include "../../../logger/logger/include/logger_guardant.h"
#include "../../../common/include/typename_holder.h"
#include "../../allocator/include/allocator_test_utils.h"
#include "../../allocator/include/allocator_with_fit_mode.h"
#include "../../allocator/include/allocator_guardant.h"

#include <mutex>
#include <cstring>

class allocator_buddies_system final:
    private allocator_guardant,
    public allocator_test_utils,
    public allocator_with_fit_mode,
    private logger_guardant,
    private typename_holder
{

private:
    
    void *_trusted_memory;

public:
    
    ~allocator_buddies_system() override;
    
    allocator_buddies_system(
        allocator_buddies_system const &other) = delete;
    
    allocator_buddies_system &operator=(
        allocator_buddies_system const &other) = delete;
    
    allocator_buddies_system(
        allocator_buddies_system &&other) noexcept;
    
    allocator_buddies_system &operator=(
        allocator_buddies_system &&other) noexcept;

public:
    
    explicit allocator_buddies_system(
        size_t space_size_power_of_two,
        allocator *parent_allocator = nullptr,
        logger *logger = nullptr,
        allocator_with_fit_mode::fit_mode allocate_fit_mode = allocator_with_fit_mode::fit_mode::first_fit);

public:
    
    [[nodiscard]] void *allocate(
        size_t value_size,
        size_t values_count) override;
    
    void deallocate(
        void *at) override;

public:
    
    inline void set_fit_mode(
        allocator_with_fit_mode::fit_mode mode) override;

private:
    
    inline allocator *get_allocator() const override;

public:
    
    std::vector<allocator_test_utils::block_info> get_blocks_info() const noexcept override;

private:
    
    inline logger *get_logger() const override;

private:
    
    inline std::string get_typename() const noexcept override;

    size_t get_allocator_meta_size() const noexcept;

    size_t get_available_block_meta_size() const noexcept;

    size_t get_occupied_block_meta_size() const noexcept;

    unsigned char get_degree(size_t number) const noexcept;

    size_t get_available_memory() const noexcept;

    void available_meta_creation(unsigned char * block, signed char degree, void * previous, void * next) const noexcept;

    void occupied_meta_creation(unsigned char * block, signed char degree) const noexcept;

    unsigned char * get_beginning_of_memory() const noexcept;

    size_t get_memory_size() const noexcept;

    allocator_with_fit_mode::fit_mode get_fit_mode() const noexcept;

    std::mutex& get_mutex() const noexcept;

    void * get_first_available_block() const noexcept;

    void set_first_available_block(void * block) const noexcept;

    size_t get_block_size(void * block) const noexcept;

    void * get_next_block(void * block) const noexcept;

    void * get_previous_block(void * block) const noexcept;

    void * split(void * block, unsigned char &degree) const noexcept;

    void * get_buddy(void * block, size_t size) const noexcept;

    void connect(void * prev, void * next) const noexcept;

    void create_available_block(void * block, signed char degree, void * previous) const noexcept;

    void decrease_available_space(size_t &to_decrease) const noexcept;

    void increase_available_space(size_t to_increase) const noexcept;

    void merge(void * block, void * end) const noexcept;

    void * find_block(void * block, void *end) const noexcept;

    unsigned char * get_occupied_block_allocator_beginning(void * block) const noexcept;

    signed char get_block_degree(void * block) const noexcept;

    std::string make_blocks_info_string(std::vector<allocator_test_utils::block_info> info) const noexcept;

    std::string block_status(bool state) const noexcept;
    
};

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_ALLOCATOR_ALLOCATOR_BUDDIES_SYSTEM_H