/** @file diff.cpp
 *
 *  Implementation of symbolic differentiation in all of GiNaC's classes. */

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

#include <stdexcept>

#include "basic.h"
#include "ex.h"
#include "add.h"
#include "constant.h"
#include "expairseq.h"
#include "indexed.h"
#include "inifcns.h"
#include "mul.h"
#include "ncmul.h"
#include "numeric.h"
#include "power.h"
#include "relational.h"
#include "pseries.h"
#include "symbol.h"
#include "utils.h"

#ifndef NO_GINAC_NAMESPACE
namespace GiNaC {
#endif // ndef NO_GINAC_NAMESPACE

/** Default implementation of ex::diff(). It prints and error message and returns a fail object.
 *  @see ex::diff */
ex basic::diff(const symbol & s) const
{
    throw(std::logic_error("differentiation not supported by this type"));
}


/** Implementation of ex::diff() for a numeric. It always returns 0.
 *
 *  @see ex::diff */
ex numeric::diff(const symbol & s) const
{
    return _ex0();
}


/** Implementation of ex::diff() for single differentiation of a symbol.
 *  It returns 1 or 0.
 *
 *  @see ex::diff */
ex symbol::diff(const symbol & s) const
{
    if (compare_same_type(s)) {
        return _ex0();
    } else {
        return _ex1();
    }
}

/** Implementation of ex::diff() for a constant. It always returns 0.
 *
 *  @see ex::diff */
ex constant::diff(const symbol & s) const
{
    return _ex0();
}

/** Implementation of ex::diff() for multiple differentiation of a symbol.
 *  It returns the symbol, 1 or 0.
 *
 *  @param nth order of differentiation
 *  @see ex::diff */
ex symbol::diff(const symbol & s, unsigned nth) const
{
    if (compare_same_type(s)) {
        switch (nth) {
        case 0:
            return s;
            break;
        case 1:
            return _ex1();
            break;
        default:
            return _ex0();
        }
    } else {
        return _ex1();
    }
}


/** Implementation of ex::diff() for an indexed object. It always returns 0.
 *  @see ex::diff */
ex indexed::diff(const symbol & s) const
{
        return _ex0();
}


/** Implementation of ex::diff() for an expairseq. It differentiates all elements of the sequence.
 *  @see ex::diff */
ex expairseq::diff(const symbol & s) const
{
    return thisexpairseq(diffchildren(s),overall_coeff);
}


/** Implementation of ex::diff() for a sum. It differentiates each term.
 *  @see ex::diff */
ex add::diff(const symbol & s) const
{
    // D(a+b+c)=D(a)+D(b)+D(c)
    return (new add(diffchildren(s)))->setflag(status_flags::dynallocated);
}


/** Implementation of ex::diff() for a product. It applies the product rule.
 *  @see ex::diff */
ex mul::diff(const symbol & s) const
{
    exvector new_seq;
    new_seq.reserve(seq.size());

    // D(a*b*c)=D(a)*b*c+a*D(b)*c+a*b*D(c)
    for (unsigned i=0; i!=seq.size(); i++) {
        epvector sub_seq=seq;
        sub_seq[i] = split_ex_to_pair(sub_seq[i].coeff*
                                      power(sub_seq[i].rest,sub_seq[i].coeff-1)*
                                      sub_seq[i].rest.diff(s));
        new_seq.push_back((new mul(sub_seq,overall_coeff))->setflag(status_flags::dynallocated));
    }
    return (new add(new_seq))->setflag(status_flags::dynallocated);
}


/** Implementation of ex::diff() for a non-commutative product. It always returns 0.
 *  @see ex::diff */
ex ncmul::diff(const symbol & s) const
{
    return _ex0();
}


/** Implementation of ex::diff() for a power.
 *  @see ex::diff */
ex power::diff(const symbol & s) const
{
    if (exponent.info(info_flags::real)) {
        // D(b^r) = r * b^(r-1) * D(b) (faster than the formula below)
        return mul(mul(exponent, power(basis, exponent - _ex1())), basis.diff(s));
    } else {
        // D(b^e) = b^e * (D(e)*ln(b) + e*D(b)/b)
        return mul(power(basis, exponent),
                   add(mul(exponent.diff(s), log(basis)),
                       mul(mul(exponent, basis.diff(s)), power(basis, -1))));
    }
}


/** Implementation of ex::diff() for functions. It applies the chain rule,
 *  except for the Order term function.
 *  @see ex::diff */
ex function::diff(const symbol & s) const
{
    exvector new_seq;
    
    if (serial==function_index_Order) {
        // Order Term function only differentiates the argument
        return Order(seq[0].diff(s));
    } else {
        // Chain rule
        ex arg_diff;
        for (unsigned i=0; i!=seq.size(); i++) {
            arg_diff = seq[i].diff(s);
            // We apply the chain rule only when it makes sense.  This is not
            // just for performance reasons but also to allow functions to
            // throw when differentiated with respect to one of its arguments
            // without running into trouble with our automatic full
            // differentiation:
            if (!arg_diff.is_zero())
                new_seq.push_back(mul(pdiff(i), arg_diff));
        }
    }
    return add(new_seq);
}


/** Implementation of ex::diff() for a power series. It treats the series as a polynomial.
 *  @see ex::diff */
ex pseries::diff(const symbol & s) const
{
    if (s == var) {
        epvector new_seq;
        epvector::const_iterator it = seq.begin(), itend = seq.end();
        
        // FIXME: coeff might depend on var
        while (it != itend) {
            if (is_order_function(it->rest)) {
                new_seq.push_back(expair(it->rest, it->coeff - 1));
            } else {
                ex c = it->rest * it->coeff;
                if (!c.is_zero())
                    new_seq.push_back(expair(c, it->coeff - 1));
            }
            it++;
        }
        return pseries(var, point, new_seq);
    } else {
        return *this;
    }
}


/** Compute partial derivative of an expression.
 *
 *  @param s  symbol by which the expression is derived
 *  @param nth  order of derivative (default 1)
 *  @return partial derivative as a new expression */

ex ex::diff(const symbol & s, unsigned nth) const
{
    GINAC_ASSERT(bp!=0);

    if (nth==0) {
        return *this;
    }

    ex ndiff = bp->diff(s);
    while (nth>1) {
        ndiff = ndiff.diff(s);
        --nth;
    }
    return ndiff;
}

#ifndef NO_GINAC_NAMESPACE
} // namespace GiNaC
#endif // ndef NO_GINAC_NAMESPACE
