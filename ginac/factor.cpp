/** @file factor.cpp
 *
 *  Polynomial factorization code (implementation).
 *
 *  Algorithms used can be found in
 *    [W1]  An Improved Multivariate Polynomial Factoring Algorithm,
 *          P.S.Wang, Mathematics of Computation, Vol. 32, No. 144 (1978) 1215--1231.
 *    [GCL] Algorithms for Computer Algebra,
 *          K.O.Geddes, S.R.Czapor, G.Labahn, Springer Verlag, 1992.
 */

/*
 *  GiNaC Copyright (C) 1999-2008 Johannes Gutenberg University Mainz, Germany
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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

//#define DEBUGFACTOR

#include "factor.h"

#include "ex.h"
#include "numeric.h"
#include "operators.h"
#include "inifcns.h"
#include "symbol.h"
#include "relational.h"
#include "power.h"
#include "mul.h"
#include "normal.h"
#include "add.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <list>
#include <vector>
#ifdef DEBUGFACTOR
#include <ostream>
#endif
using namespace std;

#include <cln/cln.h>
using namespace cln;

namespace GiNaC {

#ifdef DEBUGFACTOR
#define DCOUT(str) cout << #str << endl
#define DCOUTVAR(var) cout << #var << ": " << var << endl
#define DCOUT2(str,var) cout << #str << ": " << var << endl
#else
#define DCOUT(str)
#define DCOUTVAR(var)
#define DCOUT2(str,var)
#endif

// anonymous namespace to hide all utility functions
namespace {

typedef vector<cl_MI> mvec;
#ifdef DEBUGFACTOR
ostream& operator<<(ostream& o, const vector<cl_MI>& v)
{
	vector<cl_MI>::const_iterator i = v.begin(), end = v.end();
	while ( i != end ) {
		o << *i++ << " ";
	}
	return o;
}
ostream& operator<<(ostream& o, const vector< vector<cl_MI> >& v)
{
	vector< vector<cl_MI> >::const_iterator i = v.begin(), end = v.end();
	while ( i != end ) {
		o << *i++ << endl;
	}
	return o;
}
#endif

////////////////////////////////////////////////////////////////////////////////
// modular univariate polynomial code

//typedef cl_UP_MI umod;
typedef std::vector<cln::cl_MI> umodpoly;
//typedef vector<umod> umodvec;
typedef vector<umodpoly> upvec;

// COPY FROM UPOLY.HPP

// CHANGED size_t -> int !!!
template<typename T> static int degree(const T& p)
{
	return p.size() - 1;
}

template<typename T> static typename T::value_type lcoeff(const T& p)
{
	return p[p.size() - 1];
}

static bool normalize_in_field(umodpoly& a)
{
	if (a.size() == 0)
		return true;
	if ( lcoeff(a) == a[0].ring()->one() ) {
		return true;
	}

	const cln::cl_MI lc_1 = recip(lcoeff(a));
	for (std::size_t k = a.size(); k-- != 0; )
		a[k] = a[k]*lc_1;
	return false;
}

template<typename T> static void
canonicalize(T& p, const typename T::size_type hint = std::numeric_limits<typename T::size_type>::max())
{
	if (p.empty())
		return;

	std::size_t i = p.size() - 1;
	// Be fast if the polynomial is already canonicalized
	if (!zerop(p[i]))
		return;

	if (hint < p.size())
		i = hint;

	bool is_zero = false;
	do {
		if (!zerop(p[i])) {
			++i;
			break;
		}
		if (i == 0) {
			is_zero = true;
			break;
		}
		--i;
	} while (true);

	if (is_zero) {
		p.clear();
		return;
	}

	p.erase(p.begin() + i, p.end());
}

// END COPY FROM UPOLY.HPP

static void expt_pos(const umodpoly& a, unsigned int q, umodpoly& b)
{
	throw runtime_error("expt_pos: not implemented!");
	// code below is not correct!
//	b.clear();
//	if ( a.empty() ) return;
//	b.resize(degree(a)*q+1, a[0].ring()->zero());
//	cl_MI norm = recip(a[0]);
//	umodpoly an = a;
//	for ( size_t i=0; i<an.size(); ++i ) {
//		an[i] = an[i] * norm;
//	}
//	b[0] = a[0].ring()->one();
//	for ( size_t i=1; i<b.size(); ++i ) {
//		for ( size_t j=1; j<i; ++j ) {
//			b[i] = b[i] + ((i-j+1)*q-i-1) * a[i-j] * b[j-1];
//		}
//		b[i] = b[i] / i;
//	}
//	cl_MI corr = expt_pos(a[0], q);
//	for ( size_t i=0; i<b.size(); ++i ) {
//		b[i] = b[i] * corr;
//	}
}

static umodpoly operator+(const umodpoly& a, const umodpoly& b)
{
	int sa = a.size();
	int sb = b.size();
	if ( sa >= sb ) {
		umodpoly r(sa);
		int i = 0;
		for ( ; i<sb; ++i ) {
			r[i] = a[i] + b[i];
		}
		for ( ; i<sa; ++i ) {
			r[i] = a[i];
		}
		canonicalize(r);
		return r;
	}
	else {
		umodpoly r(sb);
		int i = 0;
		for ( ; i<sa; ++i ) {
			r[i] = a[i] + b[i];
		}
		for ( ; i<sb; ++i ) {
			r[i] = b[i];
		}
		canonicalize(r);
		return r;
	}
}

static umodpoly operator-(const umodpoly& a, const umodpoly& b)
{
	int sa = a.size();
	int sb = b.size();
	if ( sa >= sb ) {
		umodpoly r(sa);
		int i = 0;
		for ( ; i<sb; ++i ) {
			r[i] = a[i] - b[i];
		}
		for ( ; i<sa; ++i ) {
			r[i] = a[i];
		}
		canonicalize(r);
		return r;
	}
	else {
		umodpoly r(sb);
		int i = 0;
		for ( ; i<sa; ++i ) {
			r[i] = a[i] - b[i];
		}
		for ( ; i<sb; ++i ) {
			r[i] = -b[i];
		}
		canonicalize(r);
		return r;
	}
}

static umodpoly operator*(const umodpoly& a, const umodpoly& b)
{
	umodpoly c;
	if ( a.empty() || b.empty() ) return c;

	int n = degree(a) + degree(b);
	c.resize(n+1, a[0].ring()->zero());
	for ( int i=0 ; i<=n; ++i ) {
		for ( int j=0 ; j<=i; ++j ) {
			if ( j > degree(a) || (i-j) > degree(b) ) continue; // TODO optimize!
			c[i] = c[i] + a[j] * b[i-j];
		}
	}
	canonicalize(c);
	return c;
}

static umodpoly operator*(const umodpoly& a, const cl_MI& x)
{
	umodpoly r(a.size());
	for ( size_t i=0; i<a.size(); ++i ) {
		r[i] = a[i] * x;
	}
	canonicalize(r);
	return r;
}

static void umodpoly_from_ex(umodpoly& ump, const ex& e, const ex& x, const cl_modint_ring& R)
{
	// assert: e is in Z[x]
	int deg = e.degree(x);
	ump.resize(deg+1);
	int ldeg = e.ldegree(x);
	for ( ; deg>=ldeg; --deg ) {
		cl_I coeff = the<cl_I>(ex_to<numeric>(e.coeff(x, deg)).to_cl_N());
		ump[deg] = R->canonhom(coeff);
	}
	for ( ; deg>=0; --deg ) {
		ump[deg] = R->zero();
	}
	canonicalize(ump);
}

static void umodpoly_from_ex(umodpoly& ump, const ex& e, const ex& x, const cl_I& modulus)
{
	umodpoly_from_ex(ump, e, x, find_modint_ring(modulus));
}

static ex umod_to_ex(const umodpoly& a, const ex& x)
{
	if ( a.empty() ) return 0;
	cl_modint_ring R = a[0].ring();
	cl_I mod = R->modulus;
	cl_I halfmod = (mod-1) >> 1;
	ex e;
	for ( int i=degree(a); i>=0; --i ) {
		cl_I n = R->retract(a[i]);
		if ( n > halfmod ) {
			e += numeric(n-mod) * pow(x, i);
		} else {
			e += numeric(n) * pow(x, i);
		}
	}
	return e;
}

/** Divides all coefficients of the polynomial a by the integer x.
 *  All coefficients are supposed to be divisible by x. If they are not, the
 *  the<cl_I> cast will raise an exception.
 *
 *  @param[in,out] a  polynomial of which the coefficients will be reduced by x
 *  @param[in]     x  integer that divides the coefficients
 */
