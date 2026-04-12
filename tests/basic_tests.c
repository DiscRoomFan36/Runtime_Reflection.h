
#include "common.h"


void test_that_using_reflect_unknown_type_will_crash(void) {
    typedef s32 my_cool_int;
    // horary! i how this doesn't crash my entire program!
    Runtime_Reflection_Type *my_cool_int_type = Reflect(my_cool_int);

    my_cool_int baz = 3;
    (void) my_cool_int_type;
    (void) baz;
}


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

        f32  a_f32;
        f64  a_f64;

        bool a_bool;

        const char *c_str;

        void *void_ptr;
    } Basic_Types_Struct;

    {
        Runtime_Reflection_Type *basic_types_struct_type = Begin_New_Type(Basic_Types_Struct);
        basic_types_struct_type->kind = RRTK_struct;

        TEST_EXPECT_EQ(basic_types_struct_type->size_in_bytes,      sizeof(Basic_Types_Struct));
        TEST_EXPECT_EQ(basic_types_struct_type->alignment, Alignof(Basic_Types_Struct));

        Add_Field(basic_types_struct_type, Basic_Types_Struct, u8,  a_u8);
        Add_Field(basic_types_struct_type, Basic_Types_Struct, s8,  a_s8);
        Add_Field(basic_types_struct_type, Basic_Types_Struct, u16, a_u16);
        Add_Field(basic_types_struct_type, Basic_Types_Struct, s16, a_s16);
        Add_Field(basic_types_struct_type, Basic_Types_Struct, u32, a_u32);
        Add_Field(basic_types_struct_type, Basic_Types_Struct, s32, a_s32);
        Add_Field(basic_types_struct_type, Basic_Types_Struct, u64, a_u64);
        Add_Field(basic_types_struct_type, Basic_Types_Struct, s64, a_s64);

        Add_Field(basic_types_struct_type, Basic_Types_Struct, f32, a_f32);
        Add_Field(basic_types_struct_type, Basic_Types_Struct, f64, a_f64);

        Add_Field(basic_types_struct_type, Basic_Types_Struct, bool, a_bool);

        Add_Field(basic_types_struct_type, Basic_Types_Struct, const char *, c_str);

        Add_Field(basic_types_struct_type, Basic_Types_Struct, void *, void_ptr);

        // TODO maybe TEST_MA needs a TEST_SUCCESS() function
        TEST_EXPECT(true && "successfully loaded all fields");
        TEST_EXPECT_EQ(basic_types_struct_type->fields.count, 13);
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
        .c_str      = "12",
        .void_ptr   = NULL,
};

    String basic_types_struct_string = Generic_sprint(Basic_Types_Struct, &basic_types_struct);

    const char *expected = "(Basic_Types_Struct){ .a_u8 = 1, .a_s8 = 2, .a_u16 = 3, .a_s16 = 4, .a_u32 = 5, .a_s32 = 6, .a_u64 = 7, .a_s64 = 8, .a_f32 = 9.000000, .a_f64 = 10.000000, .a_bool = true, .c_str = \"12\", .void_ptr = NULL }";

    TEST_EXPECT_WITH_REASON(String_Eq(basic_types_struct_string, S(expected)), "wanted \""S_Fmt"\", got \""S_Fmt"\"", S_Arg(S(expected)), S_Arg(basic_types_struct_string));


    // serialization test
    String_Builder sb = ZEROED;
    Generic_serialize_human_readable(&sb, Basic_Types_Struct, &basic_types_struct);

    String sb_string = String_Builder_To_String(&sb);
    Basic_Types_Struct deserialized = ZEROED;
    Generic_deserialize_human_readable(sb_string, Basic_Types_Struct, &deserialized);

    // TODO make this a string, String_Eq() them to debug.
    TEST_EXPECT(Mem_Eq(&basic_types_struct, &deserialized, sizeof(Basic_Types_Struct)));
}


