/** @file normal.cpp
 *
 *  This file implements several functions that work on univariate and
 *  multivariate polynomials and rational functions.
 *  These functions include polynomial quotient and remainder, GCD and LCM
 *  computation, square-free factorization and rational function normalization.
 */

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

#include <stdexcept>
#include <algorithm>
#include <map>

#include "normal.h"
#include "basic.h"
#include "ex.h"
#include "add.h"
#include "constant.h"
#include "expairseq.h"
#include "fail.h"
#include "indexed.h"
#include "inifcns.h"
#include "lst.h"
#include "mul.h"
#include "ncmul.h"
#include "numeric.h"
#include "power.h"
#include "relational.h"
#include "series.h"
#include "symbol.h"

#ifndef NO_GINAC_NAMESPACE
namespace GiNaC {
#endif // ndef NO_GINAC_NAMESPACE

// If comparing expressions (ex::compare()) is fast, you can set this to 1.
// Some routines like quo(), rem() and gcd() will then return a quick answer
// when they are called with two identical arguments.
#define FAST_COMPARE 1

// Set this if you want divide_in_z() to use remembering
#define USE_REMEMBER 1


/** Return pointer to first symbol found in expression.  Due to GiNaC´s
 *  internal ordering of terms, it may not be obvious which symbol this
 *  function returns for a given expression.
 *
 *  @param e  expression to search
 *  @param x  pointer to first symbol found (returned)
 *  @return "false" if no symbol was found, "true" otherwise */

static bool get_first_symbol(const ex &e, const symbol *&x)
{
    if (is_ex_exactly_of_type(e, symbol)) {
        x = static_cast<symbol *>(e.bp);
        return true;
    } else if (is_ex_exactly_of_type(e, add) || is_ex_exactly_of_type(e, mul)) {
        for (int i=0; i<e.nops(); i++)
            if (get_first_symbol(e.op(i), x))
                return true;
    } else if (is_ex_exactly_of_type(e, power)) {
        if (get_first_symbol(e.op(0), x))
            return true;
    }
    return false;
}


/*
 *  Statistical information about symbols in polynomials
 */

/** This structure holds information about the highest and lowest degrees
 *  in which a symbol appears in two multivariate polynomials "a" and "b".
 *  A vector of these structures with information about all symbols in
 *  two polynomials can be created with the function get_symbol_stats().
 *
 *  @see get_symbol_stats */
struct sym_desc {
    /** Pointer to symbol */
    const symbol *sym;

    /** Highest degree of symbol in polynomial "a" */
    int deg_a;

    /** Highest degree of symbol in polynomial "b" */
    int deg_b;

    /** Lowest degree of symbol in polynomial "a" */
    int ldeg_a;

    /** Lowest degree of symbol in polynomial "b" */
    int ldeg_b;

    /** Minimum of ldeg_a and ldeg_b (Used for sorting) */
    int min_deg;

