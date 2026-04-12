//
// Runtime_Reflection.h - Runtime Reflection in C.
//
// Author   - Fletcher M
//
// Created  - 11/04/26
// Modified - 12/04/26
//
// Version  - v0.0.3
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

    // number is all integers and floats, of every size.
    RRTK_number,

    // would the API be better if we separated integer and float?
    // RRTK_Integer,
    // RRTK_Float,

    RRTK_bool,

    RRTK_c_str,
    // this is only for void pointer
    RRTK_void_pointer,

    // type struct should have a "pointer to this type" in them.
    // RRTK_pointer,

    RRTK_struct,

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
    u64 size_in_bytes;
    // might be useful
    u64 alignment;

    // is this as_number thing useful? or just a hindrance?
    struct {
        // int or float
        bool is_integer_type;
        // signed int or unsigned int
        bool is_signed_integer_type;
    } as_number;


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


#define Make_New_Array_Type(Array_Type, Inner_Type) TODO("Make_New_Array_Type function");



// to use your reflected type get it here.
#define Reflect(Type)   Get_Type_Reflection_By_Name(S(#Type))

// this function cannot return NULL, will panic.
Runtime_Reflection_Type *Get_Type_Reflection_By_Name(String type_name);

// this function might return NULL.
Runtime_Reflection_Type *Maybe_Get_Type_Reflection_By_Name(String type_name);

// will panic if:
//    1. the type passed in was not a struct.
//    2. the field dose not exist
Runtime_Reflection_Field *      Get_Field_By_Name(Runtime_Reflection_Type *struct_type, String field_name);
// will panic if:
//    1. the type passed in was not a struct.
//
// might return null, if no field with that name exists
Runtime_Reflection_Field *Maybe_Get_Field_By_Name(Runtime_Reflection_Type *struct_type, String field_name);



// will panic if:
//    1. type is not a signed integer,
//    2. size is not 8, 16, 32 or 64 bit, handle other sizes yourself.
s64 Type_As_Signed_Integer(Runtime_Reflection_Type *type, void *value);
// will panic if:
//    1. type is not an unsigned integer,
//    2. size is greater than 64-bits, handle larger types yourself.
u64 Type_As_Unsigned_Integer(Runtime_Reflection_Type *type, void *value);
// will panic if:
//    1. type is not a float
//    2. size is not 32-bit or 64-bit, no 128-bit floats allowed.
f64 Type_As_Float(Runtime_Reflection_Type *type, void *value);


typedef struct {
    // pass this allocator to the string builder,
    // also where the resulting string is allocated.
    Arena *allocator;
} Generic_sprint_Opt;

// has optional arguments: Generic_sprint_Opt
#define Generic_sprint(Type, value_ptr, ...) Generic_sprint_by_type(Reflect(Type), value_ptr, (Generic_sprint_Opt){ __VA_ARGS__ })

String Generic_sprint_by_type(Runtime_Reflection_Type *type, void *value_ptr, Generic_sprint_Opt options);


// serialize type into human readable format.
//
// accepts a string builder to put data into,
//
// returns the amount of data put in.
#define Generic_serialize_human_readable(sb, Type, value_ptr) Generic_serialize_human_readable_by_type(sb, Reflect(Type), value_ptr)
u64 Generic_serialize_human_readable_by_type(String_Builder *sb, Runtime_Reflection_Type *type, void *value_ptr);


typedef struct {
    // if the data structure requires pointers to be allocated,
    // or has any unknown length objects, use this allocator,
    // if allocator is NULL, uses BESTED_MALLOC()
    Arena *allocator;
} Generic_deserialize_human_readable_Options;

// deserialize data that was serialized by Generic_serialize_human_readable,
//
// returns if it was a success
#define Generic_deserialize_human_readable(input_data, Type, output, ...) Generic_deserialize_human_readable_by_type(input_data, Reflect(Type), output, (Generic_deserialize_human_readable_Options){ __VA_ARGS__ })
bool Generic_deserialize_human_readable_by_type(String input_data, Runtime_Reflection_Type *type, void *output, Generic_deserialize_human_readable_Options options);



// checks if 2 things are equal, using their type info to check
// stuff like the real value of stuff at pointers
#define Generic_deep_equal(Type, a_ptr, b_ptr) (false && "TODO: Generic_deep_equal")