static void reduce_coeff(umodpoly& a, const cl_I& x)
{
	if ( a.empty() ) return;

	cl_modint_ring R = a[0].ring();
	umodpoly::iterator i = a.begin(), end = a.end();
	for ( ; i!=end; ++i ) {
		// cln cannot perform this division in the modular field
		cl_I c = R->retract(*i);
		*i = cl_MI(R, the<cl_I>(c / x));
	}
}

/** Calculates remainder of a/b.
 *  Assertion: a and b not empty.
 *
 *  @param[in]  a  polynomial dividend
 *  @param[in]  b  polynomial divisor
 *  @param[out] r  polynomial remainder
 */
static void rem(const umodpoly& a, const umodpoly& b, umodpoly& r)
{
	int k, n;
	n = degree(b);
	k = degree(a) - n;
	r = a;
	if ( k < 0 ) return;

	do {
		cl_MI qk = div(r[n+k], b[n]);
		if ( !zerop(qk) ) {
			for ( int i=0; i<n; ++i ) {
				unsigned int j = n + k - 1 - i;
				r[j] = r[j] - qk * b[j-k];
			}
		}
	} while ( k-- );

	fill(r.begin()+n, r.end(), a[0].ring()->zero());
	canonicalize(r);
}

/** Calculates quotient of a/b.
 *  Assertion: a and b not empty.
 *
 *  @param[in]  a  polynomial dividend
 *  @param[in]  b  polynomial divisor
 *  @param[out] q  polynomial quotient
 */
static void div(const umodpoly& a, const umodpoly& b, umodpoly& q)
{
	int k, n;
	n = degree(b);
	k = degree(a) - n;
	q.clear();
	if ( k < 0 ) return;

	umodpoly r = a;
	q.resize(k+1, a[0].ring()->zero());
	do {
		cl_MI qk = div(r[n+k], b[n]);
		if ( !zerop(qk) ) {
			q[k] = qk;
			for ( int i=0; i<n; ++i ) {
				unsigned int j = n + k - 1 - i;
				r[j] = r[j] - qk * b[j-k];
			}
		}
	} while ( k-- );

	canonicalize(q);
}

/** Calculates quotient and remainder of a/b.
 *  Assertion: a and b not empty.
 *
 *  @param[in]  a  polynomial dividend
 *  @param[in]  b  polynomial divisor
 *  @param[out] r  polynomial remainder
 *  @param[out] q  polynomial quotient
 */
static void remdiv(const umodpoly& a, const umodpoly& b, umodpoly& r, umodpoly& q)
{
	int k, n;
	n = degree(b);
	k = degree(a) - n;
	q.clear();
	r = a;
	if ( k < 0 ) return;

	q.resize(k+1, a[0].ring()->zero());
	do {
		cl_MI qk = div(r[n+k], b[n]);
		if ( !zerop(qk) ) {
			q[k] = qk;
			for ( int i=0; i<n; ++i ) {
				unsigned int j = n + k - 1 - i;
				r[j] = r[j] - qk * b[j-k];
			}
		}
	} while ( k-- );

	fill(r.begin()+n, r.end(), a[0].ring()->zero());
	canonicalize(r);
	canonicalize(q);
}

/** Calculates the GCD of polynomial a and b.
 *
 *  @param[in]  a  polynomial
 *  @param[in]  b  polynomial
 *  @param[out] c  GCD
 */
static void gcd(const umodpoly& a, const umodpoly& b, umodpoly& c)
{
	if ( degree(a) < degree(b) ) return gcd(b, a, c);

	c = a;
	normalize_in_field(c);
	umodpoly d = b;
	normalize_in_field(d);
	umodpoly r;
	while ( !d.empty() ) {
		rem(c, d, r);
		c = d;
		d = r;
	}
	normalize_in_field(c);
}

/** Calculates the derivative of the polynomial a.
 *  
 *  @param[in]  a  polynomial of which to take the derivative
 *  @param[out] d  result/derivative
 */
static void deriv(const umodpoly& a, umodpoly& d)
{
	d.clear();
	if ( a.size() <= 1 ) return;

	d.insert(d.begin(), a.begin()+1, a.end());
	int max = d.size();
	for ( int i=1; i<max; ++i ) {
		d[i] = d[i] * (i+1);
	}
	canonicalize(d);
}

static bool unequal_one(const umodpoly& a)
{
	if ( a.empty() ) return true;
	return ( a.size() != 1 || a[0] != a[0].ring()->one() );
}

static bool equal_one(const umodpoly& a)
{
	return ( a.size() == 1 && a[0] == a[0].ring()->one() );
}

/** Returns true if polynomial a is square free.
 *
 *  @param[in] a  polynomial to check
 *  @return       true if polynomial is square free, false otherwise
 */
static bool squarefree(const umodpoly& a)
{
	umodpoly b;
	deriv(a, b);
	if ( b.empty() ) {
		return true;
	}
	umodpoly c;
	gcd(a, b, c);
	return equal_one(c);
}

// END modular univariate polynomial code
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// modular matrix

class modular_matrix
{
	friend ostream& operator<<(ostream& o, const modular_matrix& m);
public:
	modular_matrix(size_t r_, size_t c_, const cl_MI& init) : r(r_), c(c_)
	{
		m.resize(c*r, init);
	}
	size_t rowsize() const { return r; }
	size_t colsize() const { return c; }
	cl_MI& operator()(size_t row, size_t col) { return m[row*c + col]; }
	cl_MI operator()(size_t row, size_t col) const { return m[row*c + col]; }
	void mul_col(size_t col, const cl_MI x)
	{
		mvec::iterator i = m.begin() + col;
		for ( size_t rc=0; rc<r; ++rc ) {
			*i = *i * x;
			i += c;
		}
	}
	void sub_col(size_t col1, size_t col2, const cl_MI fac)
	{
		mvec::iterator i1 = m.begin() + col1;
		mvec::iterator i2 = m.begin() + col2;
		for ( size_t rc=0; rc<r; ++rc ) {
			*i1 = *i1 - *i2 * fac;
			i1 += c;
			i2 += c;
		}
	}
	void switch_col(size_t col1, size_t col2)
	{
		cl_MI buf;
		mvec::iterator i1 = m.begin() + col1;
		mvec::iterator i2 = m.begin() + col2;
		for ( size_t rc=0; rc<r; ++rc ) {
			buf = *i1; *i1 = *i2; *i2 = buf;
			i1 += c;
			i2 += c;
		}
	}
	void mul_row(size_t row, const cl_MI x)
	{
		vector<cl_MI>::iterator i = m.begin() + row*c;
		for ( size_t cc=0; cc<c; ++cc ) {
			*i = *i * x;
			++i;
		}
	}
	void sub_row(size_t row1, size_t row2, const cl_MI fac)
	{
		vector<cl_MI>::iterator i1 = m.begin() + row1*c;
		vector<cl_MI>::iterator i2 = m.begin() + row2*c;
		for ( size_t cc=0; cc<c; ++cc ) {
			*i1 = *i1 - *i2 * fac;
			++i1;
			++i2;
		}
	}
	void switch_row(size_t row1, size_t row2)
	{
		cl_MI buf;
		vector<cl_MI>::iterator i1 = m.begin() + row1*c;
		vector<cl_MI>::iterator i2 = m.begin() + row2*c;
		for ( size_t cc=0; cc<c; ++cc ) {
			buf = *i1; *i1 = *i2; *i2 = buf;
			++i1;
			++i2;
		}
	}
	bool is_col_zero(size_t col) const
	{
		mvec::const_iterator i = m.begin() + col;
		for ( size_t rr=0; rr<r; ++rr ) {
			if ( !zerop(*i) ) {
				return false;
			}
			i += c;
		}
		return true;
	}
	bool is_row_zero(size_t row) const
	{
		mvec::const_iterator i = m.begin() + row*c;
		for ( size_t cc=0; cc<c; ++cc ) {
			if ( !zerop(*i) ) {
				return false;
			}
			++i;
		}
		return true;
	}
	void set_row(size_t row, const vector<cl_MI>& newrow)
	{
		mvec::iterator i1 = m.begin() + row*c;
		mvec::const_iterator i2 = newrow.begin(), end = newrow.end();
		for ( ; i2 != end; ++i1, ++i2 ) {
			*i1 = *i2;
		}
	}
	mvec::const_iterator row_begin(size_t row) const { return m.begin()+row*c; }
	mvec::const_iterator row_end(size_t row) const { return m.begin()+row*c+r; }
private:
	size_t r, c;
	mvec m;
};

