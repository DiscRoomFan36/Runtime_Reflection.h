
#include "common.h"


void test_all_basic_c_types(void) {
    typedef struct {
        // integers
        u8   a_u8;
        s8   a_s8;
        u16  a_u16;
        s16  a_s16;
        u32  a_u32;
        s32  a_s32;
        u64  a_u64;
        s64  a_s64;

        f32 a_f32;
        f64 a_f64;

        bool a_bool;

        const char *c_str;

        void *void_ptr;
    } Foo;

    Runtime_Reflection_Type *foo_type = Begin_New_Type(Foo);
    foo_type->kind = RRTK_Struct;

    TEST_EXPECT_EQ(foo_type->size,      sizeof(Foo));
    TEST_EXPECT_EQ(foo_type->alignment, Alignof(Foo));

    Add_Field(foo_type, Foo, u8,  a_u8);
    Add_Field(foo_type, Foo, s8,  a_s8);
    Add_Field(foo_type, Foo, u16, a_u16);
    Add_Field(foo_type, Foo, s16, a_s16);
    Add_Field(foo_type, Foo, u32, a_u32);
    Add_Field(foo_type, Foo, s32, a_s32);
    Add_Field(foo_type, Foo, u64, a_u64);
    Add_Field(foo_type, Foo, s64, a_s64);

    Add_Field(foo_type, Foo, f32, a_f32);
    Add_Field(foo_type, Foo, f64, a_f64);

    Add_Field(foo_type, Foo, bool, a_bool);

    Add_Field(foo_type, Foo, const char *, c_str);

    Add_Field(foo_type, Foo, void *, void_ptr);

    TEST_EXPECT_EQ(foo_type->fields.count, 13);

    Foo foo = ZEROED;
    String foo_string = Generic_sprint(Foo, &foo);

    const char *expected = "(Foo){ .a_u8 = 0, .a_s8 = 0, .a_u16 = 0, .a_s16 = 0, .a_u32 = 0, .a_s32 = 0, .a_u64 = 0, .a_s64 = 0, .a_f32 = 0, .a_f64 = 0, .a_bool = false, .c_str = "", .void_ptr = (NULL) }";

    if (!String_Eq(foo_string, S(expected))) {
        TEST_FAIL("Incorrect foo_string, wanted \""S_Fmt"\", got \""S_Fmt"\"", S_Arg(S(expected)), S_Arg(foo_string));
    }

    // TEST_EXPECT()
}

