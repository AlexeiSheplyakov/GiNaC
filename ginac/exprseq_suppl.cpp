/** @file exprseq_suppl.cpp
 *
 *  Supplement to exprseq.cpp, contains the parts which were
 *  not automatically generated. */

#include "ginac.h"

bool exprseq::info(unsigned inf) const
{
    if (inf==info_flags::exprseq) return 1;
    return basic::info(inf);
}

ex & exprseq::let_op(int const i)
{
    ASSERT(i>=0);
    ASSERT(i<nops());

    return seq[i];
}