#ifdef DEBUGFACTOR
modular_matrix operator*(const modular_matrix& m1, const modular_matrix& m2)
{
	const unsigned int r = m1.rowsize();
	const unsigned int c = m2.colsize();
	modular_matrix o(r,c,m1(0,0));

	for ( size_t i=0; i<r; ++i ) {
		for ( size_t j=0; j<c; ++j ) {
			cl_MI buf;
			buf = m1(i,0) * m2(0,j);
			for ( size_t k=1; k<c; ++k ) {
				buf = buf + m1(i,k)*m2(k,j);
			}
			o(i,j) = buf;
		}
	}
	return o;
}

ostream& operator<<(ostream& o, const modular_matrix& m)
{
	vector<cl_MI>::const_iterator i = m.m.begin(), end = m.m.end();
	size_t wrap = 1;
	for ( ; i != end; ++i ) {
		o << *i << " ";
		if ( !(wrap++ % m.c) ) {
			o << endl;
		}
	}
	o << endl;
	return o;
}
#endif // def DEBUGFACTOR

// END modular matrix
////////////////////////////////////////////////////////////////////////////////

static void q_matrix(const umodpoly& a, modular_matrix& Q)
{
	int n = degree(a);
	unsigned int q = cl_I_to_uint(a[0].ring()->modulus);
// fast and buggy
//	vector<cl_MI> r(n, a.R->zero());
//	r[0] = a.R->one();
//	Q.set_row(0, r);
//	unsigned int max = (n-1) * q;
//	for ( size_t m=1; m<=max; ++m ) {
//		cl_MI rn_1 = r.back();
//		for ( size_t i=n-1; i>0; --i ) {
//			r[i] = r[i-1] - rn_1 * a[i];
//		}
//		r[0] = -rn_1 * a[0];
//		if ( (m % q) == 0 ) {
//			Q.set_row(m/q, r);
//		}
//	}
// slow and (hopefully) correct
	cl_MI one = a[0].ring()->one();
	cl_MI zero = a[0].ring()->zero();
	for ( int i=0; i<n; ++i ) {
		umodpoly qk(i*q+1, zero);
		qk[i*q] = one;
		umodpoly r;
		rem(qk, a, r);
		mvec rvec(n, zero);
		for ( int j=0; j<=degree(r); ++j ) {
			rvec[j] = r[j];
		}
		Q.set_row(i, rvec);
	}
}

static void nullspace(modular_matrix& M, vector<mvec>& basis)
{
	const size_t n = M.rowsize();
	const cl_MI one = M(0,0).ring()->one();
	for ( size_t i=0; i<n; ++i ) {
		M(i,i) = M(i,i) - one;
	}
	for ( size_t r=0; r<n; ++r ) {
		size_t cc = 0;
		for ( ; cc<n; ++cc ) {
			if ( !zerop(M(r,cc)) ) {
				if ( cc < r ) {
					if ( !zerop(M(cc,cc)) ) {
						continue;
					}
					M.switch_col(cc, r);
				}
				else if ( cc > r ) {
					M.switch_col(cc, r);
				}
				break;
			}
		}
		if ( cc < n ) {
			M.mul_col(r, recip(M(r,r)));
			for ( cc=0; cc<n; ++cc ) {
				if ( cc != r ) {
					M.sub_col(cc, r, M(r,cc));
				}
			}
		}
	}

	for ( size_t i=0; i<n; ++i ) {
		M(i,i) = M(i,i) - one;
	}
	for ( size_t i=0; i<n; ++i ) {
		if ( !M.is_row_zero(i) ) {
			mvec nu(M.row_begin(i), M.row_end(i));
			basis.push_back(nu);
		}
	}
}

static void berlekamp(const umodpoly& a, upvec& upv)
{
	cl_modint_ring R = a[0].ring();
	umodpoly one(1, R->one());

	modular_matrix Q(degree(a), degree(a), R->zero());
	q_matrix(a, Q);
	vector<mvec> nu;
	nullspace(Q, nu);
	const unsigned int k = nu.size();
	if ( k == 1 ) {
		return;
	}

	list<umodpoly> factors;
	factors.push_back(a);
	unsigned int size = 1;
	unsigned int r = 1;
	unsigned int q = cl_I_to_uint(R->modulus);

	list<umodpoly>::iterator u = factors.begin();

	while ( true ) {
		for ( unsigned int s=0; s<q; ++s ) {
			umodpoly nur = nu[r];
			nur[0] = nur[0] - cl_MI(R, s);
			canonicalize(nur);
			umodpoly g;
			gcd(nur, *u, g);
			if ( unequal_one(g) && g != *u ) {
				umodpoly uo;
				div(*u, g, uo);
				if ( equal_one(uo) ) {
					throw logic_error("berlekamp: unexpected divisor.");
				}
				else {
					*u = uo;
				}
				factors.push_back(g);
				size = 0;
				list<umodpoly>::const_iterator i = factors.begin(), end = factors.end();
				while ( i != end ) {
					if ( degree(*i) ) ++size; 
					++i;
				}
				if ( size == k ) {
					list<umodpoly>::const_iterator i = factors.begin(), end = factors.end();
					while ( i != end ) {
						upv.push_back(*i++);
					}
					return;
				}
			}
		}
		if ( ++r == k ) {
			r = 1;
			++u;
		}
	}
}

static void rem_xq(int q, const umodpoly& b, umodpoly& c)
{
	cl_modint_ring R = b[0].ring();

	int n = degree(b);
	if ( n > q ) {
		c.resize(q+1, R->zero());
		c[q] = R->one();
		return;
	}

	c.clear();
	c.resize(n+1, R->zero());
	int k = q-n;
	c[n] = R->one();

	int ofs = 0;
	do {
		cl_MI qk = div(c[n-ofs], b[n]);
		if ( !zerop(qk) ) {
			for ( int i=1; i<=n; ++i ) {
				c[n-i+ofs] = c[n-i] - qk * b[n-i];
			}
			ofs = ofs ? 0 : 1;
		}
	} while ( k-- );

	if ( ofs ) {
		c.pop_back();
	}
	else {
		c.erase(c.begin());
	}
	canonicalize(c);
}

static void distinct_degree_factor(const umodpoly& a_, upvec& result)
{
	umodpoly a = a_;

	cl_modint_ring R = a[0].ring();
	int q = cl_I_to_int(R->modulus);
	int n = degree(a);
	size_t nhalf = n/2;

	size_t i = 1;
	umodpoly w(1, R->one());
	umodpoly x = w;

	upvec ai;

	while ( i <= nhalf ) {
		expt_pos(w, q, w);
		rem(w, a, w);

		umodpoly buf;
		gcd(a, w-x, buf);
		ai.push_back(buf);

		if ( unequal_one(ai.back()) ) {
			div(a, ai.back(), a);
			rem(w, a, w);
		}

		++i;
	}

	result = ai;
}

static void same_degree_factor(const umodpoly& a, upvec& result)
{
	cl_modint_ring R = a[0].ring();
	int deg = degree(a);

	upvec buf;
	distinct_degree_factor(a, buf);
	int degsum = 0;

	for ( size_t i=0; i<buf.size(); ++i ) {
		if ( unequal_one(buf[i]) ) {
			degsum += degree(buf[i]);
			upvec upv;
			berlekamp(buf[i], upv);
			for ( size_t j=0; j<upv.size(); ++j ) {
				result.push_back(upv[j]);
			}
		}
	}

	if ( degsum < deg ) {
		result.push_back(a);
	}
}

