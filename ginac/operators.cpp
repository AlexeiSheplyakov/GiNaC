/** @file operators.cpp
 *
 *  Implementation of GiNaC's overloaded operators. */

#include <iostream>
#include <stdexcept>

#include "ginac.h"

// binary arithmetic operators ex with ex

ex operator+(ex const & lh, ex const & rh)
{
    debugmsg("operator+(ex,ex)",LOGLEVEL_OPERATOR);
    return lh.exadd(rh);
}

ex operator-(ex const & lh, ex const & rh)
{
    debugmsg("operator-(ex,ex)",LOGLEVEL_OPERATOR);
    return lh.exadd(rh.exmul(exMINUSONE()));
}

ex operator*(ex const & lh, ex const & rh)
{
    debugmsg("operator*(ex,ex)",LOGLEVEL_OPERATOR);
    return lh.exmul(rh);
}

ex operator/(ex const & lh, ex const & rh)
{
    debugmsg("operator*(ex,ex)",LOGLEVEL_OPERATOR);
    return lh.exmul(power(rh,exMINUSONE()));
}

ex operator%(ex const & lh, ex const & rh)
{
    debugmsg("operator%(ex,ex)",LOGLEVEL_OPERATOR);
    return lh.exncmul(rh);
}

/*

// binary arithmetic operators ex with numeric

ex operator+(ex const & lh, numeric const & rh)
{
    debugmsg("operator+(ex,numeric)",LOGLEVEL_OPERATOR);
    return lh+ex(rh);
}

ex operator-(ex const & lh, numeric const & rh)
{
    debugmsg("operator-(ex,numeric)",LOGLEVEL_OPERATOR);
    return lh-ex(rh);
}

ex operator*(ex const & lh, numeric const & rh)
{
    debugmsg("operator*(ex,numeric)",LOGLEVEL_OPERATOR);
    return lh*ex(rh);
}

ex operator/(ex const & lh, numeric const & rh)
{
    debugmsg("operator/(ex,numeric)",LOGLEVEL_OPERATOR);
    return lh/ex(rh);
}

ex operator%(ex const & lh, numeric const & rh)
{
    debugmsg("operator%(ex,numeric)",LOGLEVEL_OPERATOR);
    return lh%ex(rh);
}

// binary arithmetic operators numeric with ex

ex operator+(numeric const & lh, ex const & rh)
{
    debugmsg("operator+(numeric,ex)",LOGLEVEL_OPERATOR);
    return ex(lh)+rh;
}

ex operator-(numeric const & lh, ex const & rh)
{
    debugmsg("operator-(numeric,ex)",LOGLEVEL_OPERATOR);
    return ex(lh)-rh;
}

ex operator*(numeric const & lh, ex const & rh)
{
    debugmsg("operator*(numeric,ex)",LOGLEVEL_OPERATOR);
    return ex(lh)*rh;
}

ex operator/(numeric const & lh, ex const & rh)
{
    debugmsg("operator/(numeric,ex)",LOGLEVEL_OPERATOR);
    return ex(lh)/rh;
}

ex operator%(numeric const & lh, ex const & rh)
{
    debugmsg("operator%(numeric,ex)",LOGLEVEL_OPERATOR);
    return ex(lh)%rh;
}

*/

// binary arithmetic operators numeric with numeric

numeric operator+(numeric const & lh, numeric const & rh)
{
    debugmsg("operator+(numeric,numeric)",LOGLEVEL_OPERATOR);
    return lh.add(rh);
}

numeric operator-(numeric const & lh, numeric const & rh)
{
    debugmsg("operator-(numeric,numeric)",LOGLEVEL_OPERATOR);
    return lh.sub(rh);
}

numeric operator*(numeric const & lh, numeric const & rh)
{
    debugmsg("operator*(numeric,numeric)",LOGLEVEL_OPERATOR);
    return lh.mul(rh);
}

numeric operator/(numeric const & lh, numeric const & rh)
{
    debugmsg("operator/(numeric,ex)",LOGLEVEL_OPERATOR);
    return lh.div(rh);
}

// binary arithmetic assignment operators with ex

ex const & operator+=(ex & lh, ex const & rh)
{
    debugmsg("operator+=(ex,ex)",LOGLEVEL_OPERATOR);
    return (lh=lh+rh);
}

ex const & operator-=(ex & lh, ex const & rh)
{
    debugmsg("operator-=(ex,ex)",LOGLEVEL_OPERATOR);
    return (lh=lh-rh);
}

ex const & operator*=(ex & lh, ex const & rh)
{
    debugmsg("operator*=(ex,ex)",LOGLEVEL_OPERATOR);
    return (lh=lh*rh);
}

ex const & operator/=(ex & lh, ex const & rh)
{
    debugmsg("operator/=(ex,ex)",LOGLEVEL_OPERATOR);
    return (lh=lh/rh);
}

ex const & operator%=(ex & lh, ex const & rh)
{
    debugmsg("operator%=(ex,ex)",LOGLEVEL_OPERATOR);
    return (lh=lh%rh);
}

/*

// binary arithmetic assignment operators with numeric

ex const & operator+=(ex & lh, numeric const & rh)
{
    debugmsg("operator+=(ex,numeric)",LOGLEVEL_OPERATOR);
    return (lh=lh+ex(rh));
}

ex const & operator-=(ex & lh, numeric const & rh)
{
    debugmsg("operator-=(ex,numeric)",LOGLEVEL_OPERATOR);
    return (lh=lh-ex(rh));
}

ex const & operator*=(ex & lh, numeric const & rh)
{
    debugmsg("operator*=(ex,numeric)",LOGLEVEL_OPERATOR);
    return (lh=lh*ex(rh));
}

ex const & operator/=(ex & lh, numeric const & rh)
{
    debugmsg("operator/=(ex,numeric)",LOGLEVEL_OPERATOR);
    return (lh=lh/ex(rh));
}

ex const & operator%=(ex & lh, numeric const & rh)
{
    debugmsg("operator%=(ex,numeric)",LOGLEVEL_OPERATOR);
    return (lh=lh%ex(rh));
}

*/

