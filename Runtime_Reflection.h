//
// Runtime_Reflection.h - Runtime Reflection in C.
//
// Author   - Fletcher M
//
// Created  - 11/04/26
// Modified - 15/04/26
//
// Version  - v0.0.5
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
    // not a type, this is an error.
    RRTK_NULL = 0,

    // number is all integers and floats, of every size.
    //
    // use is_integer, is_signed_integer, and size_in_bytes to determine the exact type of number.
    //
    // Type_To_Float(), Type_To_Signed_Int(), Type_To_Unsigned_Int() functions work with numbers.
    RRTK_number,

    RRTK_bool,

    // corresponds to the type 'String' defined in Bested.h.
    // you should prefer using this for strings, or even buffers of data,
    RRTK_string,
    // kinda have to support this, but highly recommend String type.
    RRTK_c_str,


    // this is only for void pointer, the type
    // system has no idea what to do with this thing,
    //
    // when serialized, will just put the raw number.
    RRTK_void_pointer,

    RRTK_struct,


    // type struct should have a "pointer to this type" in them.
    // RRTK_pointer,
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

    // the name of the type.
    String name;
    // u64 type_id;

    // preallocate the space you need with this.
    u64 size_in_bytes;
    // might be useful
    u64 alignment;


    // things that matter if you are RRTK_number.
    //
    // but should be zero initalized so you can kind of use it anyway?
    struct {
        // int or not int (float if allready number)
        bool is_integer_type;
        // signed int or not (unsigned int if allready integer)
        bool is_signed_integer_type;
    };


    // TODO add a bool is_mem_cmp_safe; variable, so structs with
    // only number's / void *'s and bool's can be Mem_Eq() and Mem_Copy()'d quickly


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
#define Add_Field(struct_type_ptr, Struct_Type, Field_Type, field)      \
    Add_Field_Internal(struct_type_ptr, S(#Field_Type), S(#field), offsetof(Struct_Type, field))


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



//
// Generic binary format serialization and deserialization.
// slap this baby straight into a file and be done with it.
//
#define Generic_binary_format_serialize(sb, Type, value_ptr)    Generic_binary_format_serialize_by_type((sb), Reflect(Type), (value_ptr))
void Generic_binary_format_serialize_by_type(String_Builder *sb, Runtime_Reflection_Type *type, void *value_ptr);

// TODO accept an allocator.
#define Generic_binary_format_deserialize(input, Type, output)    Generic_binary_format_deserialize_by_type((input), Reflect(Type), (output))
const char *Generic_binary_format_deserialize_by_type(String input, Runtime_Reflection_Type *type, void *output);



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
// returns NULL on success,
// returns an error string on error.
#define Generic_deserialize_human_readable(input_data, Type, output, ...) Generic_deserialize_human_readable_by_type(input_data, Reflect(Type), output, (Generic_deserialize_human_readable_Options){ __VA_ARGS__ })
const char *Generic_deserialize_human_readable_by_type(String input_data, Runtime_Reflection_Type *type, void *output, Generic_deserialize_human_readable_Options options);



// checks if 2 things are equal, using their type info to check
// stuff like the real value of stuff at pointers and strings.
//
// will ignore all data in struct padding sections, or any
// struct fields not registered in the type system.
#define Generic_deep_equal(Type, a_ptr, b_ptr) (false && "TODO: Generic_deep_equal")
bool Generic_deep_equal_by_type(Runtime_Reflection_Type *type, void *a, void *b);



// TODO functions to make:
//     - separate binary serialization into packed and versioned versions.
//     - make C-Struct String to type, want to make a metaprogram
//       that scans a c file and makes the generate functions automatically.



// internal stuff, use the macro versions of this stuff
//
// TODO maybe expose this to the user, so they can make types that dont exist?
Runtime_Reflection_Type *Begin_New_Type_Internal(String type_name, u64 size_in_bytes, u64 alignment);
void Add_Field_Internal(Runtime_Reflection_Type *struct_type, String field_type_name, String field_name, u64 offset);


#endif // RUNTIME_REFLECTION_H






#ifdef RUNTIME_REFLECTION_IMPLEMENTATION


// for CHAR_MIN, to determine if char is signed or not.
#include "limits.h"


global_variable Runtime_Reflection_Type_Array runtime_reflection_type_array = ZEROED;

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
    case RRTK_string: return "string";
    case RRTK_c_str:  return "c_str";
    case RRTK_void_pointer: return "void_pointer";
    case RRTK_struct: return "struct";

    default: return temp_sprintf("(Unknown{%d})", kind);
    }
}




