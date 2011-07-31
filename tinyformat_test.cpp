#include <stdexcept>
#include <climits>
#include <cfloat>

// Throw instead of abort() so we can test error conditions.
#define TINYFORMAT_ERROR(reason) \
    throw std::runtime_error(reason);

#include "tinyformat.h"
#include <cassert>

// Compare result of tfm::format() to C's sprintf().
template<typename... Args>
void runTest(const Args&... args)
{
    std::string tfmResult = tfm::format(args...);
    char sprintfResult[200];
    sprintf(sprintfResult, args...);
    if(tfmResult != sprintfResult)
    {
        std::cout << tfmResult << std::endl;
        std::cout << sprintfResult << std::endl;
        assert(0 && "results didn't match, see above.");
    }
}

#define EXPECT_ERROR(expression)                            \
{                                                           \
    try { expression; assert(0 && "expected exception"); }  \
    catch(std::runtime_error&) {}                           \
}


// Test wrapping to create our own function which calls through to tfm::format
struct TestWrap
{
    std::ostringstream m_oss;
#   define TINYFORMAT_WRAP_FORMAT_EXTRA_ARGS int code,
    // std::string error(int code, const char* fmt, const Args&... args);
    TINYFORMAT_WRAP_FORMAT(std::string, error,
                        m_oss.clear(); m_oss << code << ": ";,
                        m_oss,
                        return m_oss.str();)
#   undef TINYFORMAT_WRAP_FORMAT_EXTRA_ARGS
};


int main()
{
    // Test various basic format specs against results of sprintf
    runTest("%s", "asdf");
    runTest("%d", 1234);
    runTest("%i", -5678);
    runTest("%o", 012);
    runTest("%u", (unsigned int)-1);
    runTest("%x", 0xdeadbeef);
    runTest("%X", 0xDEADBEEF);
    runTest("%e", 1.23456e10);
    runTest("%E", -1.23456E10);
    runTest("%f", -9.8765);
    runTest("%F", 9.8765);
    runTest("%g", DBL_MAX);
    runTest("%G", DBL_MAX);
    runTest("%c", 65);
    runTest("%hc", (short)65);
    runTest("%lc", (long)65);
    runTest("%s", "asdf_123098");
    runTest("%p", (void*)123456789);
    runTest("%%%s", "asdf"); // note: plain "%%" format gives warning with gcc
    // chars with int format specs are printed as ints:
    runTest("%hhd", (char)65);
    runTest("%hhu", (unsigned char)65);
    runTest("%hhd", (signed char)65);

    // Test precision & width
    runTest("%10d", -10);
    runTest("%.4d", 10);
    runTest("%10.4f", 1234.1234567890);
    runTest("%.f", 10.1);
    runTest("%.2s", "asdf"); // strings truncate to precision

    // Test flags
    runTest("%#x", 0x271828);
    runTest("%#o", 0x271828);
    runTest("%#f", 3.0);
    runTest("%010d", 100);
    runTest("%010d", -10); // sign should extend
    runTest("%#010X", 0xBEEF);
    // flag override precedence
    runTest("%+ 10d", 10); // '+' overrides ' '
    runTest("% +10d", 10);
    runTest("%-010d", 10); // '-' overrides '0'
    runTest("%0-10d", 10);

    // Check that length modifiers are ignored
    runTest("%hd", (short)1000);
    runTest("%ld", (long)100000);
    runTest("%lld", (long long)100000);
    runTest("%jd", (intmax_t)100000);
    runTest("%zd", (size_t)100000);
    runTest("%td", (ptrdiff_t)100000);

    // printf incompatibilities:
    // runTest("%6.4x", 10); // precision & width can't be supported independently
    // runTest("%.4d", -10); // negative numbers + precision don't quite work.

    // General "complicated" format spec test
    runTest("%0.10f:%04d:%+g:%s:%p:%c:%%:%%asdf",
            1.234, 42, 3.13, "str", (void*)1000, (int)'X');

    // Test wrong number of args
    EXPECT_ERROR(
        tfm::format("%d", 5, 10)
    )
    EXPECT_ERROR(
        tfm::format("%d")
    )
    // Unterminated format spec
    EXPECT_ERROR(
        tfm::format("%123", 10)
    )

    // Test that formatting is independent of underlying stream state.
    std::ostringstream oss;
    oss.width(20);
    oss.precision(10);
    oss.fill('*');
    oss.setf(std::ios::scientific);
    tfm::format(oss, "%f", 10.1234123412341234);
    assert(oss.str() == "10.123412");

    // Test that interface wrapping works correctly
    TestWrap wrap;
    assert(wrap.error(10, "someformat %s:%d:%d", "asdf", 2, 4) ==
           "10: someformat asdf:2:4");

    return 0;
}