static void distinct_degree_factor_BSGS(const umodpoly& a, upvec& result)
{
	cl_modint_ring R = a[0].ring();
	int q = cl_I_to_int(R->modulus);
	int n = degree(a);

	cl_N pm = 0.3;
	int l = cl_I_to_int(ceiling1(the<cl_F>(expt(n, pm))));
	upvec h(l+1);
	umodpoly qk(1, R->one());
	h[0] = qk;
	for ( int i=1; i<=l; ++i ) {
		expt_pos(h[i-1], q, qk);
		rem(qk, a, h[i]);
	}

	int m = std::ceil(((double)n)/2/l);
	upvec H(m);
	int ql = std::pow(q, l);
	H[0] = h[l];
	for ( int i=1; i<m; ++i ) {
		expt_pos(H[i-1], ql, qk);
		rem(qk, a, H[i]);
	}

	upvec I(m);
	umodpoly one(1, R->one());
	for ( int i=0; i<m; ++i ) {
		I[i] = one;
		for ( int j=0; j<l; ++j ) {
			I[i] = I[i] * (H[i] - h[j]);
		}
		rem(I[i], a, I[i]);
	}

	upvec F(m, one);
	umodpoly f = a;
	for ( int i=0; i<m; ++i ) {
		umodpoly g;
		gcd(f, I[i], g); 
		if ( g == one ) continue;
		F[i] = g;
		div(f, g, f);
	}

	result.resize(n, one);
	if ( unequal_one(f) ) {
		result[n] = f;
	}
	for ( int i=0; i<m; ++i ) {
		umodpoly f = F[i];
		for ( int j=l-1; j>=0; --j ) {
			umodpoly g;
			gcd(f, H[i]-h[j], g);
			result[l*(i+1)-j-1] = g;
			div(f, g, f);
		}
	}
}

static void cantor_zassenhaus(const umodpoly& a, upvec& result)
{
}

static void factor_modular(const umodpoly& p, upvec& upv)
{
	//same_degree_factor(p, upv);
	berlekamp(p, upv);
	return;
}

static void exteuclid(const umodpoly& a, const umodpoly& b, umodpoly& g, umodpoly& s, umodpoly& t)
{
	if ( degree(a) < degree(b) ) {
		exteuclid(b, a, g, t, s);
		return;
	}
	umodpoly one(1, a[0].ring()->one());
	umodpoly c = a; normalize_in_field(c);
	umodpoly d = b; normalize_in_field(d);
	umodpoly c1 = one;
	umodpoly c2;
	umodpoly d1;
	umodpoly d2 = one;
	while ( !d.empty() ) {
		umodpoly q;
		div(c, d, q);
		umodpoly r = c - q * d;
		umodpoly r1 = c1 - q * d1;
		umodpoly r2 = c2 - q * d2;
		c = d;
		c1 = d1;
		c2 = d2;
		d = r;
		d1 = r1;
		d2 = r2;
	}
	g = c; normalize_in_field(g);
	s = c1;
	for ( int i=0; i<=degree(s); ++i ) {
		s[i] = s[i] * recip(a[degree(a)] * c[degree(c)]);
	}
	canonicalize(s);
	s = s * g;
	t = c2;
	for ( int i=0; i<=degree(t); ++i ) {
		t[i] = t[i] * recip(b[degree(b)] * c[degree(c)]);
	}
	canonicalize(t);
	t = t * g;
}

static ex replace_lc(const ex& poly, const ex& x, const ex& lc)
{
	ex r = expand(poly + (lc - poly.lcoeff(x)) * pow(x, poly.degree(x)));
	return r;
}

static ex hensel_univar(const ex& a_, const ex& x, unsigned int p, const umodpoly& u1_, const umodpoly& w1_, const ex& gamma_ = 0)
{
	ex a = a_;
	const cl_modint_ring& R = u1_[0].ring();

	// calc bound B
	ex maxcoeff;
	for ( int i=a.degree(x); i>=a.ldegree(x); --i ) {
		maxcoeff += pow(abs(a.coeff(x, i)),2);
	}
	cl_I normmc = ceiling1(the<cl_R>(cln::sqrt(ex_to<numeric>(maxcoeff).to_cl_N())));
	cl_I maxdegree = (degree(u1_) > degree(w1_)) ? degree(u1_) : degree(w1_);
	cl_I B = normmc * expt_pos(cl_I(2), maxdegree);

	// step 1
	ex alpha = a.lcoeff(x);
	ex gamma = gamma_;
	if ( gamma == 0 ) {
		gamma = alpha;
	}
	numeric gamma_ui = ex_to<numeric>(abs(gamma));
	a = a * gamma;
	umodpoly nu1 = u1_;
	normalize_in_field(nu1);
	umodpoly nw1 = w1_;
	normalize_in_field(nw1);
	ex phi;
	phi = gamma * umod_to_ex(nu1, x);
	umodpoly u1;
	umodpoly_from_ex(u1, phi, x, R);
	phi = alpha * umod_to_ex(nw1, x);
	umodpoly w1;
	umodpoly_from_ex(w1, phi, x, R);

	// step 2
	umodpoly g;
	umodpoly s;
	umodpoly t;
	exteuclid(u1, w1, g, s, t);
	if ( unequal_one(g) ) {
		throw logic_error("gcd(u1,w1) != 1");
	}

	// step 3
	ex u = replace_lc(umod_to_ex(u1, x), x, gamma);
	ex w = replace_lc(umod_to_ex(w1, x), x, alpha);
	ex e = expand(a - u * w);
	numeric modulus = p;
	const numeric maxmodulus = 2*numeric(B)*gamma_ui;

	// step 4
	while ( !e.is_zero() && modulus < maxmodulus ) {
		ex c = e / modulus;
		phi = expand(umod_to_ex(s, x) * c);
		umodpoly sigmatilde;
		umodpoly_from_ex(sigmatilde, phi, x, R);
		phi = expand(umod_to_ex(t, x) * c);
		umodpoly tautilde;
		umodpoly_from_ex(tautilde, phi, x, R);
		umodpoly r, q;
		remdiv(sigmatilde, w1, r, q);
		umodpoly sigma = r;
		phi = expand(umod_to_ex(tautilde, x) + umod_to_ex(q, x) * umod_to_ex(u1, x));
		umodpoly tau;
		umodpoly_from_ex(tau, phi, x, R);
		u = expand(u + umod_to_ex(tau, x) * modulus);
		w = expand(w + umod_to_ex(sigma, x) * modulus);
		e = expand(a - u * w);
		modulus = modulus * p;
	}

	// step 5
	if ( e.is_zero() ) {
		ex delta = u.content(x);
		u = u / delta;
		w = w / gamma * delta;
		return lst(u, w);
	}
	else {
		return lst();
	}
}

static unsigned int next_prime(unsigned int p)
{
	static vector<unsigned int> primes;
	if ( primes.size() == 0 ) {
		primes.push_back(3); primes.push_back(5); primes.push_back(7);
	}
	vector<unsigned int>::const_iterator it = primes.begin();
	if ( p >= primes.back() ) {
		unsigned int candidate = primes.back() + 2;
		while ( true ) {
			size_t n = primes.size()/2;
			for ( size_t i=0; i<n; ++i ) {
				if ( candidate % primes[i] ) continue;
				candidate += 2;
				i=-1;
			}
			primes.push_back(candidate);
			if ( candidate > p ) break;
		}
		return candidate;
	}
	vector<unsigned int>::const_iterator end = primes.end();
	for ( ; it!=end; ++it ) {
		if ( *it > p ) {
			return *it;
		}
	}
	throw logic_error("next_prime: should not reach this point!");
}

class Partition
{
public:
	Partition(size_t n_) : n(n_)
	{
		k.resize(n, 1);
		k[0] = 0;
		sum = n-1;
	}
	int operator[](size_t i) const { return k[i]; }
	size_t size() const { return n; }
	size_t size_first() const { return n-sum; }
	size_t size_second() const { return sum; }
#ifdef DEBUGFACTOR
	void get() const
	{
		for ( size_t i=0; i<k.size(); ++i ) {
			cout << k[i] << " ";
		}
		cout << endl;
	}
#endif
	bool next()
	{
		for ( size_t i=n-1; i>=1; --i ) {
			if ( k[i] ) {
				--k[i];
				--sum;
				return sum > 0;
			}
			++k[i];
			++sum;
		}
		return false;
	}
private:
	size_t n, sum;
	vector<int> k;
};

