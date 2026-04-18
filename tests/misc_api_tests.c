
#include "common.h"


void test_will_crash_if_reflecting_before_initialization(void) {
    Runtime_Reflection_Type *s32_type = Reflect(s32);

    // do something with this
    (void) s32_type;
}

void test_that_using_reflect_unknown_type_will_crash(void) {
    Initialize_Runtime_Reflection(NULL);

    typedef s32 my_cool_int;
    // horary! i how this doesn't crash my entire program!
    Runtime_Reflection_Type *my_cool_int_type = Reflect(my_cool_int);

    my_cool_int baz = 3;
    (void) my_cool_int_type;
    (void) baz;

}

void test_calling_initialize_runtime_reflection_twice_crashes(void) {
    Initialize_Runtime_Reflection(NULL);
    Initialize_Runtime_Reflection(NULL);
}