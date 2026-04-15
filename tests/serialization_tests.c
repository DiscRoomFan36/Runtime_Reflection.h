
#include "common.h"


void test_binary_serialization_works_for_simple_packed_structures(void) {
    typedef struct {
        s32 foo;
        s32 bar;
    } Simple;

    {
        Runtime_Reflection_Type *simple_type = Begin_New_Type(Simple);
        simple_type->kind = RRTK_struct;

        Add_Field(simple_type, Simple, s32, foo);
        Add_Field(simple_type, Simple, s32, bar);
    }

    Simple simple = {
        .foo = 3,
        .bar = 994393023,
    };

    String_Builder sb = ZEROED;
    Generic_binary_format_serialize(&sb, Simple, &simple);

    String serialized = String_Builder_To_String(&sb);

    Simple deserialized;
    // put junk value in struct, better test this way.
    Mem_Set(&deserialized, -1, sizeof(deserialized));

    const char *err = Generic_binary_format_deserialize(serialized, Simple, &deserialized);
    TEST_EXPECT(err == NULL);

    TEST_EXPECT(Mem_Eq(&simple, &deserialized, sizeof(Simple)));
}

void test_binary_serialization_works_for_structs_with_padding(void) {
    typedef struct {
        s32 foo;
        bool rip_struct_padding;
        u64 bar;
    } Simple;

    {
        Runtime_Reflection_Type *simple_type = Begin_New_Type(Simple);
        simple_type->kind = RRTK_struct;

        Add_Field(simple_type, Simple, s32,  foo);
        Add_Field(simple_type, Simple, bool, rip_struct_padding);
        Add_Field(simple_type, Simple, u64,  bar);
    }

    Simple simple = {
        .foo = 420,
        .rip_struct_padding = true,
        .bar = 1337,
    };

    String_Builder sb = ZEROED;
    Generic_binary_format_serialize(&sb, Simple, &simple);

    String serialized = String_Builder_To_String(&sb);

    Simple deserialized;
    // put junk value in struct, better test this way.
    Mem_Set(&deserialized, -1, sizeof(deserialized));

    const char *err = Generic_binary_format_deserialize(serialized, Simple, &deserialized);
    TEST_EXPECT(err == NULL);

    // this test is really about weather this zeros the struct before deserialization.
    TEST_EXPECT(Mem_Eq(&simple, &deserialized, sizeof(Simple)));
}

void test_basic_types_binary_serialization(void) {
    typedef struct {
        #define X(Type, name) Type a_ ## name;
            COMMON_TYPES
        #undef X
    } Basic_Types_Struct;

    {
        Runtime_Reflection_Type *basic_types_struct_type = Begin_New_Type(Basic_Types_Struct);
        basic_types_struct_type->kind = RRTK_struct;

        TEST_EXPECT_EQ(basic_types_struct_type->size_in_bytes, sizeof(Basic_Types_Struct));
        TEST_EXPECT_EQ(basic_types_struct_type->alignment,     Alignof(Basic_Types_Struct));

        #define X(Type, name)  Add_Field(basic_types_struct_type, Basic_Types_Struct, Type, a_ ## name);
            COMMON_TYPES
        #undef X

        // TODO maybe TEST_MA needs a TEST_SUCCESS() function
        TEST_EXPECT(true && "successfully loaded all fields");
        TEST_EXPECT_EQ(basic_types_struct_type->fields.count, 14);
    }

    Basic_Types_Struct basic_types_struct = {
        .a_u8       =  1,
        .a_s8       =  2,
        .a_u16      =  3,
        .a_s16      =  4,
        .a_u32      =  5,
        .a_s32      =  6,
        .a_u64      =  7,
        .a_s64      =  8,
        .a_f32      =  9,
        .a_f64      = 10,
        .a_bool     = true,
        .a_string   = S("yabba dabba doo"),
        .a_c_str    = "1212212121212121212123412341234439871432",
        .a_void_ptr = (void*) 0xcafebabebeefbabe, // just a funny number
    };

    String_Builder sb = ZEROED;
    Generic_binary_format_serialize(&sb, Basic_Types_Struct, &basic_types_struct);

    Basic_Types_Struct result;
    const char *err = Generic_binary_format_deserialize(String_Builder_To_String(&sb), Basic_Types_Struct, &result);
    TEST_EXPECT(err == NULL);

    TEST_EXPECT_EQ(basic_types_struct.a_u8 , result.a_u8 );
    TEST_EXPECT_EQ(basic_types_struct.a_s8 , result.a_s8 );
    TEST_EXPECT_EQ(basic_types_struct.a_u16, result.a_u16);
    TEST_EXPECT_EQ(basic_types_struct.a_s16, result.a_s16);
    TEST_EXPECT_EQ(basic_types_struct.a_u32, result.a_u32);
    TEST_EXPECT_EQ(basic_types_struct.a_s32, result.a_s32);
    TEST_EXPECT_EQ(basic_types_struct.a_u64, result.a_u64);
    TEST_EXPECT_EQ(basic_types_struct.a_s64, result.a_s64);

    TEST_EXPECT_EQ(basic_types_struct.a_f32, result.a_f32);
    TEST_EXPECT_EQ(basic_types_struct.a_f64, result.a_f64);

    TEST_EXPECT_EQ(basic_types_struct.a_bool, result.a_bool);
    TEST_EXPECT(strcmp(basic_types_struct.a_c_str, result.a_c_str) == 0);
    TEST_EXPECT_EQ(basic_types_struct.a_void_ptr, result.a_void_ptr);
}

void test_binary_serialization_works_with_non_structs(void) {

    // this is very important. keep it safe.
    s8 my_cool_number = 5;

    String_Builder sb = ZEROED;
    Generic_binary_format_serialize(&sb, s8, &my_cool_number);

    s8 result;
    const char *err = Generic_binary_format_deserialize(String_Builder_To_String(&sb), s8, &result);
    TEST_EXPECT(err == NULL);

    TEST_EXPECT_EQ(my_cool_number, result);

}


void test_binary_serialization_fails_on_bad_inputs(void) {
    TEST_FAIL("TODO: make test");
}


// TODO human readable serialization.
void test_human_readable_serialization(void) {
    TEST_FAIL("TODO: make test");
}

// TODO version'd serialization.

