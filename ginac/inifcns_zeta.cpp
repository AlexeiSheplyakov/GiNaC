/** @file inifcns_zeta.cpp
 *
 *  Implementation of the Zeta-function and some related stuff. */

/*
 *  GiNaC Copyright (C) 1999-2003 Johannes Gutenberg University Mainz, Germany
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
#include "constant.h"
#include "numeric.h"
#include "power.h"
#include "symbol.h"
#include "operators.h"
#include "utils.h"

namespace GiNaC {

//////////
// Riemann's Zeta-function
//////////

static ex zeta1_evalf(const ex & x)
{
	if (is_exactly_a<numeric>(x)) {
		try {
			return zeta(ex_to<numeric>(x));
		} catch (const dunno &e) { }
	}
	
	return zeta(x).hold();
}

static ex zeta1_eval(const ex & x)
{
	if (x.info(info_flags::numeric)) {
		const numeric &y = ex_to<numeric>(x);
		// trap integer arguments:
		if (y.is_integer()) {
			if (y.is_zero())
				return _ex_1_2;
			if (y.is_equal(_num1))
				throw(std::domain_error("zeta(1): infinity"));
			if (y.info(info_flags::posint)) {
				if (y.info(info_flags::odd))
					return zeta(x).hold();
				else
					return abs(bernoulli(y))*pow(Pi,y)*pow(_num2,y-_num1)/factorial(y);
			} else {
				if (y.info(info_flags::odd))
					return -bernoulli(_num1-y)/(_num1-y);
				else
					return _ex0;
			}
		}
		// zeta(float)
		if (y.info(info_flags::numeric) && !y.info(info_flags::crational))
			return zeta1_evalf(x);
	}
	return zeta(x).hold();
}

static ex zeta1_deriv(const ex & x, unsigned deriv_param)
{
	GINAC_ASSERT(deriv_param==0);
	
	return zeta(_ex1, x);
}

unsigned zeta1_SERIAL::serial =
	function::register_new(function_options("zeta").
	                       eval_func(zeta1_eval).
	                       evalf_func(zeta1_evalf).
	                       derivative_func(zeta1_deriv).
	                       latex_name("\\zeta").
	                       overloaded(2));

//////////
// Derivatives of Riemann's Zeta-function  zeta(0,x)==zeta(x)
//////////

static ex zeta2_eval(const ex & n, const ex & x)
{
	if (n.info(info_flags::numeric)) {
		// zeta(0,x) -> zeta(x)
		if (n.is_zero())
			return zeta(x);
	}
	
	return zeta(n, x).hold();
}

static ex zeta2_deriv(const ex & n, const ex & x, unsigned deriv_param)
{
	GINAC_ASSERT(deriv_param<2);
	
	if (deriv_param==0) {
		// d/dn zeta(n,x)
		throw(std::logic_error("cannot diff zeta(n,x) with respect to n"));
	}
	// d/dx psi(n,x)
	return zeta(n+1,x);
}

unsigned zeta2_SERIAL::serial =
	function::register_new(function_options("zeta").
	                       eval_func(zeta2_eval).
	                       derivative_func(zeta2_deriv).
	                       latex_name("\\zeta").
	                       overloaded(2));

} // namespace GiNaC
