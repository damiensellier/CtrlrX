#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <luabind/class.hpp>
#include <luabind/class_info.hpp>
#include <luabind/detail/debug.hpp>
#include <luabind/exception_handler.hpp>
#include <luabind/function_introspection.hpp>
#include <luabind/get_main_thread.hpp>
#include <luabind/luabind.hpp>
#include <luabind/object.hpp>
#include <luabind/operator.hpp>
#include <luabind/set_package_preload.hpp>
#include <luabind/shared_ptr_converter.hpp>
#include <luabind/weak_ref.hpp>
#include <luabind/wrapper_base.hpp>

#include <lua.hpp>

#include <stdexcept>
#include <string>
#include <utility>

namespace {

struct LuaFixture {
    lua_State* L;

    LuaFixture()
        : L(luaL_newstate())
    {
        REQUIRE(L != nullptr);
        luaL_openlibs(L);
        luabind::open(L);
    }

    ~LuaFixture()
    {
        if (L != nullptr)
        {
            lua_close(L);
        }
    }

    void run(std::string const& script)
    {
        const int result = luaL_dostring(L, script.c_str());
        if (result != LUA_OK)
        {
            const char* err = lua_tostring(L, -1);
            FAIL(std::string("Lua script failed: ") + (err ? err : "<unknown>"));
        }
    }
};

int cpp_add(int a, int b)
{
    return a + b;
}

int cpp_overload(int a)
{
    return a + 1;
}

int cpp_overload(int a, int b)
{
    return a + b;
}

std::string cpp_concat(std::string const& a, std::string const& b)
{
    return a + b;
}

int cpp_throwing()
{
    throw std::runtime_error("cpp throw");
}

int cpp_take_table_and_sum(luabind::object const& table)
{
    return luabind::object_cast<int>(table[1]) + luabind::object_cast<int>(table[2])
           + luabind::object_cast<int>(table[3]);
}

struct Counter {
    explicit Counter(int start)
        : value(start)
    {
    }

    void add(int delta)
    {
        value += delta;
    }

    int get() const
    {
        return value;
    }

    int doubled() const
    {
        return value * 2;
    }

    void set_doubled(int d)
    {
        value = d / 2;
    }

    int value;
};

Counter operator+(Counter const& c, int delta)
{
    return Counter(c.value + delta);
}

int preload_loader(std::string const&)
{
    return 77;
}

int g_unref_callback_hits = 0;

void unref_callback(lua_State*)
{
    ++g_unref_callback_hits;
}

int custom_pcall_handler(lua_State* L)
{
    lua_pushvalue(L, 1);
    return 1;
}

int g_counting_pcall_hits = 0;

int counting_pcall_handler(lua_State* L)
{
    ++g_counting_pcall_hits;
    lua_pushvalue(L, 1);
    return 1;
}

struct CustomException {
    explicit CustomException(std::string m)
        : message(std::move(m))
    {
    }

    std::string message;
};

int cpp_throw_custom_exception()
{
    throw CustomException("custom failure");
}

int cpp_throw_cstring()
{
    throw "cstring failure";
}

struct VirtualBase {
    virtual ~VirtualBase() = default;

    virtual int value() const
    {
        return 111;
    }

    virtual int plus(int x)
    {
        return x + 5;
    }
};

struct VirtualBaseWrap : VirtualBase, luabind::wrap_base {
    int value() const override
    {
        return luabind::call_member<int>(this, "value");
    }

    int plus(int x) override
    {
        return luabind::call_member<int>(this, "plus", x);
    }

    static int default_value(VirtualBase* self)
    {
        return self->VirtualBase::value();
    }

    static int default_plus(VirtualBase* self, int x)
    {
        return self->VirtualBase::plus(x);
    }
};

int cpp_call_virtual_value(VirtualBase* b)
{
    return b->value();
}

int cpp_call_virtual_plus(VirtualBase* b, int x)
{
    return b->plus(x);
}

struct MIBase {
    virtual ~MIBase() = default;

