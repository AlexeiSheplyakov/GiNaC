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
 *    	evaluated in the whole complex plane. And of course, there is still room for speed optimizations ;-).
 *    - The calculation of classical polylogarithms is speed up by using Euler-Maclaurin summation (EuMac).
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
#include "pseries.h"


namespace GiNaC {

	
// lookup table for Euler-Maclaurin optimization
// see fill_Xn()
std::vector<std::vector<cln::cl_N> > Xn;
int xnsize = 0;


// lookup table for special Euler-Zagier-Sums (used for S_n,p(x))
// see fill_Yn()
std::vector<std::vector<cln::cl_N> > Yn;
int ynsize = 0; // number of Yn[]
int ynlength = 100; // initial length of all Yn[i]


//////////////////////
// helper functions //
//////////////////////


// This function calculates the X_n. The X_n are needed for the Euler-Maclaurin summation (EMS) of
// classical polylogarithms.
// With EMS the polylogs can be calculated as follows:
//   Li_p (x)  =  \sum_{n=0}^\infty X_{p-2}(n) u^{n+1}/(n+1)! with  u = -log(1-x)
//   X_0(n) = B_n (Bernoulli numbers)
//   X_p(n) = \sum_{k=0}^n binomial(n,k) B_{n-k} / (k+1) * X_{p-1}(k)
// The calculation of Xn depends on X0 and X{n-1}.
// X_0 is special, it holds only the non-zero Bernoulli numbers with index 2 or greater.
// This results in a slightly more complicated algorithm for the X_n.
// The first index in Xn corresponds to the index of the polylog minus 2.
// The second index in Xn corresponds to the index from the EMS.
static void fill_Xn(int n)
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


// This function calculates the Y_n. The Y_n are needed for the evaluation of S_{n,p}(x).
// The Y_n are basically Euler-Zagier sums with all m_i=1. They are subsums in the Z-sum
// representing S_{n,p}(x).
// The first index in Y_n corresponds to the parameter p minus one, i.e. the depth of the
// equivalent Z-sum.
// The second index in Y_n corresponds to the running index of the outermost sum in the full Z-sum
// representing S_{n,p}(x).
// The calculation of Y_n uses the values from Y_{n-1}.
static void fill_Yn(int n, const cln::float_format_t& prec)
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
static void make_Yn_longer(int newsize, const cln::float_format_t& prec)
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


// calculates Li(2,x) without EuMac
static cln::cl_N Li2_series(const cln::cl_N& x)
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


// calculates Li(2,x) with EuMac
static cln::cl_N Li2_series_EuMac(const cln::cl_N& x)
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


// calculates Li(n,x), n>2 without EuMac
static cln::cl_N Lin_series(int n, const cln::cl_N& x)
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


// calculates Li(n,x), n>2 with EuMac
static cln::cl_N Lin_series_EuMac(int n, const cln::cl_N& x)
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
static numeric S_num(int n, int p, const numeric& x);


// helper function for classical polylog Li
static cln::cl_N Li_projection(int n, const cln::cl_N& x, const cln::float_format_t& prec)
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
				
				return Li2_series(x);
			} else {
				return Li2_series_EuMac(x);
			}
		} else {
			// choose the faster algorithm
			if (cln::abs(cln::realpart(x)) > 0.75) {
				return -Li2_series(1-x) - cln::log(x) * cln::log(1-x) + cln::zeta(2);
			} else {
				return -Li2_series_EuMac(1-x) - cln::log(x) * cln::log(1-x) + cln::zeta(2);
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
			// with n>=12 the "normal" summation always wins against EuMac
			if ((cln::abs(cln::realpart(x)) < 0.3) || (n >= 12)) {
				return Lin_series(n, x);
			} else {
				return Lin_series_EuMac(n, x);
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
		if (x2.info(info_flags::numeric) && (!x2.info(info_flags::crational)))
			return Li_num(ex_to<numeric>(x1).to_int(), ex_to<numeric>(x2));
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
			if (x2.op(i) >= 1)
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

static ex Li_series(const ex& x1, const ex& x2, const relational& rel, int order, unsigned options)
{
	epvector seq;
	seq.push_back(expair(Li(x1,x2), 0));
	return pseries(rel,seq);
}

REGISTER_FUNCTION(Li, eval_func(Li_eval).evalf_func(Li_evalf).do_not_evalf_params().series_func(Li_series));


// Nielsen's generalized polylogarithm

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
		if ((x3 == -1) && (x2 != 1)) {
			// no formula to evaluate this ... sorry
//			return S(x1,x2,x3).hold();
		}
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

REGISTER_FUNCTION(S, eval_func(S_eval).evalf_func(S_evalf).do_not_evalf_params().series_func(S_series));


// Harmonic polylogarithm

static ex H_eval(const ex& x1, const ex& x2)
{
	if (x2.info(info_flags::numeric) && (!x2.info(info_flags::crational))) {
		return H(x1,x2).evalf();
	}
	return H(x1,x2).hold();
}

static ex H_evalf(const ex& x1, const ex& x2)
{
	if (is_a<lst>(x1) && is_a<numeric>(x2)) {
		for (int i=0; i<x1.nops(); i++) {
			if (!is_a<numeric>(x1.op(i)))
				return H(x1,x2).hold();
		}
		if (x2 >= 1) {
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

static ex H_series(const ex& x1, const ex& x2, const relational& rel, int order, unsigned options)
{
	epvector seq;
	seq.push_back(expair(H(x1,x2), 0));
	return pseries(rel,seq);
}

REGISTER_FUNCTION(H, eval_func(H_eval).evalf_func(H_evalf).do_not_evalf_params().series_func(H_series));


// Multiple zeta value

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

		return numeric(t[0]);
	}

	return mZeta(x1).hold();
}

static ex mZeta_series(const ex& x1, const relational& rel, int order, unsigned options)
{
	epvector seq;
	seq.push_back(expair(mZeta(x1), 0));
	return pseries(rel,seq);
}

REGISTER_FUNCTION(mZeta, eval_func(mZeta_eval).evalf_func(mZeta_evalf).do_not_evalf_params().series_func(mZeta_series));


} // namespace GiNaC