// internal stuff
Runtime_Reflection_Type *Begin_New_Type_Internal(String type_name, u64 size_in_bytes, u64 alignment);
void Add_Field_Internal(Runtime_Reflection_Type *result, String field_type_name, String field_name, u64 offset);


#endif // RUNTIME_REFLECTION_H






#ifdef RUNTIME_REFLECTION_IMPLEMENTATION

global_variable Runtime_Reflection_Type_Array global_runtime_reflection_array = ZEROED;

global_variable bool runtime_reflection_has_been_initalized = false;


internal void assert_runtime_reflection_has_been_initalized(void) {
    if (runtime_reflection_has_been_initalized) return;

    PANIC("Runtime Reflection has not been initalized!!! remember to call Initialize_Runtime_Reflection() at the start of your program.");
}

internal const char *RRTK_to_string(Runtime_Reflection_Type_Kind kind) {
    switch (kind) {
    case RRTK_NULL: return "RRTK_NULL";

    case RRTK_number: return "number";
    case RRTK_bool:   return "bool";
    case RRTK_c_str:  return "c_str";
    case RRTK_void_pointer: return "void_pointer";
    case RRTK_struct: return "struct";

    default: return temp_sprintf("(Unknown{%d})", kind);
    }
}



// for CHAR_MIN, to determine if char is signed or not.
#include "limits.h"

internal void initialize_dumb_c_types(void);

