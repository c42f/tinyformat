#include <stdexcept>

#define TINYFORMAT_ERROR(reason) \
    throw std::runtime_error(reason);

#include "tinyformat.h"
#include <cassert>

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

int main()
{
    runTest("%0.10f:%04d:%+g:%s:%p:%c:%%:%%asdf",
            1.234, 42, 3.13, "str", (void*)1000, (int)'X');

    EXPECT_ERROR(
        tfm::format("%d", 5, 10)
    )

    runTest("%.2s", "asdf");

    return 0;
}
