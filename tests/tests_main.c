
#include "common.h"


#include "basic_tests.c"
#include "advanced_type_tests.c"
#include "misc_api_tests.c"

#include "serialization_tests.c"

#include "deep_equal_tests.c"


int main(void) {
    { // basic tests
        ADD_TEST(test_common_c_types,          .custom_name = "All Basic C Types");
        ADD_TEST(test_dumb_and_stupid_c_types, .custom_name = "Dumb & Stupid C Types");

        // this test will fail.
        ADD_TEST(test_long_double_type, .custom_name = "Long Double Type");
    }

    { // advanced tests
        ADD_TEST(test_array_runtime_reflection);
        ADD_TEST(test_all_advanced_types_at_once, .custom_name = "Advanced Types");
    }

    { // Serialization and Deserialization
        { // binary serialization
            ADD_TEST(test_binary_serialization_works_for_simple_packed_structures);
            ADD_TEST(test_binary_serialization_works_for_structs_with_padding);
            ADD_TEST(test_basic_types_binary_serialization);

            ADD_TEST(test_binary_serialization_works_with_non_structs);
            ADD_TEST(test_binary_serialization_fails_on_bad_inputs);
        }

        ADD_TEST(test_human_readable_serialization);
    }

    { // deep equal tests
        ADD_TEST(test_deep_equal_wont_care_about_junk_padding);
        ADD_TEST(test_deep_equal_works_on_arrays_with_junk);
    }


    { // misc api tests
        // TODO TEST_MA.h testing groups. aka .test_group = "Misc API Tests"
        ADD_TEST(test_will_crash_if_reflecting_before_initialization,       .expect_crash = true);
        ADD_TEST(test_that_using_reflect_unknown_type_will_crash,           .expect_crash = true, .custom_name = "Reflect On Unknown Type Crashes");
        ADD_TEST(test_calling_initialize_runtime_reflection_twice_crashes,  .expect_crash = true);
    }




    int test_fails = RUN_TESTS();

    if (test_fails) {
        fprintf(stderr, "failed %d tests\n", test_fails);
    }

    return test_fails ? 1 : 0;
}



#define BESTED_IMPLEMENTATION
#include "Bested.h"

