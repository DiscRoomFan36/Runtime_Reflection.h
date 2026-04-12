//
// Runtime_Reflection.h - Runtime Reflection in C.
//
// Author   - Fletcher M
//
// Created  - 11/04/26
// Modified - 12/04/26
//
// Version  - v0.0.2
//
// Make sure to...
//      #define RUNTIME_REFLECTION_IMPLEMENTATION
// ...somewhere in your project
//


#ifndef RUNTIME_REFLECTION_H
#define RUNTIME_REFLECTION_H



// This library uses Bested.h as a dependency.
#include "Bested.h"



typedef enum {
    RRTK_NULL = 0,

    // integer types, TODO make easier to use.
    RRTK_u8,
    RRTK_s8,
    RRTK_u16,
    RRTK_s16,
    RRTK_u32,
    RRTK_s32,
    RRTK_u64,
    RRTK_s64,

    // float types
    RRTK_f32,
    RRTK_f64,

    RRTK_bool,

    RRTK_c_str,
    // this is only for void pointer
    RRTK_void_pointer,

    // type struct should have a "pointer to this type" in them.
    // RRTK_pointer,

    RRTK_Struct,

    // TODO String
    // RRDK_String,

    // TODO pointer to things, maybe store the item? or just warn user that we dont do that.
    // or user decides weather to store item or something else, an index into other array?

    // TODO Array of things, store entire array.
    // TODO buffer of things, or maybe just the array?
    // TODO static buffer of things. <- this one is useful,

    // TODO union? store the entire bytes?
    // TODO tagged unions? not a real c type, but user could construct themselves?

} Runtime_Reflection_Type_Kind;


typedef struct Runtime_Reflection_Type Runtime_Reflection_Type;

typedef struct {
    // name of field.
    String name;

    // offset in struct.
    u64 offset;

    // the underlying type.
    Runtime_Reflection_Type *type;

} Runtime_Reflection_Field;

typedef Array(Runtime_Reflection_Field) Runtime_Reflection_Field_Array;

struct Runtime_Reflection_Type {
    Runtime_Reflection_Type_Kind kind;

    String type_name;
    // u64 type_id;

    // preallocate the space you need with this.
    u64 size;
    // might be useful
    u64 alignment;


    // number or pointer / string / struct / whatever
    bool is_number_type;
    // int or float
    bool is_integer_type;
    // signed int or unsigned int
    bool is_signed_integer_type;


    // if this is a struct, it has fields.
    Runtime_Reflection_Field_Array fields;
};

typedef Array(Runtime_Reflection_Type) Runtime_Reflection_Type_Array;




// adds the basic types, call at beginning of program.
//
// allocator can be NULL
void Initialize_Runtime_Reflection(Arena *allocator);


// to make a new struct, start by calling this with your type, then Add_Field()'s
#define Begin_New_Type(Type)      Begin_New_Type_Internal(S(#Type), sizeof(Type), Alignof(Type))

