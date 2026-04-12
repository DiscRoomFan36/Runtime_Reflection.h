
#include "common.h"


void test_that_using_reflect_unknown_type_will_crash(void) {
    typedef s32 my_cool_int;
    // horary! i how this doesn't crash my entire program!
    Runtime_Reflection_Type *my_cool_int_type = Reflect(my_cool_int);

    my_cool_int baz = 3;
    (void) my_cool_int_type;
    (void) baz;
}
