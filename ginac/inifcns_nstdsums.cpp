/** @file inifcns_nstdsums.cpp
 *
 *  Implementation of some special functions that have a representation as nested sums.
 *  The functions are: 
 *    classical polylogarithm              Li(n,x)
 *    multiple polylogarithm               Li(lst(n_1,...,n_k),lst(x_1,...,x_k)
 *    nielsen's generalized polylogarithm  S(n,p,x)
 *    harmonic polylogarithm               H(lst(m_1,...,m_k),x)
 *    multiple zeta value                  mZeta(lst(m_1,...,m_k))
 *
 *  Some remarks:
 *    - All formulae used can be looked up in the following publication:
 *      Nielsen's Generalized Polylogarithms, K.S.Kolbig, SIAM J.Math.Anal. 17 (1986), pp. 1232-1258.
 *      This document will be referenced as [Kol] throughout this source code.
 *    - Classical polylogarithms (Li) and nielsen's generalized polylogarithms (S) can be numerically
 *    	evaluated in the whole complex plane except for S(n,p,-1) when p is not unit (no formula yet
 *    	to tackle these points). And of course, there is still room for speed optimizations ;-).
 *    - The remaining functions can only be numerically evaluated with arguments lying in the unit sphere
 *      at the moment. Sorry. The evaluation especially for mZeta is very slow ... better not use it
 *      right now.
 *    - The functions have no series expansion. To do it, you have to convert these functions
 *      into the appropriate objects from the nestedsums library, do the expansion and convert the
 *      result back. 
 *    - Numerical testing of this implementation has been performed by doing a comparison of results
 *      between this software and the commercial M.......... 4.1.
 *
 */

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

#include <stdexcept>
#include <vector>
#include <cln/cln.h>

#include "inifcns.h"
#include "lst.h"
#include "numeric.h"
#include "operators.h"
#include "relational.h"


