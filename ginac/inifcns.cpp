/** @file inifcns.cpp
 *
 *  Implementation of GiNaC's initially known functions. */

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
#include "lst.h"
#include "matrix.h"
#include "mul.h"
#include "ncmul.h"
#include "numeric.h"
#include "power.h"
#include "relational.h"
#include "series.h"
#include "symbol.h"

namespace GiNaC {

//////////
// dilogarithm
//////////

static ex Li2_eval(ex const & x)
{
    if (x.is_zero())
        return x;
    if (x.is_equal(exONE()))
        return power(Pi, 2) / 6;
    if (x.is_equal(exMINUSONE()))
        return -power(Pi, 2) / 12;
    return Li2(x).hold();
}

REGISTER_FUNCTION(Li2, Li2_eval, NULL, NULL, NULL);

//////////
// trilogarithm
//////////

static ex Li3_eval(ex const & x)
{
    if (x.is_zero())
        return x;
    return Li3(x).hold();
}

REGISTER_FUNCTION(Li3, Li3_eval, NULL, NULL, NULL);

//////////
// factorial
//////////

static ex factorial_evalf(ex const & x)
{
    return factorial(x).hold();
}

static ex factorial_eval(ex const & x)
{
    if (is_ex_exactly_of_type(x, numeric))
        return factorial(ex_to_numeric(x));
    else
        return factorial(x).hold();
}

REGISTER_FUNCTION(factorial, factorial_eval, factorial_evalf, NULL, NULL);

//////////
// binomial
//////////

static ex binomial_evalf(ex const & x, ex const & y)
{
    return binomial(x, y).hold();
}

static ex binomial_eval(ex const & x, ex const &y)
{
    if (is_ex_exactly_of_type(x, numeric) && is_ex_exactly_of_type(y, numeric))
        return binomial(ex_to_numeric(x), ex_to_numeric(y));
    else
        return binomial(x, y).hold();
}

REGISTER_FUNCTION(binomial, binomial_eval, binomial_evalf, NULL, NULL);

//////////
// Order term function (for truncated power series)
//////////

static ex Order_eval(ex const & x)
{
	if (is_ex_exactly_of_type(x, numeric)) {

		// O(c)=O(1)
		return Order(exONE()).hold();

	} else if (is_ex_exactly_of_type(x, mul)) {

	 	mul *m = static_cast<mul *>(x.bp);
		if (is_ex_exactly_of_type(m->op(m->nops() - 1), numeric)) {

			// O(c*expr)=O(expr)
			return Order(x / m->op(m->nops() - 1)).hold();
		}
	}
	return Order(x).hold();
}

static ex Order_series(ex const & x, symbol const & s, ex const & point, int order)
{
	// Just wrap the function into a series object
	epvector new_seq;
	new_seq.push_back(expair(Order(exONE()), numeric(min(x.ldegree(s), order))));
	return series(s, point, new_seq);
}

REGISTER_FUNCTION(Order, Order_eval, NULL, NULL, Order_series);

/** linear solve. */
ex lsolve(ex const &eqns, ex const &symbols)
{
    // solve a system of linear equations
    if (eqns.info(info_flags::relation_equal)) {
        if (!symbols.info(info_flags::symbol)) {
            throw(std::invalid_argument("lsolve: 2nd argument must be a symbol"));
        }
        ex sol=lsolve(lst(eqns),lst(symbols));
        
        ASSERT(sol.nops()==1);
        ASSERT(is_ex_exactly_of_type(sol.op(0),relational));
        
        return sol.op(0).op(1); // return rhs of first solution
    }
    
    // syntax checks
    if (!eqns.info(info_flags::list)) {
        throw(std::invalid_argument("lsolve: 1st argument must be a list"));
    }
    for (int i=0; i<eqns.nops(); i++) {
        if (!eqns.op(i).info(info_flags::relation_equal)) {
            throw(std::invalid_argument("lsolve: 1st argument must be a list of equations"));
        }
    }
    if (!symbols.info(info_flags::list)) {
        throw(std::invalid_argument("lsolve: 2nd argument must be a list"));
    }
    for (int i=0; i<symbols.nops(); i++) {
        if (!symbols.op(i).info(info_flags::symbol)) {
            throw(std::invalid_argument("lsolve: 2nd argument must be a list of symbols"));
        }
    }
    
    // build matrix from equation system
    matrix sys(eqns.nops(),symbols.nops());
    matrix rhs(eqns.nops(),1);
    matrix vars(symbols.nops(),1);

    for (int r=0; r<eqns.nops(); r++) {
        ex eq=eqns.op(r).op(0)-eqns.op(r).op(1); // lhs-rhs==0
        ex linpart=eq;
        for (int c=0; c<symbols.nops(); c++) {
            ex co=eq.coeff(ex_to_symbol(symbols.op(c)),1);
            linpart -= co*symbols.op(c);
            sys.set(r,c,co);
        }
        linpart=linpart.expand();
        rhs.set(r,0,-linpart);
    }
    
    // test if system is linear and fill vars matrix
    for (int i=0; i<symbols.nops(); i++) {
        vars.set(i,0,symbols.op(i));
        if (sys.has(symbols.op(i))) {
            throw(std::logic_error("lsolve: system is not linear"));
        }
        if (rhs.has(symbols.op(i))) {
            throw(std::logic_error("lsolve: system is not linear"));
        }
    }
    
    //matrix solution=sys.solve(rhs);
    matrix solution;
    try {
        solution=sys.fraction_free_elim(vars,rhs);
    } catch (runtime_error const & e) {
        // probably singular matrix (or other error)
        // return empty solution list
        // cerr << e.what() << endl;
        return lst();
    }
    
    // return a list of equations
    if (solution.cols()!=1) {
        throw(std::runtime_error("lsolve: strange number of columns returned from matrix::solve"));
    }
    if (solution.rows()!=symbols.nops()) {
        cout << "symbols.nops()=" << symbols.nops() << endl;
        cout << "solution.rows()=" << solution.rows() << endl;
        throw(std::runtime_error("lsolve: strange number of rows returned from matrix::solve"));
    }
    
    // return list of the form lst(var1==sol1,var2==sol2,...)
    lst sollist;
    for (int i=0; i<symbols.nops(); i++) {
        sollist.append(symbols.op(i)==solution(i,0));
    }
    
    return sollist;
}

/** non-commutative power. */
ex ncpower(ex const &basis, unsigned exponent)
{
    if (exponent==0) {
        return exONE();
    }

    exvector v;
    v.reserve(exponent);
    for (unsigned i=0; i<exponent; ++i) {
        v.push_back(basis);
    }

    return ncmul(v,1);
}

} // namespace GiNaC