internal void initialize_dumb_c_types(void);

void Initialize_Runtime_Reflection(Arena *allocator) {
    if (runtime_reflection_has_been_initalized) {
        PANIC("Trying to initialize runtime reflection twice, don't do that. Have better code structure.");
    }
    runtime_reflection_has_been_initalized = true;


    Mem_Zero_Struct(&runtime_reflection_type_array);
    runtime_reflection_type_array.allocator = allocator;

    // NOTE it might be better to use the Begin_New_Type() method for these...

    // this is a pretty sweet macro
    #define TYPE_IS_SIGNED(Type) (((Type)-1) < 0)


    // X(Type, is_integer)
    #define BESTED_NUMBER_TYPES     \
        X(u8 , true)        \
        X(u16, true)        \
        X(u32, true)        \
        X(u64, true)        \
                            \
        X(s8 , true)        \
        X(s16, true)        \
        X(s32, true)        \
        X(s64, true)        \
                            \
        X(f32, false)       \
        X(f64, false)       \

    #define X(Type, is_integer)                      \
        {                                                       \
            Runtime_Reflection_Type *new_type = Array_Add(&runtime_reflection_type_array, 1);     \
            Mem_Zero_Struct(new_type);                          \
            new_type->name                   = S(#Type);        \
            new_type->kind                   = RRTK_number;     \
            new_type->alignment              = Alignof(Type);   \
            new_type->size_in_bytes          = sizeof(Type);    \
            new_type->is_integer_type        = is_integer;      \
            new_type->is_signed_integer_type = (is_integer) && TYPE_IS_SIGNED(Type);      \
        }

        BESTED_NUMBER_TYPES
    #undef X


    { // bools
        // bools are not numbers.
        Runtime_Reflection_Type type_bool = { .kind = RRTK_bool, .name = S("bool"), .size_in_bytes = sizeof(bool), .alignment = Alignof(bool) };
        Array_Append(&runtime_reflection_type_array, type_bool);

        static_assert(sizeof(_Bool) == sizeof(bool), "both should exist, and both should be the same.");
        // this one is also here. kinda pointless though
        Runtime_Reflection_Type type__Bool = { .kind = RRTK_bool, .name = S("_Bool"), .size_in_bytes = sizeof(bool), .alignment = Alignof(bool) };
        Array_Append(&runtime_reflection_type_array, type__Bool);
    }

    Runtime_Reflection_Type type_string = { .kind = RRTK_string, .name = S("String"), .size_in_bytes = sizeof(String), .alignment = Alignof(String) };
    Array_Append(&runtime_reflection_type_array, type_string);

    Runtime_Reflection_Type type_c_str = { .kind = RRTK_c_str, .name = S("const char *"), .size_in_bytes = sizeof(const char *), .alignment = Alignof(const char *) };
    Array_Append(&runtime_reflection_type_array, type_c_str);



    Runtime_Reflection_Type type_void_ptr = { .kind = RRTK_void_pointer, .name = S("void *"), .size_in_bytes = sizeof(void *), .alignment = Alignof(void *) };
    Array_Append(&runtime_reflection_type_array, type_void_ptr);



    // add all the dumb c types after the good types.
    initialize_dumb_c_types();
}

internal void initialize_dumb_c_types(void) {

    // X(Type, is_integer);
    #define DUMB_C_TYPES_X_MACRO            \
        X(char                  , true)     \
        X(signed char           , true)     \
        X(unsigned char         , true)     \
                                            \
        X(short                 , true)     \
        X(short int             , true)     \
        X(signed short          , true)     \
        X(signed short int      , true)     \
                                            \
        X(unsigned short        , true)     \
        X(unsigned short int    , true)     \
                                            \
        X(int                   , true)     \
        X(signed                , true)     \
        X(signed int            , true)     \
                                            \
        X(unsigned              , true)     \
        X(unsigned int          , true)     \
                                            \
        X(long                  , true)     \
        X(long int              , true)     \
        X(signed long           , true)     \
        X(signed long int       , true)     \
                                            \
        X(unsigned long         , true)     \
        X(unsigned long int     , true)     \
                                            \
        X(long long             , true)     \
        X(long long int         , true)     \
        X(signed long long      , true)     \
        X(signed long long int  , true)     \
                                            \
        X(unsigned long long    , true)     \
        X(unsigned long long int, true)     \
                                            \
        X(size_t                , true)     \
        X(ssize_t               , true)     \
        X(ptrdiff_t             , true)     \
                                            \
        X(float                 , false)    \
        X(double                , false)    \
        X(long double           , false) // did you know that this is a 128-bit float?

    #define X(Type, is_integer)                                 \
        {                                                       \
            Runtime_Reflection_Type *new_type = Array_Add(&runtime_reflection_type_array, 1);     \
            Mem_Zero_Struct(new_type);                          \
            new_type->name                   = S(#Type);        \
            new_type->kind                   = RRTK_number;     \
            new_type->alignment              = Alignof(Type);   \
            new_type->size_in_bytes          = sizeof(Type);    \
            new_type->is_integer_type        = is_integer;      \
            new_type->is_signed_integer_type = (is_integer) && TYPE_IS_SIGNED(Type);  \
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
    Array_For_Each(Runtime_Reflection_Type, it, &runtime_reflection_type_array) {
        if (String_Eq(type_name, it->name)) {
            if (it->kind == RRTK_NULL) {
                PANIC("The Type you were trying to get ('"S_Fmt"') did not set its kind, the first thing you should do when mking a new type is setting the kind. See the Runtime_Reflection_Type_Kind for possible values.", S_Arg(it->name));
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

    Runtime_Reflection_Type *new_type = Array_Add(&runtime_reflection_type_array, 1);

    Mem_Zero_Struct(new_type);

    // if the user doesn't set this, its gonna panic at some point.
    new_type->kind = RRTK_NULL;

    // user could be doing some funky stuff, allow it.
    new_type->name          = String_Duplicate(runtime_reflection_type_array.allocator, type_name);
    new_type->size_in_bytes = size_in_bytes;
    new_type->alignment     = alignment;

    new_type->fields.allocator = runtime_reflection_type_array.allocator;

    return new_type;
}



void Add_Field_Internal(Runtime_Reflection_Type *struct_type, String field_type_name, String field_name, u64 offset) {
    assert_runtime_reflection_has_been_initalized();

    if (struct_type->kind != RRTK_struct) {
        if (struct_type->kind == RRTK_NULL) {
            PANIC("tried to add a field to something that has not set its kind yet, make sure to set the kind of '"S_Fmt"' to RRTK_struct before adding any fields.", S_Arg(struct_type->name));
        } else {
            PANIC("tried to add a field to the '"S_Fmt"' type, which isn't a struct, was %s", S_Arg(struct_type->name), RRTK_to_string(struct_type->kind));
        }
    }

    // assert that 2 fields cannot have the same name.
    Array_For_Each(Runtime_Reflection_Field, other_field, &struct_type->fields) {
        u64 index = other_field - struct_type->fields.items;
        if (String_Eq(other_field->name, field_name)) {
            PANIC("tried to add a field with the same name as another field, ('"S_Fmt"' at index %zu)", S_Arg(other_field->name), index);
        }
    }

    Runtime_Reflection_Field field = ZEROED;

    // user could be doing some funky stuff, allow it.
    field.name   = String_Duplicate(runtime_reflection_type_array.allocator, field_name);
    field.type   = Get_Type_Reflection_By_Name(field_type_name);
    field.offset = offset;

    Array_Append(&struct_type->fields, field);
}


Runtime_Reflection_Field *Get_Field_By_Name(Runtime_Reflection_Type *struct_type, String field_name) {
    // assert(struct_type); // TODO

    if (struct_type->kind != RRTK_struct) PANIC("cannot get the field of non struct type, was '%s' type", RRTK_to_string(struct_type->kind));

    Runtime_Reflection_Field *maybe_field = Maybe_Get_Field_By_Name(struct_type, field_name);
    if (maybe_field != NULL) return maybe_field;

    PANIC("field '"S_Fmt"' dose not exist on struct '"S_Fmt"'", S_Arg(field_name), S_Arg(struct_type->name));
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
    if (type->kind != RRTK_number)              PANIC("Cannot convert non number type into signed integer");
    if (type->is_integer_type == false)         PANIC("Cannot convert non integer type into signed integer");
    if (type->is_signed_integer_type == false)  PANIC("Cannot convert non signed integer type into signed integer");

    switch (type->size_in_bytes) {
    case sizeof(s8 ): return (s64) (*(s8 *)value);
    case sizeof(s16): return (s64) (*(s16*)value);
    case sizeof(s32): return (s64) (*(s32*)value);
    case sizeof(s64): return       (*(s64*)value);

    default: PANIC("Cannot convert a signed integer (of type '"S_Fmt"') (of size %zu-bit) into signed integer, only accepts 8, 16, 32 and 64-bit sizes", S_Arg(type->name), type->size_in_bytes*8);
    }
}
u64 Type_As_Unsigned_Integer(Runtime_Reflection_Type *type, void *value) {
    if (type->kind != RRTK_number)              PANIC("Cannot convert non number type into unsigned integer");
    if (type->is_integer_type == false)         PANIC("Cannot convert non integer type into unsigned integer");
    if (type->is_signed_integer_type == true)   PANIC("Cannot convert signed integer type into unsigned integer");

    if (type->size_in_bytes > sizeof(u64))      PANIC("Cannot convert unsigned integer (of type '"S_Fmt"') (of size %zu-bit) into unsigned integer, size was bigger than 64-bit", S_Arg(type->name), type->size_in_bytes*8);

    u64 v = 0;
    // pretty funny trick, doesn't work with signed numbers.
    Mem_Copy(&v, value, type->size_in_bytes);
    return v;
}
f64 Type_As_Float(Runtime_Reflection_Type *type, void *value) {
    if (type->kind != RRTK_number)              PANIC("Cannot convert non number type into float");
    if (type->is_integer_type == true)          PANIC("Cannot convert integer type into float");

    switch (type->size_in_bytes) {
    case sizeof(f32): return (f64) (*(f32*) value);
    case sizeof(f64): return       (*(f64*) value);

    default: PANIC("Cannot convert float (of type '"S_Fmt"') (of size %zu-bit) to float, accepted sizes are 32 and 64-bit", S_Arg(type->name), type->size_in_bytes*8);
    }
}




internal void runtime_reflection_sprint(String_Builder *sb, Runtime_Reflection_Type *type, void *value) {
    assert_runtime_reflection_has_been_initalized();

    switch (type->kind) {
    case RRTK_NULL: UNREACHABLE();

    case RRTK_number: {
        if (type->is_integer_type) {
            if (type->is_signed_integer_type) {
                s64 v = Type_As_Signed_Integer(type, value);
                String_Builder_printf(sb, "%ld", v);
            } else {
                u64 v = Type_As_Unsigned_Integer(type, value);
                String_Builder_printf(sb, "%zu", v);
            }
        } else {

            if (String_Eq(type->name, S("long double"))) {
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

    case RRTK_string: {
        String *v = value;
        String_Builder_printf(sb, "\"" S_Fmt "\"", S_Arg(*v));
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
        String_Builder_printf(sb, "("S_Fmt"){ ", S_Arg(type->name));

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

    default: PANIC("Unknown Type Kind %s, ("S_Fmt")", RRTK_to_string(type->kind), S_Arg(type->name));
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




typedef struct {
    u32 magic_number;
    u32 version_number;

    // TODO maybe have struct name in here as well?
    // or just leave that up to the user?
} Generic_Binary_Format_Header;

internal void Generic_serialize_to_binary_format_recur(String_Builder *sb, Runtime_Reflection_Type *type, void *value_ptr) {
    switch (type->kind) {
    case RRTK_NULL: PANIC("RRTK_NULL in type '"S_Fmt"'", S_Arg(type->name));

    case RRTK_number: {
        String_Builder_Ptr_And_Size(sb, value_ptr, type->size_in_bytes);
    } break;

    case RRTK_bool: {
        // TODO is this safe? might be better to put a known value...
        String_Builder_Ptr_And_Size(sb, value_ptr, type->size_in_bytes);
    } break;

    case RRTK_string:
    case RRTK_c_str: {

        String as_string;
        if (type->kind == RRTK_string) {
            String *v = value_ptr;
            as_string = *v;
        } else {
            // just turn it into a known size string.
            const char *v = *((const char **)value_ptr);
            as_string = S(v);
        }

        String_Builder_Ptr_And_Size(sb, (void*) &as_string.length, sizeof(as_string.length));
        String_Builder_Ptr_And_Size(sb, as_string.data, as_string.length);
    } break;

    case RRTK_void_pointer: {
        // i guess they want the bytes?
        String_Builder_Ptr_And_Size(sb, value_ptr, sizeof(void*));
    } break;

    case RRTK_struct: {
        Array_For_Each(Runtime_Reflection_Field, field, &type->fields) {
            void *field_value_ptr = value_ptr + field->offset;
            Generic_serialize_to_binary_format_recur(sb, field->type, field_value_ptr);
        }
    } break;

    default: UNREACHABLE();
    }
}

#define GENERIC_BINARY_FORMAT_MAGIC_NUMBER (0xcafebabe)

void Generic_binary_format_serialize_by_type(String_Builder *sb, Runtime_Reflection_Type *type, void *value_ptr) {
    Generic_Binary_Format_Header header = {
        .magic_number = GENERIC_BINARY_FORMAT_MAGIC_NUMBER,
        .version_number = 1,
    };

    String_Builder_Ptr_And_Size(sb, (void*) &header, sizeof(header));

    Generic_serialize_to_binary_format_recur(sb, type, value_ptr);
}




const char *Generic_deserialize_to_binary_format_recur(String *input, Runtime_Reflection_Type *type, void *output, Arena *allocator) {
    switch (type->kind) {
    case RRTK_NULL: PANIC("RRTK_NULL in type '"S_Fmt"'", S_Arg(type->name));

    case RRTK_number:
    case RRTK_bool:
    case RRTK_void_pointer: { // i guess they just want the bytes?
        if (input->length < type->size_in_bytes) {
            return temp_sprintf("Input is to small, wanted to parse %zu, only has %zu", type->size_in_bytes, input->length);
        }
        Mem_Move(output, input->data, type->size_in_bytes);
        String_Advance(input, type->size_in_bytes);
    } break;

    case RRTK_string:
    case RRTK_c_str: {

        if (input->length < sizeof(u64)) {
            return temp_sprintf("Input is to small, wanted to parse %zu, only has %zu", type->size_in_bytes, input->length);
        }

        u64 string_length = *((u64*)input->data);
        String_Advance(input, sizeof(string_length));

        if (input->length < string_length) {
            return temp_sprintf("Input is to small to hold string of length %zu, was %zu", string_length, input->length);
        }

        String the_string = { input->data, string_length };
        String resulting_string = String_Duplicate(allocator, the_string, .null_terminate = true);


        if (type->kind == RRTK_string) {
            Mem_Copy(output, &resulting_string, sizeof(String));
        } else {
            // put the pointer to the data into place.
            Mem_Copy(output, &resulting_string.data, sizeof(const char *));
        }

        String_Advance(input, string_length);
    } break;

    case RRTK_struct: {
        Array_For_Each(Runtime_Reflection_Field, field, &type->fields) {
            void *output_value_ptr = output + field->offset;
            const char *result = Generic_deserialize_to_binary_format_recur(input, field->type, output_value_ptr, allocator);
            if (result != NULL) return result;
        }
    } break;

    default: UNREACHABLE();
    }

    // serialization

    return NULL;
}

const char *Generic_binary_format_deserialize_by_type(String input, Runtime_Reflection_Type *type, void *output) {
    if (input.length < sizeof(Generic_Binary_Format_Header)) {
        return temp_sprintf("Header was to small, expected something at least %zu long, was %zu", sizeof(Generic_Binary_Format_Header), input.length);
    }

    Generic_Binary_Format_Header *header = (void*) input.data;
    if (header->magic_number != GENERIC_BINARY_FORMAT_MAGIC_NUMBER) {
        return temp_sprintf("Header magic number was incorrect, expected %d, was %d", GENERIC_BINARY_FORMAT_MAGIC_NUMBER, header->magic_number);
    }

    if (header->version_number != 1) {
        return temp_sprintf("Unrecognised Header Version number, expected %d, got %u", 1, header->version_number);
    }


    String_Advance(&input, sizeof(Generic_Binary_Format_Header));

    // TODO make a parameter.
    Arena *allocator = NULL;

    // this clears any junk bits, binary serialization should not
    // care about stuff in the padding of the struct.
    //
    // if issues arise because of this, you only
    // have your pour progarming to blame.
    Mem_Zero(output, type->size_in_bytes);

    const char *err = Generic_deserialize_to_binary_format_recur(&input, type, output, allocator);
    if (err != NULL) return err;

    if (input.length != 0) {
        return temp_sprintf("Junk found at end of file, %zu bytes left after parseing entire type", input.length);
    }

    return NULL;
}






u64 Generic_serialize_human_readable_by_type(String_Builder *sb, Runtime_Reflection_Type *type, void *value_ptr) {
    // step 1, version number. but skip for now.

    // step 2, loop over every field, and put down if there

    if (type->kind != RRTK_struct) TODO("handle when passed in something that wasn't a struct.");

    u64 bytes_output = 0;

    Array_For_Each(Runtime_Reflection_Field, field, &type->fields) {
        // u64 index = field - type->fields.items;

        // "# [struct_name].[field_name] - [field_type]""
        bytes_output += String_Builder_printf(sb, "# "S_Fmt"."S_Fmt" - "S_Fmt"\n", S_Arg(type->name), S_Arg(field->name), S_Arg(field->type->name));

        void *field_value_ptr = value_ptr + field->offset;
        switch (field->type->kind) {
            // TODO maybe a "validate_type() function"
            case RRTK_NULL: PANIC("RRTK_NULL in type '"S_Fmt"'", S_Arg(field->type->name));

            case RRTK_number: {
                if (field->type->is_integer_type) {
                    if (field->type->is_signed_integer_type) {
                        bytes_output += String_Builder_printf(sb, "%ld\n", Type_As_Signed_Integer(field->type, field_value_ptr));
                    } else {
                        bytes_output += String_Builder_printf(sb, "%zu\n", Type_As_Unsigned_Integer(field->type, field_value_ptr));
                    }
                } else {
                    // float
                    f64 field_value = Type_As_Float(field->type, field_value_ptr);

                    // we could maybe skip this if the size of the float cannot print well.
                    bytes_output += String_Builder_printf(sb, "%f - ", field_value);

                    // we also supply a hex value because float printing decoding is not perfect
                    for (u64 i = 0; i < field->type->size_in_bytes; i++) {
                        u8 byte = *(u8*)(field_value_ptr + i);
                        // TODO check if this is right

                        // this puts it down in little endian... should probably fix this...
                        bytes_output += String_Builder_printf(sb, "%02x", byte);
                    }
                    bytes_output += String_Builder_printf(sb, "\n");
                }
            } break;

            case RRTK_bool:         TODO("Generic_serialize: RRTK_bool");
            case RRTK_string:       TODO("Generic_serialize: RRTK_string");
            case RRTK_c_str:        TODO("Generic_serialize: RRTK_c_str");
            case RRTK_void_pointer: TODO("Generic_serialize: RRTK_void_pointer");
            case RRTK_struct:       TODO("Generic_serialize: RRTK_struct");

            default: UNREACHABLE();
        }

        // extra newline
        bytes_output += String_Builder_printf(sb, "\n");
    }

    return bytes_output;
}


const char *Generic_deserialize_human_readable_by_type(String input_data, Runtime_Reflection_Type *type, void *output, Generic_deserialize_human_readable_Options options) {
    (void) input_data;
    (void) options;

    // step 1, go versioning stuff. skip for now.

    if (type->kind != RRTK_struct) TODO("handle non-struct types in deserialize");

    u64 line_number = 0;

    Array_For_Each(Runtime_Reflection_Field, field, &type->fields) {

        void *field_value_ptr = output + field->offset;

        switch (field->type->kind) {
            case RRTK_NULL: PANIC("RRTK_NULL in type '"S_Fmt"'", S_Arg(field->type->name));

            case RRTK_number: {
                if (type->is_integer_type) {
                    if (type->is_signed_integer_type) {

                        // remove comments gets rid of things that start with '#'
                        String line = String_Get_Next_Line(&input_data, &line_number, SGNL_Skip_Empty | SGNL_Remove_Comments | SGNL_Trim);

                        // line should contain only numbers, maybe '-' sign

                        s64 result = atoll(temp_String_To_C_Str(line));

                        TODO("make sure this number fits");
                        Mem_Copy(field_value_ptr, &result, field->type->size_in_bytes);

                        TODO("signed");
                    } else {
                        TODO("unsigned");
                    }
                } else {
                    // float
                    TODO("float");
                }
            } break;

            case RRTK_bool:         TODO("Generic_deserialize: RRTK_bool");
            case RRTK_string:       TODO("Generic_serialize: RRTK_string");
            case RRTK_c_str:        TODO("Generic_deserialize: RRTK_c_str");
            case RRTK_void_pointer: TODO("Generic_deserialize: RRTK_void_pointer");
            case RRTK_struct:       TODO("Generic_deserialize: RRTK_struct");

            default: UNREACHABLE();
        }

        // String_Get_Next_Line(input_data, &line_number, SGNL_Skip_Empty);
    }

    return NULL;
}






bool Generic_deep_equal_by_type(Runtime_Reflection_Type *type, void *a, void *b) {
    // this would be an easy function to switch to stack based recursion.

    switch (type->kind) {
        case RRTK_NULL: PANIC("RRTK_NULL in type '"S_Fmt"'", S_Arg(type->name));

        case RRTK_number:
        case RRTK_bool:
        case RRTK_void_pointer: {
            // just check the raw memory,
            return Mem_Eq(a, b, type->size_in_bytes);
        }

        case RRTK_string:
        case RRTK_c_str: {
            String string_a;
            String string_b;
            if (type->kind == RRTK_string) {
                string_a = *(String*)a;
                string_b = *(String*)b;
            } else {
                const char **c_str_a = a;
                const char **c_str_b = b;

                // I know this is inefficient, I dont care
                // about c strings, use a better string type.
                string_a = S(*c_str_a);
                string_b = S(*c_str_b);
            }
            // this is also pretty much a Mem_Eq under the hood.
            return String_Eq(string_a, string_b);
        }

        case RRTK_struct: {
            Array_For_Each(Runtime_Reflection_Field, field, &type->fields) {
                void *field_a = a + field->offset;
                void *field_b = b + field->offset;

                // short circuiting in action.
                if (!Generic_deep_equal_by_type(field->type, field_a, field_b)) return false;
            }
            return true;
        }

        default: UNREACHABLE();
    }
}





#endif // RUNTIME_REFLECTION_IMPLEMENTATION
