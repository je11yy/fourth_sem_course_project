cmake_minimum_required(VERSION 3.21)
project(crs_prj)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(allocators)
add_subdirectory(associative_container)
add_subdirectory(common)
add_subdirectory(logger)

find_package(
        Boost 1.55.0 
        REQUIRED COMPONENTS system filesystem)
add_executable(
        crs_prj
        main.cpp)
target_link_libraries(
        crs_prj 
        PUBLIC
        ${Boost_LIBRARIES})
target_link_libraries(
        crs_prj
        PUBLIC
        mp_os_lggr_lggr)
target_link_libraries(
        crs_prj
        PUBLIC
        mp_os_lggr_clnt_lggr)
target_link_libraries(
        crs_prj
        PUBLIC
        mp_os_allctr_allctr)
target_link_libraries(
        crs_prj
        PUBLIC
        mp_os_allctr_allctr_bndr_tgs)
target_link_libraries(
        crs_prj
        PUBLIC
        mp_os_allctr_allctr_bdds_sstm)
target_link_libraries(
        crs_prj
        PUBLIC
        mp_os_allctr_allctr_glbl_hp)
target_link_libraries(
        crs_prj
        PUBLIC
        mp_os_allctr_allctr_srtd_lst)
target_link_libraries(
        crs_prj
        PUBLIC
        mp_os_assctv_cntnr_srch_tr_indxng_tr_b_tr)
set_target_properties(
        crs_prj PROPERTIES
        LANGUAGES CXX
        LINKER_LANGUAGE CXX
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        VERSION 1.0
        DESCRIPTION "Course work")
target_compile_options(
        crs_prj 
        PUBLIC "-g")