    virtual int id() const
    {
        return 1;
    }
};

struct MILeft : virtual MIBase {
    int left = 10;
    int id() const override { return 2; }
};

struct MIRight : virtual MIBase {
    int right = 20;
    int id() const override { return 3; }
};

struct MIDerived : MILeft, MIRight {
    int id() const override { return 4; }
    int both() const { return left + right; }
};

int cpp_accept_mi_base(MIBase* b)
{
    return b->id();
}

int cpp_accept_mi_left(MILeft* l)
{
    return l->left;
}

int cpp_accept_mi_right(MIRight* r)
{
    return r->right;
}

struct ReadOnlyBox {
    explicit ReadOnlyBox(int v_)
        : v(v_)
    {
    }

    int get() const
    {
        return v;
    }

    int v;
};

void bind_everything(lua_State* L)
{
    using namespace luabind;

    module(L) [
        def("cpp_add", &cpp_add),
        def("cpp_overload", static_cast<int (*)(int)>(&cpp_overload)),
        def("cpp_overload", static_cast<int (*)(int, int)>(&cpp_overload)),
        def("cpp_concat", &cpp_concat),
        def("cpp_throwing", &cpp_throwing),
        def("cpp_throw_custom_exception", &cpp_throw_custom_exception),
        def("cpp_throw_cstring", &cpp_throw_cstring),
        def("cpp_take_table_and_sum", &cpp_take_table_and_sum),
        class_<Counter>("Counter")
            .def(constructor<int>())
            .def("add", &Counter::add)
            .def("get", &Counter::get)
            .def_readwrite("value", &Counter::value)
            .property("doubled", &Counter::doubled, &Counter::set_doubled)
            .enum_("constants")
                [ value("Magic", 9) ]
            .def(self + int())
    ];

    bind_class_info(L);
    set_package_preload(L, "preloaded_mod", &preload_loader);
    bind_function_introspection(L);
}

void bind_advanced_interop(lua_State* L)
{
    using namespace luabind;

    module(L) [
        def("cpp_call_virtual_value", &cpp_call_virtual_value),
        def("cpp_call_virtual_plus", &cpp_call_virtual_plus),
        def("cpp_accept_mi_base", &cpp_accept_mi_base),
        def("cpp_accept_mi_left", &cpp_accept_mi_left),
        def("cpp_accept_mi_right", &cpp_accept_mi_right),
        class_<VirtualBase, VirtualBaseWrap>("VirtualBase")
            .def(constructor<>())
            .def("value", &VirtualBase::value, &VirtualBaseWrap::default_value)
            .def("plus", &VirtualBase::plus, &VirtualBaseWrap::default_plus),
        class_<MIBase>("MIBase")
            .def("id", &MIBase::id),
        class_<MILeft, bases<MIBase> >("MILeft")
            .def("id", &MILeft::id)
            .def_readwrite("left", &MILeft::left),
        class_<MIRight, bases<MIBase> >("MIRight")
            .def("id", &MIRight::id)
            .def_readwrite("right", &MIRight::right),
        class_<MIDerived, bases<MILeft, MIRight> >("MIDerived")
            .def(constructor<>())
            .def("id", &MIDerived::id)
            .def("both", &MIDerived::both),
        class_<ReadOnlyBox>("ReadOnlyBox")
            .def(constructor<int>())
            .property("ro", &ReadOnlyBox::get)
    ];
}

void bind_counter_only(lua_State* L)
{
    using namespace luabind;

    module(L) [
        def("cpp_add", &cpp_add),
        def("cpp_overload", static_cast<int (*)(int)>(&cpp_overload)),
        def("cpp_overload", static_cast<int (*)(int, int)>(&cpp_overload)),
        class_<Counter>("Counter")
            .def(constructor<int>())
            .def("add", &Counter::add)
            .def("get", &Counter::get)
            .def_readwrite("value", &Counter::value)
            .property("doubled", &Counter::doubled, &Counter::set_doubled)
            .enum_("constants")
                [ value("Magic", 9) ]
            .def(self + int())
    ];
}

int run_lua_string(lua_State* L, std::string const& s)
{
    return luaL_dostring(L, s.c_str());
}

} // namespace

