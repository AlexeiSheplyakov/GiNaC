/** @file genex.cpp
 *
 *  Provides some routines for generating expressions that are later used as input
 *  in the consistency checks. */

/*
 *  GiNaC Copyright (C) 1999-2000 Johannes Gutenberg University Mainz, Germany
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

// For rand() and friends:
#include <stdlib.h>

#include "ginac.h"

#ifndef NO_NAMESPACE_GINAC
using namespace GiNaC;
#endif // ndef NO_NAMESPACE_GINAC

/* Create a dense univariate random polynomial in x.
 * (of the form 9 - 22*a - 17*a^2 + 14*a^3 + 7*a^4 + 7a^5 if degree==5) */
const ex
dense_univariate_poly(const symbol & x, unsigned degree)
{
    ex unipoly;
    
    for (unsigned i=0; i<=degree; ++i)
        unipoly += numeric((rand()-RAND_MAX/2))*pow(x,i);
    
    return unipoly;
}

/* Create a dense bivariate random polynomial in x1 and x2.
 * (of the form 9 + 52*x1 - 27*x1^2 + 84*x2 + 7*x2^2 - 12*x1*x2 if degree ==2) */
const ex
dense_bivariate_poly(const symbol & x1, const symbol & x2, unsigned degree)
{
    ex bipoly;
    
    for (unsigned i1=0; i1<=degree; ++i1)
        for (unsigned i2=0; i2<=degree-i1; ++i2)
            bipoly += numeric((rand()-RAND_MAX/2))*pow(x1,i1)*pow(x2,i2);
    
    return bipoly;
}
