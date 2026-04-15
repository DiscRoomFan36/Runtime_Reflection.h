
#include "common.h"


void test_advanced_types(void) {
    typedef struct {
        s32 bar;
        f64 baz;
        bool rip_struct_packing;
    } Foo;

    Runtime_Reflection_Type *foo_type = Begin_New_Type(Foo);
    foo_type->kind = RRTK_struct;

    Add_Field(foo_type, Foo, s32,  bar);
    Add_Field(foo_type, Foo, s64,  baz);
    Add_Field(foo_type, Foo, bool, rip_struct_packing);


    typedef Array(Foo) Foo_Array;

    Make_New_Array_Type(Foo_Array, Foo);


    #define N 6

    typedef struct {
        // probably have this in basic_types?
        String my_string;

        Int_Array int_array;

        Foo_Array foo_array;

        u64 *pointer_to_unsigned;
        Foo *pointer_to_foo;

        s32 buffer_10[10];
        s32 buffer_N[N];

        // unions?
        // tagged unions?
    } Advanced_Types;


    Runtime_Reflection_Type *advanced_types_type = Begin_New_Type(Advanced_Types);

    Add_Field(advanced_types_type, Advanced_Types, String,      my_string);
    Add_Field(advanced_types_type, Advanced_Types, Int_Array,   int_array);
    Add_Field(advanced_types_type, Advanced_Types, Foo_Array,   foo_array);

    // pointers can probably be handled by this call
    Add_Field(advanced_types_type, Advanced_Types, u64 *,       pointer_to_unsigned);
    Add_Field(advanced_types_type, Advanced_Types, Foo *,       pointer_to_foo);

    // could these be done with just this call?
    Add_Field(advanced_types_type, Advanced_Types, s32 [10],    buffer_10);
    // this one might be super tricky, we want the type to be s32 [6]
    Add_Field(advanced_types_type, Advanced_Types, s32 [N],     buffer_N);


    // TODO put stuff in these fields
    Advanced_Types advanced_types = ZEROED;

    // serialization test
    String_Builder sb = ZEROED;
    Generic_serialize_human_readable(&sb, Advanced_Types, &advanced_types);

    String sb_string = String_Builder_To_String(&sb);
    Advanced_Types deserialized = ZEROED;
    Generic_deserialize_human_readable(sb_string, Advanced_Types, &deserialized);

    TEST_EXPECT(Generic_deep_equal(Advanced_Types, &advanced_types, &deserialized));
}
