cmake_minimum_required(VERSION 3.16)

project(luabind
    VERSION 0.93
    DESCRIPTION "C++ <-> Lua binding library"
    LANGUAGES CXX)

include(GNUInstallDirs)
include(CTest)

option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(LUABIND_BUILD_TESTS "Build luabind tests" ON)
option(LUABIND_BUILD_EXAMPLES "Build luabind examples" ON)
option(LUABIND_ENABLE_COVERAGE "Enable gcov/llvm-cov instrumentation" OFF)

# Build-time feature toggles used by generated luabind/build_information.hpp
option(LUABIND_NO_ERROR_CHECKING "Disable luabind runtime error checking" OFF)
option(LUABIND_NO_EXCEPTIONS "Disable luabind exceptions" OFF)
option(LUABIND_CPLUSPLUS_LUA "Build against C++ Lua implementation" OFF)
option(LUABIND_USE_NOEXCEPT "Force noexcept support macro" ON)
option(LUABIND_NOT_THREADSAFE "Disable luabind thread safety support" OFF)
option(LUABIND_NO_SCOPED_ENUM "Disable scoped enum support" OFF)

find_package(Lua REQUIRED)

if(NOT TARGET Lua::Lua)
    add_library(Lua::Lua INTERFACE IMPORTED)
    set_target_properties(Lua::Lua PROPERTIES
        INTERFACE_LINK_LIBRARIES "${LUA_LIBRARIES}"
        INTERFACE_INCLUDE_DIRECTORIES "${LUA_INCLUDE_DIR}")
endif()

set(LUABIND_GENERATED_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
file(MAKE_DIRECTORY "${LUABIND_GENERATED_INCLUDE_DIR}/luabind")
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/build_information.hpp.cmake_in"
    "${LUABIND_GENERATED_INCLUDE_DIR}/luabind/build_information.hpp"
    @ONLY)

set(LUABIND_SOURCES
    src/class.cpp
    src/class_info.cpp
    src/class_registry.cpp
    src/class_rep.cpp
    src/create_class.cpp
    src/error.cpp
    src/exception_handler.cpp
    src/function.cpp
    src/function_introspection.cpp
    src/inheritance.cpp
    src/link_compatibility.cpp
    src/lua51compat.cpp
    src/object_rep.cpp
    src/open.cpp
    src/operator.cpp
    src/pcall.cpp
    src/scope.cpp
    src/set_package_preload.cpp
    src/shared_ptr_converter.cpp
    src/stack_content_by_name.cpp
    src/weak_ref.cpp
    src/wrapper_base.cpp)

add_library(luabind ${LUABIND_SOURCES})
add_library(luabind::luabind ALIAS luabind)

target_compile_features(luabind PUBLIC cxx_std_17)

target_include_directories(luabind
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${LUABIND_GENERATED_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${LUA_INCLUDE_DIR})

target_link_libraries(luabind
    PUBLIC
        Lua::Lua)

if(LUABIND_ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(luabind PRIVATE -O0 -g --coverage)
        target_link_options(luabind PRIVATE --coverage)
    else()
        message(WARNING "Coverage requested but compiler is not GNU/Clang.")
    endif()
endif()

if(BUILD_SHARED_LIBS)
    target_compile_definitions(luabind PUBLIC LUABIND_DYNAMIC_LINK)
endif()

set_target_properties(luabind PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    SOVERSION ${PROJECT_VERSION_MAJOR}
    VERSION ${PROJECT_VERSION})

install(TARGETS luabind
    EXPORT luabindTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/luabind"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(FILES
    "${LUABIND_GENERATED_INCLUDE_DIR}/luabind/build_information.hpp"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/luabind")

install(EXPORT luabindTargets
    FILE luabindTargets.cmake
    NAMESPACE luabind::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/luabind")

if(LUABIND_BUILD_EXAMPLES)
    add_executable(cpp_lua_roundtrip examples/cpp_lua_roundtrip.cpp)
    target_link_libraries(cpp_lua_roundtrip PRIVATE luabind::luabind Lua::Lua)
    add_test(NAME example.cpp_lua_roundtrip COMMAND cpp_lua_roundtrip)
endif()

if(BUILD_TESTING AND LUABIND_BUILD_TESTS)
    add_subdirectory(tests)
endif()
