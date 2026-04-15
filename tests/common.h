
#ifndef COMMON_H
#define COMMON_H


#include "Bested.h"

#define RUNTIME_REFLECTION_IMPLEMENTATION
#include "../Runtime_Reflection.h"

#include "TEST_MA.h"


// X(Type, name)
#define COMMON_TYPES            \
    X(u8          , u8)         \
    X(s8          , s8)         \
    X(u16         , u16)        \
    X(s16         , s16)        \
    X(u32         , u32)        \
    X(s32         , s32)        \
    X(u64         , u64)        \
    X(s64         , s64)        \
                                \
    X(f32         , f32)        \
    X(f64         , f64)        \
                                \
    /* watch out for this one, bool is a macro to _Bool */    \
    X(bool        , bool)       \
                                \
    X(String      , string)     \
    X(const char *, c_str)      \
    X(void *      , void_ptr)


// X(Type, name)
//
// X(bool, a_bool) // i would never lump you in with these guys bool, your better than that.
#define MOST_NORMAL_C_TYPES                           \
    X(char                  , char                 )  \
    X(signed char           , signed_char          )  \
    X(unsigned char         , unsigned_char        )  \
                                                      \
    X(short                 , short                )  \
    X(short int             , short_int            )  \
    X(signed short          , signed_short         )  \
    X(signed short int      , signed_short_int     )  \
                                                      \
    X(unsigned short        , unsigned_short       )  \
    X(unsigned short int    , unsigned_short_int   )  \
                                                      \
    X(int                   , int                  )  \
    X(signed                , signed               )  \
    X(signed int            , signed_int           )  \
                                                      \
    X(unsigned              , unsigned             )  \
    X(unsigned int          , unsigned_int         )  \
                                                      \
    X(long                  , long                 )  \
    X(long int              , long_int             )  \
    X(signed long           , signed_long          )  \
    X(signed long int       , signed_long_int      )  \
                                                      \
    X(unsigned long         , unsigned_long        )  \
    X(unsigned long int     , unsigned_long_int    )  \
                                                      \
    X(long long             , long_long            )  \
    X(long long int         , long_long_int        )  \
    X(signed long long      , signed_long_long     )  \
    X(signed long long int  , signed_long_long_int )  \
                                                      \
    X(unsigned long long    , unsigned_long_long   )  \
    X(unsigned long long int, unsigned_long_long_in)  \
                                                      \
    X(float                 , float                )  \
    X(double                , double               )

#define ALL_NORMAL_C_TYPES                              \
    MOST_NORMAL_C_TYPES                                 \
    X(long double           , long_double) // this thing sucks, i have half a mind to just ban it.



#endif // COMMON_H
