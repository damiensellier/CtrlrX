#include <luabind/luabind.hpp>
#include <luabind/object.hpp>

#include <lua.hpp>

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

class Counter {
public:
    explicit Counter(int start)
        : value_(start)
    {
    }

    void add(int delta)
    {
        value_ += delta;
    }

    int value() const
    {
        return value_;
    }

private:
    int value_;
};

int cpp_multiply(int a, int b)
{
    return a * b;
}

void require_equal(const std::string& label, int actual, int expected)
{
    if (actual != expected)
    {
        throw std::runtime_error(
            label + " mismatch: expected " + std::to_string(expected) +
            ", got " + std::to_string(actual));
    }
}

} // namespace

int main()
{
    lua_State* L = luaL_newstate();
    if (!L)
    {
        std::cerr << "failed to create lua state" << std::endl;
        return 1;
    }

    try
    {
        luaL_openlibs(L);
        luabind::open(L);

        luabind::module(L) [
            luabind::def("cpp_multiply", &cpp_multiply),
            luabind::class_<Counter>("Counter")
                .def(luabind::constructor<int>())
                .def("add", &Counter::add)
                .def("value", &Counter::value)
        ];

        const char* script =
            "function lua_increment(x)\n"
            "    return x + 1\n"
            "end\n"
            "\n"
            "function lua_use_counter(c)\n"
            "    c:add(5)\n"
            "    return c:value()\n"
            "end\n"
            "\n"
            "cpp_result = cpp_multiply(6, 7)\n";

        if (luaL_dostring(L, script) != LUA_OK)
        {
            const char* message = lua_tostring(L, -1);
            throw std::runtime_error(
                std::string("luaL_dostring failed: ") +
                (message ? message : "<unknown>"));
        }

        // C++ calling Lua free function.
        int incremented = luabind::call_function<int>(L, "lua_increment", 41);
        require_equal("lua_increment", incremented, 42);

        // Lua calling C++ free function (result stored in global by script).
        luabind::object globals = luabind::globals(L);
        int multiplied = luabind::object_cast<int>(globals["cpp_result"]);
        require_equal("cpp_multiply", multiplied, 42);

        // C++ creates Lua-side object, then calls Lua with that object.
        luabind::object counter_ctor = globals["Counter"];
        luabind::object lua_counter = counter_ctor(10);
        int lua_counter_value =
            luabind::call_function<int>(L, "lua_use_counter", lua_counter);
        require_equal("lua_use_counter(lua object)", lua_counter_value, 15);

        // C++ object passed into Lua and manipulated from Lua.
        Counter cpp_counter(1);
        int cpp_counter_value =
            luabind::call_function<int>(L, "lua_use_counter", &cpp_counter);
        require_equal("lua_use_counter(cpp object)", cpp_counter_value, 6);
        require_equal("cpp object updated from lua", cpp_counter.value(), 6);

        std::cout << "roundtrip example passed" << std::endl;
    }
    catch (const luabind::error& e)
    {
        const char* message = lua_tostring(e.state(), -1);
        std::cerr << "luabind::error: " << (message ? message : "<unknown>") << std::endl;
        lua_close(L);
        return 1;
    }
    catch (const std::exception& e)
    {
        std::cerr << "exception: " << e.what() << std::endl;
        lua_close(L);
        return 1;
    }

    lua_close(L);
    return 0;
}