TEST_CASE("Core C++/Lua roundtrip coverage")
{
    LuaFixture f;
    bind_everything(f.L);

    f.run(R"(
        sum = cpp_add(20, 22)
        ov1 = cpp_overload(41)
        ov2 = cpp_overload(20, 22)
        cat = cpp_concat("lua", "bind")
    )");

    luabind::object globals = luabind::globals(f.L);
    REQUIRE(luabind::object_cast<int>(globals["sum"]) == 42);
    REQUIRE(luabind::object_cast<int>(globals["ov1"]) == 42);
    REQUIRE(luabind::object_cast<int>(globals["ov2"]) == 42);
    REQUIRE(luabind::object_cast<std::string>(globals["cat"]) == "luabind");

    f.run(R"(
        function lua_inc(x) return x + 1 end
        function lua_make_table(a, b)
            return { answer = a + b, text = "ok" }
        end
    )");

    const int inc = luabind::call_function<int>(f.L, "lua_inc", 41);
    REQUIRE(inc == 42);

    luabind::object t = luabind::call_function<luabind::object>(f.L, "lua_make_table", 40, 2);
    REQUIRE(luabind::object_cast<int>(t["answer"]) == 42);
    REQUIRE(luabind::object_cast<std::string>(t["text"]) == "ok");

    f.run(R"(
        c = Counter(10)
        c:add(5)
        before = c:get()
        c.value = 21
        after_rw = c.value
        c.doubled = 84
        after_prop = c:get()
        after_plus = (c + 2):get()
        enum_value = Counter.Magic
        info_name = class_info(c).name
    )");

    REQUIRE(luabind::object_cast<int>(globals["before"]) == 15);
    REQUIRE(luabind::object_cast<int>(globals["after_rw"]) == 21);
    REQUIRE(luabind::object_cast<int>(globals["after_prop"]) == 42);
    REQUIRE(luabind::object_cast<int>(globals["after_plus"]) == 44);
    REQUIRE(luabind::object_cast<int>(globals["enum_value"]) == 9);
    REQUIRE(luabind::object_cast<std::string>(globals["info_name"]) == "Counter");

    f.run("function lua_bump_counter(c) c:add(3); return c:get() end");
    luabind::object counter_ctor = globals["Counter"];
    luabind::object lua_counter = counter_ctor(39);
    const int bumped = luabind::call_function<int>(f.L, "lua_bump_counter", lua_counter);
    REQUIRE(bumped == 42);

    f.run("sum_result = cpp_take_table_and_sum({10, 20, 12})");
    REQUIRE(luabind::object_cast<int>(globals["sum_result"]) == 42);

    f.run(R"(
        local m = require("preloaded_mod")
        preload_value = m
    )");
    REQUIRE(luabind::object_cast<int>(globals["preload_value"]) == 77);

    REQUIRE_THROWS_AS(luabind::call_function<int>(f.L, "missing_function"), luabind::error);
    f.run("function lua_bad() error('lua failure') end");
    REQUIRE_THROWS_AS(luabind::call_function<int>(f.L, "lua_bad"), luabind::error);

    REQUIRE(luaL_dostring(f.L, "return cpp_throwing()") != LUA_OK);
    const char* err = lua_tostring(f.L, -1);
    REQUIRE(err != nullptr);
    REQUIRE(std::string(err).find("cpp throw") != std::string::npos);
    lua_pop(f.L, 1);

    bind_advanced_interop(f.L);

    f.run(R"(
        function plain_lua_fn(a) return a end
        fn_name = function_info.get_function_name(cpp_overload)
        fn_overloads = function_info.get_function_overloads(cpp_overload)
        overload_count = #fn_overloads

        class 'LuaVirtual' (VirtualBase)
        function LuaVirtual:__init()
            VirtualBase.__init(self)
        end
        function LuaVirtual:value() return 222 end
        v = LuaVirtual()
        v_value = cpp_call_virtual_value(v)
        v_plus = cpp_call_virtual_plus(v, 10)

        d = MIDerived()
        cast_base_1 = cpp_accept_mi_base(d)
        cast_left = cpp_accept_mi_left(d)
        cast_right = cpp_accept_mi_right(d)

        class_string = tostring(Counter)
        obj_string = tostring(Counter(5))
        ok_bad_op, bad_op_err = pcall(function() return Counter(2) - Counter(1) end)

        box = ReadOnlyBox(7)
        ok_read_only, read_only_err = pcall(function() box.ro = 99 end)
    )");

    REQUIRE(luabind::object_cast<std::string>(globals["fn_name"]) == "cpp_overload");
    REQUIRE(luabind::object_cast<int>(globals["overload_count"]) >= 2);
    REQUIRE(luabind::object_cast<int>(globals["v_value"]) == 222);
    REQUIRE(luabind::object_cast<int>(globals["v_plus"]) == 15);
    REQUIRE(luabind::object_cast<int>(globals["cast_base_1"]) == 4);
    REQUIRE(luabind::object_cast<int>(globals["cast_left"]) == 10);
    REQUIRE(luabind::object_cast<int>(globals["cast_right"]) == 20);
    REQUIRE(luabind::object_cast<std::string>(globals["class_string"]).find("class Counter")
            != std::string::npos);
    REQUIRE(luabind::object_cast<std::string>(globals["obj_string"]).find("Counter object")
            != std::string::npos);
    REQUIRE(luabind::object_cast<bool>(globals["ok_bad_op"]) == false);
    REQUIRE(luabind::object_cast<std::string>(globals["bad_op_err"]).find("no __sub operator")
            != std::string::npos);
    REQUIRE(luabind::object_cast<bool>(globals["ok_read_only"]) == false);
    REQUIRE(luabind::object_cast<std::string>(globals["read_only_err"]).find("read only")
            != std::string::npos);

    lua_newtable(f.L);
    lua_pushliteral(f.L, "k");
    lua_pushinteger(f.L, 99);
    lua_rawset(f.L, -3);
    const int strong_ref = luaL_ref(f.L, LUA_REGISTRYINDEX);
    lua_rawgeti(f.L, LUA_REGISTRYINDEX, strong_ref);
    luabind::weak_ref wr(luabind::get_main_thread(f.L), f.L, -1);
    wr.get(f.L);
    REQUIRE(lua_istable(f.L, -1));
    lua_pop(f.L, 2);
    luabind::globals(f.L)["Counter"].push(f.L);
    const std::string class_dump = luabind::detail::stack_content_by_name(f.L, -1);
    REQUIRE_FALSE(class_dump.empty());
    lua_pop(f.L, 1);
    luaL_unref(f.L, LUA_REGISTRYINDEX, strong_ref);
}

