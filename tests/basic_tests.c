
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
        basic_types_struct_type->kind = RRTK_Struct;

        TEST_EXPECT_EQ(basic_types_struct_type->size,      sizeof(Basic_Types_Struct));
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
}


void test_dumb_and_stupid_c_types(void) {
    typedef struct {
        // bool a_bool; // i would never lump you in with these guys bool, your better than that.

        char                    a_char;
        signed char             a_signed_char;
        unsigned char           a_unsigned_char;

        short                   a_short;
        short int               a_short_int;
        signed short            a_signed_short;
        signed short int        a_signed_short_int;

        unsigned short          a_unsigned_short;
        unsigned short int      a_unsigned_short_int;

        int                     a_int;
        signed                  a_signed;
        signed int              a_signed_int;

        unsigned                a_unsigned;
        unsigned int            a_unsigned_int;

        long                    a_long;
        long int                a_long_int;
        signed long             a_signed_long;
        signed long int         a_signed_long_int;

        unsigned long           a_unsigned_long;
        unsigned long int       a_unsigned_long_int;

        long long               a_long_long;
        long long int           a_long_long_int;
        signed long long        a_signed_long_long;
        signed long long int    a_signed_long_long_int;

        unsigned long long      a_unsigned_long_long;
        unsigned long long int  a_unsigned_long_long_in;

        float                   a_float;
        double                  a_double;
        long double             a_long_double;

        // maybe these go here as well?
        // const char *;
        // void *;
    } Dumb_C_Types;

    {
        Runtime_Reflection_Type *dumb_c_types_type = Begin_New_Type(Dumb_C_Types);
        dumb_c_types_type->kind = RRTK_Struct;

        Add_Field(dumb_c_types_type, Dumb_C_Types, char                  , a_char);
        Add_Field(dumb_c_types_type, Dumb_C_Types, signed char           , a_signed_char);
        Add_Field(dumb_c_types_type, Dumb_C_Types, unsigned char         , a_unsigned_char);

        Add_Field(dumb_c_types_type, Dumb_C_Types, short                 , a_short);
        Add_Field(dumb_c_types_type, Dumb_C_Types, short int             , a_short_int);
        Add_Field(dumb_c_types_type, Dumb_C_Types, signed short          , a_signed_short);
        Add_Field(dumb_c_types_type, Dumb_C_Types, signed short int      , a_signed_short_int);

        Add_Field(dumb_c_types_type, Dumb_C_Types, unsigned short        , a_unsigned_short);
        Add_Field(dumb_c_types_type, Dumb_C_Types, unsigned short int    , a_unsigned_short_int);

        Add_Field(dumb_c_types_type, Dumb_C_Types, int                   , a_int);
        Add_Field(dumb_c_types_type, Dumb_C_Types, signed                , a_signed);
        Add_Field(dumb_c_types_type, Dumb_C_Types, signed int            , a_signed_int);

        Add_Field(dumb_c_types_type, Dumb_C_Types, unsigned              , a_unsigned);
        Add_Field(dumb_c_types_type, Dumb_C_Types, unsigned int          , a_unsigned_int);

        Add_Field(dumb_c_types_type, Dumb_C_Types, long                  , a_long);
        Add_Field(dumb_c_types_type, Dumb_C_Types, long int              , a_long_int);
        Add_Field(dumb_c_types_type, Dumb_C_Types, signed long           , a_signed_long);
        Add_Field(dumb_c_types_type, Dumb_C_Types, signed long int       , a_signed_long_int);

        Add_Field(dumb_c_types_type, Dumb_C_Types, unsigned long         , a_unsigned_long);
        Add_Field(dumb_c_types_type, Dumb_C_Types, unsigned long int     , a_unsigned_long_int);

        Add_Field(dumb_c_types_type, Dumb_C_Types, long long             , a_long_long);
        Add_Field(dumb_c_types_type, Dumb_C_Types, long long int         , a_long_long_int);
        Add_Field(dumb_c_types_type, Dumb_C_Types, signed long long      , a_signed_long_long);
        Add_Field(dumb_c_types_type, Dumb_C_Types, signed long long int  , a_signed_long_long_int);

        Add_Field(dumb_c_types_type, Dumb_C_Types, unsigned long long    , a_unsigned_long_long);
        Add_Field(dumb_c_types_type, Dumb_C_Types, unsigned long long int, a_unsigned_long_long_in);

        Add_Field(dumb_c_types_type, Dumb_C_Types, float                 , a_float);
        Add_Field(dumb_c_types_type, Dumb_C_Types, double                , a_double);
        Add_Field(dumb_c_types_type, Dumb_C_Types, long double           , a_long_double);

        // TODO maybe TEST_MA needs a TEST_SUCCESS() function
        TEST_EXPECT(true && "successfully loaded all fields");
        TEST_EXPECT_EQ(dumb_c_types_type->fields.count, 29);
    }

    {
        // make sure the c types are the right size.
        Runtime_Reflection_Type *dumb_c_types_type = Reflect(Dumb_C_Types);

        #define DUMB_C_TYPES_X_MACRO    \
            X(char                  )   \
            X(signed char           )   \
            X(unsigned char         )   \
            X(short                 )   \
            X(short int             )   \
            X(signed short          )   \
            X(signed short int      )   \
            X(unsigned short        )   \
            X(unsigned short int    )   \
            X(int                   )   \
            X(signed                )   \
            X(signed int            )   \
            X(unsigned              )   \
            X(unsigned int          )   \
            X(long                  )   \
            X(long int              )   \
            X(signed long           )   \
            X(signed long int       )   \
            X(unsigned long         )   \
            X(unsigned long int     )   \
            X(long long             )   \
            X(long long int         )   \
            X(signed long long      )   \
            X(signed long long int  )   \
            X(unsigned long long    )   \
            X(unsigned long long int)   \
            X(float                 )   \
            X(double                )   \
            X(long double           )

        u64 index = 0;

        #define X(Type)         \
            {                   \
                TEST_EXPECT_EQ(Reflect(Type)->size, sizeof(Type));                                  \
                TEST_EXPECT_EQ(dumb_c_types_type->fields.items[index].type->size, sizeof(Type));    \
                index += 1;     \
            }
            DUMB_C_TYPES_X_MACRO
        #undef X
    }
}

void test_advanced_types(void) {
    TEST_FAIL("TODO: test_advanced_types");
}

