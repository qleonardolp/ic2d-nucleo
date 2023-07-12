#include <unity.h>
#include <forecast/com.hpp>
#include <functional>
#include <random>
#include <string>

using namespace forecast;
using namespace forecast::com;

void comEscape() {
    uint8_t unescaped[] = {0x00, 0x00, 0x01, 0x02, 0x03,
                           0x04, 0x1b, 0x1b, 0x39, 0x03};
    uint8_t groundTrue[] = {0x00, 0x00, 0x01, 0x1b, 0x02, 0x1b, 0x03, 0x04,
                            0x1b, 0x1b, 0x1b, 0x1b, 0x39, 0x1b, 0x03};
    uint8_t escaped[24];  // 12 should be enough

    auto sz = escape(unescaped, escaped, sizeof(unescaped), 24);

    TEST_ASSERT_MESSAGE(sz == sizeof(groundTrue),
                        std::string(std::to_string(sz) +
                                    "!=" + std::to_string(sizeof(groundTrue)))
                            .c_str());

    for (size_t i = 0; i < sz; ++i) {
        auto msg = "escaped[" + std::to_string(i) +
                   "] is different from groundTrue[" + std::to_string(i) + "]";
        TEST_ASSERT_MESSAGE(escaped[i] == groundTrue[i], msg.c_str());
    }
}

void comUnescape() {
    uint8_t unescaped[24];
    uint8_t groundTrue[] = {0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x1b, 0x1b};
    uint8_t escaped[] = {0x00, 0x00, 0x01, 0x1b, 0x02, 0x1b,
                         0x03, 0x04, 0x1b, 0x1b, 0x1b, 0x1b};

    auto sz = unescape(escaped, unescaped, sizeof(escaped), 24);

    TEST_ASSERT_MESSAGE(sz == sizeof(groundTrue),
                        std::string(std::to_string(sz) +
                                    "!=" + std::to_string(sizeof(groundTrue)))
                            .c_str());

    for (size_t i = 0; i < sz; ++i) {
        auto msg = "unescaped[" + std::to_string(i) +
                   "] is different from groundTrue[" + std::to_string(i) + "]";
        TEST_ASSERT_MESSAGE(unescaped[i] == groundTrue[i], msg.c_str());
    }
}

/**
 * @brief It creates numberOfStrings byte's strings of byteStrLength bytes each,
 * it trys to escape that, and it checks if the unescaped variable is the same
 * of the original.
 *
 * @param byteStrLength
 * @param numberOfStrings
 */
void comEscapeUnescape(uint byteStrLength, uint numberOfStrings) {
    std::default_random_engine generator;
    std::uniform_int_distribution<uint8_t> distribution(0, 255);
    auto byte = std::bind(distribution, generator);

    auto str = new uint8_t[byteStrLength];       // first string
    auto checkStr = new uint8_t[byteStrLength];  // final string

    // in the worst case I escape all the characters
    auto escaped = new uint8_t[2 * byteStrLength];

    for (size_t i = 0; i < numberOfStrings; ++i) {
        // generating the random number
        for (size_t j = 0; j < byteStrLength; ++j)
            str[j] = byte();

        // escaping the stringt
        auto sz = escape(str, escaped, byteStrLength, 2 * byteStrLength);

        // unescaping the escaped string
        sz = unescape(escaped, checkStr, sz, byteStrLength);

        // assert on length
        TEST_ASSERT_EQUAL_MESSAGE(
            byteStrLength, sz,
            "the unescaped size is not equal to the pre escaping string");

        // check if the bytes are ok
        TEST_ASSERT_EQUAL_UINT8_ARRAY(str, checkStr, byteStrLength);
    }

    delete str;
    delete checkStr;
    delete escaped;
}

void checkEtxTest() {
    uint8_t t1[] = {0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x1b, 0x1b, 0x03};
    uint8_t f1[] = {0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x1b, 0x1b, 0x1b, 0x03};
    uint8_t t2[] = {0x02, 0x04, 0x03};
    uint8_t f2[] = {0x02, 0x1b, 0x03};

    #define T(X) TEST_ASSERT(com::checkEtx((X), sizeof((X))))
    #define F(X) TEST_ASSERT_FALSE(com::checkEtx((X), sizeof((X))))

    T(t1);
    T(t2);

    F(f1);
    F(f2);

    #undef T
    #undef F
};

int main() {
    auto test_10_1 = []() { comEscapeUnescape(10, 1); };
    auto test_10_2 = []() { comEscapeUnescape(10, 2); };
    auto test_10_3 = []() { comEscapeUnescape(10, 3); };
    auto test_10_10 = []() { comEscapeUnescape(10, 10); };
    auto test_100_10 = []() { comEscapeUnescape(100, 10); };
    auto test_100_100 = []() { comEscapeUnescape(100, 100); };
    auto test_1000_100 = []() { comEscapeUnescape(1000, 1000); };
    auto test_1000_1000 = []() { comEscapeUnescape(1000, 10000); };
    auto test_1000_10000 = []() { comEscapeUnescape(1000, 100000); };

    UNITY_BEGIN();

    RUN_TEST(comEscape);
    RUN_TEST(comUnescape);
    RUN_TEST(checkEtxTest);
    RUN_TEST(test_10_1);
    RUN_TEST(test_10_2);
    RUN_TEST(test_10_3);
    RUN_TEST(test_10_10);
    RUN_TEST(test_100_10);
    RUN_TEST(test_100_100);
    RUN_TEST(test_1000_100);
    RUN_TEST(test_1000_1000);
    RUN_TEST(test_1000_10000);
    UNITY_END();
}