
#include "common.h"

typedef struct {
    s64 a;
    s8  b;
    u32 c;
} Bad_Padding_Struct;

void add_bad_padding_struct_reflection(void) {
    Runtime_Reflection_Type *Bad_Padding_Struct_type = Begin_New_Type(Bad_Padding_Struct);
    Bad_Padding_Struct_type->kind = RRTK_struct;

    Add_Field(Bad_Padding_Struct_type, Bad_Padding_Struct, s64, a);
    Add_Field(Bad_Padding_Struct_type, Bad_Padding_Struct, s8,  b);
    Add_Field(Bad_Padding_Struct_type, Bad_Padding_Struct, u32, c);
}

void test_deep_equal_wont_care_about_junk_padding(void) {
    Initialize_Runtime_Reflection(NULL);

    add_bad_padding_struct_reflection();

    Bad_Padding_Struct two_bad_paddings[2] = ZEROED;

    // give the first foo some junk in padding.
    Mem_Set(&two_bad_paddings[0], -1, sizeof(Bad_Padding_Struct));

    for (u64 i = 0; i < Array_Len(two_bad_paddings); i++) {
        two_bad_paddings[i].a = 525235;
        two_bad_paddings[i].b = 5;
        two_bad_paddings[i].c = 71;
    }

    // oh no! this memory is not the same!
    TEST_EXPECT(Mem_Eq(&two_bad_paddings[0], &two_bad_paddings[1], sizeof(Bad_Padding_Struct)) == false);

    // but the generic equal is good!
    TEST_EXPECT(Generic_deep_equal(Bad_Padding_Struct, &two_bad_paddings[0], &two_bad_paddings[1]) == true);
}


void test_deep_equal_works_on_arrays_with_junk(void) {
    Initialize_Runtime_Reflection(NULL);

    add_bad_padding_struct_reflection();

    typedef Array(Bad_Padding_Struct) Bad_Padding_Struct_Array;

    Make_New_Array_Type(Bad_Padding_Struct_Array, Bad_Padding_Struct);

    Bad_Padding_Struct_Array two_arrays[2] = ZEROED;

    u64 count = 10;
    Array_Add(&two_arrays[0], count, false);
    Array_Add(&two_arrays[1], count, true);

    // mess with the memory
    Mem_Set(two_arrays[0].items, -1, sizeof(Bad_Padding_Struct) * count);

    for (size_t j = 0; j < Array_Len(two_arrays); j++) {
        for (u64 i = 0; i < count; i++) {
            two_arrays[j].items[i].a = 10;
            two_arrays[j].items[i].b = 5;
            two_arrays[j].items[i].c = 48143;
        }
    }

    // not memory equal
    TEST_EXPECT(Mem_Eq(two_arrays[0].items, two_arrays[1].items, sizeof(Bad_Padding_Struct) * count) == false);
    // but is generic equal
    TEST_EXPECT(Generic_deep_equal(Bad_Padding_Struct_Array, &two_arrays[0], &two_arrays[1]) == true);
}