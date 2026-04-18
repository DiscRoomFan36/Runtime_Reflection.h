
# Runtime_Reflection.h - Runtime Reflection in C

Gain the ability to write functions that work on any type.


## Quick Start

```bash
# compile the build script once.
$ cc -o nob nob.c

# compile and run
$ ./nob example && ./build/example
$ ./nob tests   && ./build/tests
```


## Example

```c
#include "Bested.h" // this library uses Bested.h as a dependency.

#define RUNTIME_REFLECTION_IMPLEMENTATION
#include "Runtime_Reflection.h"


typedef struct {
    s32 age;
    f64 height;
    bool has_any_loved_ones;

    String name;

    String_Array friends;
} Person;


void Generate_Person_Runtime_Reflection_Type_Info(void) {
    static_assert(sizeof(Person) == 72, "update when struct changes");

    // to use runtime reflection, you must
    // register the type you want to use
    Runtime_Reflection_Type *person_type = Begin_New_Type(Person);
    person_type->kind = RRTK_struct;

    Add_Field(person_type, Person, s32,  age);
    Add_Field(person_type, Person, f64,  height);
    Add_Field(person_type, Person, bool, has_any_loved_ones);

    Add_Field(person_type, Person, String, name);

    Add_Field(person_type, Person, String_Array, friends);
}


int main(void) {
    // this adds the basic types, must be called before anything else.
    Initialize_Runtime_Reflection(NULL);

    Generate_Person_Runtime_Reflection_Type_Info();


    Person jim = {
        .name = S("jim"),

        .age = 32,
        .height = 3.45, // in meters, obviously

        .has_any_loved_ones = false,
        .friends = ZEROED,
    };
    Array_Append(&jim.friends, S("no-one"));

    // you can now print any type you want.
    String jim_string = Generic_sprint(Person, &jim);
    printf("jim -> "S_Fmt"\n", S_Arg(jim_string));

    // turn the data into packed binary data,
    // including the strings and the arrays.
    String_Builder sb = ZEROED;
    Generic_serialize_packed_binary_format(&sb, Person, &jim);

    String jim_as_packed_binary = String_Builder_To_String(&sb);

    Person jom;
    Generic_deserialize_packed_binary_format(jim_as_packed_binary, Person, &jom);

    String jom_string = Generic_sprint(Person, &jom);
    printf("jom -> "S_Fmt"\n", S_Arg(jom_string));


    // test if 2 things are equal, better than Mem_Eq().
    bool is_equal = Generic_deep_equal(Person, &jim, &jom);
    printf("jim == jom: %s\n", is_equal ? "true" : "false");

    return 0;
}
```