// binary arithmetic assignment operators with numeric

numeric const & operator+=(numeric & lh, numeric const & rh)
{
    debugmsg("operator+=(numeric,numeric)",LOGLEVEL_OPERATOR);
    return (lh=lh.add(rh));
}

numeric const & operator-=(numeric & lh, numeric const & rh)
{
    debugmsg("operator-=(numeric,numeric)",LOGLEVEL_OPERATOR);
    return (lh=lh.sub(rh));
}

numeric const & operator*=(numeric & lh, numeric const & rh)
{
    debugmsg("operator*=(numeric,numeric)",LOGLEVEL_OPERATOR);
    return (lh=lh.mul(rh));
}

numeric const & operator/=(numeric & lh, numeric const & rh)
{
    debugmsg("operator/=(numeric,numeric)",LOGLEVEL_OPERATOR);
    return (lh=lh.div(rh));
}

// unary operators

ex operator+(ex const & lh)
{
    return lh;
}

ex operator-(ex const & lh)
{
    return exMINUSONE()*lh;
}

numeric operator+(numeric const & lh)
{
    return lh;
}

numeric operator-(numeric const & lh)
{
    return (numeric(-1)*lh);
}

// binary relational operators ex with ex

relational operator==(ex const & lh, ex const & rh)
{
    debugmsg("operator==(ex,ex)",LOGLEVEL_OPERATOR);
    return relational(lh,rh,relational::equal);
}

relational operator!=(ex const & lh, ex const & rh)
{
    debugmsg("operator!=(ex,ex)",LOGLEVEL_OPERATOR);
    return relational(lh,rh,relational::not_equal);
}

relational operator<(ex const & lh, ex const & rh)
{
    debugmsg("operator<(ex,ex)",LOGLEVEL_OPERATOR);
    return relational(lh,rh,relational::less);
}

relational operator<=(ex const & lh, ex const & rh)
{
    debugmsg("operator<=(ex,ex)",LOGLEVEL_OPERATOR);
    return relational(lh,rh,relational::less_or_equal);
}

relational operator>(ex const & lh, ex const & rh)
{
    debugmsg("operator>(ex,ex)",LOGLEVEL_OPERATOR);
    return relational(lh,rh,relational::greater);
}

relational operator>=(ex const & lh, ex const & rh)
{
    debugmsg("operator>=(ex,ex)",LOGLEVEL_OPERATOR);
    return relational(lh,rh,relational::greater_or_equal);
}

/*

// binary relational operators ex with numeric

relational operator==(ex const & lh, numeric const & rh)
{
    debugmsg("operator==(ex,numeric)",LOGLEVEL_OPERATOR);
    return relational(lh,rh,relational::equal);
}

relational operator!=(ex const & lh, numeric const & rh)
{
    debugmsg("operator!=(ex,numeric)",LOGLEVEL_OPERATOR);
    return relational(lh,rh,relational::not_equal);
}

relational operator<(ex const & lh, numeric const & rh)
{
    debugmsg("operator<(ex,numeric)",LOGLEVEL_OPERATOR);
    return relational(lh,rh,relational::less);
}

relational operator<=(ex const & lh, numeric const & rh)
{
    debugmsg("operator<=(ex,numeric)",LOGLEVEL_OPERATOR);
    return relational(lh,rh,relational::less_or_equal);
}

relational operator>(ex const & lh, numeric const & rh)
{
    debugmsg("operator>(ex,numeric)",LOGLEVEL_OPERATOR);
    return relational(lh,rh,relational::greater);
}

relational operator>=(ex const & lh, numeric const & rh)
{
    debugmsg("operator>=(ex,numeric)",LOGLEVEL_OPERATOR);
    return relational(lh,rh,relational::greater_or_equal);
}

// binary relational operators numeric with ex

relational operator==(numeric const & lh, ex const & rh)
{
    debugmsg("operator==(numeric,ex)",LOGLEVEL_OPERATOR);
    return relational(lh,rh,relational::equal);
}

relational operator!=(numeric const & lh, ex const & rh)
{
    debugmsg("operator!=(numeric,ex)",LOGLEVEL_OPERATOR);
    return relational(lh,rh,relational::not_equal);
}

relational operator<(numeric const & lh, ex const & rh)
{
    debugmsg("operator<(numeric,ex)",LOGLEVEL_OPERATOR);
    return relational(lh,rh,relational::less);
}

relational operator<=(numeric const & lh, ex const & rh)
{
    debugmsg("operator<=(numeric,ex)",LOGLEVEL_OPERATOR);
    return relational(lh,rh,relational::less_or_equal);
}

relational operator>(numeric const & lh, ex const & rh)
{
    debugmsg("operator>(numeric,ex)",LOGLEVEL_OPERATOR);
    return relational(lh,rh,relational::greater);
}

relational operator>=(numeric const & lh, ex const & rh)
{
    debugmsg("operator>=(numeric,ex)",LOGLEVEL_OPERATOR);
    return relational(lh,rh,relational::greater_or_equal);
}

*/

// input/output stream operators

ostream & operator<<(ostream & os, ex const & e)
{
    e.print(os);
    return os;
}

istream & operator>>(istream & is, ex & e)
{
    throw(std::logic_error("input from streams not yet implemented"));
}

