
#include "common.h"

void test_deep_equal_wont_care_about_junk_padding(void) {

    typedef struct {
        s64 a;
        s8  b;
        u32 c;
    } Foo;

    Runtime_Reflection_Type *foo_type = Begin_New_Type(Foo);
    foo_type->kind = RRTK_struct;

    Add_Field(foo_type, Foo, s64, a);
    Add_Field(foo_type, Foo, s8,  b);
    Add_Field(foo_type, Foo, u32, c);

    Foo two_foo[2] = ZEROED;

    // give the first foo some junk in padding.
    Mem_Set(&two_foo[0], -1, sizeof(Foo));

    for (u64 i = 0; i < Array_Len(two_foo); i++) {
        two_foo[i].a = 525235;
        two_foo[i].b = 5;
        two_foo[i].c = 71;
    }

    // oh no! this memory is not the same!
    TEST_EXPECT(Mem_Eq(&two_foo[0], &two_foo[1], sizeof(Foo)) == false);

    // but the generic equal is good!
    TEST_EXPECT(Generic_deep_equal(Foo, &two_foo[0], &two_foo[1]) == true);
}
