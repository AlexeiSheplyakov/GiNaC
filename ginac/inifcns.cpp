/** @file inifcns.cpp
 *
 *  Implementation of GiNaC's initially known functions. */

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
#include "pseries.h"
#include "symbol.h"
#include "utils.h"

#ifndef NO_NAMESPACE_GINAC
namespace GiNaC {
#endif // ndef NO_NAMESPACE_GINAC

//////////
// absolute value
//////////

static ex abs_evalf(const ex & x)
{
    BEGIN_TYPECHECK
        TYPECHECK(x,numeric)
    END_TYPECHECK(abs(x))
    
    return abs(ex_to_numeric(x));
}

static ex abs_eval(const ex & x)
{
    if (is_ex_exactly_of_type(x, numeric))
        return abs(ex_to_numeric(x));
    else
        return abs(x).hold();
}

REGISTER_FUNCTION(abs, eval_func(abs_eval).
                       evalf_func(abs_evalf));

//////////
// dilogarithm
//////////

static ex Li2_eval(const ex & x)
{
    if (x.is_zero())
        return x;
    if (x.is_equal(_ex1()))
        return power(Pi, _ex2()) / _ex6();
    if (x.is_equal(_ex_1()))
        return -power(Pi, _ex2()) / _ex12();
    return Li2(x).hold();
}

REGISTER_FUNCTION(Li2, eval_func(Li2_eval));

//////////
// trilogarithm
//////////

static ex Li3_eval(const ex & x)
{
    if (x.is_zero())
        return x;
    return Li3(x).hold();
}

REGISTER_FUNCTION(Li3, eval_func(Li3_eval));

//////////
// factorial
//////////

static ex factorial_evalf(const ex & x)
{
    return factorial(x).hold();
}

static ex factorial_eval(const ex & x)
{
    if (is_ex_exactly_of_type(x, numeric))
        return factorial(ex_to_numeric(x));
    else
        return factorial(x).hold();
}

REGISTER_FUNCTION(factorial, eval_func(factorial_eval).
                             evalf_func(factorial_evalf));

//////////
// binomial
//////////

static ex binomial_evalf(const ex & x, const ex & y)
{
    return binomial(x, y).hold();
}

static ex binomial_eval(const ex & x, const ex &y)
{
    if (is_ex_exactly_of_type(x, numeric) && is_ex_exactly_of_type(y, numeric))
        return binomial(ex_to_numeric(x), ex_to_numeric(y));
    else
        return binomial(x, y).hold();
}

REGISTER_FUNCTION(binomial, eval_func(binomial_eval).
                            evalf_func(binomial_evalf));

//////////
// Order term function (for truncated power series)
//////////

static ex Order_eval(const ex & x)
{
	if (is_ex_exactly_of_type(x, numeric)) {

		// O(c)=O(1)
		return Order(_ex1()).hold();

	} else if (is_ex_exactly_of_type(x, mul)) {

	 	mul *m = static_cast<mul *>(x.bp);
		if (is_ex_exactly_of_type(m->op(m->nops() - 1), numeric)) {

			// O(c*expr)=O(expr)
			return Order(x / m->op(m->nops() - 1)).hold();
		}
	}
	return Order(x).hold();
}

static ex Order_series(const ex & x, const relational & r, int order)
{
	// Just wrap the function into a pseries object
	epvector new_seq;
    GINAC_ASSERT(is_ex_exactly_of_type(r.lhs(),symbol));
    const symbol *s = static_cast<symbol *>(r.lhs().bp);
	new_seq.push_back(expair(Order(_ex1()), numeric(min(x.ldegree(*s), order))));
	return pseries(r, new_seq);
}

// Differentiation is handled in function::derivative because of its special requirements

REGISTER_FUNCTION(Order, eval_func(Order_eval).
                         series_func(Order_series));

//////////
// Inert partial differentiation operator
//////////

static ex Derivative_eval(const ex & f, const ex & l)
{
	if (!is_ex_exactly_of_type(f, function)) {
        throw(std::invalid_argument("Derivative(): 1st argument must be a function"));
	}
    if (!is_ex_exactly_of_type(l, lst)) {
        throw(std::invalid_argument("Derivative(): 2nd argument must be a list"));
    }
	return Derivative(f, l).hold();
}

REGISTER_FUNCTION(Derivative, eval_func(Derivative_eval));

//////////
// Solve linear system
//////////

ex lsolve(const ex &eqns, const ex &symbols)
{
    // solve a system of linear equations
    if (eqns.info(info_flags::relation_equal)) {
        if (!symbols.info(info_flags::symbol)) {
            throw(std::invalid_argument("lsolve: 2nd argument must be a symbol"));
        }
        ex sol=lsolve(lst(eqns),lst(symbols));
        
        GINAC_ASSERT(sol.nops()==1);
        GINAC_ASSERT(is_ex_exactly_of_type(sol.op(0),relational));
        
        return sol.op(0).op(1); // return rhs of first solution
    }
    
    // syntax checks
    if (!eqns.info(info_flags::list)) {
        throw(std::invalid_argument("lsolve: 1st argument must be a list"));
    }
    for (unsigned i=0; i<eqns.nops(); i++) {
        if (!eqns.op(i).info(info_flags::relation_equal)) {
            throw(std::invalid_argument("lsolve: 1st argument must be a list of equations"));
        }
    }
    if (!symbols.info(info_flags::list)) {
        throw(std::invalid_argument("lsolve: 2nd argument must be a list"));
    }
    for (unsigned i=0; i<symbols.nops(); i++) {
        if (!symbols.op(i).info(info_flags::symbol)) {
            throw(std::invalid_argument("lsolve: 2nd argument must be a list of symbols"));
        }
    }
    
    // build matrix from equation system
    matrix sys(eqns.nops(),symbols.nops());
    matrix rhs(eqns.nops(),1);
    matrix vars(symbols.nops(),1);
    
    for (unsigned r=0; r<eqns.nops(); r++) {
        ex eq = eqns.op(r).op(0)-eqns.op(r).op(1); // lhs-rhs==0
        ex linpart = eq;
        for (unsigned c=0; c<symbols.nops(); c++) {
            ex co = eq.coeff(ex_to_symbol(symbols.op(c)),1);
            linpart -= co*symbols.op(c);
            sys.set(r,c,co);
        }
        linpart=linpart.expand();
        rhs.set(r,0,-linpart);
    }
    
    // test if system is linear and fill vars matrix
    for (unsigned i=0; i<symbols.nops(); i++) {
        vars.set(i,0,symbols.op(i));
        if (sys.has(symbols.op(i)))
            throw(std::logic_error("lsolve: system is not linear"));
        if (rhs.has(symbols.op(i)))
            throw(std::logic_error("lsolve: system is not linear"));
    }
    
    //matrix solution=sys.solve(rhs);
    matrix solution;
    try {
        solution = sys.fraction_free_elim(vars,rhs);
    } catch (const runtime_error & e) {
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
    for (unsigned i=0; i<symbols.nops(); i++) {
        sollist.append(symbols.op(i)==solution(i,0));
    }
    
    return sollist;
}

/** non-commutative power. */
ex ncpower(const ex &basis, unsigned exponent)
{
    if (exponent==0) {
        return _ex1();
    }

    exvector v;
    v.reserve(exponent);
    for (unsigned i=0; i<exponent; ++i) {
        v.push_back(basis);
    }

    return ncmul(v,1);
}

/** Force inclusion of functions from initcns_gamma and inifcns_zeta
 *  for static lib (so ginsh will see them). */
unsigned force_include_tgamma = function_index_tgamma;
unsigned force_include_zeta1 = function_index_zeta1;

#ifndef NO_NAMESPACE_GINAC
} // namespace GiNaC
#endif // ndef NO_NAMESPACE_GINAC
