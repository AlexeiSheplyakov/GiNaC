/** @file operators.h
 *
 *  Interface to GiNaC's overloaded operators. */

#ifndef _OPERATORS_H_
#define _OPERATORS_H_

#include <iostream>
#include "basic.h"
#include "ex.h"
#include "numeric.h"
#include "relational.h"


// binary arithmetic operators ex with ex
ex operator+(ex const & lh, ex const & rh);
ex operator-(ex const & lh, ex const & rh);
ex operator*(ex const & lh, ex const & rh);
ex operator/(ex const & lh, ex const & rh);
ex operator%(ex const & lh, ex const & rh); // non-commutative multiplication

/*

// binary arithmetic operators ex with numeric
ex operator+(ex const & lh, numeric const & rh);
ex operator-(ex const & lh, numeric const & rh);
ex operator*(ex const & lh, numeric const & rh);
ex operator/(ex const & lh, numeric const & rh);
ex operator%(ex const & lh, numeric const & rh); // non-commutative multiplication

// binary arithmetic operators numeric with ex
ex operator+(numeric const & lh, ex const & rh);
ex operator-(numeric const & lh, ex const & rh);
ex operator*(numeric const & lh, ex const & rh);
ex operator/(numeric const & lh, ex const & rh);
ex operator%(numeric const & lh, ex const & rh); // non-commutative multiplication

*/

// binary arithmetic operators numeric with numeric
numeric operator+(numeric const & lh, numeric const & rh);
numeric operator-(numeric const & lh, numeric const & rh);
numeric operator*(numeric const & lh, numeric const & rh);
numeric operator/(numeric const & lh, numeric const & rh);

// binary arithmetic assignment operators with ex
ex const & operator+=(ex & lh, ex const & rh);
ex const & operator-=(ex & lh, ex const & rh);
ex const & operator*=(ex & lh, ex const & rh);
ex const & operator/=(ex & lh, ex const & rh);
ex const & operator%=(ex & lh, ex const & rh); // non-commutative multiplication

/*
  
// binary arithmetic assignment operators with numeric
ex const & operator+=(ex & lh, numeric const & rh);
ex const & operator-=(ex & lh, numeric const & rh);
ex const & operator*=(ex & lh, numeric const & rh);
ex const & operator/=(ex & lh, numeric const & rh);
ex const & operator%=(ex & lh, numeric const & rh); // non-commutative multiplication

*/

// binary arithmetic assignment operators with numeric
numeric const & operator+=(numeric & lh, numeric const & rh);
numeric const & operator-=(numeric & lh, numeric const & rh);
numeric const & operator*=(numeric & lh, numeric const & rh);
numeric const & operator/=(numeric & lh, numeric const & rh);

// unary operators
ex operator+(ex const & lh);
ex operator-(ex const & lh);

numeric operator+(numeric const & lh);
numeric operator-(numeric const & lh);

// binary relational operators ex with ex
relational operator==(ex const & lh, ex const & rh);
relational operator!=(ex const & lh, ex const & rh);
relational operator<(ex const & lh, ex const & rh);
relational operator<=(ex const & lh, ex const & rh);
relational operator>(ex const & lh, ex const & rh);
relational operator>=(ex const & lh, ex const & rh);

/*

// binary relational operators ex with numeric
relational operator==(ex const & lh, numeric const & rh);
relational operator!=(ex const & lh, numeric const & rh);
relational operator<(ex const & lh, numeric const & rh);
relational operator<=(ex const & lh, numeric const & rh);
relational operator>(ex const & lh, numeric const & rh);
relational operator>=(ex const & lh, numeric const & rh);

// binary relational operators numeric with ex
relational operator==(numeric const & lh, ex const & rh);
relational operator!=(numeric const & lh, ex const & rh);
relational operator<(numeric const & lh, ex const & rh);
relational operator<=(numeric const & lh, ex const & rh);
relational operator>(numeric const & lh, ex const & rh);
relational operator>=(numeric const & lh, ex const & rh);

*/

// input/output stream operators
ostream & operator<<(ostream & os, ex const & e);
istream & operator>>(istream & is, ex & e);

#endif // ndef _OPERATORS_H_