static void split(const upvec& factors, const Partition& part, umodpoly& a, umodpoly& b)
{
	umodpoly one(1, factors.front()[0].ring()->one());
	a = one;
	b = one;
	for ( size_t i=0; i<part.size(); ++i ) {
		if ( part[i] ) {
			b = b * factors[i];
		}
		else {
			a = a * factors[i];
		}
	}
}

struct ModFactors
{
	ex poly;
	upvec factors;
};

static ex factor_univariate(const ex& poly, const ex& x)
{
	ex unit, cont, prim;
	poly.unitcontprim(x, unit, cont, prim);

	// determine proper prime and minimize number of modular factors
	unsigned int p = 3, lastp = 3;
	cl_modint_ring R;
	unsigned int trials = 0;
	unsigned int minfactors = 0;
	numeric lcoeff = ex_to<numeric>(prim.lcoeff(x));
	upvec factors;
	while ( trials < 2 ) {
		while ( true ) {
			p = next_prime(p);
			if ( irem(lcoeff, p) != 0 ) {
				R = find_modint_ring(p);
				umodpoly modpoly;
				umodpoly_from_ex(modpoly, prim, x, R);
				if ( squarefree(modpoly) ) break;
			}
		}

		// do modular factorization
		umodpoly modpoly;
		umodpoly_from_ex(modpoly, prim, x, R);
		upvec trialfactors;
		factor_modular(modpoly, trialfactors);
		if ( trialfactors.size() <= 1 ) {
			// irreducible for sure
			return poly;
		}

		if ( minfactors == 0 || trialfactors.size() < minfactors ) {
			factors = trialfactors;
			minfactors = factors.size();
			lastp = p;
			trials = 1;
		}
		else {
			++trials;
		}
	}
	p = lastp;
	R = find_modint_ring(p);
	cl_univpoly_modint_ring UPR = find_univpoly_ring(R);

	// lift all factor combinations
	stack<ModFactors> tocheck;
	ModFactors mf;
	mf.poly = prim;
	mf.factors = factors;
	tocheck.push(mf);
	ex result = 1;
	while ( tocheck.size() ) {
		const size_t n = tocheck.top().factors.size();
		Partition part(n);
		while ( true ) {
			umodpoly a, b;
			split(tocheck.top().factors, part, a, b);

			ex answer = hensel_univar(tocheck.top().poly, x, p, a, b);
			if ( answer != lst() ) {
				if ( part.size_first() == 1 ) {
					if ( part.size_second() == 1 ) {
						result *= answer.op(0) * answer.op(1);
						tocheck.pop();
						break;
					}
					result *= answer.op(0);
					tocheck.top().poly = answer.op(1);
					for ( size_t i=0; i<n; ++i ) {
						if ( part[i] == 0 ) {
							tocheck.top().factors.erase(tocheck.top().factors.begin()+i);
							break;
						}
					}
					break;
				}
				else if ( part.size_second() == 1 ) {
					if ( part.size_first() == 1 ) {
						result *= answer.op(0) * answer.op(1);
						tocheck.pop();
						break;
					}
					result *= answer.op(1);
					tocheck.top().poly = answer.op(0);
					for ( size_t i=0; i<n; ++i ) {
						if ( part[i] == 1 ) {
							tocheck.top().factors.erase(tocheck.top().factors.begin()+i);
							break;
						}
					}
					break;
				}
				else {
					upvec newfactors1(part.size_first()), newfactors2(part.size_second());
					upvec::iterator i1 = newfactors1.begin(), i2 = newfactors2.begin();
					for ( size_t i=0; i<n; ++i ) {
						if ( part[i] ) {
							*i2++ = tocheck.top().factors[i];
						}
						else {
							*i1++ = tocheck.top().factors[i];
						}
					}
					tocheck.top().factors = newfactors1;
					tocheck.top().poly = answer.op(0);
					ModFactors mf;
					mf.factors = newfactors2;
					mf.poly = answer.op(1);
					tocheck.push(mf);
					break;
				}
			}
			else {
				if ( !part.next() ) {
					result *= tocheck.top().poly;
					tocheck.pop();
					break;
				}
			}
		}
	}

	return unit * cont * result;
}

struct EvalPoint
{
	ex x;
	int evalpoint;
};

// forward declaration
vector<ex> multivar_diophant(const vector<ex>& a_, const ex& x, const ex& c, const vector<EvalPoint>& I, unsigned int d, unsigned int p, unsigned int k);

upvec multiterm_eea_lift(const upvec& a, const ex& x, unsigned int p, unsigned int k)
{
	const size_t r = a.size();
	cl_modint_ring R = find_modint_ring(expt_pos(cl_I(p),k));
	upvec q(r-1);
	q[r-2] = a[r-1];
	for ( size_t j=r-2; j>=1; --j ) {
		q[j-1] = a[j] * q[j];
	}
	umodpoly beta(1, R->one());
	upvec s;
	for ( size_t j=1; j<r; ++j ) {
		vector<ex> mdarg(2);
		mdarg[0] = umod_to_ex(q[j-1], x);
		mdarg[1] = umod_to_ex(a[j-1], x);
		vector<EvalPoint> empty;
		vector<ex> exsigma = multivar_diophant(mdarg, x, umod_to_ex(beta, x), empty, 0, p, k);
		umodpoly sigma1;
		umodpoly_from_ex(sigma1, exsigma[0], x, R);
		umodpoly sigma2;
		umodpoly_from_ex(sigma2, exsigma[1], x, R);
		beta = sigma1;
		s.push_back(sigma2);
	}
	s.push_back(beta);
	return s;
}

/**
 *  Assert: a not empty.
 */
void change_modulus(const cl_modint_ring& R, umodpoly& a)
{
	if ( a.empty() ) return;
	cl_modint_ring oldR = a[0].ring();
	umodpoly::iterator i = a.begin(), end = a.end();
	for ( ; i!=end; ++i ) {
		*i = R->canonhom(oldR->retract(*i));
	}
	canonicalize(a);
}

void eea_lift(const umodpoly& a, const umodpoly& b, const ex& x, unsigned int p, unsigned int k, umodpoly& s_, umodpoly& t_)
{
	cl_modint_ring R = find_modint_ring(p);
	umodpoly amod = a;
	change_modulus(R, amod);
	umodpoly bmod = b;
	change_modulus(R, bmod);

	umodpoly g;
	umodpoly smod;
	umodpoly tmod;
	exteuclid(amod, bmod, g, smod, tmod);
	if ( unequal_one(g) ) {
		throw logic_error("gcd(amod,bmod) != 1");
	}

	cl_modint_ring Rpk = find_modint_ring(expt_pos(cl_I(p),k));
	umodpoly s = smod;
	change_modulus(Rpk, s);
	umodpoly t = tmod;
	change_modulus(Rpk, t);

	cl_I modulus(p);
	umodpoly one(1, Rpk->one());
	for ( size_t j=1; j<k; ++j ) {
		umodpoly e = one - a * s - b * t;
		reduce_coeff(e, modulus);
		umodpoly c = e;
		change_modulus(R, c);
		umodpoly sigmabar = smod * c;
		umodpoly taubar = tmod * c;
		umodpoly sigma, q;
		remdiv(sigmabar, bmod, sigma, q);
		umodpoly tau = taubar + q * amod;
		umodpoly sadd = sigma;
		change_modulus(Rpk, sadd);
		cl_MI modmodulus(Rpk, modulus);
		s = s + sadd * modmodulus;
		umodpoly tadd = tau;
		change_modulus(Rpk, tadd);
		t = t + tadd * modmodulus;
		modulus = modulus * p;
	}

	s_ = s; t_ = t;
}

upvec univar_diophant(const upvec& a, const ex& x, unsigned int m, unsigned int p, unsigned int k)
{
	cl_modint_ring R = find_modint_ring(expt_pos(cl_I(p),k));

	const size_t r = a.size();
	upvec result;
	if ( r > 2 ) {
		upvec s = multiterm_eea_lift(a, x, p, k);
		for ( size_t j=0; j<r; ++j ) {
			ex phi = expand(pow(x,m) * umod_to_ex(s[j], x));
			umodpoly bmod;
			umodpoly_from_ex(bmod, phi, x, R);
			umodpoly buf;
			rem(bmod, a[j], buf);
			result.push_back(buf);
		}
	}
	else {
		umodpoly s;
		umodpoly t;
		eea_lift(a[1], a[0], x, p, k, s, t);
		ex phi = expand(pow(x,m) * umod_to_ex(s, x));
		umodpoly bmod;
		umodpoly_from_ex(bmod, phi, x, R);
		umodpoly buf, q;
		remdiv(bmod, a[0], buf, q);
		result.push_back(buf);
		phi = expand(pow(x,m) * umod_to_ex(t, x));
		umodpoly t1mod;
		umodpoly_from_ex(t1mod, phi, x, R);
		umodpoly buf2 = t1mod + q * a[1];
		result.push_back(buf2);
	}

	return result;
}

