
#include "../Runtime_Reflection.h"



typedef struct {
    s32 age;
    f64 height;
    bool has_any_loved_ones;

    String name;
} Person;


void Generate_Person_Runtime_Reflection_Type_Info(void) {
    static_assert(sizeof(Person) == 40, "update when struct changes");

    Runtime_Reflection_Type *person_type = Begin_New_Type(Person);
    person_type->kind = RRTK_struct;

    Add_Field(person_type, Person, s32,  age);
    Add_Field(person_type, Person, f64,  height);
    Add_Field(person_type, Person, bool, has_any_loved_ones);

    Add_Field(person_type, Person, String, name);
}



int main(void) {
    Initialize_Runtime_Reflection(NULL);

    Generate_Person_Runtime_Reflection_Type_Info();


    Person jim = {
        .name = S("jim"),

        .age = 32,
        .height = 3.45, // in meters, obviously

        .has_any_loved_ones = false,
    };

    String jim_string = Generic_sprint(Person, &jim);

    printf("result -> "S_Fmt"\n", S_Arg(jim_string));

    // Serialize_To_File("./build/jim.txt", Person, &jim);
    // Person new_jim;
    // bool success = Serialize_From_File("./build/jim.txt", Person, &new_jim);

    return 0;
}




////////////////////////////////////////////
//             final includes
////////////////////////////////////////////

#define BESTED_IMPLEMENTATION
#include "Bested.h"


#define RUNTIME_REFLECTION_IMPLEMENTATION
#include "../Runtime_Reflection.h"