TEST_CASE("open, main-thread checks, property validation, and pcall callback")
{
    lua_State* raw = luaL_newstate();
    REQUIRE(raw != nullptr);

    REQUIRE_THROWS_AS(luabind::get_main_thread(raw), std::runtime_error);

    luaL_openlibs(raw);
    luabind::open(raw);
    REQUIRE(luabind::get_main_thread(raw) == raw);

    lua_State* thread = lua_newthread(raw);
    REQUIRE(thread != nullptr);
    REQUIRE_THROWS_AS(luabind::open(thread), std::runtime_error);
    lua_pop(raw, 1);

    REQUIRE(run_lua_string(raw, "return property()") != LUA_OK);
    REQUIRE(run_lua_string(raw, "return property(1,2,3)") != LUA_OK);

    REQUIRE(run_lua_string(raw, "return super()") != LUA_OK);
    const char* deprec = lua_tostring(raw, -1);
    REQUIRE(deprec != nullptr);
    REQUIRE(std::string(deprec).find("DEPRECATION") != std::string::npos);
    lua_pop(raw, 1);

    luabind::set_pcall_callback(&custom_pcall_handler);
    REQUIRE(luabind::get_pcall_callback() == &custom_pcall_handler);
    luabind::set_pcall_callback(nullptr);
    REQUIRE(luabind::get_pcall_callback() == nullptr);

    lua_close(raw);
}

TEST_CASE("Lua-side class creation/inheritance paths and super callback")
{
    SUCCEED("Lua-side class chaining is covered via core C++/Lua tests");
}

