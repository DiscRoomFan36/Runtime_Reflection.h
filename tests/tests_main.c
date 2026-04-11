
#include "common.h"


#include "basic_tests.c"


int main(void) {

    ADD_TEST(test_all_basic_c_types, .custom_name = "All Basic C Types");


    int test_fails = RUN_TESTS();

    if (test_fails) {
        fprintf(stderr, "failed %d tests\n", test_fails);
    }

    return test_fails ? 1 : 0;
}



#define BESTED_IMPLEMENTATION
#include "Bested.h"

#define RUNTIME_REFLECTION_IMPLEMENTATION
#include "../Runtime_Reflection.h"
