#pragma once

#define DEFINE_EMPTY_CLASS(base, name) \
    class name : public base           \
    {                                  \
    };

#define EXPAND(x) x

#define GET_MACRO(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                  _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, NAME, ...) NAME

#define FOR_EACH_1(m, base, x) m(base, x)
#define FOR_EACH_2(m, base, x, ...) m(base, x) FOR_EACH_1(m, base, __VA_ARGS__)
#define FOR_EACH_3(m, base, x, ...) m(base, x) FOR_EACH_2(m, base, __VA_ARGS__)
#define FOR_EACH_4(m, base, x, ...) m(base, x) FOR_EACH_3(m, base, __VA_ARGS__)
#define FOR_EACH_5(m, base, x, ...) m(base, x) FOR_EACH_4(m, base, __VA_ARGS__)
#define FOR_EACH_6(m, base, x, ...) m(base, x) FOR_EACH_5(m, base, __VA_ARGS__)
#define FOR_EACH_7(m, base, x, ...) m(base, x) FOR_EACH_6(m, base, __VA_ARGS__)
#define FOR_EACH_8(m, base, x, ...) m(base, x) FOR_EACH_7(m, base, __VA_ARGS__)
#define FOR_EACH_9(m, base, x, ...) m(base, x) FOR_EACH_8(m, base, __VA_ARGS__)
#define FOR_EACH_10(m, base, x, ...) m(base, x) FOR_EACH_9(m, base, __VA_ARGS__)
#define FOR_EACH_11(m, base, x, ...) m(base, x) FOR_EACH_10(m, base, __VA_ARGS__)
#define FOR_EACH_12(m, base, x, ...) m(base, x) FOR_EACH_11(m, base, __VA_ARGS__)
#define FOR_EACH_13(m, base, x, ...) m(base, x) FOR_EACH_12(m, base, __VA_ARGS__)
#define FOR_EACH_14(m, base, x, ...) m(base, x) FOR_EACH_13(m, base, __VA_ARGS__)
#define FOR_EACH_15(m, base, x, ...) m(base, x) FOR_EACH_14(m, base, __VA_ARGS__)
#define FOR_EACH_16(m, base, x, ...) m(base, x) FOR_EACH_15(m, base, __VA_ARGS__)
#define FOR_EACH_17(m, base, x, ...) m(base, x) FOR_EACH_16(m, base, __VA_ARGS__)
#define FOR_EACH_18(m, base, x, ...) m(base, x) FOR_EACH_17(m, base, __VA_ARGS__)
#define FOR_EACH_19(m, base, x, ...) m(base, x) FOR_EACH_18(m, base, __VA_ARGS__)
#define FOR_EACH_20(m, base, x, ...) m(base, x) FOR_EACH_19(m, base, __VA_ARGS__)

#define FOR_EACH(macro, base, ...)                                       \
    EXPAND(GET_MACRO(__VA_ARGS__,                                        \
                     FOR_EACH_20, FOR_EACH_19, FOR_EACH_18, FOR_EACH_17, \
                     FOR_EACH_16, FOR_EACH_15, FOR_EACH_14, FOR_EACH_13, \
                     FOR_EACH_12, FOR_EACH_11, FOR_EACH_10, FOR_EACH_9,  \
                     FOR_EACH_8, FOR_EACH_7, FOR_EACH_6, FOR_EACH_5,     \
                     FOR_EACH_4, FOR_EACH_3, FOR_EACH_2, FOR_EACH_1)(macro, base, __VA_ARGS__))

#define DEFINE_TEST_CASES(base, ...) \
    FOR_EACH(DEFINE_EMPTY_CLASS, base, __VA_ARGS__)
