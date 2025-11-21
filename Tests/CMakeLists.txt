include(FetchContent)
FetchContent_Declare(
    googletest
    # Specify the commit you depend on and update it regularly.
    # Note that we use C++14, so we should only choose a commit that is not depending on higher c++ standards.
    URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

message(STATUS "Tests enabled")

# Copy the test panel to the build directory
configure_file("test.panel" "test.panel" COPYONLY)

# find all test sources
file(GLOB_RECURSE TestFiles CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
# Organize the test source in the Tests/ folder in Xcode
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/ PREFIX "" FILES ${TestFiles})

add_executable(Tests ${TestFiles})
target_link_libraries(Tests
    PRIVATE
    gtest_main
    ${PROJECT_NAME})
target_compile_definitions(Tests PRIVATE $<TARGET_PROPERTY:${PROJECT_NAME},COMPILE_DEFINITIONS>)
target_include_directories(Tests PRIVATE $<TARGET_PROPERTY:${PROJECT_NAME},INCLUDE_DIRECTORIES>)
target_compile_features(Tests PRIVATE cxx_std_14)
if ((DEFINED ENV{CI}))
    target_compile_definitions(Tests PUBLIC CI=1)
endif ()

add_test(NAME Tests COMMAND Tests)

