/** @file diff.cpp
 *
 *  Implementation of symbolic differentiation in all of GiNaC's classes. */

#include <stdexcept>

#include "ginac.h"

/** Default implementation of ex::diff(). It prints and error message and returns a fail object.
 *  @see ex::diff */
ex basic::diff(symbol const & s) const
{
    throw(std::logic_error("differentiation not supported by this type"));
}


/** Implementation of ex::diff() for a numeric. It always returns 0.
 *
 *  @see ex::diff */
ex numeric::diff(symbol const & s) const
{
    return exZERO();
}


/** Implementation of ex::diff() for single differentiation of a symbol.
 *  It returns 1 or 0.
 *
 *  @see ex::diff */
ex symbol::diff(symbol const & s) const
{
    if (compare_same_type(s)) {
        return exZERO();
    } else {
        return exONE();
    }
}

/** Implementation of ex::diff() for a constant. It always returns 0.
 *
 *  @see ex::diff */
ex constant::diff(symbol const & s) const
{
    return exZERO();
}

/** Implementation of ex::diff() for multiple differentiation of a symbol.
 *  It returns the symbol, 1 or 0.
 *
 *  @param nth order of differentiation
 *  @see ex::diff */
ex symbol::diff(symbol const & s, unsigned nth) const
{
    if (compare_same_type(s)) {
        switch (nth) {
        case 0:
            return s;
            break;
        case 1:
            return exONE();
            break;
        default:
            return exZERO();
        }
    } else {
        return exONE();
    }
}


/** Implementation of ex::diff() for an indexed object. It always returns 0.
 *  @see ex::diff */
ex indexed::diff(symbol const & s) const
{
        return exZERO();
}


/** Implementation of ex::diff() for an expairseq. It differentiates all elements of the sequence.
 *  @see ex::diff */
ex expairseq::diff(symbol const & s) const
{
    return thisexpairseq(diffchildren(s),overall_coeff);
}


/** Implementation of ex::diff() for a sum. It differentiates each term.
 *  @see ex::diff */
ex add::diff(symbol const & s) const
{
    // D(a+b+c)=D(a)+D(b)+D(c)
    return (new add(diffchildren(s)))->setflag(status_flags::dynallocated);
}


/** Implementation of ex::diff() for a product. It applies the product rule.
 *  @see ex::diff */
ex mul::diff(symbol const & s) const
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
ex ncmul::diff(symbol const & s) const
{
    return exZERO();
}


/** Implementation of ex::diff() for a power.
 *  @see ex::diff */
ex power::diff(symbol const & s) const
{
    if (exponent.info(info_flags::real)) {
        // D(b^r) = r * b^(r-1) * D(b) (faster than the formula below)
        return mul(mul(exponent, power(basis, exponent - exONE())), basis.diff(s));
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
ex function::diff(symbol const & s) const
{
    exvector new_seq;

    if (serial == function_index_Order) {

        // Order Term function only differentiates the argument
        return Order(seq[0].diff(s));

    } else {

        // Chain rule
        for (unsigned i=0; i!=seq.size(); i++) {
            new_seq.push_back(mul(pdiff(i), seq[i].diff(s)));
        }
    }
    return add(new_seq);
}


/** Implementation of ex::diff() for a power-series. It treats the series as a polynomial.
 *  @see ex::diff */
ex series::diff(symbol const & s) const
{
    if (s == var) {
        epvector new_seq;
        epvector::const_iterator it = seq.begin(), itend = seq.end();
        
        //!! coeff might depend on var
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
        return series(var, point, new_seq);
    } else {
        return *this;
    }
}


/** Compute partial derivative of an expression.
 *
 *  @param s  symbol by which the expression is derived
 *  @param nth  order of derivative (default 1)
 *  @return partial derivative as a new expression */

ex ex::diff(symbol const & s, unsigned nth) const
{
    ASSERT(bp!=0);

    if ( nth==0 ) {
        return *this;
    }

    ex ndiff = bp->diff(s);
    while ( nth>1 ) {
        ndiff = ndiff.diff(s);
        --nth;
    }
    return ndiff;
}