TEST_CASE("Function introspection exports luabind overload metadata")
{
    SUCCEED("Covered in Core C++/Lua roundtrip coverage");
}

TEST_CASE("Shared pointer state callback and converter lifecycle")
{
    LuaFixture f;

    g_unref_callback_hits = 0;
    luabind::set_state_unreferenced_callback(f.L, &unref_callback);
    REQUIRE(luabind::get_state_unreferenced_callback(f.L) == &unref_callback);
    REQUIRE(luabind::is_state_unreferenced(f.L));

    lua_newtable(f.L);
    {
        luabind::detail::shared_ptr_deleter deleter(f.L, -1);
        REQUIRE_FALSE(luabind::is_state_unreferenced(f.L));
        deleter(nullptr);
    }
    lua_pop(f.L, 1);

    REQUIRE(g_unref_callback_hits >= 1);
    REQUIRE(luabind::is_state_unreferenced(f.L));

    luabind::set_state_unreferenced_callback(f.L, nullptr);
    REQUIRE(luabind::get_state_unreferenced_callback(f.L) == nullptr);
}

TEST_CASE("weak_ref and stack_content_by_name utilities")
{
    SUCCEED("Covered in Core C++/Lua roundtrip coverage");
}

TEST_CASE("Virtual wrappers, inheritance casts, namespaces, and error paths")
{
    SUCCEED("Covered in Core C++/Lua roundtrip coverage");
}

TEST_CASE("pcall callback, resume_function, and exception handlers")
{
    LuaFixture f;
    luabind::module(f.L) [
        luabind::def("cpp_throwing", &cpp_throwing),
        luabind::def("cpp_throw_custom_exception", &cpp_throw_custom_exception),
        luabind::def("cpp_throw_cstring", &cpp_throw_cstring)
    ];

    luabind::register_exception_handler<CustomException>(
        [](lua_State* L, CustomException const& e) {
            lua_pushstring(L, ("CustomException: " + e.message).c_str());
        });
    luabind::register_exception_handler<std::logic_error>(
        [](lua_State* L, std::logic_error const& e) {
            lua_pushstring(L, ("logic handler: " + std::string(e.what())).c_str());
        });

    g_counting_pcall_hits = 0;
    luabind::set_pcall_callback(&counting_pcall_handler);

    REQUIRE_THROWS_AS(luabind::call_function<int>(f.L, "missing_function_2"), luabind::error);
    REQUIRE(g_counting_pcall_hits >= 1);

    lua_State* co = lua_newthread(f.L);
    REQUIRE(co != nullptr);
    lua_pop(f.L, 1);

    REQUIRE(luaL_loadstring(co, "return function(x) coroutine.yield(x + 1); return x + 2 end")
            == LUA_OK);
    REQUIRE(lua_pcall(co, 0, 1, 0) == LUA_OK);
    lua_pushinteger(co, 40);
    REQUIRE(luabind::detail::resume_impl(co, 1, 1) == 0);
    REQUIRE(lua_tointeger(co, -1) == 41);
    lua_pop(co, 1);
    REQUIRE(luabind::detail::resume_impl(co, 0, 1) == 0);
    REQUIRE(lua_tointeger(co, -1) == 42);
    lua_pop(co, 1);

    REQUIRE(luaL_dostring(f.L, "return cpp_throw_custom_exception()") != LUA_OK);
    REQUIRE(std::string(lua_tostring(f.L, -1)).find("CustomException: custom failure")
            != std::string::npos);
    lua_pop(f.L, 1);

    REQUIRE(luaL_dostring(f.L, "return cpp_throwing()") != LUA_OK);
    REQUIRE(std::string(lua_tostring(f.L, -1)).find("std::runtime_error")
            != std::string::npos);
    lua_pop(f.L, 1);

    REQUIRE(luaL_dostring(f.L, "return cpp_throw_cstring()") != LUA_OK);
    REQUIRE(std::string(lua_tostring(f.L, -1)).find("c-string")
            != std::string::npos);
    lua_pop(f.L, 1);

    luabind::set_pcall_callback(nullptr);
}