struct make_modular_map : public map_function {
	cl_modint_ring R;
	make_modular_map(const cl_modint_ring& R_) : R(R_) { }
	ex operator()(const ex& e)
	{
		if ( is_a<add>(e) || is_a<mul>(e) ) {
			return e.map(*this);
		}
		else if ( is_a<numeric>(e) ) {
			numeric mod(R->modulus);
			numeric halfmod = (mod-1)/2;
			cl_MI emod = R->canonhom(the<cl_I>(ex_to<numeric>(e).to_cl_N()));
			numeric n(R->retract(emod));
			if ( n > halfmod ) {
				return n-mod;
			}
			else {
				return n;
			}
		}
		return e;
	}
};

static ex make_modular(const ex& e, const cl_modint_ring& R)
{
	make_modular_map map(R);
	return map(e.expand());
}

vector<ex> multivar_diophant(const vector<ex>& a_, const ex& x, const ex& c, const vector<EvalPoint>& I, unsigned int d, unsigned int p, unsigned int k)
{
	vector<ex> a = a_;

	const cl_modint_ring R = find_modint_ring(expt_pos(cl_I(p),k));
	const size_t r = a.size();
	const size_t nu = I.size() + 1;

	vector<ex> sigma;
	if ( nu > 1 ) {
		ex xnu = I.back().x;
		int alphanu = I.back().evalpoint;

		ex A = 1;
		for ( size_t i=0; i<r; ++i ) {
			A *= a[i];
		}
		vector<ex> b(r);
		for ( size_t i=0; i<r; ++i ) {
			b[i] = normal(A / a[i]);
		}

		vector<ex> anew = a;
		for ( size_t i=0; i<r; ++i ) {
			anew[i] = anew[i].subs(xnu == alphanu);
		}
		ex cnew = c.subs(xnu == alphanu);
		vector<EvalPoint> Inew = I;
		Inew.pop_back();
		sigma = multivar_diophant(anew, x, cnew, Inew, d, p, k);

		ex buf = c;
		for ( size_t i=0; i<r; ++i ) {
			buf -= sigma[i] * b[i];
		}
		ex e = make_modular(buf, R);

		ex monomial = 1;
		for ( size_t m=1; m<=d; ++m ) {
			while ( !e.is_zero() && e.has(xnu) ) {
				monomial *= (xnu - alphanu);
				monomial = expand(monomial);
				ex cm = e.diff(ex_to<symbol>(xnu), m).subs(xnu==alphanu) / factorial(m);
				cm = make_modular(cm, R);
				if ( !cm.is_zero() ) {
					vector<ex> delta_s = multivar_diophant(anew, x, cm, Inew, d, p, k);
					ex buf = e;
					for ( size_t j=0; j<delta_s.size(); ++j ) {
						delta_s[j] *= monomial;
						sigma[j] += delta_s[j];
						buf -= delta_s[j] * b[j];
					}
					e = make_modular(buf, R);
				}
			}
		}
	}
	else {
		upvec amod;
		for ( size_t i=0; i<a.size(); ++i ) {
			umodpoly up;
			umodpoly_from_ex(up, a[i], x, R);
			amod.push_back(up);
		}

		sigma.insert(sigma.begin(), r, 0);
		size_t nterms;
		ex z;
		if ( is_a<add>(c) ) {
			nterms = c.nops();
			z = c.op(0);
		}
		else {
			nterms = 1;
			z = c;
		}
		for ( size_t i=0; i<nterms; ++i ) {
			int m = z.degree(x);
			cl_I cm = the<cl_I>(ex_to<numeric>(z.lcoeff(x)).to_cl_N());
			upvec delta_s = univar_diophant(amod, x, m, p, k);
			cl_MI modcm;
			cl_I poscm = cm;
			while ( poscm < 0 ) {
				poscm = poscm + expt_pos(cl_I(p),k);
			}
			modcm = cl_MI(R, poscm);
			for ( size_t j=0; j<delta_s.size(); ++j ) {
				delta_s[j] = delta_s[j] * modcm;
				sigma[j] = sigma[j] + umod_to_ex(delta_s[j], x);
			}
			if ( nterms > 1 ) {
				z = c.op(i+1);
			}
		}
	}

	for ( size_t i=0; i<sigma.size(); ++i ) {
		sigma[i] = make_modular(sigma[i], R);
	}

	return sigma;
}

#ifdef DEBUGFACTOR
ostream& operator<<(ostream& o, const vector<EvalPoint>& v)
{
	for ( size_t i=0; i<v.size(); ++i ) {
		o << "(" << v[i].x << "==" << v[i].evalpoint << ") ";
	}
	return o;
}
#endif // def DEBUGFACTOR

ex hensel_multivar(const ex& a, const ex& x, const vector<EvalPoint>& I, unsigned int p, const cl_I& l, const upvec& u, const vector<ex>& lcU)
{
	const size_t nu = I.size() + 1;
	const cl_modint_ring R = find_modint_ring(expt_pos(cl_I(p),l));

	vector<ex> A(nu);
	A[nu-1] = a;

	for ( size_t j=nu; j>=2; --j ) {
		ex x = I[j-2].x;
		int alpha = I[j-2].evalpoint;
		A[j-2] = A[j-1].subs(x==alpha);
		A[j-2] = make_modular(A[j-2], R);
	}

	int maxdeg = a.degree(I.front().x);
	for ( size_t i=1; i<I.size(); ++i ) {
		int maxdeg2 = a.degree(I[i].x);
		if ( maxdeg2 > maxdeg ) maxdeg = maxdeg2;
	}

	const size_t n = u.size();
	vector<ex> U(n);
	for ( size_t i=0; i<n; ++i ) {
		U[i] = umod_to_ex(u[i], x);
	}

	for ( size_t j=2; j<=nu; ++j ) {
		vector<ex> U1 = U;
		ex monomial = 1;
		for ( size_t m=0; m<n; ++m) {
			if ( lcU[m] != 1 ) {
				ex coef = lcU[m];
				for ( size_t i=j-1; i<nu-1; ++i ) {
					coef = coef.subs(I[i].x == I[i].evalpoint);
				}
				coef = make_modular(coef, R);
				int deg = U[m].degree(x);
				U[m] = U[m] - U[m].lcoeff(x) * pow(x,deg) + coef * pow(x,deg);
			}
		}
		ex Uprod = 1;
		for ( size_t i=0; i<n; ++i ) {
			Uprod *= U[i];
		}
		ex e = expand(A[j-1] - Uprod);

		vector<EvalPoint> newI;
		for ( size_t i=1; i<=j-2; ++i ) {
			newI.push_back(I[i-1]);
		}

		ex xj = I[j-2].x;
		int alphaj = I[j-2].evalpoint;
		size_t deg = A[j-1].degree(xj);
		for ( size_t k=1; k<=deg; ++k ) {
			if ( !e.is_zero() ) {
				monomial *= (xj - alphaj);
				monomial = expand(monomial);
				ex dif = e.diff(ex_to<symbol>(xj), k);
				ex c = dif.subs(xj==alphaj) / factorial(k);
				if ( !c.is_zero() ) {
					vector<ex> deltaU = multivar_diophant(U1, x, c, newI, maxdeg, p, cl_I_to_uint(l));
					for ( size_t i=0; i<n; ++i ) {
						deltaU[i] *= monomial;
						U[i] += deltaU[i];
						U[i] = make_modular(U[i], R);
					}
					ex Uprod = 1;
					for ( size_t i=0; i<n; ++i ) {
						Uprod *= U[i];
					}
					e = A[j-1] - Uprod;
					e = make_modular(e, R);
				}
			}
		}
	}

	ex acand = 1;
	for ( size_t i=0; i<U.size(); ++i ) {
		acand *= U[i];
	}
	if ( expand(a-acand).is_zero() ) {
		lst res;
		for ( size_t i=0; i<U.size(); ++i ) {
			res.append(U[i]);
		}
		return res;
	}
	else {
		lst res;
		return lst();
	}
}

