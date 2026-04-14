
#include "common.h"


void test_common_c_types(void) {
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
        .a_c_str      = "12",
        .a_void_ptr   = NULL,
    };

    // this is just testing to see if this works with all basic types.
    String basic_types_struct_string = Generic_sprint(Basic_Types_Struct, &basic_types_struct);

    const char *expected = "(Basic_Types_Struct){ .a_u8 = 1, .a_s8 = 2, .a_u16 = 3, .a_s16 = 4, .a_u32 = 5, .a_s32 = 6, .a_u64 = 7, .a_s64 = 8, .a_f32 = 9.000000, .a_f64 = 10.000000, .a_bool = true, .a_c_str = \"12\", .a_void_ptr = NULL }";

    TEST_EXPECT_WITH_REASON(String_Eq(basic_types_struct_string, S(expected)), "wanted \""S_Fmt"\", got \""S_Fmt"\"", S_Arg(S(expected)), S_Arg(basic_types_struct_string));
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
        X(long double, a_long_double) // this thing sucks, i have half a mind to just ban it.


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

    { // generic print test.
        Dumb_C_Types dumb_c_types = {
            #define X(Type, name) .name = strlen(#Type),
            TEST_DUMB_C_TYPES_X_MACRO
            #undef X
        };

        String result = Generic_sprint(Dumb_C_Types, &dumb_c_types);
        TEST_EXPECT_EQ(result.length, 644);
    }
}


void test_long_double_type(void) {
    { // this sounds reasonable, but is it?
        long double x = 235;
        f64 float_result = Type_As_Float(Reflect(long double), &x);
        printf("long double: %f\n", float_result);
    }

    typedef struct {
        long double a_long_double;
    } Long_Double_Struct;

    {
        Runtime_Reflection_Type *Long_Double_Struct_type = Begin_New_Type(Long_Double_Struct);
        Long_Double_Struct_type->kind = RRTK_struct;

        Add_Field(Long_Double_Struct_type, Long_Double_Struct, long double, a_long_double);
    }

    Long_Double_Struct ld_stuct = { .a_long_double = 31 };
    { // print test
        String result = Generic_sprint(Long_Double_Struct, &ld_stuct);
        printf("Long_Double_Struct: "S_Fmt"\n", S_Arg(result));
    }
}
