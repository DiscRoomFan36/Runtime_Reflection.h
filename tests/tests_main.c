
#include "common.h"


#include "basic_tests.c"


int main(void) {
    Arena runtime_reflection_arena = ZEROED;
    Initialize_Runtime_Reflection(&runtime_reflection_arena);


    // TODO have Expect_Failed or something in TEST_MA.h
    ADD_TEST(test_that_using_reflect_unknown_type_will_crash, .custom_name = "Reflect On Unknown Type Crashes");
    // ADD_TEST(test_that_using_reflect_unknown_type_will_crash, .expect_crash = true);

    ADD_TEST(test_all_basic_c_types,       .custom_name = "All Basic C Types");
    ADD_TEST(test_dumb_and_stupid_c_types, .custom_name = "Dumb & Stupid C Types");
    ADD_TEST(test_advanced_types,          .custom_name = "Advanced Types");
    // ADD_TEST(test_all_basic_c_types, .custom_name = "All Basic C Types", .run_without_sandbox = true);


    int test_fails = RUN_TESTS();

    if (test_fails) {
        fprintf(stderr, "failed %d tests\n", test_fails);
    }

    Arena_Free(&runtime_reflection_arena);
    return test_fails ? 1 : 0;
}



#define BESTED_IMPLEMENTATION
#include "Bested.h"