static ex put_factors_into_lst(const ex& e)
{
	lst result;

	if ( is_a<numeric>(e) ) {
		result.append(e);
		return result;
	}
	if ( is_a<power>(e) ) {
		result.append(1);
		result.append(e.op(0));
		result.append(e.op(1));
		return result;
	}
	if ( is_a<symbol>(e) || is_a<add>(e) ) {
		result.append(1);
		result.append(e);
		result.append(1);
		return result;
	}
	if ( is_a<mul>(e) ) {
		ex nfac = 1;
		for ( size_t i=0; i<e.nops(); ++i ) {
			ex op = e.op(i);
			if ( is_a<numeric>(op) ) {
				nfac = op;
			}
			if ( is_a<power>(op) ) {
				result.append(op.op(0));
				result.append(op.op(1));
			}
			if ( is_a<symbol>(op) || is_a<add>(op) ) {
				result.append(op);
				result.append(1);
			}
		}
		result.prepend(nfac);
		return result;
	}
	throw runtime_error("put_factors_into_lst: bad term.");
}

#ifdef DEBUGFACTOR
ostream& operator<<(ostream& o, const vector<numeric>& v)
{
	for ( size_t i=0; i<v.size(); ++i ) {
		o << v[i] << " ";
	}
	return o;
}
#endif // def DEBUGFACTOR

static bool checkdivisors(const lst& f, vector<numeric>& d)
{
	const int k = f.nops()-2;
	numeric q, r;
	d[0] = ex_to<numeric>(f.op(0) * f.op(f.nops()-1));
	if ( d[0] == 1 && k == 1 && abs(f.op(1)) != 1 ) {
		return false;
	}
	for ( int i=1; i<=k; ++i ) {
		q = ex_to<numeric>(abs(f.op(i)));
		for ( int j=i-1; j>=0; --j ) {
			r = d[j];
			do {
				r = gcd(r, q);
				q = q/r;
			} while ( r != 1 );
			if ( q == 1 ) {
				return true;
			}
		}
		d[i] = q;
	}
	return false;
}

static bool generate_set(const ex& u, const ex& vn, const exset& syms, const ex& f, const numeric& modulus, vector<numeric>& a, vector<numeric>& d)
{
	// computation of d is actually not necessary
	const ex& x = *syms.begin();
	bool trying = true;
	do {
		ex u0 = u;
		ex vna = vn;
		ex vnatry;
		exset::const_iterator s = syms.begin();
		++s;
		for ( size_t i=0; i<a.size(); ++i ) {
			do {
				a[i] = mod(numeric(rand()), 2*modulus) - modulus;
				vnatry = vna.subs(*s == a[i]);
			} while ( vnatry == 0 );
			vna = vnatry;
			u0 = u0.subs(*s == a[i]);
			++s;
		}
		if ( gcd(u0,u0.diff(ex_to<symbol>(x))) != 1 ) {
			continue;
		}
		if ( is_a<numeric>(vn) ) {
			trying = false;
		}
		else {
			lst fnum;
			lst::const_iterator i = ex_to<lst>(f).begin();
			fnum.append(*i++);
			bool problem = false;
			while ( i!=ex_to<lst>(f).end() ) {
				ex fs = *i;
				if ( !is_a<numeric>(fs) ) {
					s = syms.begin();
					++s;
					for ( size_t j=0; j<a.size(); ++j ) {
						fs = fs.subs(*s == a[j]);
						++s;
					}
					if ( abs(fs) == 1 ) {
						problem = true;
						break;
					}
				}
				fnum.append(fs);
				++i; ++i;
			}
			if ( problem ) {
				return true;
			}
			ex con = u0.content(x);
			fnum.append(con);
			trying = checkdivisors(fnum, d);
		}
	} while ( trying );
	return false;
}

