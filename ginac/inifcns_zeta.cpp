/** @file inifcns_zeta.cpp
 *
 *  Implementation of the Zeta-function and some related stuff. */

/*
 *  GiNaC Copyright (C) 1999 Johannes Gutenberg University Mainz, Germany
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

#include <vector>
#include <stdexcept>

#include "inifcns.h"
#include "ex.h"
#include "constant.h"
#include "numeric.h"
#include "power.h"
#include "symbol.h"

namespace GiNaC {

//////////
// Riemann's Zeta-function
//////////

static ex zeta_eval(ex const & x)
{
    if (x.info(info_flags::numeric)) {
        numeric y = ex_to_numeric(x);
        // trap integer arguments:
        if (y.is_integer()) {
            if (y.is_zero())
                return -exHALF();
            if (!x.compare(exONE()))
                throw(std::domain_error("zeta(1): infinity"));
            if (x.info(info_flags::posint)) {
                if (x.info(info_flags::odd))
                    return zeta(x).hold();
                else
                    return abs(bernoulli(y))*pow(Pi,x)*numTWO().power(y-numONE())/factorial(y);
            } else {
                if (x.info(info_flags::odd))
                    return -bernoulli(numONE()-y)/(numONE()-y);
                else
                    return numZERO();
            }
        }
    }
    return zeta(x).hold();
}

static ex zeta_evalf(ex const & x)
{
    BEGIN_TYPECHECK
        TYPECHECK(x,numeric)
    END_TYPECHECK(zeta(x))
    
    return zeta(ex_to_numeric(x));
}

static ex zeta_diff(ex const & x, unsigned diff_param)
{
    ASSERT(diff_param==0);
    
    return exZERO();  // should return zeta(numONE(),x);
}

static ex zeta_series(ex const & x, symbol const & s, ex const & point, int order)
{
    throw(std::logic_error("don't know the series expansion of the zeta function"));
}

REGISTER_FUNCTION(zeta, zeta_eval, zeta_evalf, zeta_diff, zeta_series);

} // namespace GiNaC
