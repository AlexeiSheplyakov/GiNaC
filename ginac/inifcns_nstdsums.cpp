/** @file inifcns_nstdsums.cpp
 *
 *  Implementation of some special functions that have a representation as nested sums.
 *  
 *  The functions are: 
 *    classical polylogarithm              Li(n,x)
 *    multiple polylogarithm               Li(lst(n_1,...,n_k),lst(x_1,...,x_k))
 *    nielsen's generalized polylogarithm  S(n,p,x)
 *    harmonic polylogarithm               H(n,x) or H(lst(n_1,...,n_k),x)
 *    multiple zeta value                  zeta(n) or zeta(lst(n_1,...,n_k))
 *
 *  Some remarks:
 *    
 *    - All formulae used can be looked up in the following publications:
 *      [Kol] Nielsen's Generalized Polylogarithms, K.S.Kolbig, SIAM J.Math.Anal. 17 (1986), pp. 1232-1258.
 *    	[Cra] Fast Evaluation of Multiple Zeta Sums, R.E.Crandall, Math.Comp. 67 (1998), pp. 1163-1172.
 *    	[ReV] Harmonic Polylogarithms, E.Remiddi, J.A.M.Vermaseren, Int.J.Mod.Phys. A15 (2000), pp. 725-754
 *
 *    - The order of parameters and arguments of H, Li and zeta is defined according to their order in the
 *      nested sums representation.
 *    	
 *    - Except for the multiple polylogarithm all functions can be nummerically evaluated with arguments in
 *      the whole complex plane. Multiple polylogarithms evaluate only if each argument x_i is smaller than
 *      one. The parameters for every function (n, p or n_i) must be positive integers.
 *      
 *    - The calculation of classical polylogarithms is speed up by using Bernoulli numbers and 
 *      look-up tables. S uses look-up tables as well. The zeta function applies the algorithm in
 *      [Cra] for speed up.
 *      
 *    - The functions have no series expansion as nested sums. To do it, you have to convert these functions
 *      into the appropriate objects from the nestedsums library, do the expansion and convert the
 *      result back. 
 *      
 *    - Numerical testing of this implementation has been performed by doing a comparison of results
 *      between this software and the commercial M.......... 4.1. Multiple zeta values have been checked
 *      by means of evaluations into simple zeta values. Harmonic polylogarithms have been checked by
 *      comparison to S(n,p,x) for corresponding parameter combinations and by continuity checks
 *      around |x|=1 along with comparisons to corresponding zeta functions.
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

#include "add.h"
#include "constant.h"
#include "lst.h"
#include "mul.h"
#include "numeric.h"
#include "operators.h"
#include "power.h"
#include "pseries.h"
#include "relational.h"
#include "symbol.h"
#include "utils.h"
#include "wildcard.h"


namespace GiNaC {


//////////////////////////////////////////////////////////////////////
//
// Classical polylogarithm  Li
//
// helper functions
//
//////////////////////////////////////////////////////////////////////


// anonymous namespace for helper functions
namespace {


// lookup table for factors built from Bernoulli numbers
// see fill_Xn()
std::vector<std::vector<cln::cl_N> > Xn;
int xnsize = 0;


// This function calculates the X_n. The X_n are needed for speed up of classical polylogarithms.
// With these numbers the polylogs can be calculated as follows:
//   Li_p (x)  =  \sum_{n=0}^\infty X_{p-2}(n) u^{n+1}/(n+1)! with  u = -log(1-x)
//   X_0(n) = B_n (Bernoulli numbers)
//   X_p(n) = \sum_{k=0}^n binomial(n,k) B_{n-k} / (k+1) * X_{p-1}(k)
// The calculation of Xn depends on X0 and X{n-1}.
// X_0 is special, it holds only the non-zero Bernoulli numbers with index 2 or greater.
// This results in a slightly more complicated algorithm for the X_n.
// The first index in Xn corresponds to the index of the polylog minus 2.
// The second index in Xn corresponds to the index from the actual sum.
void fill_Xn(int n)
{
	// rule of thumb. needs to be improved. TODO
	const int initsize = Digits * 3 / 2;

	if (n>1) {
		// calculate X_2 and higher (corresponding to Li_4 and higher)
		std::vector<cln::cl_N> buf(initsize);
		std::vector<cln::cl_N>::iterator it = buf.begin();
		cln::cl_N result;
		*it = -(cln::expt(cln::cl_I(2),n+1) - 1) / cln::expt(cln::cl_I(2),n+1); // i == 1
		it++;
		for (int i=2; i<=initsize; i++) {
			if (i&1) {
				result = 0; // k == 0
			} else {
				result = Xn[0][i/2-1]; // k == 0
			}
			for (int k=1; k<i-1; k++) {
				if ( !(((i-k) & 1) && ((i-k) > 1)) ) {
					result = result + cln::binomial(i,k) * Xn[0][(i-k)/2-1] * Xn[n-1][k-1] / (k+1);
				}
			}
			result = result - cln::binomial(i,i-1) * Xn[n-1][i-2] / 2 / i; // k == i-1
			result = result + Xn[n-1][i-1] / (i+1); // k == i
			
			*it = result;
			it++;
		}
		Xn.push_back(buf);
	} else if (n==1) {
		// special case to handle the X_0 correct
		std::vector<cln::cl_N> buf(initsize);
		std::vector<cln::cl_N>::iterator it = buf.begin();
		cln::cl_N result;
		*it = cln::cl_I(-3)/cln::cl_I(4); // i == 1
		it++;
		*it = cln::cl_I(17)/cln::cl_I(36); // i == 2
		it++;
		for (int i=3; i<=initsize; i++) {
			if (i & 1) {
				result = -Xn[0][(i-3)/2]/2;
				*it = (cln::binomial(i,1)/cln::cl_I(2) + cln::binomial(i,i-1)/cln::cl_I(i))*result;
				it++;
			} else {
				result = Xn[0][i/2-1] + Xn[0][i/2-1]/(i+1);
				for (int k=1; k<i/2; k++) {
					result = result + cln::binomial(i,k*2) * Xn[0][k-1] * Xn[0][i/2-k-1] / (k*2+1);
				}
				*it = result;
				it++;
			}
		}
		Xn.push_back(buf);
	} else {
		// calculate X_0
		std::vector<cln::cl_N> buf(initsize/2);
		std::vector<cln::cl_N>::iterator it = buf.begin();
		for (int i=1; i<=initsize/2; i++) {
			*it = bernoulli(i*2).to_cl_N();
			it++;
		}
		Xn.push_back(buf);
	}

	xnsize++;
}


// calculates Li(2,x) without Xn
cln::cl_N Li2_do_sum(const cln::cl_N& x)
{
	cln::cl_N res = x;
	cln::cl_N resbuf;
	cln::cl_N num = x;
	cln::cl_I den = 1; // n^2 = 1
	unsigned i = 3;
	do {
		resbuf = res;
		num = num * x;
		den = den + i;  // n^2 = 4, 9, 16, ...
		i += 2;
		res = res + num / den;
	} while (res != resbuf);
	return res;
}


// calculates Li(2,x) with Xn
cln::cl_N Li2_do_sum_Xn(const cln::cl_N& x)
{
	std::vector<cln::cl_N>::const_iterator it = Xn[0].begin();
	cln::cl_N u = -cln::log(1-x);
	cln::cl_N factor = u;
	cln::cl_N res = u - u*u/4;
	cln::cl_N resbuf;
	unsigned i = 1;
	do {
		resbuf = res;
		factor = factor * u*u / (2*i * (2*i+1));
		res = res + (*it) * factor;
		it++; // should we check it? or rely on initsize? ...
		i++;
	} while (res != resbuf);
	return res;
}


// calculates Li(n,x), n>2 without Xn
cln::cl_N Lin_do_sum(int n, const cln::cl_N& x)
{
	cln::cl_N factor = x;
	cln::cl_N res = x;
	cln::cl_N resbuf;
	int i=2;
	do {
		resbuf = res;
		factor = factor * x;
		res = res + factor / cln::expt(cln::cl_I(i),n);
		i++;
	} while (res != resbuf);
	return res;
}


// calculates Li(n,x), n>2 with Xn
cln::cl_N Lin_do_sum_Xn(int n, const cln::cl_N& x)
{
	std::vector<cln::cl_N>::const_iterator it = Xn[n-2].begin();
	cln::cl_N u = -cln::log(1-x);
	cln::cl_N factor = u;
	cln::cl_N res = u;
	cln::cl_N resbuf;
	unsigned i=2;
	do {
		resbuf = res;
		factor = factor * u / i;
		res = res + (*it) * factor;
		it++; // should we check it? or rely on initsize? ...
		i++;
	} while (res != resbuf);
	return res;
}


// forward declaration needed by function Li_projection and C below
numeric S_num(int n, int p, const numeric& x);


// helper function for classical polylog Li
cln::cl_N Li_projection(int n, const cln::cl_N& x, const cln::float_format_t& prec)
{
	// treat n=2 as special case
	if (n == 2) {
		// check if precalculated X0 exists
		if (xnsize == 0) {
			fill_Xn(0);
		}

		if (cln::realpart(x) < 0.5) {
			// choose the faster algorithm
			// the switching point was empirically determined. the optimal point
			// depends on hardware, Digits, ... so an approx value is okay.
			// it solves also the problem with precision due to the u=-log(1-x) transformation
			if (cln::abs(cln::realpart(x)) < 0.25) {
				
				return Li2_do_sum(x);
			} else {
				return Li2_do_sum_Xn(x);
			}
		} else {
			// choose the faster algorithm
			if (cln::abs(cln::realpart(x)) > 0.75) {
				return -Li2_do_sum(1-x) - cln::log(x) * cln::log(1-x) + cln::zeta(2);
			} else {
				return -Li2_do_sum_Xn(1-x) - cln::log(x) * cln::log(1-x) + cln::zeta(2);
			}
		}
	} else {
		// check if precalculated Xn exist
		if (n > xnsize+1) {
			for (int i=xnsize; i<n-1; i++) {
				fill_Xn(i);
			}
		}

		if (cln::realpart(x) < 0.5) {
			// choose the faster algorithm
			// with n>=12 the "normal" summation always wins against the method with Xn
			if ((cln::abs(cln::realpart(x)) < 0.3) || (n >= 12)) {
				return Lin_do_sum(n, x);
			} else {
				return Lin_do_sum_Xn(n, x);
			}
		} else {
			cln::cl_N result = -cln::expt(cln::log(x), n-1) * cln::log(1-x) / cln::factorial(n-1);
			for (int j=0; j<n-1; j++) {
				result = result + (S_num(n-j-1, 1, 1).to_cl_N() - S_num(1, n-j-1, 1-x).to_cl_N())
					* cln::expt(cln::log(x), j) / cln::factorial(j);
			}
			return result;
		}
	}
}


// helper function for classical polylog Li
numeric Li_num(int n, const numeric& x)
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


} // end of anonymous namespace


//////////////////////////////////////////////////////////////////////
//
// Multiple polylogarithm  Li
//
// helper function
//
//////////////////////////////////////////////////////////////////////


// anonymous namespace for helper function
namespace {


cln::cl_N multipleLi_do_sum(const std::vector<int>& s, const std::vector<cln::cl_N>& x)
{
	const int j = s.size();

	std::vector<cln::cl_N> t(j);
	cln::cl_F one = cln::cl_float(1, cln::float_format(Digits));

	cln::cl_N t0buf;
	int q = 0;
	do {
		t0buf = t[0];
		q++;
		t[j-1] = t[j-1] + cln::expt(x[j-1], q) / cln::expt(cln::cl_I(q),s[j-1]) * one;
		for (int k=j-2; k>=0; k--) {
			t[k] = t[k] + t[k+1] * cln::expt(x[k], q+j-1-k) / cln::expt(cln::cl_I(q+j-1-k), s[k]);
		}
	} while (t[0] != t0buf);
	
	return t[0];
}


} // end of anonymous namespace


//////////////////////////////////////////////////////////////////////
//
// Classical polylogarithm and multiple polylogarithm  Li
//
// GiNaC function
//
//////////////////////////////////////////////////////////////////////


static ex Li_eval(const ex& x1, const ex& x2)
{
	if (x2.is_zero()) {
		return _ex0;
	}
	else {
		if (x2.info(info_flags::numeric) && (!x2.info(info_flags::crational)))
			return Li_num(ex_to<numeric>(x1).to_int(), ex_to<numeric>(x2));
		if (is_a<lst>(x2)) {
			for (int i=0; i<x2.nops(); i++) {
				if (!is_a<numeric>(x2.op(i))) {
					return Li(x1,x2).hold();
				}
			}
			return Li(x1,x2).evalf();
		}
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
			if (!x1.op(i).info(info_flags::posint)) {
				return Li(x1,x2).hold();
			}
			if (!is_a<numeric>(x2.op(i))) {
				return Li(x1,x2).hold();
			}
			if (x2.op(i) >= 1) {
				return Li(x1,x2).hold();
			}
		}

		std::vector<int> m;
		std::vector<cln::cl_N> x;
		for (int i=0; i<ex_to<numeric>(x1.nops()).to_int(); i++) {
			m.push_back(ex_to<numeric>(x1.op(i)).to_int());
			x.push_back(ex_to<numeric>(x2.op(i)).to_cl_N());
		}

		return numeric(multipleLi_do_sum(m, x));
	}

	return Li(x1,x2).hold();
}


static ex Li_series(const ex& x1, const ex& x2, const relational& rel, int order, unsigned options)
{
	epvector seq;
	seq.push_back(expair(Li(x1,x2), 0));
	return pseries(rel,seq);
}


static ex Li_deriv(const ex& x1, const ex& x2, unsigned deriv_param)
{
	GINAC_ASSERT(deriv_param < 2);
	if (deriv_param == 0) {
		return _ex0;
	}
	if (x1 > 0) {
		return Li(x1-1, x2) / x2;
	} else {
		return 1/(1-x2);
	}
}


REGISTER_FUNCTION(Li,
		eval_func(Li_eval).
		evalf_func(Li_evalf).
		do_not_evalf_params().
		series_func(Li_series).
		derivative_func(Li_deriv));


//////////////////////////////////////////////////////////////////////
//
// Nielsen's generalized polylogarithm  S
//
// helper functions
//
//////////////////////////////////////////////////////////////////////


// anonymous namespace for helper functions
namespace {


// lookup table for special Euler-Zagier-Sums (used for S_n,p(x))
// see fill_Yn()
std::vector<std::vector<cln::cl_N> > Yn;
int ynsize = 0; // number of Yn[]
int ynlength = 100; // initial length of all Yn[i]


// This function calculates the Y_n. The Y_n are needed for the evaluation of S_{n,p}(x).
// The Y_n are basically Euler-Zagier sums with all m_i=1. They are subsums in the Z-sum
// representing S_{n,p}(x).
// The first index in Y_n corresponds to the parameter p minus one, i.e. the depth of the
// equivalent Z-sum.
// The second index in Y_n corresponds to the running index of the outermost sum in the full Z-sum
// representing S_{n,p}(x).
// The calculation of Y_n uses the values from Y_{n-1}.
void fill_Yn(int n, const cln::float_format_t& prec)
{
	const int initsize = ynlength;
	//const int initsize = initsize_Yn;
	cln::cl_N one = cln::cl_float(1, prec);

	if (n) {
		std::vector<cln::cl_N> buf(initsize);
		std::vector<cln::cl_N>::iterator it = buf.begin();
		std::vector<cln::cl_N>::iterator itprev = Yn[n-1].begin();
		*it = (*itprev) / cln::cl_N(n+1) * one;
		it++;
		itprev++;
		// sums with an index smaller than the depth are zero and need not to be calculated.
		// calculation starts with depth, which is n+2)
		for (int i=n+2; i<=initsize+n; i++) {
			*it = *(it-1) + (*itprev) / cln::cl_N(i) * one;
			it++;
			itprev++;
		}
		Yn.push_back(buf);
	} else {
		std::vector<cln::cl_N> buf(initsize);
		std::vector<cln::cl_N>::iterator it = buf.begin();
		*it = 1 * one;
		it++;
		for (int i=2; i<=initsize; i++) {
			*it = *(it-1) + 1 / cln::cl_N(i) * one;
			it++;
		}
		Yn.push_back(buf);
	}
	ynsize++;
}


// make Yn longer ... 
void make_Yn_longer(int newsize, const cln::float_format_t& prec)
{

	cln::cl_N one = cln::cl_float(1, prec);

	Yn[0].resize(newsize);
	std::vector<cln::cl_N>::iterator it = Yn[0].begin();
	it += ynlength;
	for (int i=ynlength+1; i<=newsize; i++) {
		*it = *(it-1) + 1 / cln::cl_N(i) * one;
		it++;
	}

	for (int n=1; n<ynsize; n++) {
		Yn[n].resize(newsize);
		std::vector<cln::cl_N>::iterator it = Yn[n].begin();
		std::vector<cln::cl_N>::iterator itprev = Yn[n-1].begin();
		it += ynlength;
		itprev += ynlength;
		for (int i=ynlength+n+1; i<=newsize+n; i++) {
			*it = *(it-1) + (*itprev) / cln::cl_N(i) * one;
			it++;
			itprev++;
		}
	}
	
	ynlength = newsize;
}


// helper function for S(n,p,x)
// [Kol] (7.2)
cln::cl_N C(int n, int p)
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
cln::cl_N a_k(int k)
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
cln::cl_N b_k(int k)
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
cln::cl_N S_do_sum(int n, int p, const cln::cl_N& x, const cln::float_format_t& prec)
{
	if (p==1) {
		return Li_projection(n+1, x, prec);
	}
	
	// check if precalculated values are sufficient
	if (p > ynsize+1) {
		for (int i=ynsize; i<p-1; i++) {
			fill_Yn(i, prec);
		}
	}

	// should be done otherwise
	cln::cl_N xf = x * cln::cl_float(1, prec);

	cln::cl_N res;
	cln::cl_N resbuf;
	cln::cl_N factor = cln::expt(xf, p);
	int i = p;
	do {
		resbuf = res;
		if (i-p >= ynlength) {
			// make Yn longer
			make_Yn_longer(ynlength*2, prec);
		}
		res = res + factor / cln::expt(cln::cl_I(i),n+1) * Yn[p-2][i-p]; // should we check it? or rely on magic number? ...
		//res = res + factor / cln::expt(cln::cl_I(i),n+1) * (*it); // should we check it? or rely on magic number? ...
		factor = factor * xf;
		i++;
	} while (res != resbuf);
	
	return res;
}


// helper function for S(n,p,x)
cln::cl_N S_projection(int n, int p, const cln::cl_N& x, const cln::float_format_t& prec)
{
	// [Kol] (5.3)
	if (cln::abs(cln::realpart(x)) > cln::cl_F("0.5")) {

		cln::cl_N result = cln::expt(cln::cl_I(-1),p) * cln::expt(cln::log(x),n)
			* cln::expt(cln::log(1-x),p) / cln::factorial(n) / cln::factorial(p);

		for (int s=0; s<n; s++) {
			cln::cl_N res2;
			for (int r=0; r<p; r++) {
				res2 = res2 + cln::expt(cln::cl_I(-1),r) * cln::expt(cln::log(1-x),r)
					* S_do_sum(p-r,n-s,1-x,prec) / cln::factorial(r);
			}
			result = result + cln::expt(cln::log(x),s) * (S_num(n-s,p,1).to_cl_N() - res2) / cln::factorial(s);
		}

		return result;
	}
	
	return S_do_sum(n, p, x, prec);
}


// helper function for S(n,p,x)
numeric S_num(int n, int p, const numeric& x)
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
//		throw std::runtime_error("don't know how to evaluate this function!");
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
	if (cln::abs(value) > 1) {
		
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


} // end of anonymous namespace


//////////////////////////////////////////////////////////////////////
//
// Nielsen's generalized polylogarithm  S
//
// GiNaC function
//
//////////////////////////////////////////////////////////////////////


static ex S_eval(const ex& x1, const ex& x2, const ex& x3)
{
	if (x2 == 1) {
		return Li(x1+1,x3);
	}
	if (x3.info(info_flags::numeric) && (!x3.info(info_flags::crational)) && 
			x1.info(info_flags::posint) && x2.info(info_flags::posint)) {
		return S_num(ex_to<numeric>(x1).to_int(), ex_to<numeric>(x2).to_int(), ex_to<numeric>(x3));
	}
	return S(x1,x2,x3).hold();
}


static ex S_evalf(const ex& x1, const ex& x2, const ex& x3)
{
	if (is_a<numeric>(x1) && is_a<numeric>(x2) && is_a<numeric>(x3)) {
		return S_num(ex_to<numeric>(x1).to_int(), ex_to<numeric>(x2).to_int(), ex_to<numeric>(x3));
	}
	return S(x1,x2,x3).hold();
}


static ex S_series(const ex& x1, const ex& x2, const ex& x3, const relational& rel, int order, unsigned options)
{
	epvector seq;
	seq.push_back(expair(S(x1,x2,x3), 0));
	return pseries(rel,seq);
}


static ex S_deriv(const ex& x1, const ex& x2, const ex& x3, unsigned deriv_param)
{
	GINAC_ASSERT(deriv_param < 3);
	if (deriv_param < 2) {
		return _ex0;
	}
	if (x1 > 0) {
		return S(x1-1, x2, x3) / x3;
	} else {
		return S(x1, x2-1, x3) / (1-x3);
	}
}


REGISTER_FUNCTION(S,
		eval_func(S_eval).
		evalf_func(S_evalf).
		do_not_evalf_params().
		series_func(S_series).
		derivative_func(S_deriv));


//////////////////////////////////////////////////////////////////////
//
// Harmonic polylogarithm  H
//
// helper function
//
//////////////////////////////////////////////////////////////////////


// anonymous namespace for helper functions
namespace {


// forward declaration
ex convert_from_RV(const lst& parameterlst, const ex& arg);


// multiplies an one-dimensional H with another H
// [ReV] (18)
ex trafo_H_mult(const ex& h1, const ex& h2)
{
	ex res;
	ex hshort;
	lst hlong;
	ex h1nops = h1.op(0).nops();
	ex h2nops = h2.op(0).nops();
	if (h1nops > 1) {
		hshort = h2.op(0).op(0);
		hlong = ex_to<lst>(h1.op(0));
	} else {
		hshort = h1.op(0).op(0);
		if (h2nops > 1) {
			hlong = ex_to<lst>(h2.op(0));
		} else {
			hlong = h2.op(0).op(0);
		}
	}
	for (int i=0; i<=hlong.nops(); i++) {
		lst newparameter;
		int j=0;
		for (; j<i; j++) {
			newparameter.append(hlong[j]);
		}
		newparameter.append(hshort);
		for (; j<hlong.nops(); j++) {
			newparameter.append(hlong[j]);
		}
		res += H(newparameter, h1.op(1)).hold();
	}
	return res;
}


// applies trafo_H_mult recursively on expressions
struct map_trafo_H_mult : public map_function
{
	ex operator()(const ex& e)
	{
		if (is_a<add>(e)) {
			return e.map(*this);
		}

		if (is_a<mul>(e)) {

			ex result = 1;
			ex firstH;
			lst Hlst;
			for (int pos=0; pos<e.nops(); pos++) {
				if (is_a<power>(e.op(pos)) && is_a<function>(e.op(pos).op(0))) {
					std::string name = ex_to<function>(e.op(pos).op(0)).get_name();
					if (name == "H") {
						for (ex i=0; i<e.op(pos).op(1); i++) {
							Hlst.append(e.op(pos).op(0));
						}
						continue;
					}
				} else if (is_a<function>(e.op(pos))) {
					std::string name = ex_to<function>(e.op(pos)).get_name();
					if (name == "H") {
						if (e.op(pos).op(0).nops() > 1) {
							firstH = e.op(pos);
						} else {
							Hlst.append(e.op(pos));
						}
						continue;
					}
				}
				result *= e.op(pos);
			}
			if (firstH == 0) {
				if (Hlst.nops() > 0) {
					firstH = Hlst[Hlst.nops()-1];
					Hlst.remove_last();
				} else {
					return e;
				}
			}

			if (Hlst.nops() > 0) {
				ex buffer = trafo_H_mult(firstH, Hlst.op(0));
				result *= buffer;
				for (int i=1; i<Hlst.nops(); i++) {
					result *= Hlst.op(i);
				}
				result = result.expand();
				map_trafo_H_mult recursion;
				return recursion(result);
			} else {
				return e;
			}

		}
		return e;
	}
};


// do integration [ReV] (49)
// put parameter 1 in front of existing parameters
ex trafo_H_prepend_one(const ex& e, const ex& arg)
{
	ex h;
	std::string name;
	if (is_a<function>(e)) {
		name = ex_to<function>(e).get_name();
	}
	if (name == "H") {
		h = e;
	} else {
		for (int i=0; i<e.nops(); i++) {
			if (is_a<function>(e.op(i))) {
				std::string name = ex_to<function>(e.op(i)).get_name();
				if (name == "H") {
					h = e.op(i);
				}
			}
		}
	}
	if (h != 0) {
		lst newparameter = ex_to<lst>(h.op(0));
		newparameter.prepend(1);
		return e.subs(h == H(newparameter, h.op(1)).hold());
	} else {
		return e * H(lst(1),1-arg).hold();
	}
}


// do integration [ReV] (55)
// put parameter 0 in front of existing parameters
ex trafo_H_prepend_zero(const ex& e, const ex& arg)
{
	ex h;
	std::string name;
	if (is_a<function>(e)) {
		name = ex_to<function>(e).get_name();
	}
	if (name == "H") {
		h = e;
	} else {
		for (int i=0; i<e.nops(); i++) {
			if (is_a<function>(e.op(i))) {
				std::string name = ex_to<function>(e.op(i)).get_name();
				if (name == "H") {
					h = e.op(i);
				}
			}
		}
	}
	if (h != 0) {
		lst newparameter = ex_to<lst>(h.op(0));
		newparameter.prepend(0);
		ex addzeta = convert_from_RV(newparameter, 1).subs(H(wild(1),wild(2))==zeta(wild(1)));
		return e.subs(h == (addzeta-H(newparameter, h.op(1)).hold())).expand();
	} else {
		return e * (-H(lst(0),1/arg).hold());
	}
}


// do x -> 1-x transformation
struct map_trafo_H_1mx : public map_function
{
	ex operator()(const ex& e)
	{
		if (is_a<add>(e) || is_a<mul>(e)) {
			return e.map(*this);
		}
		
		if (is_a<function>(e)) {
			std::string name = ex_to<function>(e).get_name();
			if (name == "H") {

				lst parameter = ex_to<lst>(e.op(0));
				ex arg = e.op(1);

				// if all parameters are either zero or one return the transformed function
				if (find(parameter.begin(), parameter.end(), 0) == parameter.end()) {
					lst newparameter;
					for (int i=parameter.nops(); i>0; i--) {
						newparameter.append(0);
					}
					return pow(-1, parameter.nops()) * H(newparameter, 1-arg).hold();
				} else if (find(parameter.begin(), parameter.end(), 1) == parameter.end()) {
					lst newparameter;
					for (int i=parameter.nops(); i>0; i--) {
						newparameter.append(1);
					}
					return pow(-1, parameter.nops()) * H(newparameter, 1-arg).hold();
				}

				lst newparameter = parameter;
				newparameter.remove_first();

				if (parameter.op(0) == 0) {

					// leading zero
					ex res = convert_from_RV(parameter, 1).subs(H(wild(1),wild(2))==zeta(wild(1)));
					map_trafo_H_1mx recursion;
					ex buffer = recursion(H(newparameter, arg).hold());
					if (is_a<add>(buffer)) {
						for (int i=0; i<buffer.nops(); i++) {
							res -= trafo_H_prepend_one(buffer.op(i), arg);
						}
					} else {
						res -= trafo_H_prepend_one(buffer, arg);
					}
					return res;

				} else {

					// leading one
					map_trafo_H_1mx recursion;
					map_trafo_H_mult unify;
					ex res;
					int firstzero = 0;
					while (parameter.op(firstzero) == 1) {
						firstzero++;
					}
					for (int i=firstzero-1; i<parameter.nops()-1; i++) {
						lst newparameter;
						int j=0;
						for (; j<=i; j++) {
							newparameter.append(parameter[j+1]);
						}
						newparameter.append(1);
						for (; j<parameter.nops()-1; j++) {
							newparameter.append(parameter[j+1]);
						}
						res -= H(newparameter, arg).hold();
					}
					return (unify((-H(lst(0), 1-arg).hold() * recursion(H(newparameter, arg).hold())).expand()) +
							recursion(res)) / firstzero;

				}

			}
		}
		return e;
	}
};


// do x -> 1/x transformation
struct map_trafo_H_1overx : public map_function
{
	ex operator()(const ex& e)
	{
		if (is_a<add>(e) || is_a<mul>(e)) {
			return e.map(*this);
		}

		if (is_a<function>(e)) {
			std::string name = ex_to<function>(e).get_name();
			if (name == "H") {

				lst parameter = ex_to<lst>(e.op(0));
				ex arg = e.op(1);

				// if all parameters are either zero or one return the transformed function
				if (find(parameter.begin(), parameter.end(), 0) == parameter.end()) {
					map_trafo_H_mult unify;
					return unify((pow(H(lst(1),1/arg).hold() + H(lst(0),1/arg).hold() - I*Pi, parameter.nops()) / 
								factorial(parameter.nops())).expand());
				} else if (find(parameter.begin(), parameter.end(), 1) == parameter.end()) {
					return pow(-1, parameter.nops()) * H(parameter, 1/arg).hold();
				}

				lst newparameter = parameter;
				newparameter.remove_first();

				if (parameter.op(0) == 0) {
					
					// leading zero
					ex res = convert_from_RV(parameter, 1).subs(H(wild(1),wild(2))==zeta(wild(1)));
					map_trafo_H_1overx recursion;
					ex buffer = recursion(H(newparameter, arg).hold());
					if (is_a<add>(buffer)) {
						for (int i=0; i<buffer.nops(); i++) {
							res += trafo_H_prepend_zero(buffer.op(i), arg);
						}
					} else {
						res += trafo_H_prepend_zero(buffer, arg);
					}
					return res;

				} else {

					// leading one
					map_trafo_H_1overx recursion;
					map_trafo_H_mult unify;
					ex res = H(lst(1), arg).hold() * H(newparameter, arg).hold();
					int firstzero = 0;
					while (parameter.op(firstzero) == 1) {
						firstzero++;
					}
					for (int i=firstzero-1; i<parameter.nops()-1; i++) {
						lst newparameter;
						int j=0;
						for (; j<=i; j++) {
							newparameter.append(parameter[j+1]);
						}
						newparameter.append(1);
						for (; j<parameter.nops()-1; j++) {
							newparameter.append(parameter[j+1]);
						}
						res -= H(newparameter, arg).hold();
					}
					res = recursion(res).expand() / firstzero;
					return unify(res);

				}

			}
		}
		return e;
	}
};


// remove trailing zeros from H-parameters
struct map_trafo_H_reduce_trailing_zeros : public map_function
{
	ex operator()(const ex& e)
	{
		if (is_a<add>(e) || is_a<mul>(e)) {
			return e.map(*this);
		}
		if (is_a<function>(e)) {
			std::string name = ex_to<function>(e).get_name();
			if (name == "H") {
				lst parameter;
				if (is_a<lst>(e.op(0))) {
						parameter = ex_to<lst>(e.op(0));
				} else {
					parameter = lst(e.op(0));
				}
				ex arg = e.op(1);
				if (parameter.op(parameter.nops()-1) == 0) {
					
					//
					if (parameter.nops() == 1) {
						return log(arg);
					}
					
					//
					lst::const_iterator it = parameter.begin();
					while ((it != parameter.end()) && (*it == 0)) {
						it++;
					}
					if (it == parameter.end()) {
						return pow(log(arg),parameter.nops()) / factorial(parameter.nops());
					}
					
					//
					parameter.remove_last();
					int lastentry = parameter.nops();
					while ((lastentry > 0) && (parameter[lastentry-1] == 0)) {
						lastentry--;
					}
					
					//
					ex result = log(arg) * H(parameter,arg).hold();
					for (ex i=0; i<lastentry; i++) {
						parameter[i]++;
						result -= (parameter[i]-1) * H(parameter, arg).hold();
						parameter[i]--;
					}
					
					if (lastentry < parameter.nops()) {
						result = result / (parameter.nops()-lastentry+1);
						return result.map(*this);
					} else {
						return result;
					}
				}
			}
		}
		return e;
	}
};


// recursively call convert_from_RV on expression
struct map_trafo_H_convert : public map_function
{
	ex operator()(const ex& e)
	{
		if (is_a<add>(e) || is_a<mul>(e) || is_a<power>(e)) {
			return e.map(*this);
		}
		if (is_a<function>(e)) {
			std::string name = ex_to<function>(e).get_name();
			if (name == "H") {
				lst parameter = ex_to<lst>(e.op(0));
				ex arg = e.op(1);
				return convert_from_RV(parameter, arg);
			}
		}
		return e;
	}
};


// translate notation from nested sums to Remiddi/Vermaseren
lst convert_to_RV(const lst& o)
{
	lst res;
	for (lst::const_iterator it = o.begin(); it != o.end(); it++) {
		for (ex i=0; i<(*it)-1; i++) {
			res.append(0);
		}
		res.append(1);
	}
	return res;
}


// translate notation from Remiddi/Vermaseren to nested sums
ex convert_from_RV(const lst& parameterlst, const ex& arg)
{
	lst newparameterlst;

	lst::const_iterator it = parameterlst.begin();
	int count = 1;
	while (it != parameterlst.end()) {
		if (*it == 0) {
			count++;
		} else {
			newparameterlst.append((*it>0) ? count : -count);
			count = 1;
		}
		it++;
	}
	for (int i=1; i<count; i++) {
		newparameterlst.append(0);
	}
	
	map_trafo_H_reduce_trailing_zeros filter;
	return filter(H(newparameterlst, arg).hold());
}


// do the actual summation.
cln::cl_N H_do_sum(const std::vector<int>& s, const cln::cl_N& x)
{
	const int j = s.size();

	std::vector<cln::cl_N> t(j);

	cln::cl_F one = cln::cl_float(1, cln::float_format(Digits));
	cln::cl_N factor = cln::expt(x, j) * one;
	cln::cl_N t0buf;
	int q = 0;
	do {
		t0buf = t[0];
		q++;
		t[j-1] = t[j-1] + 1 / cln::expt(cln::cl_I(q),s[j-1]);
		for (int k=j-2; k>=1; k--) {
			t[k] = t[k] + t[k+1] / cln::expt(cln::cl_I(q+j-1-k), s[k]);
		}
		t[0] = t[0] + t[1] * factor / cln::expt(cln::cl_I(q+j-1), s[0]);
		factor = factor * x;
	} while (t[0] != t0buf);
	
	return t[0];
}


} // end of anonymous namespace


//////////////////////////////////////////////////////////////////////
//
// Harmonic polylogarithm  H
//
// GiNaC function
//
//////////////////////////////////////////////////////////////////////


static ex H_eval(const ex& x1, const ex& x2)
{
	if (x2 == 0) {
		return 0;
	}
	if (x2 == 1) {
		return zeta(x1);
	}
	if (x1.nops() == 1) {
		return Li(x1.op(0), x2);
	}
	if (x2.info(info_flags::numeric) && (!x2.info(info_flags::crational))) {
		return H(x1,x2).evalf();
	}
	return H(x1,x2).hold();
}


static ex H_evalf(const ex& x1, const ex& x2)
{
	if (is_a<lst>(x1) && is_a<numeric>(x2)) {
		for (int i=0; i<x1.nops(); i++) {
			if (!x1.op(i).info(info_flags::posint)) {
				return H(x1,x2).hold();
			}
		}
		if (x1.nops() < 1) {
			return _ex1;
		}
		if (x1.nops() == 1) {
			return Li(x1.op(0), x2).evalf();
		}
		cln::cl_N x = ex_to<numeric>(x2).to_cl_N();
		if (x == 1) {
			return zeta(x1).evalf();
		}

		// choose trafo
		if (cln::abs(x) > 1) {
			symbol xtemp("xtemp");
			map_trafo_H_1overx trafo;
			ex res = trafo(H(convert_to_RV(ex_to<lst>(x1)), xtemp));
			map_trafo_H_convert converter;
			res = converter(res);
			return res.subs(xtemp==x2).evalf();
		}

		// since the x->1-x transformation produces a lot of terms, it is only
		// efficient for argument near one.
		if (cln::realpart(x) > 0.95) {
			symbol xtemp("xtemp");
			map_trafo_H_1mx trafo;
			ex res = trafo(H(convert_to_RV(ex_to<lst>(x1)), xtemp));
			map_trafo_H_convert converter;
			res = converter(res);
			return res.subs(xtemp==x2).evalf();
		}

		// no trafo -> do summation
		int count = x1.nops();
		std::vector<int> r(count);
		for (int i=0; i<count; i++) {
			r[i] = ex_to<numeric>(x1.op(i)).to_int();
		}

		return numeric(H_do_sum(r,x));
	}

	return H(x1,x2).hold();
}


static ex H_series(const ex& x1, const ex& x2, const relational& rel, int order, unsigned options)
{
	epvector seq;
	seq.push_back(expair(H(x1,x2), 0));
	return pseries(rel,seq);
}


static ex H_deriv(const ex& x1, const ex& x2, unsigned deriv_param)
{
	GINAC_ASSERT(deriv_param < 2);
	if (deriv_param == 0) {
		return _ex0;
	}
	if (is_a<lst>(x1)) {
		lst newparameter = ex_to<lst>(x1);
		if (x1.op(0) == 1) {
			newparameter.remove_first();
			return 1/(1-x2) * H(newparameter, x2);
		} else {
			newparameter[0]--;
			return H(newparameter, x2).hold() / x2;
		}
	} else {
		if (x1 == 1) {
			return 1/(1-x2);
		} else {
			return H(x1-1, x2).hold() / x2;
		}
	}
}


REGISTER_FUNCTION(H,
		eval_func(H_eval).
		evalf_func(H_evalf).
		do_not_evalf_params().
		series_func(H_series).
		derivative_func(H_deriv));


//////////////////////////////////////////////////////////////////////
//
// Multiple zeta values  zeta
//
// helper functions
//
//////////////////////////////////////////////////////////////////////


// anonymous namespace for helper functions
namespace {


// parameters and data for [Cra] algorithm
const cln::cl_N lambda = cln::cl_N("319/320");
int L1;
int L2;
std::vector<std::vector<cln::cl_N> > f_kj;
std::vector<cln::cl_N> crB;
std::vector<std::vector<cln::cl_N> > crG;
std::vector<cln::cl_N> crX;


void halfcyclic_convolute(const std::vector<cln::cl_N>& a, const std::vector<cln::cl_N>& b, std::vector<cln::cl_N>& c)
{
	const int size = a.size();
	for (int n=0; n<size; n++) {
		c[n] = 0;
		for (int m=0; m<=n; m++) {
			c[n] = c[n] + a[m]*b[n-m];
		}
	}
}


// [Cra] section 4
void initcX(const std::vector<int>& s)
{
	const int k = s.size();

	crX.clear();
	crG.clear();
	crB.clear();

	for (int i=0; i<=L2; i++) {
		crB.push_back(bernoulli(i).to_cl_N() / cln::factorial(i));
	}

	int Sm = 0;
	int Smp1 = 0;
	for (int m=0; m<k-1; m++) {
		std::vector<cln::cl_N> crGbuf;
		Sm = Sm + s[m];
		Smp1 = Sm + s[m+1];
		for (int i=0; i<=L2; i++) {
			crGbuf.push_back(cln::factorial(i + Sm - m - 2) / cln::factorial(i + Smp1 - m - 2));
		}
		crG.push_back(crGbuf);
	}

	crX = crB;

	for (int m=0; m<k-1; m++) {
		std::vector<cln::cl_N> Xbuf;
		for (int i=0; i<=L2; i++) {
			Xbuf.push_back(crX[i] * crG[m][i]);
		}
		halfcyclic_convolute(Xbuf, crB, crX);
	}
}


// [Cra] section 4
cln::cl_N crandall_Y_loop(const cln::cl_N& Sqk)
{
	cln::cl_F one = cln::cl_float(1, cln::float_format(Digits));
	cln::cl_N factor = cln::expt(lambda, Sqk);
	cln::cl_N res = factor / Sqk * crX[0] * one;
	cln::cl_N resbuf;
	int N = 0;
	do {
		resbuf = res;
		factor = factor * lambda;
		N++;
		res = res + crX[N] * factor / (N+Sqk);
	} while ((res != resbuf) || cln::zerop(crX[N]));
	return res;
}


// [Cra] section 4
void calc_f(int maxr)
{
	f_kj.clear();
	f_kj.resize(L1);
	
	cln::cl_N t0, t1, t2, t3, t4;
	int i, j, k;
	std::vector<std::vector<cln::cl_N> >::iterator it = f_kj.begin();
	cln::cl_F one = cln::cl_float(1, cln::float_format(Digits));
	
	t0 = cln::exp(-lambda);
	t2 = 1;
	for (k=1; k<=L1; k++) {
		t1 = k * lambda;
		t2 = t0 * t2;
		for (j=1; j<=maxr; j++) {
			t3 = 1;
			t4 = 1;
			for (i=2; i<=j; i++) {
				t4 = t4 * (j-i+1);
				t3 = t1 * t3 + t4;
			}
			(*it).push_back(t2 * t3 * cln::expt(cln::cl_I(k),-j) * one);
		}
		it++;
	}
}


// [Cra] (3.1)
cln::cl_N crandall_Z(const std::vector<int>& s)
{
	const int j = s.size();

	if (j == 1) {	
		cln::cl_N t0;
		cln::cl_N t0buf;
		int q = 0;
		do {
			t0buf = t0;
			q++;
			t0 = t0 + f_kj[q+j-2][s[0]-1];
		} while (t0 != t0buf);
		
		return t0 / cln::factorial(s[0]-1);
	}

	std::vector<cln::cl_N> t(j);

	cln::cl_N t0buf;
	int q = 0;
	do {
		t0buf = t[0];
		q++;
		t[j-1] = t[j-1] + 1 / cln::expt(cln::cl_I(q),s[j-1]);
		for (int k=j-2; k>=1; k--) {
			t[k] = t[k] + t[k+1] / cln::expt(cln::cl_I(q+j-1-k), s[k]);
		}
		t[0] = t[0] + t[1] * f_kj[q+j-2][s[0]-1];
	} while (t[0] != t0buf);
	
	return t[0] / cln::factorial(s[0]-1);
}


// [Cra] (2.4)
cln::cl_N zeta_do_sum_Crandall(const std::vector<int>& s)
{
	std::vector<int> r = s;
	const int j = r.size();

	// decide on maximal size of f_kj for crandall_Z
	if (Digits < 50) {
		L1 = 150;
	} else {
		L1 = Digits * 3 + j*2;
	}

	// decide on maximal size of crX for crandall_Y
	if (Digits < 38) {
		L2 = 63;
	} else if (Digits < 86) {
		L2 = 127;
	} else if (Digits < 192) {
		L2 = 255;
	} else if (Digits < 394) {
		L2 = 511;
	} else if (Digits < 808) {
		L2 = 1023;
	} else {
		L2 = 2047;
	}

	cln::cl_N res;

	int maxr = 0;
	int S = 0;
	for (int i=0; i<j; i++) {
		S += r[i];
		if (r[i] > maxr) {
			maxr = r[i];
		}
	}

	calc_f(maxr);

	const cln::cl_N r0factorial = cln::factorial(r[0]-1);

	std::vector<int> rz;
	int skp1buf;
	int Srun = S;
	for (int k=r.size()-1; k>0; k--) {

		rz.insert(rz.begin(), r.back());
		skp1buf = rz.front();
		Srun -= skp1buf;
		r.pop_back();

		initcX(r);
		
		for (int q=0; q<skp1buf; q++) {
			
			cln::cl_N pp1 = crandall_Y_loop(Srun+q-k);
			cln::cl_N pp2 = crandall_Z(rz);

			rz.front()--;
			
			if (q & 1) {
				res = res - pp1 * pp2 / cln::factorial(q);
			} else {
				res = res + pp1 * pp2 / cln::factorial(q);
			}
		}
		rz.front() = skp1buf;
	}
	rz.insert(rz.begin(), r.back());

	initcX(rz);

	res = (res + crandall_Y_loop(S-j)) / r0factorial + crandall_Z(rz);

	return res;
}


cln::cl_N zeta_do_sum_simple(const std::vector<int>& r)
{
	const int j = r.size();

	// buffer for subsums
	std::vector<cln::cl_N> t(j);
	cln::cl_F one = cln::cl_float(1, cln::float_format(Digits));

	cln::cl_N t0buf;
	int q = 0;
	do {
		t0buf = t[0];
		q++;
		t[j-1] = t[j-1] + one / cln::expt(cln::cl_I(q),r[j-1]);
		for (int k=j-2; k>=0; k--) {
			t[k] = t[k] + one * t[k+1] / cln::expt(cln::cl_I(q+j-1-k), r[k]);
		}
	} while (t[0] != t0buf);

	return t[0];
}


} // end of anonymous namespace


//////////////////////////////////////////////////////////////////////
//
// Multiple zeta values  zeta
//
// GiNaC function
//
//////////////////////////////////////////////////////////////////////


static ex zeta1_evalf(const ex& x)
{
	if (is_exactly_a<lst>(x) && (x.nops()>1)) {

		// multiple zeta value
		const int count = x.nops();
		const lst& xlst = ex_to<lst>(x);
		std::vector<int> r(count);

		// check parameters and convert them
		lst::const_iterator it1 = xlst.begin();
		std::vector<int>::iterator it2 = r.begin();
		do {
			if (!(*it1).info(info_flags::posint)) {
				return zeta(x).hold();
			}
			*it2 = ex_to<numeric>(*it1).to_int();
			it1++;
			it2++;
		} while (it2 != r.end());

		// check for divergence
		if (r[0] == 1) {
			return zeta(x).hold();
		}

		// decide on summation algorithm
		// this is still a bit clumsy
		int limit = (Digits>17) ? 10 : 6;
		if ((r[0] < limit) || ((count > 3) && (r[1] < limit/2))) {
			return numeric(zeta_do_sum_Crandall(r));
		} else {
			return numeric(zeta_do_sum_simple(r));
		}
	}
		
	// single zeta value
	if (is_exactly_a<numeric>(x) && (x != 1)) {
		try {
			return zeta(ex_to<numeric>(x));
		} catch (const dunno &e) { }
	}

	return zeta(x).hold();
}


static ex zeta1_eval(const ex& x)
{
	if (is_exactly_a<lst>(x)) {
		if (x.nops() == 1) {
			return zeta(x.op(0));
		}
		return zeta(x).hold();
	}

	if (x.info(info_flags::numeric)) {
		const numeric& y = ex_to<numeric>(x);
		// trap integer arguments:
		if (y.is_integer()) {
			if (y.is_zero()) {
				return _ex_1_2;
			}
			if (y.is_equal(_num1)) {
				return zeta(x).hold();
			}
			if (y.info(info_flags::posint)) {
				if (y.info(info_flags::odd)) {
					return zeta(x).hold();
				} else {
					return abs(bernoulli(y)) * pow(Pi, y) * pow(_num2, y-_num1) / factorial(y);
				}
			} else {
				if (y.info(info_flags::odd)) {
					return -bernoulli(_num1-y) / (_num1-y);
				} else {
					return _ex0;
				}
			}
		}
		// zeta(float)
		if (y.info(info_flags::numeric) && !y.info(info_flags::crational))
			return zeta1_evalf(x);
	}
	return zeta(x).hold();
}


static ex zeta1_deriv(const ex& x, unsigned deriv_param)
{
	GINAC_ASSERT(deriv_param==0);

	if (is_exactly_a<lst>(x)) {
		return _ex0;
	} else {
		return zeta(_ex1, x);
	}
}


unsigned zeta1_SERIAL::serial =
			function::register_new(function_options("zeta").
						eval_func(zeta1_eval).
						evalf_func(zeta1_evalf).
						do_not_evalf_params().
						derivative_func(zeta1_deriv).
						latex_name("\\zeta").
						overloaded(2));


//////////////////////////////////////////////////////////////////////
//
// Multiple zeta values  mZeta
//
// The use of mZeta is deprecated! This function will be removed
// from GiNaC source soon. Use zeta instead!!
//
// GiNaC function
//
//////////////////////////////////////////////////////////////////////


static ex mZeta_eval(const ex& x1)
{
	return mZeta(x1).hold();
}


static ex mZeta_evalf(const ex& x1)
{
	if (is_a<lst>(x1)) {
		for (int i=0; i<x1.nops(); i++) {
			if (!x1.op(i).info(info_flags::posint))
				return mZeta(x1).hold();
		}

		const int j = x1.nops();

		std::vector<int> r(j);
		for (int i=0; i<j; i++) {
			r[j-1-i] = ex_to<numeric>(x1.op(i)).to_int();
		}

		// check for divergence
		if (r[0] == 1) {
			return mZeta(x1).hold();
		}

		// if only one argument, use cln::zeta
		if (j == 1) {
			return numeric(cln::zeta(r[0]));
		}
		
		// decide on summation algorithm
		// this is still a bit clumsy
		int limit = (Digits>17) ? 10 : 6;
		if (r[0]<limit || (j>3 && r[1]<limit/2)) {
			return numeric(zeta_do_sum_Crandall(r));
		} else {
			return numeric(zeta_do_sum_simple(r));
		}
	} else if (x1.info(info_flags::posint) && (x1 != 1)) {
		return numeric(cln::zeta(ex_to<numeric>(x1).to_int()));
	}

	return mZeta(x1).hold();
}


static ex mZeta_deriv(const ex& x, unsigned deriv_param)
{
	return 0;
}


REGISTER_FUNCTION(mZeta, 
		eval_func(mZeta_eval).
		evalf_func(mZeta_evalf).
		do_not_evalf_params().
		derivative_func(mZeta_deriv));


} // namespace GiNaC

