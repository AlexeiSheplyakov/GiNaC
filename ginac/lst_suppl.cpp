/** @file lst_suppl.cpp
 *
 *  Supplement to lst.cpp, contains the parts which were
 *  not automatically generated. */

#include "ginac.h"

bool lst::info(unsigned inf) const
{
    if (inf==info_flags::list) return 1;
    return basic::info(inf);
}


