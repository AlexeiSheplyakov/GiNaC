/** @file normal.h
 *
 *  Functions for polynomial quotient and remainder, GCD and LCM computation
 *  and square-free factorization. */

#ifndef NORMAL_H
#define NORMAL_H

#include "ex.h"

class symbol;


// Quotient q(x) of polynomials a(x) and b(x) in Q[x], so that a(x)=b(x)*q(x)+r(x)
extern ex quo(const ex &a, const ex &b, const symbol &x, bool check_args = true);

// Remainder r(x) of polynomials a(x) and b(x) in Q[x], so that a(x)=b(x)*q(x)+r(x)
extern ex rem(const ex &a, const ex &b, const symbol &x, bool check_args = true);

// Pseudo-remainder of polynomials a(x) and b(x) in Z[x]
extern ex prem(const ex &a, const ex &b, const symbol &x, bool check_args = true);

// Exact polynomial division of a(X) by b(X) in Q[X] (quotient returned in q), returns false when exact division fails
extern bool divide(const ex &a, const ex &b, ex &q, bool check_args = true);

// Polynomial GCD in Z[X], cofactors are returned in ca and cb, if desired
extern ex gcd(const ex &a, const ex &b, ex *ca = NULL, ex *cb = NULL, bool check_args = true);

// Polynomial LCM in Z[X]
extern ex lcm(const ex &a, const ex &b, bool check_args = true);

// Square-free factorization of a polynomial a(x)
extern ex sqrfree(const ex &a, const symbol &x);

#endif