void Initialize_Runtime_Reflection(Arena *allocator) {
    if (runtime_reflection_has_been_initalized) {
        PANIC("Trying to initialize runtime reflection twice, don't do that. Have better code structure.");
    }
    runtime_reflection_has_been_initalized = true;


    Mem_Zero_Struct(&global_runtime_reflection_array);
    global_runtime_reflection_array.allocator = allocator;

    // NOTE it might be better to use the Begin_New_Type() method for these...

    // X(Type, is_integer, is_signed)
    #define BESTED_NUMBER_TYPES     \
        X(u8, true, false)          \
        X(u16, true,  false)        \
        X(u32, true,  false)        \
        X(u64, true,  false)        \
                                    \
        X(s8 , true,  true)         \
        X(s16, true,  true)         \
        X(s32, true,  true)         \
        X(s64, true,  true)         \
                                    \
        X(f32, false, false)        \
        X(f64, false, false)        \

    #define X(Type, is_integer, is_signed)                      \
        {                                                       \
            Runtime_Reflection_Type *new_type = Array_Add(&global_runtime_reflection_array, 1);     \
            Mem_Zero_Struct(new_type);                          \
            new_type->type_name              = S(#Type);        \
            new_type->kind                   = RRTK_number;     \
            new_type->alignment              = Alignof(Type);   \
            new_type->size_in_bytes          = sizeof(Type);    \
            new_type->as_number.is_integer_type        = is_integer;      \
            new_type->as_number.is_signed_integer_type = is_signed;       \
        }

        BESTED_NUMBER_TYPES
    #undef X



    // bools are not numbers.
    Runtime_Reflection_Type type_bool = { .kind = RRTK_bool, .type_name = S("bool"), .size_in_bytes = sizeof(bool), .alignment = Alignof(bool) };
    Array_Append(&global_runtime_reflection_array, type_bool);

    Runtime_Reflection_Type type_c_str = { .kind = RRTK_c_str, .type_name = S("const char *"), .size_in_bytes = sizeof(const char *), .alignment = Alignof(const char *) };
    Array_Append(&global_runtime_reflection_array, type_c_str);

    Runtime_Reflection_Type type_void_ptr = { .kind = RRTK_void_pointer, .type_name = S("void *"), .size_in_bytes = sizeof(void *), .alignment = Alignof(void *) };
    Array_Append(&global_runtime_reflection_array, type_void_ptr);

    // add all the dumb c types after the good types.
    initialize_dumb_c_types();
}

internal void initialize_dumb_c_types(void) {

    // X(Type, is_integer, is_signed);
    #define DUMB_C_TYPES_X_MACRO                        \
        X(char                  , true,  CHAR_MIN < 0)  \
        X(signed char           , true,  true )         \
        X(unsigned char         , true,  false)         \
        X(short                 , true,  true )         \
        X(short int             , true,  true )         \
        X(signed short          , true,  true )         \
        X(signed short int      , true,  true )         \
        X(unsigned short        , true,  false)         \
        X(unsigned short int    , true,  false)         \
        X(int                   , true,  true )         \
        X(signed                , true,  true )         \
        X(signed int            , true,  true )         \
        X(unsigned              , true,  false)         \
        X(unsigned int          , true,  false)         \
        X(long                  , true,  true )         \
        X(long int              , true,  true )         \
        X(signed long           , true,  true )         \
        X(signed long int       , true,  true )         \
        X(unsigned long         , true,  false)         \
        X(unsigned long int     , true,  false)         \
        X(long long             , true,  true )         \
        X(long long int         , true,  true )         \
        X(signed long long      , true,  true )         \
        X(signed long long int  , true,  true )         \
        X(unsigned long long    , true,  false)         \
        X(unsigned long long int, true,  false)         \
        X(float                 , false, false)         \
        X(double                , false, false)         \
        X(long double           , false, false) // did you know that this is a 128-bit float?

    #define X(Type, is_integer, is_signed)                      \
        {                                                       \
            Runtime_Reflection_Type *new_type = Array_Add(&global_runtime_reflection_array, 1);     \
            Mem_Zero_Struct(new_type);                          \
            new_type->type_name              = S(#Type);        \
            new_type->kind                   = RRTK_number;     \
            new_type->alignment              = Alignof(Type);   \
            new_type->size_in_bytes          = sizeof(Type);    \
            new_type->as_number.is_integer_type        = is_integer;      \
            new_type->as_number.is_signed_integer_type = is_signed;       \
        }

        DUMB_C_TYPES_X_MACRO
    #undef X
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


Runtime_Reflection_Type *Begin_New_Type_Internal(String type_name, u64 size_in_bytes, u64 alignment) {
    assert_runtime_reflection_has_been_initalized();

    Runtime_Reflection_Type *new_type = Array_Add(&global_runtime_reflection_array, 1);

    Mem_Zero_Struct(new_type);

    // if the user doesn't set this, its gonna panic at some point.
    new_type->kind = RRTK_NULL;

    new_type->type_name = type_name;
    new_type->size_in_bytes      = size_in_bytes;
    new_type->alignment = alignment;

    new_type->fields.allocator = global_runtime_reflection_array.allocator;

    return new_type;
}



void Add_Field_Internal(Runtime_Reflection_Type *result, String field_type_name, String field_name, u64 offset) {
    assert_runtime_reflection_has_been_initalized();

    // assert that 2 fields cannot have the same name.
    Array_For_Each(Runtime_Reflection_Field, other_field, &result->fields) {
        u64 index = other_field - result->fields.items;
        if (String_Eq(other_field->name, field_name)) {
            PANIC("tried to add a field with the same name as another field, ('"S_Fmt"' at index %zu)", S_Arg(other_field->name), index);
        }
    }

    Runtime_Reflection_Field field = ZEROED;

    field.name   = field_name;
    field.type   = Get_Type_Reflection_By_Name(field_type_name);
    field.offset = offset;

    Array_Append(&result->fields, field);
}


Runtime_Reflection_Field *Get_Field_By_Name(Runtime_Reflection_Type *struct_type, String field_name) {
    // assert(struct_type); // TODO

    if (struct_type->kind != RRTK_struct) PANIC("cannot get the field of non struct type, was '%s' type", RRTK_to_string(struct_type->kind));

    Runtime_Reflection_Field *maybe_field = Maybe_Get_Field_By_Name(struct_type, field_name);
    if (maybe_field != NULL) return maybe_field;

    PANIC("field '"S_Fmt"' dose not exist on struct '"S_Fmt"'", S_Arg(field_name), S_Arg(struct_type->type_name));
}
Runtime_Reflection_Field *Maybe_Get_Field_By_Name(Runtime_Reflection_Type *struct_type, String field_name) {
    if (struct_type->kind != RRTK_struct) PANIC("cannot get the field of non struct type, was '%s' type", RRTK_to_string(struct_type->kind));

    Array_For_Each(Runtime_Reflection_Field, it, &struct_type->fields) {
        if (String_Eq(field_name, it->name)) {
            return it;
        }
    }
    return NULL;
}




s64 Type_As_Signed_Integer(Runtime_Reflection_Type *type, void *value) {
    if (type->kind != RRTK_number)                          PANIC("Cannot convert non number type into signed integer");
    if (type->as_number.is_integer_type == false)           PANIC("Cannot convert non integer type into signed integer");
    if (type->as_number.is_signed_integer_type == false)    PANIC("Cannot convert non signed integer type into signed integer");

    switch (type->size_in_bytes) {
    case sizeof(s8 ): return (s64) (*(s8 *)value);
    case sizeof(s16): return (s64) (*(s16*)value);
    case sizeof(s32): return (s64) (*(s32*)value);
    case sizeof(s64): return       (*(s64*)value);

    default: PANIC("Cannot convert a signed integer (of type '"S_Fmt"') (of size %zu-bit) into signed integer, only accepts 8, 16, 32 and 64-bit sizes", S_Arg(type->type_name), type->size_in_bytes*8);
    }
}
u64 Type_As_Unsigned_Integer(Runtime_Reflection_Type *type, void *value) {
    if (type->kind != RRTK_number)                          PANIC("Cannot convert non number type into unsigned integer");
    if (type->as_number.is_integer_type == false)           PANIC("Cannot convert non integer type into unsigned integer");
    if (type->as_number.is_signed_integer_type == true)     PANIC("Cannot convert signed integer type into unsigned integer");

    if (type->size_in_bytes > sizeof(u64)) PANIC("Cannot convert unsigned integer (of type '"S_Fmt"') (of size %zu-bit) into unsigned integer, size was bigger than 64-bit", S_Arg(type->type_name), type->size_in_bytes*8);

    u64 v = 0;
    // pretty funny trick, doesn't work with signed numbers.
    Mem_Copy(&v, value, type->size_in_bytes);
    return v;
}
f64 Type_As_Float(Runtime_Reflection_Type *type, void *value) {
    if (type->kind != RRTK_number)                          PANIC("Cannot convert non number type into float");
    if (type->as_number.is_integer_type == true)            PANIC("Cannot convert integer type into float");

    switch (type->size_in_bytes) {
    case sizeof(f32): return (f64) (*(f32*) value);
    case sizeof(f64): return       (*(f64*) value);

    default: PANIC("Cannot convert float (of type '"S_Fmt"') (of size %zu-bit) to float, accepted sizes are 32 and 64-bit", S_Arg(type->type_name), type->size_in_bytes*8);
    }

    (void) type;
    (void) value;
    TODO("Type_As_Float");
}




internal void runtime_reflection_sprint(String_Builder *sb, Runtime_Reflection_Type *type, void *value) {
    assert_runtime_reflection_has_been_initalized();

    switch (type->kind) {
    case RRTK_NULL: UNREACHABLE();

    case RRTK_number: {
        if (type->as_number.is_integer_type) {
            if (type->as_number.is_signed_integer_type) {
                s64 v = Type_As_Signed_Integer(type, value);
                String_Builder_printf(sb, "%ld", v);
            } else {
                u64 v = Type_As_Unsigned_Integer(type, value);
                String_Builder_printf(sb, "%zu", v);
            }
        } else {

            if (String_Eq(type->type_name, S("long double"))) {
                // this dumbass type could possibly be 128-bit long. why
                long double v = *(long double*) value;
                String_Builder_printf(sb, "%Lf", v);
            } else {
                f64 v = Type_As_Float(type, value);
                String_Builder_printf(sb, "%f", v);
            }


        }
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

    case RRTK_struct: {
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

    default: PANIC("Unknown Type Kind %s, ("S_Fmt")", RRTK_to_string(type->kind), S_Arg(type->type_name));
    }
}


String Generic_sprint_by_type(Runtime_Reflection_Type *type, void *value_ptr, Generic_sprint_Opt options) {
    assert_runtime_reflection_has_been_initalized();

    String_Builder sb = ZEROED;
    sb.allocator = options.allocator;

    runtime_reflection_sprint(&sb, type, value_ptr);

    String result = String_Builder_To_String(&sb);
    if (sb.allocator != NULL) String_Builder_Free(&sb);

    return result;
}





u64 Generic_serialize_human_readable_by_type(String_Builder *sb, Runtime_Reflection_Type *type, void *value_ptr) {
    (void) sb;
    (void) type;
    (void) value_ptr;

    TODO("Generic_serialize_human_readable_by_type");
}


bool Generic_deserialize_human_readable_by_type(String input_data, Runtime_Reflection_Type *type, void *output, Generic_deserialize_human_readable_Options options) {
    (void) input_data;
    (void) type;
    (void) output;
    (void) options;

    TODO("Generic_deserialize_human_readable_by_type");
}



#endif // RUNTIME_REFLECTION_IMPLEMENTATION
