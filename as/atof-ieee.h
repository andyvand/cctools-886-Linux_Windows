#ifndef _ATOF_IEEE_H_
#define _ATOF_IEEE_H_ 1

#include "flonum.h"

extern char *atof_ieee(
    char *str,
    char what_kind,
    LITTLENUM_TYPE *words);

extern int gen_to_words(
    LITTLENUM_TYPE *words,
    int precision,
    int exponent_bits);

extern void int_to_gen(
    int x);

#endif /* _ATOF_IEEE_H_ */

