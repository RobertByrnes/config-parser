#include "emulation.h"
#include "ConfigParser.h"

using namespace fakeit;

void setUp() {}
void tearDown() {}

void test_find_int_positive(void)
{
    int number = ConfigParser::findInt(F("positiveIntKey"));
    TEST_ASSERT_EQUAL(123, number);
}

void test_find_int_negative(void)
{
    int number = ConfigParser::findInt(F("negativeIntKey"));
    TEST_ASSERT_EQUAL(-123, number);
}

void test_find_float_positive(void)
{
    float number = ConfigParser::findFloat(F("positiveFloatKey"));
    TEST_ASSERT_FLOAT_WITHIN(0.0001, 12.3, number);
}

void test_find_float_negative(void)
{
    float number = ConfigParser::findFloat(F("negativeFloatKey"));
    TEST_ASSERT_FLOAT_WITHIN(0.0001, -12.3, number);
}

void test_find_string(void)
{
    std::string str = ConfigParser::findString(F("stringKey"));
    TEST_ASSERT_EQUAL_STRING("test", str.c_str());
}

void setup()
{
    UNITY_BEGIN();
    RUN_TEST(test_find_int_positive);
    RUN_TEST(test_find_int_negative);
    RUN_TEST(test_find_float_positive);
    RUN_TEST(test_find_float_negative);
    RUN_TEST(test_find_string);
    UNITY_END();
}

void loop()
{
}

int main(int argc, char **argv)
{
    setup();

    return 0;
}