static ex factor_multivariate(const ex& poly, const exset& syms)
{
	exset::const_iterator s;
	const ex& x = *syms.begin();

	/* make polynomial primitive */
	ex p = poly.expand().collect(x);
	ex cont = p.lcoeff(x);
	for ( numeric i=p.degree(x)-1; i>=p.ldegree(x); --i ) {
		cont = gcd(cont, p.coeff(x,ex_to<numeric>(i).to_int()));
		if ( cont == 1 ) break;
	}
	ex pp = expand(normal(p / cont));
	if ( !is_a<numeric>(cont) ) {
		return factor(cont) * factor(pp);
	}

	/* factor leading coefficient */
	pp = pp.collect(x);
	ex vn = pp.lcoeff(x);
	pp = pp.expand();
	ex vnlst;
	if ( is_a<numeric>(vn) ) {
		vnlst = lst(vn);
	}
	else {
		ex vnfactors = factor(vn);
		vnlst = put_factors_into_lst(vnfactors);
	}

	const numeric maxtrials = 3;
	numeric modulus = (vnlst.nops()-1 > 3) ? vnlst.nops()-1 : 3;
	numeric minimalr = -1;
	vector<numeric> a(syms.size()-1, 0);
	vector<numeric> d((vnlst.nops()-1)/2+1, 0);

	while ( true ) {
		numeric trialcount = 0;
		ex u, delta;
		unsigned int prime = 3;
		size_t factor_count = 0;
		ex ufac;
		ex ufaclst;
		while ( trialcount < maxtrials ) {
			bool problem = generate_set(pp, vn, syms, vnlst, modulus, a, d);
			if ( problem ) {
				++modulus;
				continue;
			}
			u = pp;
			s = syms.begin();
			++s;
			for ( size_t i=0; i<a.size(); ++i ) {
				u = u.subs(*s == a[i]);
				++s;
			}
			delta = u.content(x);

			// determine proper prime
			prime = 3;
			cl_modint_ring R = find_modint_ring(prime);
			while ( true ) {
				if ( irem(ex_to<numeric>(u.lcoeff(x)), prime) != 0 ) {
					umodpoly modpoly;
					umodpoly_from_ex(modpoly, u, x, R);
					if ( squarefree(modpoly) ) break;
				}
				prime = next_prime(prime);
				R = find_modint_ring(prime);
			}

			ufac = factor(u);
			ufaclst = put_factors_into_lst(ufac);
			factor_count = (ufaclst.nops()-1)/2;

			// veto factorization for which gcd(u_i, u_j) != 1 for all i,j
			upvec tryu;
			for ( size_t i=0; i<(ufaclst.nops()-1)/2; ++i ) {
				umodpoly newu;
				umodpoly_from_ex(newu, ufaclst.op(i*2+1), x, R);
				tryu.push_back(newu);
			}
			bool veto = false;
			for ( size_t i=0; i<tryu.size()-1; ++i ) {
				for ( size_t j=i+1; j<tryu.size(); ++j ) {
					umodpoly tryg;
					gcd(tryu[i], tryu[j], tryg);
					if ( unequal_one(tryg) ) {
						veto = true;
						goto escape_quickly;
					}
				}
			}
			escape_quickly: ;
			if ( veto ) {
				continue;
			}

			if ( factor_count <= 1 ) {
				return poly;
			}

			if ( minimalr < 0 ) {
				minimalr = factor_count;
			}
			else if ( minimalr == factor_count ) {
				++trialcount;
				++modulus;
			}
			else if ( minimalr > factor_count ) {
				minimalr = factor_count;
				trialcount = 0;
			}
			if ( minimalr <= 1 ) {
				return poly;
			}
		}

		vector<numeric> ftilde((vnlst.nops()-1)/2+1);
		ftilde[0] = ex_to<numeric>(vnlst.op(0));
		for ( size_t i=1; i<ftilde.size(); ++i ) {
			ex ft = vnlst.op((i-1)*2+1);
			s = syms.begin();
			++s;
			for ( size_t j=0; j<a.size(); ++j ) {
				ft = ft.subs(*s == a[j]);
				++s;
			}
			ftilde[i] = ex_to<numeric>(ft);
		}

		vector<bool> used_flag((vnlst.nops()-1)/2+1, false);
		vector<ex> D(factor_count, 1);
		for ( size_t i=0; i<=factor_count; ++i ) {
			numeric prefac;
			if ( i == 0 ) {
				prefac = ex_to<numeric>(ufaclst.op(0));
				ftilde[0] = ftilde[0] / prefac;
				vnlst.let_op(0) = vnlst.op(0) / prefac;
				continue;
			}
			else {
				prefac = ex_to<numeric>(ufaclst.op(2*(i-1)+1).lcoeff(x));
			}
			for ( size_t j=(vnlst.nops()-1)/2+1; j>0; --j ) {
				if ( abs(ftilde[j-1]) == 1 ) {
					used_flag[j-1] = true;
					continue;
				}
				numeric g = gcd(prefac, ftilde[j-1]);
				if ( g != 1 ) {
					prefac = prefac / g;
					numeric count = abs(iquo(g, ftilde[j-1]));
					used_flag[j-1] = true;
					if ( i > 0 ) {
						if ( j == 1 ) {
							D[i-1] = D[i-1] * pow(vnlst.op(0), count);
						}
						else {
							D[i-1] = D[i-1] * pow(vnlst.op(2*(j-2)+1), count);
						}
					}
					else {
						ftilde[j-1] = ftilde[j-1] / prefac;
						break;
					}
					++j;
				}
			}
		}

		bool some_factor_unused = false;
		for ( size_t i=0; i<used_flag.size(); ++i ) {
			if ( !used_flag[i] ) {
				some_factor_unused = true;
				break;
			}
		}
		if ( some_factor_unused ) {
			continue;
		}

		vector<ex> C(factor_count);
		if ( delta == 1 ) {
			for ( size_t i=0; i<D.size(); ++i ) {
				ex Dtilde = D[i];
				s = syms.begin();
				++s;
				for ( size_t j=0; j<a.size(); ++j ) {
					Dtilde = Dtilde.subs(*s == a[j]);
					++s;
				}
				C[i] = D[i] * (ufaclst.op(2*i+1).lcoeff(x) / Dtilde);
			}
		}
		else {
			for ( size_t i=0; i<D.size(); ++i ) {
				ex Dtilde = D[i];
				s = syms.begin();
				++s;
				for ( size_t j=0; j<a.size(); ++j ) {
					Dtilde = Dtilde.subs(*s == a[j]);
					++s;
				}
				ex ui;
				if ( i == 0 ) {
					ui = ufaclst.op(0);
				}
				else {
					ui = ufaclst.op(2*(i-1)+1);
				}
				while ( true ) {
					ex d = gcd(ui.lcoeff(x), Dtilde);
					C[i] = D[i] * ( ui.lcoeff(x) / d );
					ui = ui * ( Dtilde[i] / d );
					delta = delta / ( Dtilde[i] / d );
					if ( delta == 1 ) break;
					ui = delta * ui;
					C[i] = delta * C[i];
					pp = pp * pow(delta, D.size()-1);
				}
			}
		}

		EvalPoint ep;
		vector<EvalPoint> epv;
		s = syms.begin();
		++s;
		for ( size_t i=0; i<a.size(); ++i ) {
			ep.x = *s++;
			ep.evalpoint = a[i].to_int();
			epv.push_back(ep);
		}

		// calc bound B
		ex maxcoeff;
		for ( int i=u.degree(x); i>=u.ldegree(x); --i ) {
			maxcoeff += pow(abs(u.coeff(x, i)),2);
		}
		cl_I normmc = ceiling1(the<cl_R>(cln::sqrt(ex_to<numeric>(maxcoeff).to_cl_N())));
		unsigned int maxdegree = 0;
		for ( size_t i=0; i<factor_count; ++i ) {
			if ( ufaclst[2*i+1].degree(x) > (int)maxdegree ) {
				maxdegree = ufaclst[2*i+1].degree(x);
			}
		}
		cl_I B = normmc * expt_pos(cl_I(2), maxdegree);
		cl_I l = 1;
		cl_I pl = prime;
		while ( pl < B ) {
			l = l + 1;
			pl = pl * prime;
		}

		upvec uvec;
		cl_modint_ring R = find_modint_ring(expt_pos(cl_I(prime),l));
		for ( size_t i=0; i<(ufaclst.nops()-1)/2; ++i ) {
			umodpoly newu;
			umodpoly_from_ex(newu, ufaclst.op(i*2+1), x, R);
			uvec.push_back(newu);
		}

		ex res = hensel_multivar(ufaclst.op(0)*pp, x, epv, prime, l, uvec, C);
		if ( res != lst() ) {
			ex result = cont * ufaclst.op(0);
			for ( size_t i=0; i<res.nops(); ++i ) {
				result *= res.op(i).content(x) * res.op(i).unit(x);
				result *= res.op(i).primpart(x);
			}
			return result;
		}
	}
}

struct find_symbols_map : public map_function {
	exset syms;
	ex operator()(const ex& e)
	{
		if ( is_a<symbol>(e) ) {
			syms.insert(e);
			return e;
		}
		return e.map(*this);
	}
};

static ex factor_sqrfree(const ex& poly)
{
	// determine all symbols in poly
	find_symbols_map findsymbols;
	findsymbols(poly);
	if ( findsymbols.syms.size() == 0 ) {
		return poly;
	}

	if ( findsymbols.syms.size() == 1 ) {
		// univariate case
		const ex& x = *(findsymbols.syms.begin());
		if ( poly.ldegree(x) > 0 ) {
			int ld = poly.ldegree(x);
			ex res = factor_univariate(expand(poly/pow(x, ld)), x);
			return res * pow(x,ld);
		}
		else {
			ex res = factor_univariate(poly, x);
			return res;
		}
	}

	// multivariate case
	ex res = factor_multivariate(poly, findsymbols.syms);
	return res;
}

struct apply_factor_map : public map_function {
	unsigned options;
	apply_factor_map(unsigned options_) : options(options_) { }
	ex operator()(const ex& e)
	{
		if ( e.info(info_flags::polynomial) ) {
			return factor(e, options);
		}
		if ( is_a<add>(e) ) {
			ex s1, s2;
			for ( size_t i=0; i<e.nops(); ++i ) {
				if ( e.op(i).info(info_flags::polynomial) ) {
					s1 += e.op(i);
				}
				else {
					s2 += e.op(i);
				}
			}
			s1 = s1.eval();
			s2 = s2.eval();
			return factor(s1, options) + s2.map(*this);
		}
		return e.map(*this);
	}
};

} // anonymous namespace

ex factor(const ex& poly, unsigned options)
{
	// check arguments
	if ( !poly.info(info_flags::polynomial) ) {
		if ( options & factor_options::all ) {
			options &= ~factor_options::all;
			apply_factor_map factor_map(options);
			return factor_map(poly);
		}
		return poly;
	}

	// determine all symbols in poly
	find_symbols_map findsymbols;
	findsymbols(poly);
	if ( findsymbols.syms.size() == 0 ) {
		return poly;
	}
	lst syms;
	exset::const_iterator i=findsymbols.syms.begin(), end=findsymbols.syms.end();
	for ( ; i!=end; ++i ) {
		syms.append(*i);
	}

	// make poly square free
	ex sfpoly = sqrfree(poly, syms);

	// factorize the square free components
	if ( is_a<power>(sfpoly) ) {
		// case: (polynomial)^exponent
		const ex& base = sfpoly.op(0);
		if ( !is_a<add>(base) ) {
			// simple case: (monomial)^exponent
			return sfpoly;
		}
		ex f = factor_sqrfree(base);
		return pow(f, sfpoly.op(1));
	}
	if ( is_a<mul>(sfpoly) ) {
		// case: multiple factors
		ex res = 1;
		for ( size_t i=0; i<sfpoly.nops(); ++i ) {
			const ex& t = sfpoly.op(i);
			if ( is_a<power>(t) ) {
				const ex& base = t.op(0);
				if ( !is_a<add>(base) ) {
					res *= t;
				}
				else {
					ex f = factor_sqrfree(base);
					res *= pow(f, t.op(1));
				}
			}
			else if ( is_a<add>(t) ) {
				ex f = factor_sqrfree(t);
				res *= f;
			}
			else {
				res *= t;
			}
		}
		return res;
	}
	if ( is_a<symbol>(sfpoly) ) {
		return poly;
	}
	// case: (polynomial)
	ex f = factor_sqrfree(sfpoly);
	return f;
}

} // namespace GiNaC

#ifdef DEBUGFACTOR
#include "test.h"
#endif
