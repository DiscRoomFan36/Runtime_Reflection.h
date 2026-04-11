//
// Runtime_Reflection.h - Runtime Reflection in C.
//
// Author   - Fletcher M
//
// Created  - 11/04/26
// Modified - 11/04/26
//
// Version  - v0.0.1
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

    RRTK_s32,
    RRTK_f64,
    RRTK_bool,

    RRTK_c_str,

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

// global_variable bool initalized_runtime_reflection = false;
global_variable Runtime_Reflection_Type_Array global_runtime_reflection_array = ZEROED;




void Initialize_Runtime_Reflection(Arena *allocator) {
    Mem_Zero_Struct(&global_runtime_reflection_array);
    global_runtime_reflection_array.allocator = allocator;

    #define BASIC_TYPE(Type) (Runtime_Reflection_Type){ .kind = RRTK_##Type, .type_name = S(#Type), .size = sizeof(Type), .alignment = Alignof(Type) }

    Array_Append(&global_runtime_reflection_array, BASIC_TYPE(s32));
    Array_Append(&global_runtime_reflection_array, BASIC_TYPE(bool));
    Array_Append(&global_runtime_reflection_array, BASIC_TYPE(f64));

    Runtime_Reflection_Type type_c_str = { .kind = RRTK_c_str, .type_name = S("const char *"), .size = sizeof(const char *), .alignment = Alignof(const char *) };
    Array_Append(&global_runtime_reflection_array, type_c_str);
}


Runtime_Reflection_Type *Maybe_Get_Type_Reflection_By_Name(String type_name) {
    // TODO this would need some help to detect c_str's, 'const char *' vs 'const char*'
    // TODO also would need to handle pointers better
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
    Runtime_Reflection_Type *type = Maybe_Get_Type_Reflection_By_Name(type_name);
    if (type != NULL) return type;

    PANIC("no type with name '"S_Fmt"' found", S_Arg(type_name));
}


Runtime_Reflection_Type *Begin_New_Type_Internal(String type_name, u64 size, u64 alignment) {
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
    Runtime_Reflection_Field field = ZEROED;

    field.name   = field_name;
    field.type   = Get_Type_Reflection_By_Name(field_type_name);
    field.offset = offset;

    Array_Append(&result->fields, field);
}




internal void runtime_reflection_sprint(String_Builder *sb, Runtime_Reflection_Type *type, void *value) {
    switch (type->kind) {
    case RRTK_NULL: UNREACHABLE();

    case RRTK_s32: {
        s32 *v = value;
        String_Builder_printf(sb, "%d", *v);
    } break;
    case RRTK_f64: {
        f64 *v = value;
        String_Builder_printf(sb, "%f", *v);
    } break;
    case RRTK_bool: {
        bool *v = value;
        String_Builder_printf(sb, "%s", (*v) ? "true" : "false");
    } break;
    case RRTK_c_str: {
        const char **v = value; // this is correct.
        String_Builder_printf(sb, "\"%s\"", *v);
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
    String_Builder sb = ZEROED;
    sb.allocator = options.allocator;

    runtime_reflection_sprint(&sb, type, value_ptr);

    String result = String_Builder_To_String(&sb);
    if (sb.allocator != NULL) String_Builder_Free(&sb);

    return result;
}


#endif // RUNTIME_REFLECTION_IMPLEMENTATION