// sorry this is 4 parameters, its gotta be this way
#define Add_Field(type_ptr, Struct_Type, Field_Type, field)      \
    Add_Field_Internal(type_ptr, S(#Field_Type), S(#field), offsetof(Struct_Type, field))


// to use your reflected type get it here.
#define Reflect(Type)   Get_Type_Reflection_By_Name(S(#Type))

// this function cannot return NULL, will panic.
Runtime_Reflection_Type *Get_Type_Reflection_By_Name(String type_name);

// this function might return NULL.
Runtime_Reflection_Type *Maybe_Get_Type_Reflection_By_Name(String type_name);



typedef struct {
    // pass this allocator to the string builder,
    // also where the resulting string is allocated.
    Arena *allocator;
} Generic_sprint_Opt;

// has optional arguments: Generic_sprint_Opt
#define Generic_sprint(Type, value_ptr, ...) Generic_sprint_Helper(Reflect(Type), value_ptr, (Generic_sprint_Opt){ __VA_ARGS__ })

String Generic_sprint_Helper(Runtime_Reflection_Type *type, void *value_ptr, Generic_sprint_Opt options);







// internal stuff
Runtime_Reflection_Type *Begin_New_Type_Internal(String type_name, u64 size, u64 alignment);
void Add_Field_Internal(Runtime_Reflection_Type *result, String field_type_name, String field_name, u64 offset);


#endif // RUNTIME_REFLECTION_H






#ifdef RUNTIME_REFLECTION_IMPLEMENTATION

global_variable Runtime_Reflection_Type_Array global_runtime_reflection_array = ZEROED;

global_variable bool runtime_reflection_has_been_initalized = false;


internal void assert_runtime_reflection_has_been_initalized(void) {
    if (runtime_reflection_has_been_initalized) return;

    PANIC("Runtime Reflection has not been initalized!!! remember to call Initialize_Runtime_Reflection() at the start of your program.");
}


// for CHAR_MIN, to determine if char is signed or not.
#include "limits.h"

internal void initialize_dumb_c_types(void) {

    // X(Type, is_signed);
    #define DUMB_C_INT_TYPES_X_MACRO            \
        X(char                  , CHAR_MIN < 0) \
        X(signed char           , true)         \
        X(unsigned char         , false)        \
        X(short                 , true)         \
        X(short int             , true)         \
        X(signed short          , true)         \
        X(signed short int      , true)         \
        X(unsigned short        , false)        \
        X(unsigned short int    , false)        \
        X(int                   , true)         \
        X(signed                , true)         \
        X(signed int            , true)         \
        X(unsigned              , false)        \
        X(unsigned int          , false)        \
        X(long                  , true)         \
        X(long int              , true)         \
        X(signed long           , true)         \
        X(signed long int       , true)         \
        X(unsigned long         , false)        \
        X(unsigned long int     , false)        \
        X(long long             , true)         \
        X(long long int         , true)         \
        X(signed long long      , true)         \
        X(signed long long int  , true)         \
        X(unsigned long long    , false)        \
        X(unsigned long long int, false)

    #define X(Type, is_signed)  \
        {                       \
            Runtime_Reflection_Type *new_type = Array_Add(&global_runtime_reflection_array, 1);     \
            Mem_Zero_Struct(new_type);                          \
            new_type->type_name              = S(#Type);        \
            new_type->alignment              = Alignof(Type);   \
            new_type->size                   = sizeof(Type);    \
            new_type->is_number_type         = true;            \
            new_type->is_integer_type        = true;            \
            new_type->is_signed_integer_type = is_signed;       \
                                                                \
            switch (new_type->size) {                           \
                case sizeof(u8 ): { new_type->kind = (is_signed) ? RRTK_s8  : RRTK_u8 ; } break;    \
                case sizeof(u16): { new_type->kind = (is_signed) ? RRTK_s16 : RRTK_u16; } break;    \
                case sizeof(u32): { new_type->kind = (is_signed) ? RRTK_s32 : RRTK_u32; } break;    \
                case sizeof(u64): { new_type->kind = (is_signed) ? RRTK_s64 : RRTK_u64; } break;    \
                default: PANIC("sizeof(%s) was not 8, 16, 32 or 64 bits. Unknown int of size %zu bits", #Type, new_type->size * 8);                  \
            }                                                   \
        }

        DUMB_C_INT_TYPES_X_MACRO
    #undef X


    // X(Type)
    #define DUMB_C_FLOAT_TYPES_X_MACRO  \
        X(float      )                  \
        X(double     )                  \
        // X(long double)

    #define X(Type)         \
        {   \
            Runtime_Reflection_Type *new_type = Array_Add(&global_runtime_reflection_array, 1); \
            Mem_Zero_Struct(new_type);              \
            new_type->type_name = S(#Type);         \
            new_type->alignment = Alignof(Type);    \
            new_type->size      = sizeof(Type);     \
            new_type->is_number_type    = true;     \
            new_type->is_integer_type   = false;    \
                                                    \
            switch (new_type->size) {               \
                case sizeof(f32): { new_type->kind = RRTK_f32; } break;     \
                case sizeof(f64): { new_type->kind = RRTK_f64; } break;     \
                default: PANIC("sizeof(%s) was not 32 or 64 bits, Unknown float of size %zu bits", #Type, new_type->size * 8);   \
            }                                       \
        }
        DUMB_C_FLOAT_TYPES_X_MACRO
    #undef X

    // #if __HAVE_FLOAT128
    //     typedef __float128 f128;
    // #else
    //     typedef long double f128;
    // #endif

    // sizeof(__float128);
    // // __HAVE_FLOAT128;
    // // double double x = 0;

}

void Initialize_Runtime_Reflection(Arena *allocator) {
    if (runtime_reflection_has_been_initalized) {
        PANIC("Trying to initialize runtime reflection twice, don't do that. Have better code structure.");
    }
    runtime_reflection_has_been_initalized = true;


    Mem_Zero_Struct(&global_runtime_reflection_array);
    global_runtime_reflection_array.allocator = allocator;

    // NOTE it might be better to use the Begin_New_Type() method for these...

    #define BASIC_NUMBER_TYPE(Type, is_integer, is_signed)  \
        (Runtime_Reflection_Type){                          \
            .kind       = RRTK_##Type,                      \
            .type_name  = S(#Type),                         \
            .size       = sizeof(Type),                     \
            .alignment  = Alignof(Type),                    \
            .is_number_type         = true,                 \
            .is_integer_type        = is_integer,           \
            .is_signed_integer_type = is_signed,            \
        }

    Array_Append(&global_runtime_reflection_array, BASIC_NUMBER_TYPE(u8 , true, false));
    Array_Append(&global_runtime_reflection_array, BASIC_NUMBER_TYPE(u16, true, false));
    Array_Append(&global_runtime_reflection_array, BASIC_NUMBER_TYPE(u32, true, false));
    Array_Append(&global_runtime_reflection_array, BASIC_NUMBER_TYPE(u64, true, false));

    Array_Append(&global_runtime_reflection_array, BASIC_NUMBER_TYPE(s8 , true, true));
    Array_Append(&global_runtime_reflection_array, BASIC_NUMBER_TYPE(s16, true, true));
    Array_Append(&global_runtime_reflection_array, BASIC_NUMBER_TYPE(s32, true, true));
    Array_Append(&global_runtime_reflection_array, BASIC_NUMBER_TYPE(s64, true, true));

    Array_Append(&global_runtime_reflection_array, BASIC_NUMBER_TYPE(f32, false, false));
    Array_Append(&global_runtime_reflection_array, BASIC_NUMBER_TYPE(f64, false, false));

    // bools are not numbers.
    Runtime_Reflection_Type type_bool = { .kind = RRTK_bool, .type_name = S("bool"), .size = sizeof(bool), .alignment = Alignof(bool) };
    Array_Append(&global_runtime_reflection_array, type_bool);

    Runtime_Reflection_Type type_c_str = { .kind = RRTK_c_str, .type_name = S("const char *"), .size = sizeof(const char *), .alignment = Alignof(const char *) };
    Array_Append(&global_runtime_reflection_array, type_c_str);

    Runtime_Reflection_Type type_void_ptr = { .kind = RRTK_void_pointer, .type_name = S("void *"), .size = sizeof(void *), .alignment = Alignof(void *) };
    Array_Append(&global_runtime_reflection_array, type_void_ptr);

    // add all the dumb c types after the good types.
    initialize_dumb_c_types();
}


Runtime_Reflection_Type *Maybe_Get_Type_Reflection_By_Name(String type_name) {
    assert_runtime_reflection_has_been_initalized();

    // TODO this would need some help to detect c_str's, 'const char *' vs 'const char*'
    // TODO also would need to handle pointers better
    //
    // TODO maybe handle when 2 different types have the same name?
    Array_For_Each(Runtime_Reflection_Type, it, &global_runtime_reflection_array) {
        if (String_Eq(type_name, it->type_name)) {
            if (it->kind == RRTK_NULL) {
                PANIC("The Type you were trying to get ('"S_Fmt"') did not set its kind, the first thing you should do when mking a new type is setting the kind. See the Runtime_Reflection_Type_Kind for possible values.", S_Arg(it->type_name));
            }
            return it;
        }
    }
    return NULL;
}
Runtime_Reflection_Type *Get_Type_Reflection_By_Name(String type_name) {
    assert_runtime_reflection_has_been_initalized();

    Runtime_Reflection_Type *type = Maybe_Get_Type_Reflection_By_Name(type_name);
    if (type != NULL) return type;

    PANIC("no type with name '"S_Fmt"' found", S_Arg(type_name));
}


Runtime_Reflection_Type *Begin_New_Type_Internal(String type_name, u64 size, u64 alignment) {
    assert_runtime_reflection_has_been_initalized();

    Runtime_Reflection_Type *new_type = Array_Add(&global_runtime_reflection_array, 1);

    Mem_Zero_Struct(new_type);

    // if the user doesn't set this, its gonna panic at some point.
    new_type->kind = RRTK_NULL;

    new_type->type_name = type_name;
    new_type->size      = size;
    new_type->alignment = alignment;

    new_type->fields.allocator = global_runtime_reflection_array.allocator;

    return new_type;
}



void Add_Field_Internal(Runtime_Reflection_Type *result, String field_type_name, String field_name, u64 offset) {
    assert_runtime_reflection_has_been_initalized();

    Runtime_Reflection_Field field = ZEROED;

    field.name   = field_name;
    field.type   = Get_Type_Reflection_By_Name(field_type_name);
    field.offset = offset;

    Array_Append(&result->fields, field);
}




internal void runtime_reflection_sprint(String_Builder *sb, Runtime_Reflection_Type *type, void *value) {
    assert_runtime_reflection_has_been_initalized();

    switch (type->kind) {
    case RRTK_NULL: UNREACHABLE();

    case RRTK_u8:  // fall though
    case RRTK_u16: // fall though
    case RRTK_u32: // fall though
    case RRTK_u64: {
        // TODO as_unsigned_int()
        u64 v;
        if      (type->kind == RRTK_u8 ) v = (u64) *((u8 *)value);
        else if (type->kind == RRTK_u16) v = (u64) *((u16*)value);
        else if (type->kind == RRTK_u32) v = (u64) *((u32*)value);
        else if (type->kind == RRTK_u64) v =       *((u64*)value);
        else UNREACHABLE();

        String_Builder_printf(sb, "%zu", v);
    } break;

    case RRTK_s8:  // fall though
    case RRTK_s16: // fall though
    case RRTK_s32: // fall though
    case RRTK_s64: {
        // TODO as_signed_int()
        s64 v;
        if      (type->kind == RRTK_s8 ) v = (s64) *((s8 *)value);
        else if (type->kind == RRTK_s16) v = (s64) *((s16*)value);
        else if (type->kind == RRTK_s32) v = (s64) *((s32*)value);
        else if (type->kind == RRTK_s64) v =       *((s64*)value);
        else UNREACHABLE();

        String_Builder_printf(sb, "%ld", v);
    } break;

    case RRTK_f32: // fall though
    case RRTK_f64: {
        // TODO as_float() function.
        f64 v;
        if      (type->kind == RRTK_f32) v = (f64) *((f32*)value);
        else if (type->kind == RRTK_f64) v =       *((f64*)value);
        else UNREACHABLE();

        String_Builder_printf(sb, "%f", v);
    } break;

    case RRTK_bool: {
        bool *v = value;
        String_Builder_printf(sb, "%s", (*v) ? "true" : "false");
    } break;

    case RRTK_c_str: {
        const char **v = value; // this is correct.
        if (*v != NULL) {
            String_Builder_printf(sb, "\"%s\"", *v);
        } else {
            String_Builder_printf(sb, "NULL");
        }
    } break;

    case RRTK_void_pointer: {
        void **v = value;
        if (*v != NULL) {
            String_Builder_printf(sb, "%p", *v);
        } else {
            String_Builder_printf(sb, "NULL");
        }
    } break;

    case RRTK_Struct: {
        String_Builder_printf(sb, "("S_Fmt"){ ", S_Arg(type->type_name));

        Array_For_Each(Runtime_Reflection_Field, field, &type->fields) {
            u64 index = field - type->fields.items;

            if (index != 0) {
                String_Builder_printf(sb, ", ");
            }

            String_Builder_printf(sb, "."S_Fmt" = ", S_Arg(field->name));
            runtime_reflection_sprint(sb, field->type, value + field->offset);
        }

        String_Builder_printf(sb, " }");
    } break;

    default: PANIC("Unknown Type Kind %d, ("S_Fmt")", type->kind, S_Arg(type->type_name));
    }
}


String Generic_sprint_Helper(Runtime_Reflection_Type *type, void *value_ptr, Generic_sprint_Opt options) {
    assert_runtime_reflection_has_been_initalized();

    String_Builder sb = ZEROED;
    sb.allocator = options.allocator;

    runtime_reflection_sprint(&sb, type, value_ptr);

    String result = String_Builder_To_String(&sb);
    if (sb.allocator != NULL) String_Builder_Free(&sb);

    return result;
}


#endif // RUNTIME_REFLECTION_IMPLEMENTATION