    /** Commparison operator for sorting */
    bool operator<(const sym_desc &x) const {return min_deg < x.min_deg;}
};

// Vector of sym_desc structures
typedef vector<sym_desc> sym_desc_vec;

// Add symbol the sym_desc_vec (used internally by get_symbol_stats())
static void add_symbol(const symbol *s, sym_desc_vec &v)
{
    sym_desc_vec::iterator it = v.begin(), itend = v.end();
    while (it != itend) {
        if (it->sym->compare(*s) == 0)  // If it's already in there, don't add it a second time
            return;
        it++;
    }
    sym_desc d;
    d.sym = s;
    v.push_back(d);
}

// Collect all symbols of an expression (used internally by get_symbol_stats())
static void collect_symbols(const ex &e, sym_desc_vec &v)
{
    if (is_ex_exactly_of_type(e, symbol)) {
        add_symbol(static_cast<symbol *>(e.bp), v);
    } else if (is_ex_exactly_of_type(e, add) || is_ex_exactly_of_type(e, mul)) {
        for (int i=0; i<e.nops(); i++)
            collect_symbols(e.op(i), v);
    } else if (is_ex_exactly_of_type(e, power)) {
        collect_symbols(e.op(0), v);
    }
}

/** Collect statistical information about symbols in polynomials.
 *  This function fills in a vector of "sym_desc" structs which contain
 *  information about the highest and lowest degrees of all symbols that
 *  appear in two polynomials. The vector is then sorted by minimum
 *  degree (lowest to highest). The information gathered by this
 *  function is used by the GCD routines to identify trivial factors
 *  and to determine which variable to choose as the main variable
 *  for GCD computation.
 *
 *  @param a  first multivariate polynomial
 *  @param b  second multivariate polynomial
 *  @param v  vector of sym_desc structs (filled in) */

static void get_symbol_stats(const ex &a, const ex &b, sym_desc_vec &v)
{
    collect_symbols(a.eval(), v);   // eval() to expand assigned symbols
    collect_symbols(b.eval(), v);
    sym_desc_vec::iterator it = v.begin(), itend = v.end();
    while (it != itend) {
        int deg_a = a.degree(*(it->sym));
        int deg_b = b.degree(*(it->sym));
        it->deg_a = deg_a;
        it->deg_b = deg_b;
        it->min_deg = min(deg_a, deg_b);
        it->ldeg_a = a.ldegree(*(it->sym));
        it->ldeg_b = b.ldegree(*(it->sym));
        it++;
    }
    sort(v.begin(), v.end());
}


/*
 *  Computation of LCM of denominators of coefficients of a polynomial
 */

// Compute LCM of denominators of coefficients by going through the
// expression recursively (used internally by lcm_of_coefficients_denominators())
static numeric lcmcoeff(const ex &e, const numeric &l)
{
    if (e.info(info_flags::rational))
        return lcm(ex_to_numeric(e).denom(), l);
    else if (is_ex_exactly_of_type(e, add) || is_ex_exactly_of_type(e, mul)) {
        numeric c = numONE();
        for (int i=0; i<e.nops(); i++) {
            c = lcmcoeff(e.op(i), c);
        }
        return lcm(c, l);
    } else if (is_ex_exactly_of_type(e, power))
        return lcmcoeff(e.op(0), l);
    return l;
}

/** Compute LCM of denominators of coefficients of a polynomial.
 *  Given a polynomial with rational coefficients, this function computes
 *  the LCM of the denominators of all coefficients. This can be used
 *  To bring a polynomial from Q[X] to Z[X].
 *
 *  @param e  multivariate polynomial
 *  @return LCM of denominators of coefficients */

static numeric lcm_of_coefficients_denominators(const ex &e)
{
    return lcmcoeff(e.expand(), numONE());
}


/** Compute the integer content (= GCD of all numeric coefficients) of an
 *  expanded polynomial.
 *
 *  @param e  expanded polynomial
 *  @return integer content */

numeric ex::integer_content(void) const
{
    GINAC_ASSERT(bp!=0);
    return bp->integer_content();
}

numeric basic::integer_content(void) const
{
    return numONE();
}

numeric numeric::integer_content(void) const
{
    return abs(*this);
}

numeric add::integer_content(void) const
{
    epvector::const_iterator it = seq.begin();
    epvector::const_iterator itend = seq.end();
    numeric c = numZERO();
    while (it != itend) {
        GINAC_ASSERT(!is_ex_exactly_of_type(it->rest,numeric));
        GINAC_ASSERT(is_ex_exactly_of_type(it->coeff,numeric));
        c = gcd(ex_to_numeric(it->coeff), c);
        it++;
    }
    GINAC_ASSERT(is_ex_exactly_of_type(overall_coeff,numeric));
    c = gcd(ex_to_numeric(overall_coeff),c);
    return c;
}

numeric mul::integer_content(void) const
{
#ifdef DO_GINAC_ASSERT
    epvector::const_iterator it = seq.begin();
    epvector::const_iterator itend = seq.end();
    while (it != itend) {
        GINAC_ASSERT(!is_ex_exactly_of_type(recombine_pair_to_ex(*it),numeric));
        ++it;
    }
#endif // def DO_GINAC_ASSERT
    GINAC_ASSERT(is_ex_exactly_of_type(overall_coeff,numeric));
    return abs(ex_to_numeric(overall_coeff));
}


/*
 *  Polynomial quotients and remainders
 */

/** Quotient q(x) of polynomials a(x) and b(x) in Q[x].
 *  It satisfies a(x)=b(x)*q(x)+r(x).
 *
 *  @param a  first polynomial in x (dividend)
 *  @param b  second polynomial in x (divisor)
 *  @param x  a and b are polynomials in x
 *  @param check_args  check whether a and b are polynomials with rational
 *         coefficients (defaults to "true")
 *  @return quotient of a and b in Q[x] */

ex quo(const ex &a, const ex &b, const symbol &x, bool check_args)
{
    if (b.is_zero())
        throw(std::overflow_error("quo: division by zero"));
    if (is_ex_exactly_of_type(a, numeric) && is_ex_exactly_of_type(b, numeric))
        return a / b;
#if FAST_COMPARE
    if (a.is_equal(b))
        return exONE();
#endif
    if (check_args && (!a.info(info_flags::rational_polynomial) || !b.info(info_flags::rational_polynomial)))
        throw(std::invalid_argument("quo: arguments must be polynomials over the rationals"));

    // Polynomial long division
    ex q = exZERO();
    ex r = a.expand();
    if (r.is_zero())
        return r;
    int bdeg = b.degree(x);
    int rdeg = r.degree(x);
    ex blcoeff = b.expand().coeff(x, bdeg);
    bool blcoeff_is_numeric = is_ex_exactly_of_type(blcoeff, numeric);
    while (rdeg >= bdeg) {
        ex term, rcoeff = r.coeff(x, rdeg);
        if (blcoeff_is_numeric)
            term = rcoeff / blcoeff;
        else {
            if (!divide(rcoeff, blcoeff, term, false))
                return *new ex(fail());
        }
        term *= power(x, rdeg - bdeg);
        q += term;
        r -= (term * b).expand();
        if (r.is_zero())
            break;
        rdeg = r.degree(x);
    }
    return q;
}


/** Remainder r(x) of polynomials a(x) and b(x) in Q[x].
 *  It satisfies a(x)=b(x)*q(x)+r(x).
 *
 *  @param a  first polynomial in x (dividend)
 *  @param b  second polynomial in x (divisor)
 *  @param x  a and b are polynomials in x
 *  @param check_args  check whether a and b are polynomials with rational
 *         coefficients (defaults to "true")
 *  @return remainder of a(x) and b(x) in Q[x] */

ex rem(const ex &a, const ex &b, const symbol &x, bool check_args)
{
    if (b.is_zero())
        throw(std::overflow_error("rem: division by zero"));
    if (is_ex_exactly_of_type(a, numeric)) {
        if  (is_ex_exactly_of_type(b, numeric))
            return exZERO();
        else
            return b;
    }
#if FAST_COMPARE
    if (a.is_equal(b))
        return exZERO();
#endif
    if (check_args && (!a.info(info_flags::rational_polynomial) || !b.info(info_flags::rational_polynomial)))
        throw(std::invalid_argument("rem: arguments must be polynomials over the rationals"));

    // Polynomial long division
    ex r = a.expand();
    if (r.is_zero())
        return r;
    int bdeg = b.degree(x);
    int rdeg = r.degree(x);
    ex blcoeff = b.expand().coeff(x, bdeg);
    bool blcoeff_is_numeric = is_ex_exactly_of_type(blcoeff, numeric);
    while (rdeg >= bdeg) {
        ex term, rcoeff = r.coeff(x, rdeg);
        if (blcoeff_is_numeric)
            term = rcoeff / blcoeff;
        else {
            if (!divide(rcoeff, blcoeff, term, false))
                return *new ex(fail());
        }
        term *= power(x, rdeg - bdeg);
        r -= (term * b).expand();
        if (r.is_zero())
            break;
        rdeg = r.degree(x);
    }
    return r;
}


/** Pseudo-remainder of polynomials a(x) and b(x) in Z[x].
 *
 *  @param a  first polynomial in x (dividend)
 *  @param b  second polynomial in x (divisor)
 *  @param x  a and b are polynomials in x
 *  @param check_args  check whether a and b are polynomials with rational
 *         coefficients (defaults to "true")
 *  @return pseudo-remainder of a(x) and b(x) in Z[x] */

ex prem(const ex &a, const ex &b, const symbol &x, bool check_args)
{
    if (b.is_zero())
        throw(std::overflow_error("prem: division by zero"));
    if (is_ex_exactly_of_type(a, numeric)) {
        if (is_ex_exactly_of_type(b, numeric))
            return exZERO();
        else
            return b;
    }
    if (check_args && (!a.info(info_flags::rational_polynomial) || !b.info(info_flags::rational_polynomial)))
        throw(std::invalid_argument("prem: arguments must be polynomials over the rationals"));

    // Polynomial long division
    ex r = a.expand();
    ex eb = b.expand();
    int rdeg = r.degree(x);
    int bdeg = eb.degree(x);
    ex blcoeff;
    if (bdeg <= rdeg) {
        blcoeff = eb.coeff(x, bdeg);
        if (bdeg == 0)
            eb = exZERO();
        else
            eb -= blcoeff * power(x, bdeg);
    } else
        blcoeff = exONE();

    int delta = rdeg - bdeg + 1, i = 0;
    while (rdeg >= bdeg && !r.is_zero()) {
        ex rlcoeff = r.coeff(x, rdeg);
        ex term = (power(x, rdeg - bdeg) * eb * rlcoeff).expand();
        if (rdeg == 0)
            r = exZERO();
        else
            r -= rlcoeff * power(x, rdeg);
        r = (blcoeff * r).expand() - term;
        rdeg = r.degree(x);
        i++;
    }
    return power(blcoeff, delta - i) * r;
}


/** Exact polynomial division of a(X) by b(X) in Q[X].
 *  
 *  @param a  first multivariate polynomial (dividend)
 *  @param b  second multivariate polynomial (divisor)
 *  @param q  quotient (returned)
 *  @param check_args  check whether a and b are polynomials with rational
 *         coefficients (defaults to "true")
 *  @return "true" when exact division succeeds (quotient returned in q),
 *          "false" otherwise */

bool divide(const ex &a, const ex &b, ex &q, bool check_args)
{
    q = exZERO();
    if (b.is_zero())
        throw(std::overflow_error("divide: division by zero"));
    if (is_ex_exactly_of_type(b, numeric)) {
        q = a / b;
        return true;
    } else if (is_ex_exactly_of_type(a, numeric))
        return false;
#if FAST_COMPARE
    if (a.is_equal(b)) {
        q = exONE();
        return true;
    }
#endif
    if (check_args && (!a.info(info_flags::rational_polynomial) || !b.info(info_flags::rational_polynomial)))
        throw(std::invalid_argument("divide: arguments must be polynomials over the rationals"));

    // Find first symbol
    const symbol *x;
    if (!get_first_symbol(a, x) && !get_first_symbol(b, x))
        throw(std::invalid_argument("invalid expression in divide()"));

    // Polynomial long division (recursive)
    ex r = a.expand();
    if (r.is_zero())
        return true;
    int bdeg = b.degree(*x);
    int rdeg = r.degree(*x);
    ex blcoeff = b.expand().coeff(*x, bdeg);
    bool blcoeff_is_numeric = is_ex_exactly_of_type(blcoeff, numeric);
    while (rdeg >= bdeg) {
        ex term, rcoeff = r.coeff(*x, rdeg);
        if (blcoeff_is_numeric)
            term = rcoeff / blcoeff;
        else
            if (!divide(rcoeff, blcoeff, term, false))
                return false;
        term *= power(*x, rdeg - bdeg);
        q += term;
        r -= (term * b).expand();
        if (r.is_zero())
            return true;
        rdeg = r.degree(*x);
    }
    return false;
}


#if USE_REMEMBER
/*
 *  Remembering
 */

typedef pair<ex, ex> ex2;
typedef pair<ex, bool> exbool;

struct ex2_less {
    bool operator() (const ex2 p, const ex2 q) const 
    {
        return p.first.compare(q.first) < 0 || (!(q.first.compare(p.first) < 0) && p.second.compare(q.second) < 0);        
    }
};

typedef map<ex2, exbool, ex2_less> ex2_exbool_remember;
#endif


/** Exact polynomial division of a(X) by b(X) in Z[X].
 *  This functions works like divide() but the input and output polynomials are
 *  in Z[X] instead of Q[X] (i.e. they have integer coefficients). Unlike
 *  divide(), it doesn´t check whether the input polynomials really are integer
 *  polynomials, so be careful of what you pass in. Also, you have to run
 *  get_symbol_stats() over the input polynomials before calling this function
 *  and pass an iterator to the first element of the sym_desc vector. This
 *  function is used internally by the heur_gcd().
 *  
 *  @param a  first multivariate polynomial (dividend)
 *  @param b  second multivariate polynomial (divisor)
 *  @param q  quotient (returned)
 *  @param var  iterator to first element of vector of sym_desc structs
 *  @return "true" when exact division succeeds (the quotient is returned in
 *          q), "false" otherwise.
 *  @see get_symbol_stats, heur_gcd */
static bool divide_in_z(const ex &a, const ex &b, ex &q, sym_desc_vec::const_iterator var)
{
    q = exZERO();
    if (b.is_zero())
        throw(std::overflow_error("divide_in_z: division by zero"));
    if (b.is_equal(exONE())) {
        q = a;
        return true;
    }
    if (is_ex_exactly_of_type(a, numeric)) {
        if (is_ex_exactly_of_type(b, numeric)) {
            q = a / b;
            return q.info(info_flags::integer);
        } else
            return false;
    }
#if FAST_COMPARE
    if (a.is_equal(b)) {
        q = exONE();
        return true;
    }
#endif

#if USE_REMEMBER
    // Remembering
    static ex2_exbool_remember dr_remember;
    ex2_exbool_remember::const_iterator remembered = dr_remember.find(ex2(a, b));
    if (remembered != dr_remember.end()) {
        q = remembered->second.first;
        return remembered->second.second;
    }
#endif

    // Main symbol
    const symbol *x = var->sym;

    // Compare degrees
    int adeg = a.degree(*x), bdeg = b.degree(*x);
    if (bdeg > adeg)
        return false;

#if 1

    // Polynomial long division (recursive)
    ex r = a.expand();
    if (r.is_zero())
        return true;
    int rdeg = adeg;
    ex eb = b.expand();
    ex blcoeff = eb.coeff(*x, bdeg);
    while (rdeg >= bdeg) {
        ex term, rcoeff = r.coeff(*x, rdeg);
        if (!divide_in_z(rcoeff, blcoeff, term, var+1))
            break;
        term = (term * power(*x, rdeg - bdeg)).expand();
        q += term;
        r -= (term * eb).expand();
        if (r.is_zero()) {
#if USE_REMEMBER
            dr_remember[ex2(a, b)] = exbool(q, true);
#endif
            return true;
        }
        rdeg = r.degree(*x);
    }
#if USE_REMEMBER
    dr_remember[ex2(a, b)] = exbool(q, false);
#endif
    return false;

#else

    // Trial division using polynomial interpolation
    int i, k;

    // Compute values at evaluation points 0..adeg
    vector<numeric> alpha; alpha.reserve(adeg + 1);
    exvector u; u.reserve(adeg + 1);
    numeric point = numZERO();
    ex c;
    for (i=0; i<=adeg; i++) {
        ex bs = b.subs(*x == point);
        while (bs.is_zero()) {
            point += numONE();
            bs = b.subs(*x == point);
        }
        if (!divide_in_z(a.subs(*x == point), bs, c, var+1))
            return false;
        alpha.push_back(point);
        u.push_back(c);
        point += numONE();
    }

    // Compute inverses
    vector<numeric> rcp; rcp.reserve(adeg + 1);
    rcp.push_back(0);
    for (k=1; k<=adeg; k++) {
        numeric product = alpha[k] - alpha[0];
        for (i=1; i<k; i++)
            product *= alpha[k] - alpha[i];
        rcp.push_back(product.inverse());
    }

    // Compute Newton coefficients
    exvector v; v.reserve(adeg + 1);
    v.push_back(u[0]);
    for (k=1; k<=adeg; k++) {
        ex temp = v[k - 1];
        for (i=k-2; i>=0; i--)
            temp = temp * (alpha[k] - alpha[i]) + v[i];
        v.push_back((u[k] - temp) * rcp[k]);
    }

    // Convert from Newton form to standard form
    c = v[adeg];
    for (k=adeg-1; k>=0; k--)
        c = c * (*x - alpha[k]) + v[k];

    if (c.degree(*x) == (adeg - bdeg)) {
        q = c.expand();
        return true;
    } else
        return false;
#endif
}


/*
 *  Separation of unit part, content part and primitive part of polynomials
 */

/** Compute unit part (= sign of leading coefficient) of a multivariate
 *  polynomial in Z[x]. The product of unit part, content part, and primitive
 *  part is the polynomial itself.
 *
 *  @param x  variable in which to compute the unit part
 *  @return unit part
 *  @see ex::content, ex::primpart */
ex ex::unit(const symbol &x) const
{
    ex c = expand().lcoeff(x);
    if (is_ex_exactly_of_type(c, numeric))
        return c < exZERO() ? exMINUSONE() : exONE();
    else {
        const symbol *y;
        if (get_first_symbol(c, y))
            return c.unit(*y);
        else
            throw(std::invalid_argument("invalid expression in unit()"));
    }
}


/** Compute content part (= unit normal GCD of all coefficients) of a
 *  multivariate polynomial in Z[x].  The product of unit part, content part,
 *  and primitive part is the polynomial itself.
 *
 *  @param x  variable in which to compute the content part
 *  @return content part
 *  @see ex::unit, ex::primpart */
ex ex::content(const symbol &x) const
{
    if (is_zero())
        return exZERO();
    if (is_ex_exactly_of_type(*this, numeric))
        return info(info_flags::negative) ? -*this : *this;
    ex e = expand();
    if (e.is_zero())
        return exZERO();

    // First, try the integer content
    ex c = e.integer_content();
    ex r = e / c;
    ex lcoeff = r.lcoeff(x);
    if (lcoeff.info(info_flags::integer))
        return c;

    // GCD of all coefficients
    int deg = e.degree(x);
    int ldeg = e.ldegree(x);
    if (deg == ldeg)
        return e.lcoeff(x) / e.unit(x);
    c = exZERO();
    for (int i=ldeg; i<=deg; i++)
        c = gcd(e.coeff(x, i), c, NULL, NULL, false);
    return c;
}


/** Compute primitive part of a multivariate polynomial in Z[x].
 *  The product of unit part, content part, and primitive part is the
 *  polynomial itself.
 *
 *  @param x  variable in which to compute the primitive part
 *  @return primitive part
 *  @see ex::unit, ex::content */
ex ex::primpart(const symbol &x) const
{
    if (is_zero())
        return exZERO();
    if (is_ex_exactly_of_type(*this, numeric))
        return exONE();

    ex c = content(x);
    if (c.is_zero())
        return exZERO();
    ex u = unit(x);
    if (is_ex_exactly_of_type(c, numeric))
        return *this / (c * u);
    else
        return quo(*this, c * u, x, false);
}


/** Compute primitive part of a multivariate polynomial in Z[x] when the
 *  content part is already known. This function is faster in computing the
 *  primitive part than the previous function.
 *
 *  @param x  variable in which to compute the primitive part
 *  @param c  previously computed content part
 *  @return primitive part */

ex ex::primpart(const symbol &x, const ex &c) const
{
    if (is_zero())
        return exZERO();
    if (c.is_zero())
        return exZERO();
    if (is_ex_exactly_of_type(*this, numeric))
        return exONE();

    ex u = unit(x);
    if (is_ex_exactly_of_type(c, numeric))
        return *this / (c * u);
    else
        return quo(*this, c * u, x, false);
}


/*
 *  GCD of multivariate polynomials
 */

/** Compute GCD of multivariate polynomials using the subresultant PRS
 *  algorithm. This function is used internally gy gcd().
 *
 *  @param a  first multivariate polynomial
 *  @param b  second multivariate polynomial
 *  @param x  pointer to symbol (main variable) in which to compute the GCD in
 *  @return the GCD as a new expression
 *  @see gcd */

static ex sr_gcd(const ex &a, const ex &b, const symbol *x)
{
    // Sort c and d so that c has higher degree
    ex c, d;
    int adeg = a.degree(*x), bdeg = b.degree(*x);
    int cdeg, ddeg;
    if (adeg >= bdeg) {
        c = a;
        d = b;
        cdeg = adeg;
        ddeg = bdeg;
    } else {
        c = b;
        d = a;
        cdeg = bdeg;
        ddeg = adeg;
    }

    // Remove content from c and d, to be attached to GCD later
    ex cont_c = c.content(*x);
    ex cont_d = d.content(*x);
    ex gamma = gcd(cont_c, cont_d, NULL, NULL, false);
    if (ddeg == 0)
        return gamma;
    c = c.primpart(*x, cont_c);
    d = d.primpart(*x, cont_d);

    // First element of subresultant sequence
    ex r = exZERO(), ri = exONE(), psi = exONE();
    int delta = cdeg - ddeg;

    for (;;) {
        // Calculate polynomial pseudo-remainder
        r = prem(c, d, *x, false);
        if (r.is_zero())
            return gamma * d.primpart(*x);
        c = d;
        cdeg = ddeg;
        if (!divide(r, ri * power(psi, delta), d, false))
            throw(std::runtime_error("invalid expression in sr_gcd(), division failed"));
        ddeg = d.degree(*x);
        if (ddeg == 0) {
            if (is_ex_exactly_of_type(r, numeric))
                return gamma;
            else
                return gamma * r.primpart(*x);
        }

        // Next element of subresultant sequence
        ri = c.expand().lcoeff(*x);
        if (delta == 1)
            psi = ri;
        else if (delta)
            divide(power(ri, delta), power(psi, delta-1), psi, false);
        delta = cdeg - ddeg;
    }
}


/** Return maximum (absolute value) coefficient of a polynomial.
 *  This function is used internally by heur_gcd().
 *
 *  @param e  expanded multivariate polynomial
 *  @return maximum coefficient
 *  @see heur_gcd */

numeric ex::max_coefficient(void) const
{
    GINAC_ASSERT(bp!=0);
    return bp->max_coefficient();
}

numeric basic::max_coefficient(void) const
{
    return numONE();
}

numeric numeric::max_coefficient(void) const
{
    return abs(*this);
}

numeric add::max_coefficient(void) const
{
    epvector::const_iterator it = seq.begin();
    epvector::const_iterator itend = seq.end();
    GINAC_ASSERT(is_ex_exactly_of_type(overall_coeff,numeric));
    numeric cur_max = abs(ex_to_numeric(overall_coeff));
    while (it != itend) {
        numeric a;
        GINAC_ASSERT(!is_ex_exactly_of_type(it->rest,numeric));
        a = abs(ex_to_numeric(it->coeff));
        if (a > cur_max)
            cur_max = a;
        it++;
    }
    return cur_max;
}

numeric mul::max_coefficient(void) const
{
#ifdef DO_GINAC_ASSERT
    epvector::const_iterator it = seq.begin();
    epvector::const_iterator itend = seq.end();
    while (it != itend) {
        GINAC_ASSERT(!is_ex_exactly_of_type(recombine_pair_to_ex(*it),numeric));
        it++;
    }
#endif // def DO_GINAC_ASSERT
    GINAC_ASSERT(is_ex_exactly_of_type(overall_coeff,numeric));
    return abs(ex_to_numeric(overall_coeff));
}


/** Apply symmetric modular homomorphism to a multivariate polynomial.
 *  This function is used internally by heur_gcd().
 *
 *  @param e  expanded multivariate polynomial
 *  @param xi  modulus
 *  @return mapped polynomial
 *  @see heur_gcd */

ex ex::smod(const numeric &xi) const
{
    GINAC_ASSERT(bp!=0);
    return bp->smod(xi);
}

ex basic::smod(const numeric &xi) const
{
    return *this;
}

ex numeric::smod(const numeric &xi) const
{
#ifndef NO_GINAC_NAMESPACE
    return GiNaC::smod(*this, xi);
#else // ndef NO_GINAC_NAMESPACE
    return ::smod(*this, xi);
#endif // ndef NO_GINAC_NAMESPACE
}

ex add::smod(const numeric &xi) const
{
    epvector newseq;
    newseq.reserve(seq.size()+1);
    epvector::const_iterator it = seq.begin();
    epvector::const_iterator itend = seq.end();
    while (it != itend) {
        GINAC_ASSERT(!is_ex_exactly_of_type(it->rest,numeric));
#ifndef NO_GINAC_NAMESPACE
        numeric coeff = GiNaC::smod(ex_to_numeric(it->coeff), xi);
#else // ndef NO_GINAC_NAMESPACE
        numeric coeff = ::smod(ex_to_numeric(it->coeff), xi);
#endif // ndef NO_GINAC_NAMESPACE
        if (!coeff.is_zero())
            newseq.push_back(expair(it->rest, coeff));
        it++;
    }
    GINAC_ASSERT(is_ex_exactly_of_type(overall_coeff,numeric));
#ifndef NO_GINAC_NAMESPACE
    numeric coeff = GiNaC::smod(ex_to_numeric(overall_coeff), xi);
#else // ndef NO_GINAC_NAMESPACE
    numeric coeff = ::smod(ex_to_numeric(overall_coeff), xi);
#endif // ndef NO_GINAC_NAMESPACE
    return (new add(newseq,coeff))->setflag(status_flags::dynallocated);
}

ex mul::smod(const numeric &xi) const
{
#ifdef DO_GINAC_ASSERT
    epvector::const_iterator it = seq.begin();
    epvector::const_iterator itend = seq.end();
    while (it != itend) {
        GINAC_ASSERT(!is_ex_exactly_of_type(recombine_pair_to_ex(*it),numeric));
        it++;
    }
#endif // def DO_GINAC_ASSERT
    mul * mulcopyp=new mul(*this);
    GINAC_ASSERT(is_ex_exactly_of_type(overall_coeff,numeric));
#ifndef NO_GINAC_NAMESPACE
    mulcopyp->overall_coeff = GiNaC::smod(ex_to_numeric(overall_coeff),xi);
#else // ndef NO_GINAC_NAMESPACE
    mulcopyp->overall_coeff = ::smod(ex_to_numeric(overall_coeff),xi);
#endif // ndef NO_GINAC_NAMESPACE
    mulcopyp->clearflag(status_flags::evaluated);
    mulcopyp->clearflag(status_flags::hash_calculated);
    return mulcopyp->setflag(status_flags::dynallocated);
}


/** Exception thrown by heur_gcd() to signal failure. */
class gcdheu_failed {};

/** Compute GCD of multivariate polynomials using the heuristic GCD algorithm.
 *  get_symbol_stats() must have been called previously with the input
 *  polynomials and an iterator to the first element of the sym_desc vector
 *  passed in. This function is used internally by gcd().
 *
 *  @param a  first multivariate polynomial (expanded)
 *  @param b  second multivariate polynomial (expanded)
 *  @param ca  cofactor of polynomial a (returned), NULL to suppress
 *             calculation of cofactor
 *  @param cb  cofactor of polynomial b (returned), NULL to suppress
 *             calculation of cofactor
 *  @param var iterator to first element of vector of sym_desc structs
 *  @return the GCD as a new expression
 *  @see gcd
 *  @exception gcdheu_failed() */

static ex heur_gcd(const ex &a, const ex &b, ex *ca, ex *cb, sym_desc_vec::const_iterator var)
{
    if (is_ex_exactly_of_type(a, numeric) && is_ex_exactly_of_type(b, numeric)) {
        numeric g = gcd(ex_to_numeric(a), ex_to_numeric(b));
        numeric rg;
        if (ca || cb)
            rg = g.inverse();
        if (ca)
            *ca = ex_to_numeric(a).mul(rg);
        if (cb)
            *cb = ex_to_numeric(b).mul(rg);
        return g;
    }

    // The first symbol is our main variable
    const symbol *x = var->sym;

    // Remove integer content
    numeric gc = gcd(a.integer_content(), b.integer_content());
    numeric rgc = gc.inverse();
    ex p = a * rgc;
    ex q = b * rgc;
    int maxdeg = max(p.degree(*x), q.degree(*x));

    // Find evaluation point
    numeric mp = p.max_coefficient(), mq = q.max_coefficient();
    numeric xi;
    if (mp > mq)
        xi = mq * numTWO() + numTWO();
    else
        xi = mp * numTWO() + numTWO();

    // 6 tries maximum
    for (int t=0; t<6; t++) {
        if (xi.int_length() * maxdeg > 50000)
            throw gcdheu_failed();

        // Apply evaluation homomorphism and calculate GCD
        ex gamma = heur_gcd(p.subs(*x == xi), q.subs(*x == xi), NULL, NULL, var+1).expand();
        if (!is_ex_exactly_of_type(gamma, fail)) {

            // Reconstruct polynomial from GCD of mapped polynomials
            ex g = exZERO();
            numeric rxi = xi.inverse();
            for (int i=0; !gamma.is_zero(); i++) {
                ex gi = gamma.smod(xi);
                g += gi * power(*x, i);
                gamma = (gamma - gi) * rxi;
            }
            // Remove integer content
            g /= g.integer_content();

            // If the calculated polynomial divides both a and b, this is the GCD
            ex dummy;
            if (divide_in_z(p, g, ca ? *ca : dummy, var) && divide_in_z(q, g, cb ? *cb : dummy, var)) {
                g *= gc;
                ex lc = g.lcoeff(*x);
                if (is_ex_exactly_of_type(lc, numeric) && lc.compare(exZERO()) < 0)
                    return -g;
                else
                    return g;
            }
        }

        // Next evaluation point
        xi = iquo(xi * isqrt(isqrt(xi)) * numeric(73794), numeric(27011));
    }
    return *new ex(fail());
}


/** Compute GCD (Greatest Common Divisor) of multivariate polynomials a(X)
 *  and b(X) in Z[X].
 *
 *  @param a  first multivariate polynomial
 *  @param b  second multivariate polynomial
 *  @param check_args  check whether a and b are polynomials with rational
 *         coefficients (defaults to "true")
 *  @return the GCD as a new expression */

ex gcd(const ex &a, const ex &b, ex *ca, ex *cb, bool check_args)
{
    // Some trivial cases
	ex aex = a.expand(), bex = b.expand();
    if (aex.is_zero()) {
        if (ca)
            *ca = exZERO();
        if (cb)
            *cb = exONE();
        return b;
    }
    if (bex.is_zero()) {
        if (ca)
            *ca = exONE();
        if (cb)
            *cb = exZERO();
        return a;
    }
    if (aex.is_equal(exONE()) || bex.is_equal(exONE())) {
        if (ca)
            *ca = a;
        if (cb)
            *cb = b;
        return exONE();
    }
#if FAST_COMPARE
    if (a.is_equal(b)) {
        if (ca)
            *ca = exONE();
        if (cb)
            *cb = exONE();
        return a;
    }
#endif
    if (is_ex_exactly_of_type(aex, numeric) && is_ex_exactly_of_type(bex, numeric)) {
        numeric g = gcd(ex_to_numeric(aex), ex_to_numeric(bex));
        if (ca)
            *ca = ex_to_numeric(aex) / g;
        if (cb)
            *cb = ex_to_numeric(bex) / g;
        return g;
    }
    if (check_args && !a.info(info_flags::rational_polynomial) || !b.info(info_flags::rational_polynomial)) {
        throw(std::invalid_argument("gcd: arguments must be polynomials over the rationals"));
    }

    // Gather symbol statistics
    sym_desc_vec sym_stats;
    get_symbol_stats(a, b, sym_stats);

    // The symbol with least degree is our main variable
    sym_desc_vec::const_iterator var = sym_stats.begin();
    const symbol *x = var->sym;

    // Cancel trivial common factor
    int ldeg_a = var->ldeg_a;
    int ldeg_b = var->ldeg_b;
    int min_ldeg = min(ldeg_a, ldeg_b);
    if (min_ldeg > 0) {
        ex common = power(*x, min_ldeg);
//clog << "trivial common factor " << common << endl;
        return gcd((aex / common).expand(), (bex / common).expand(), ca, cb, false) * common;
    }

    // Try to eliminate variables
    if (var->deg_a == 0) {
//clog << "eliminating variable " << *x << " from b" << endl;
        ex c = bex.content(*x);
        ex g = gcd(aex, c, ca, cb, false);
        if (cb)
            *cb *= bex.unit(*x) * bex.primpart(*x, c);
        return g;
    } else if (var->deg_b == 0) {
//clog << "eliminating variable " << *x << " from a" << endl;
        ex c = aex.content(*x);
        ex g = gcd(c, bex, ca, cb, false);
        if (ca)
            *ca *= aex.unit(*x) * aex.primpart(*x, c);
        return g;
    }

    // Try heuristic algorithm first, fall back to PRS if that failed
    ex g;
    try {
        g = heur_gcd(aex, bex, ca, cb, var);
    } catch (gcdheu_failed) {
        g = *new ex(fail());
    }
    if (is_ex_exactly_of_type(g, fail)) {
//clog << "heuristics failed\n";
        g = sr_gcd(aex, bex, x);
        if (ca)
            divide(aex, g, *ca, false);
        if (cb)
            divide(bex, g, *cb, false);
    }
    return g;
}


/** Compute LCM (Least Common Multiple) of multivariate polynomials in Z[X].
 *
 *  @param a  first multivariate polynomial
 *  @param b  second multivariate polynomial
 *  @param check_args  check whether a and b are polynomials with rational
 *         coefficients (defaults to "true")
 *  @return the LCM as a new expression */
ex lcm(const ex &a, const ex &b, bool check_args)
{
    if (is_ex_exactly_of_type(a, numeric) && is_ex_exactly_of_type(b, numeric))
        return gcd(ex_to_numeric(a), ex_to_numeric(b));
    if (check_args && !a.info(info_flags::rational_polynomial) || !b.info(info_flags::rational_polynomial))
        throw(std::invalid_argument("lcm: arguments must be polynomials over the rationals"));
    
    ex ca, cb;
    ex g = gcd(a, b, &ca, &cb, false);
    return ca * cb * g;
}


/*
 *  Square-free factorization
 */

// Univariate GCD of polynomials in Q[x] (used internally by sqrfree()).
// a and b can be multivariate polynomials but they are treated as univariate polynomials in x.
static ex univariate_gcd(const ex &a, const ex &b, const symbol &x)
{
    if (a.is_zero())
        return b;
    if (b.is_zero())
        return a;
    if (a.is_equal(exONE()) || b.is_equal(exONE()))
        return exONE();
    if (is_ex_of_type(a, numeric) && is_ex_of_type(b, numeric))
        return gcd(ex_to_numeric(a), ex_to_numeric(b));
    if (!a.info(info_flags::rational_polynomial) || !b.info(info_flags::rational_polynomial))
        throw(std::invalid_argument("univariate_gcd: arguments must be polynomials over the rationals"));

    // Euclidean algorithm
    ex c, d, r;
    if (a.degree(x) >= b.degree(x)) {
        c = a;
        d = b;
    } else {
        c = b;
        d = a;
    }
    for (;;) {
        r = rem(c, d, x, false);
        if (r.is_zero())
            break;
        c = d;
        d = r;
    }
    return d / d.lcoeff(x);
}


/** Compute square-free factorization of multivariate polynomial a(x) using
 *  Yun´s algorithm.
 *
 * @param a  multivariate polynomial
 * @param x  variable to factor in
 * @return factored polynomial */
ex sqrfree(const ex &a, const symbol &x)
{
    int i = 1;
    ex res = exONE();
    ex b = a.diff(x);
    ex c = univariate_gcd(a, b, x);
    ex w;
    if (c.is_equal(exONE())) {
        w = a;
    } else {
        w = quo(a, c, x);
        ex y = quo(b, c, x);
        ex z = y - w.diff(x);
        while (!z.is_zero()) {
            ex g = univariate_gcd(w, z, x);
            res *= power(g, i);
            w = quo(w, g, x);
            y = quo(z, g, x);
            z = y - w.diff(x);
            i++;
        }
    }
    return res * power(w, i);
}


/*
 *  Normal form of rational functions
 */

// Create a symbol for replacing the expression "e" (or return a previously
// assigned symbol). The symbol is appended to sym_list and returned, the
// expression is appended to repl_list.
static ex replace_with_symbol(const ex &e, lst &sym_lst, lst &repl_lst)
{
    // Expression already in repl_lst? Then return the assigned symbol
    for (int i=0; i<repl_lst.nops(); i++)
        if (repl_lst.op(i).is_equal(e))
            return sym_lst.op(i);

    // Otherwise create new symbol and add to list, taking care that the
	// replacement expression doesn't contain symbols from the sym_lst
	// because subs() is not recursive
	symbol s;
	ex es(s);
	ex e_replaced = e.subs(sym_lst, repl_lst);
    sym_lst.append(es);
    repl_lst.append(e_replaced);
    return es;
}


/** Default implementation of ex::normal(). It replaces the object with a
 *  temporary symbol.
 *  @see ex::normal */
ex basic::normal(lst &sym_lst, lst &repl_lst, int level) const
{
    return replace_with_symbol(*this, sym_lst, repl_lst);
}


/** Implementation of ex::normal() for symbols. This returns the unmodifies symbol.
 *  @see ex::normal */
ex symbol::normal(lst &sym_lst, lst &repl_lst, int level) const
{
    return *this;
}


/** Implementation of ex::normal() for a numeric. It splits complex numbers
 *  into re+I*im and replaces I and non-rational real numbers with a temporary
 *  symbol.
 *  @see ex::normal */
ex numeric::normal(lst &sym_lst, lst &repl_lst, int level) const
{
    if (is_real())
        if (is_rational())
            return *this;
		else
		    return replace_with_symbol(*this, sym_lst, repl_lst);
    else { // complex
        numeric re = real(), im = imag();
		ex re_ex = re.is_rational() ? re : replace_with_symbol(re, sym_lst, repl_lst);
		ex im_ex = im.is_rational() ? im : replace_with_symbol(im, sym_lst, repl_lst);
		return re_ex + im_ex * replace_with_symbol(I, sym_lst, repl_lst);
	}
}


/*
 *  Helper function for fraction cancellation (returns cancelled fraction n/d)
 */

static ex frac_cancel(const ex &n, const ex &d)
{
    ex num = n;
    ex den = d;
    ex pre_factor = exONE();

    // Handle special cases where numerator or denominator is 0
    if (num.is_zero())
        return exZERO();
    if (den.expand().is_zero())
        throw(std::overflow_error("frac_cancel: division by zero in frac_cancel"));

    // More special cases
    if (is_ex_exactly_of_type(den, numeric))
        return num / den;
    if (num.is_zero())
        return exZERO();

    // Bring numerator and denominator to Z[X] by multiplying with
    // LCM of all coefficients' denominators
    ex num_lcm = lcm_of_coefficients_denominators(num);
    ex den_lcm = lcm_of_coefficients_denominators(den);
    num *= num_lcm;
    den *= den_lcm;
    pre_factor = den_lcm / num_lcm;

    // Cancel GCD from numerator and denominator
    ex cnum, cden;
    if (gcd(num, den, &cnum, &cden, false) != exONE()) {
		num = cnum;
		den = cden;
	}

	// Make denominator unit normal (i.e. coefficient of first symbol
	// as defined by get_first_symbol() is made positive)
	const symbol *x;
	if (get_first_symbol(den, x)) {
		if (den.unit(*x).compare(exZERO()) < 0) {
			num *= exMINUSONE();
			den *= exMINUSONE();
		}
	}
    return pre_factor * num / den;
}


/** Implementation of ex::normal() for a sum. It expands terms and performs
 *  fractional addition.
 *  @see ex::normal */
ex add::normal(lst &sym_lst, lst &repl_lst, int level) const
{
    // Normalize and expand children
    exvector o;
    o.reserve(seq.size()+1);
    epvector::const_iterator it = seq.begin(), itend = seq.end();
    while (it != itend) {
        ex n = recombine_pair_to_ex(*it).bp->normal(sym_lst, repl_lst, level-1).expand();
        if (is_ex_exactly_of_type(n, add)) {
            epvector::const_iterator bit = (static_cast<add *>(n.bp))->seq.begin(), bitend = (static_cast<add *>(n.bp))->seq.end();
            while (bit != bitend) {
                o.push_back(recombine_pair_to_ex(*bit));
                bit++;
            }
            o.push_back((static_cast<add *>(n.bp))->overall_coeff);
        } else
            o.push_back(n);
        it++;
    }
    o.push_back(overall_coeff.bp->normal(sym_lst, repl_lst, level-1));

    // Determine common denominator
    ex den = exONE();
    exvector::const_iterator ait = o.begin(), aitend = o.end();
    while (ait != aitend) {
        den = lcm((*ait).denom(false), den, false);
        ait++;
    }

    // Add fractions
    if (den.is_equal(exONE()))
        return (new add(o))->setflag(status_flags::dynallocated);
    else {
        exvector num_seq;
        for (ait=o.begin(); ait!=aitend; ait++) {
            ex q;
            if (!divide(den, (*ait).denom(false), q, false)) {
                // should not happen
                throw(std::runtime_error("invalid expression in add::normal, division failed"));
            }
            num_seq.push_back((*ait).numer(false) * q);
        }
        ex num = add(num_seq);

        // Cancel common factors from num/den
        return frac_cancel(num, den);
    }
}


/** Implementation of ex::normal() for a product. It cancels common factors
 *  from fractions.
 *  @see ex::normal() */
ex mul::normal(lst &sym_lst, lst &repl_lst, int level) const
{
    // Normalize children
    exvector o;
    o.reserve(seq.size()+1);
    epvector::const_iterator it = seq.begin(), itend = seq.end();
    while (it != itend) {
        o.push_back(recombine_pair_to_ex(*it).bp->normal(sym_lst, repl_lst, level-1));
        it++;
    }
    o.push_back(overall_coeff.bp->normal(sym_lst, repl_lst, level-1));
    ex n = (new mul(o))->setflag(status_flags::dynallocated);
    return frac_cancel(n.numer(false), n.denom(false));
}


/** Implementation of ex::normal() for powers. It normalizes the basis,
 *  distributes integer exponents to numerator and denominator, and replaces
 *  non-integer powers by temporary symbols.
 *  @see ex::normal */
ex power::normal(lst &sym_lst, lst &repl_lst, int level) const
{
    if (exponent.info(info_flags::integer)) {
        // Integer powers are distributed
        ex n = basis.bp->normal(sym_lst, repl_lst, level-1);
        ex num = n.numer(false);
        ex den = n.denom(false);
        return power(num, exponent) / power(den, exponent);
    } else {
        // Non-integer powers are replaced by temporary symbol (after normalizing basis)
        ex n = power(basis.bp->normal(sym_lst, repl_lst, level-1), exponent);
        return replace_with_symbol(n, sym_lst, repl_lst);
    }
}


/** Implementation of ex::normal() for series. It normalizes each coefficient and
 *  replaces the series by a temporary symbol.
 *  @see ex::normal */
ex series::normal(lst &sym_lst, lst &repl_lst, int level) const
{
    epvector new_seq;
    new_seq.reserve(seq.size());

    epvector::const_iterator it = seq.begin(), itend = seq.end();
    while (it != itend) {
        new_seq.push_back(expair(it->rest.normal(), it->coeff));
        it++;
    }

    ex n = series(var, point, new_seq);
    return replace_with_symbol(n, sym_lst, repl_lst);
}


/** Normalization of rational functions.
 *  This function converts an expression to its normal form
 *  "numerator/denominator", where numerator and denominator are (relatively
 *  prime) polynomials. Any subexpressions which are not rational functions
 *  (like non-rational numbers, non-integer powers or functions like Sin(),
 *  Cos() etc.) are replaced by temporary symbols which are re-substituted by
 *  the (normalized) subexpressions before normal() returns (this way, any
 *  expression can be treated as a rational function). normal() is applied
 *  recursively to arguments of functions etc.
 *
 *  @param level maximum depth of recursion
 *  @return normalized expression */
ex ex::normal(int level) const
{
    lst sym_lst, repl_lst;
    ex e = bp->normal(sym_lst, repl_lst, level);
    if (sym_lst.nops() > 0)
        return e.subs(sym_lst, repl_lst);
    else
        return e;
}

#ifndef NO_GINAC_NAMESPACE
} // namespace GiNaC
#endif // ndef NO_GINAC_NAMESPACE