namespace GiNaC {

	
//////////////////////
// helper functions //
//////////////////////


// helper function for classical polylog Li
static cln::cl_N Li_series(int n, const cln::cl_N& x, const cln::float_format_t& prec)
{
	// Note: argument must be in the unit circle
	cln::cl_N aug, acc;
	cln::cl_N num = cln::complex(cln::cl_float(1, prec), 0);
	cln::cl_N den = 0;
	int i = 1;
	do {
		num = num * x;
		cln::cl_R ii = i;
		den = cln::expt(ii, n);
		i++;
		aug = num / den;
		acc = acc + aug;
	} while (acc != acc+aug);
	return acc;
}


// helper function for classical polylog Li
static cln::cl_N Li_projection(int n, const cln::cl_N& x, const cln::float_format_t& prec)
{
	return Li_series(n, x, prec);
}


// helper function for classical polylog Li
static numeric Li_num(int n, const numeric& x)
{
	if (n == 1) {
		// just a log
		return -cln::log(1-x.to_cl_N());
	}
	if (x.is_zero()) {
		return 0;
	}
	if (x == 1) {
		// [Kol] (2.22)
		return cln::zeta(n);
	}
	else if (x == -1) {
		// [Kol] (2.22)
		return -(1-cln::expt(cln::cl_I(2),1-n)) * cln::zeta(n);
	}
	
	// what is the desired float format?
	// first guess: default format
	cln::float_format_t prec = cln::default_float_format;
	const cln::cl_N value = x.to_cl_N();
	// second guess: the argument's format
	if (!x.real().is_rational())
		prec = cln::float_format(cln::the<cln::cl_F>(cln::realpart(value)));
	else if (!x.imag().is_rational())
		prec = cln::float_format(cln::the<cln::cl_F>(cln::imagpart(value)));
	
	// [Kol] (5.15)
	if (cln::abs(value) > 1) {
		cln::cl_N result = -cln::expt(cln::log(-value),n) / cln::factorial(n);
		// check if argument is complex. if it is real, the new polylog has to be conjugated.
		if (cln::zerop(cln::imagpart(value))) {
			if (n & 1) {
				result = result + conjugate(Li_projection(n, cln::recip(value), prec));
			}
			else {
				result = result - conjugate(Li_projection(n, cln::recip(value), prec));
			}
		}
		else {
			if (n & 1) {
				result = result + Li_projection(n, cln::recip(value), prec);
			}
			else {
				result = result - Li_projection(n, cln::recip(value), prec);
			}
		}
		cln::cl_N add;
		for (int j=0; j<n-1; j++) {
			add = add + (1+cln::expt(cln::cl_I(-1),n-j)) * (1-cln::expt(cln::cl_I(2),1-n+j))
					* Li_num(n-j,1).to_cl_N() * cln::expt(cln::log(-value),j) / cln::factorial(j);
		}
		result = result - add;
		return result;
	}
	else {
		return Li_projection(n, value, prec);
	}
}


// helper function for S(n,p,x)
static cln::cl_N numeric_nielsen(int n, int step)
{
	if (step) {
		cln::cl_N res;
		for (int i=1; i<n; i++) {
			res = res + numeric_nielsen(i, step-1) / cln::cl_I(i);
		}
		return res;
	}
	else {
		return 1;
	}
}


// forward declaration needed by function C below
static numeric S_num(int n, int p, const numeric& x);

	
// helper function for S(n,p,x)
// [Kol] (7.2)
static cln::cl_N C(int n, int p)
{
	cln::cl_N result;

	for (int k=0; k<p; k++) {
		for (int j=0; j<=(n+k-1)/2; j++) {
			if (k == 0) {
				if (n & 1) {
					if (j & 1) {
						result = result - 2 * cln::expt(cln::pi(),2*j) * S_num(n-2*j,p,1).to_cl_N() / cln::factorial(2*j);
					}
					else {
						result = result + 2 * cln::expt(cln::pi(),2*j) * S_num(n-2*j,p,1).to_cl_N() / cln::factorial(2*j);
					}
				}
			}
			else {
				if (k & 1) {
					if (j & 1) {
						result = result + cln::factorial(n+k-1)
							* cln::expt(cln::pi(),2*j) * S_num(n+k-2*j,p-k,1).to_cl_N()
							/ (cln::factorial(k) * cln::factorial(n-1) * cln::factorial(2*j));
					}
					else {
						result = result - cln::factorial(n+k-1)
							* cln::expt(cln::pi(),2*j) * S_num(n+k-2*j,p-k,1).to_cl_N()
							/ (cln::factorial(k) * cln::factorial(n-1) * cln::factorial(2*j));
					}
				}
				else {
					if (j & 1) {
						result = result - cln::factorial(n+k-1) * cln::expt(cln::pi(),2*j) * S_num(n+k-2*j,p-k,1).to_cl_N()
							/ (cln::factorial(k) * cln::factorial(n-1) * cln::factorial(2*j));
					}
					else {
						result = result + cln::factorial(n+k-1)
							* cln::expt(cln::pi(),2*j) * S_num(n+k-2*j,p-k,1).to_cl_N()
							/ (cln::factorial(k) * cln::factorial(n-1) * cln::factorial(2*j));
					}
				}
			}
		}
	}
	int np = n+p;
	if ((np-1) & 1) {
		if (((np)/2+n) & 1) {
			result = -result - cln::expt(cln::pi(),np) / (np * cln::factorial(n-1) * cln::factorial(p));
		}
		else {
			result = -result + cln::expt(cln::pi(),np) / (np * cln::factorial(n-1) * cln::factorial(p));
		}
	}

	return result;
}


// helper function for S(n,p,x)
// [Kol] remark to (9.1)
static cln::cl_N a_k(int k)
{
	cln::cl_N result;

	if (k == 0) {
		return 1;
	}

	result = result;
	for (int m=2; m<=k; m++) {
		result = result + cln::expt(cln::cl_N(-1),m) * cln::zeta(m) * a_k(k-m);
	}

	return -result / k;
}


// helper function for S(n,p,x)
// [Kol] remark to (9.1)
static cln::cl_N b_k(int k)
{
	cln::cl_N result;

	if (k == 0) {
		return 1;
	}

	result = result;
	for (int m=2; m<=k; m++) {
		result = result + cln::expt(cln::cl_N(-1),m) * cln::zeta(m) * b_k(k-m);
	}

	return result / k;
}


// helper function for S(n,p,x)
static cln::cl_N S_series(int n, int p, const cln::cl_N& x, const cln::float_format_t& prec)
{
	n++;
	int i = p;
	p--;
	cln::cl_N aug, acc;
	cln::cl_N num = cln::expt(x,p);
	cln::cl_N converter = cln::complex(cln::cl_float(1, prec), 0);
	cln::cl_N den = 0;
	do {
		num = num * x;
		den = cln::expt(cln::cl_I(i), n);
		aug = num / den * numeric_nielsen(i, p);
		i++;
		acc = acc + aug;
	} while (acc != acc+aug);

	return acc;
}


// helper function for S(n,p,x)
static cln::cl_N S_projection(int n, int p, const cln::cl_N& x, const cln::float_format_t& prec)
{
	// [Kol] (5.3)
	if (cln::abs(cln::realpart(x)) > cln::cl_F("0.5")) {

		cln::cl_N result = cln::expt(cln::cl_I(-1),p) * cln::expt(cln::log(x),n)
			* cln::expt(cln::log(1-x),p) / cln::factorial(n) / cln::factorial(p);

		for (int s=0; s<n; s++) {
			cln::cl_N res2;
			for (int r=0; r<p; r++) {
				res2 = res2 + cln::expt(cln::cl_I(-1),r) * cln::expt(cln::log(1-x),r)
					* S_series(p-r,n-s,1-x,prec) / cln::factorial(r);
			}
			result = result + cln::expt(cln::log(x),s) * (S_num(n-s,p,1).to_cl_N() - res2) / cln::factorial(s);
		}

		return result;
	}
	
	return S_series(n, p, x, prec);
}


// helper function for S(n,p,x)
static numeric S_num(int n, int p, const numeric& x)
{
	if (x == 1) {
		if (n == 1) {
		    // [Kol] (2.22) with (2.21)
			return cln::zeta(p+1);
		}

		if (p == 1) {
		    // [Kol] (2.22)
			return cln::zeta(n+1);
		}

		// [Kol] (9.1)
		cln::cl_N result;
		for (int nu=0; nu<n; nu++) {
			for (int rho=0; rho<=p; rho++) {
				result = result + b_k(n-nu-1) * b_k(p-rho) * a_k(nu+rho+1)
					* cln::factorial(nu+rho+1) / cln::factorial(rho) / cln::factorial(nu+1);
			}
		}
		result = result * cln::expt(cln::cl_I(-1),n+p-1);

		return result;
	}
	else if (x == -1) {
		// [Kol] (2.22)
		if (p == 1) {
			return -(1-cln::expt(cln::cl_I(2),-n)) * cln::zeta(n+1);
		}
		throw std::runtime_error("don't know how to evaluate this function!");
	}

	// what is the desired float format?
	// first guess: default format
	cln::float_format_t prec = cln::default_float_format;
	const cln::cl_N value = x.to_cl_N();
	// second guess: the argument's format
	if (!x.real().is_rational())
		prec = cln::float_format(cln::the<cln::cl_F>(cln::realpart(value)));
	else if (!x.imag().is_rational())
		prec = cln::float_format(cln::the<cln::cl_F>(cln::imagpart(value)));


	// [Kol] (5.3)
	if (cln::realpart(value) < -0.5) {

		cln::cl_N result = cln::expt(cln::cl_I(-1),p) * cln::expt(cln::log(value),n)
			* cln::expt(cln::log(1-value),p) / cln::factorial(n) / cln::factorial(p);

		for (int s=0; s<n; s++) {
			cln::cl_N res2;
			for (int r=0; r<p; r++) {
				res2 = res2 + cln::expt(cln::cl_I(-1),r) * cln::expt(cln::log(1-value),r)
					* S_num(p-r,n-s,1-value).to_cl_N() / cln::factorial(r);
			}
			result = result + cln::expt(cln::log(value),s) * (S_num(n-s,p,1).to_cl_N() - res2) / cln::factorial(s);
		}

		return result;
		
	}
	// [Kol] (5.12)
	else if (cln::abs(value) > 1) {
		
		cln::cl_N result;

		for (int s=0; s<p; s++) {
			for (int r=0; r<=s; r++) {
				result = result + cln::expt(cln::cl_I(-1),s) * cln::expt(cln::log(-value),r) * cln::factorial(n+s-r-1)
					/ cln::factorial(r) / cln::factorial(s-r) / cln::factorial(n-1)
					* S_num(n+s-r,p-s,cln::recip(value)).to_cl_N();
			}
		}
		result = result * cln::expt(cln::cl_I(-1),n);

		cln::cl_N res2;
		for (int r=0; r<n; r++) {
			res2 = res2 + cln::expt(cln::log(-value),r) * C(n-r,p) / cln::factorial(r);
		}
		res2 = res2 + cln::expt(cln::log(-value),n+p) / cln::factorial(n+p);

		result = result + cln::expt(cln::cl_I(-1),p) * res2;

		return result;
	}
	else {
		return S_projection(n, p, value, prec);
	}
}


// helper function for multiple polylogarithm
static cln::cl_N numeric_zsum(int n, std::vector<cln::cl_N>& x, std::vector<cln::cl_N>& m)
{
	cln::cl_N res;
	if (x.empty()) {
		return 1;
	}
	for (int i=1; i<n; i++) {
		std::vector<cln::cl_N>::iterator be;
		std::vector<cln::cl_N>::iterator en;
		be = x.begin();
		be++;
		en = x.end();
		std::vector<cln::cl_N> xbuf(be, en);
		be = m.begin();
		be++;
		en = m.end();
		std::vector<cln::cl_N> mbuf(be, en);
		res = res + cln::expt(x[0],i) / cln::expt(i,m[0]) * numeric_zsum(i, xbuf, mbuf);
	}
	return res;
}


// helper function for harmonic polylogarithm
static cln::cl_N numeric_harmonic(int n, std::vector<cln::cl_N>& m)
{
	cln::cl_N res;
	if (m.empty()) {
		return 1;
	}
	for (int i=1; i<n; i++) {
		std::vector<cln::cl_N>::iterator be;
		std::vector<cln::cl_N>::iterator en;
		be = m.begin();
		be++;
		en = m.end();
		std::vector<cln::cl_N> mbuf(be, en);
		res = res + cln::recip(cln::expt(i,m[0])) * numeric_harmonic(i, mbuf);
	}
	return res;
}


/////////////////////////////
// end of helper functions //
/////////////////////////////


// Polylogarithm and multiple polylogarithm

static ex Li_eval(const ex& x1, const ex& x2)
{
	if (x2.is_zero()) {
		return 0;
	}
	else {
		return Li(x1,x2).hold();
	}
}

static ex Li_evalf(const ex& x1, const ex& x2)
{
	// classical polylogs
	if (is_a<numeric>(x1) && is_a<numeric>(x2)) {
		return Li_num(ex_to<numeric>(x1).to_int(), ex_to<numeric>(x2));
	}
	// multiple polylogs
	else if (is_a<lst>(x1) && is_a<lst>(x2)) {
		for (int i=0; i<x1.nops(); i++) {
			if (!is_a<numeric>(x1.op(i)))
				return Li(x1,x2).hold();
			if (!is_a<numeric>(x2.op(i)))
				return Li(x1,x2).hold();
			if (x2 >= 1)
				return Li(x1,x2).hold();
		}

		cln::cl_N m_1 = ex_to<numeric>(x1.op(x1.nops()-1)).to_cl_N();
		cln::cl_N x_1 = ex_to<numeric>(x2.op(x2.nops()-1)).to_cl_N();
		std::vector<cln::cl_N> x;
		std::vector<cln::cl_N> m;
		const int nops = ex_to<numeric>(x1.nops()).to_int();
		for (int i=nops-2; i>=0; i--) {
			m.push_back(ex_to<numeric>(x1.op(i)).to_cl_N());
			x.push_back(ex_to<numeric>(x2.op(i)).to_cl_N());
		}

		cln::cl_N res;
		cln::cl_N resbuf;
		for (int i=nops; true; i++) {
			resbuf = res;
			res = res + cln::expt(x_1,i) / cln::expt(i,m_1) * numeric_zsum(i, x, m);
			if (cln::zerop(res-resbuf))
				break;
		}

		return numeric(res);

	}

	return Li(x1,x2).hold();
}

REGISTER_FUNCTION(Li, eval_func(Li_eval).evalf_func(Li_evalf).do_not_evalf_params());


// Nielsen's generalized polylogarithm

static ex S_eval(const ex& x1, const ex& x2, const ex& x3)
{
	if (x2 == 1) {
		return Li(x1+1,x3);
	}
	return S(x1,x2,x3).hold();
}

static ex S_evalf(const ex& x1, const ex& x2, const ex& x3)
{
	if (is_a<numeric>(x1) && is_a<numeric>(x2) && is_a<numeric>(x3)) {
		if ((x3 == -1) && (x2 != 1)) {
			// no formula to evaluate this ... sorry
			return S(x1,x2,x3).hold();
		}
		return S_num(ex_to<numeric>(x1).to_int(), ex_to<numeric>(x2).to_int(), ex_to<numeric>(x3));
	}
	return S(x1,x2,x3).hold();
}

REGISTER_FUNCTION(S, eval_func(S_eval).evalf_func(S_evalf).do_not_evalf_params());


// Harmonic polylogarithm

static ex H_eval(const ex& x1, const ex& x2)
{
	return H(x1,x2).hold();
}

static ex H_evalf(const ex& x1, const ex& x2)
{
	if (is_a<lst>(x1) && is_a<numeric>(x2)) {
		for (int i=0; i<x1.nops(); i++) {
			if (!is_a<numeric>(x1.op(i)))
				return H(x1,x2).hold();
		}

		cln::cl_N m_1 = ex_to<numeric>(x1.op(x1.nops()-1)).to_cl_N();
		cln::cl_N x_1 = ex_to<numeric>(x2).to_cl_N();
		std::vector<cln::cl_N> m;
		const int nops = ex_to<numeric>(x1.nops()).to_int();
		for (int i=nops-2; i>=0; i--) {
			m.push_back(ex_to<numeric>(x1.op(i)).to_cl_N());
		}

		cln::cl_N res;
		cln::cl_N resbuf;
		for (int i=nops; true; i++) {
			resbuf = res;
			res = res + cln::expt(x_1,i) / cln::expt(i,m_1) * numeric_harmonic(i, m);
			if (cln::zerop(res-resbuf))
				break;
		}

		return numeric(res);

	}

	return H(x1,x2).hold();
}

REGISTER_FUNCTION(H, eval_func(H_eval).evalf_func(H_evalf).do_not_evalf_params());


// Multiple zeta value

static ex mZeta_eval(const ex& x1)
{
	return mZeta(x1).hold();
}

static ex mZeta_evalf(const ex& x1)
{
	if (is_a<lst>(x1)) {
		for (int i=0; i<x1.nops(); i++) {
			if (!is_a<numeric>(x1.op(i)))
				return mZeta(x1).hold();
		}

		cln::cl_N m_1 = ex_to<numeric>(x1.op(x1.nops()-1)).to_cl_N();
		std::vector<cln::cl_N> m;
		const int nops = ex_to<numeric>(x1.nops()).to_int();
		for (int i=nops-2; i>=0; i--) {
			m.push_back(ex_to<numeric>(x1.op(i)).to_cl_N());
		}

		cln::float_format_t prec = cln::default_float_format;
		cln::cl_N res = cln::complex(cln::cl_float(0, prec), 0);
		cln::cl_N resbuf;
		for (int i=nops; true; i++) {
			// to infinity and beyond ... timewise
			resbuf = res;
			res = res + cln::recip(cln::expt(i,m_1)) * numeric_harmonic(i, m);
			if (cln::zerop(res-resbuf))
				break;
		}

		return numeric(res);

	}

	return mZeta(x1).hold();
}

REGISTER_FUNCTION(mZeta, eval_func(mZeta_eval).evalf_func(mZeta_evalf).do_not_evalf_params());


} // namespace GiNaC