void test_dumb_and_stupid_c_types(void) {

    // X(Type, name)
    //
    // X(bool, a_bool) // i would never lump you in with these guys bool, your better than that.
    #define TEST_DUMB_C_TYPES_X_MACRO                       \
        X(char                  , a_char                 )  \
        X(signed char           , a_signed_char          )  \
        X(unsigned char         , a_unsigned_char        )  \
                                                            \
        X(short                 , a_short                )  \
        X(short int             , a_short_int            )  \
        X(signed short          , a_signed_short         )  \
        X(signed short int      , a_signed_short_int     )  \
                                                            \
        X(unsigned short        , a_unsigned_short       )  \
        X(unsigned short int    , a_unsigned_short_int   )  \
                                                            \
        X(int                   , a_int                  )  \
        X(signed                , a_signed               )  \
        X(signed int            , a_signed_int           )  \
                                                            \
        X(unsigned              , a_unsigned             )  \
        X(unsigned int          , a_unsigned_int         )  \
                                                            \
        X(long                  , a_long                 )  \
        X(long int              , a_long_int             )  \
        X(signed long           , a_signed_long          )  \
        X(signed long int       , a_signed_long_int      )  \
                                                            \
        X(unsigned long         , a_unsigned_long        )  \
        X(unsigned long int     , a_unsigned_long_int    )  \
                                                            \
        X(long long             , a_long_long            )  \
        X(long long int         , a_long_long_int        )  \
        X(signed long long      , a_signed_long_long     )  \
        X(signed long long int  , a_signed_long_long_int )  \
                                                            \
        X(unsigned long long    , a_unsigned_long_long   )  \
        X(unsigned long long int, a_unsigned_long_long_in)  \
                                                            \
        X(float                 , a_float                )  \
        X(double                , a_double               )  \
        X(long double           , a_long_double          )


    typedef struct {

        #define X(Type, name) Type name;
            TEST_DUMB_C_TYPES_X_MACRO
        #undef X

        // maybe these go here as well?
        // const char *;
        // void *;
    } Dumb_C_Types;

    {
        Runtime_Reflection_Type *dumb_c_types_type = Begin_New_Type(Dumb_C_Types);
        dumb_c_types_type->kind = RRTK_struct;


        #define X(Type, name) Add_Field(dumb_c_types_type, Dumb_C_Types, Type, name);
            TEST_DUMB_C_TYPES_X_MACRO
        #undef X

        // TODO maybe TEST_MA needs a TEST_SUCCESS() function
        TEST_EXPECT(true && "successfully loaded all fields");
        TEST_EXPECT_EQ(dumb_c_types_type->fields.count, 29);
    }

    {
        // make sure the c types are the right size.
        Runtime_Reflection_Type *dumb_c_types_type = Reflect(Dumb_C_Types);

        #define X(Type, name)   \
            {                   \
                if (Reflect(Type)->size_in_bytes != sizeof(Type)) TEST_EXPECT_EQ(Reflect(Type)->size_in_bytes, sizeof(Type));                                         \
                if (Get_Field_By_Name(dumb_c_types_type, S(#name))->type->size_in_bytes != sizeof(Type)) TEST_EXPECT_EQ(Get_Field_By_Name(dumb_c_types_type, S(#name))->type->size_in_bytes, sizeof(Type));  \
                TEST_EXPECT_WITH_REASON(true, #Type " passed test");    \
            }
            TEST_DUMB_C_TYPES_X_MACRO
        #undef X
    }

    Dumb_C_Types dumb_c_types = {
        #define X(Type, name) .name = strlen(#Type),
            TEST_DUMB_C_TYPES_X_MACRO
        #undef X
    };

    String result = Generic_sprint(Dumb_C_Types, &dumb_c_types);
    TEST_EXPECT_EQ(result.length, 644);


    // serialization test
    String_Builder sb = ZEROED;
    Generic_serialize_human_readable(&sb, Dumb_C_Types, &dumb_c_types);

    String sb_string = String_Builder_To_String(&sb);
    Dumb_C_Types deserialized = ZEROED;
    Generic_deserialize_human_readable(sb_string, Dumb_C_Types, &deserialized);

    // TODO make this a string, String_Eq() them to debug.
    TEST_EXPECT(Mem_Eq(&dumb_c_types, &deserialized, sizeof(Dumb_C_Types)));
}

void test_advanced_types(void) {
    typedef struct {
        s32 bar;
        f64 baz;
        bool rip_struct_packing;
    } Foo;

    Runtime_Reflection_Type *foo_type = Begin_New_Type(Foo);
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

    // TODO make this a string, String_Eq() them to debug.
    //
    // TODO Mem_Eq wont work, need Generic_Deep_Equal()
    TEST_EXPECT(Generic_deep_equal(Advanced_Types, &advanced_types, &deserialized));
}

