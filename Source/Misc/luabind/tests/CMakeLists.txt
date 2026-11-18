find_package(Catch2 3 QUIET)

if(NOT TARGET Catch2::Catch2)
    find_path(CATCH2_INCLUDE_DIR catch2/catch.hpp)
    if(NOT CATCH2_INCLUDE_DIR)
        message(FATAL_ERROR "Catch2 not found. Install Catch2 or provide CATCH2_INCLUDE_DIR.")
    endif()
    add_library(Catch2::Catch2 INTERFACE IMPORTED)
    set_target_properties(Catch2::Catch2 PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${CATCH2_INCLUDE_DIR}")
endif()

add_executable(luabind_tests
    luabind_roundtrip_tests.cpp)

target_link_libraries(luabind_tests
    PRIVATE
        luabind::luabind
        Lua::Lua
        Catch2::Catch2)

target_compile_features(luabind_tests PRIVATE cxx_std_17)

if(LUABIND_ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(luabind_tests PRIVATE -O0 -g --coverage)
    target_link_options(luabind_tests PRIVATE --coverage)
endif()

add_test(NAME luabind.tests COMMAND luabind_tests)
