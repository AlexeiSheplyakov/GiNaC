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

#ifdef DEBUGFACTOR
#include <ostream>
#include <ginac/ginac.h>
using namespace GiNaC;
#else
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
#endif

#include <algorithm>
#include <cmath>
#include <list>
#include <vector>
using namespace std;

#include <cln/cln.h>
using namespace cln;

#ifdef DEBUGFACTOR
namespace Factor {
#else
namespace GiNaC {
#endif

#ifdef DEBUGFACTOR
#define DCOUT(str) cout << #str << endl
#define DCOUTVAR(var) cout << #var << ": " << var << endl
#define DCOUT2(str,var) cout << #str << ": " << var << endl
#else
#define DCOUT(str)
#define DCOUTVAR(var)
#define DCOUT2(str,var)
#endif

// forward declaration
ex factor(const ex& poly, unsigned options);

// anonymous namespace to hide all utility functions
namespace {

typedef vector<cl_MI> mvec;

#ifdef DEBUGFACTOR
ostream& operator<<(ostream& o, const mvec& v)
{
	mvec::const_iterator i = v.begin(), end = v.end();
	while ( i != end ) {
		o << *i++ << " ";
	}
	return o;
}
#endif // def DEBUGFACTOR

#ifdef DEBUGFACTOR
ostream& operator<<(ostream& o, const vector<mvec>& v)
{
	vector<mvec>::const_iterator i = v.begin(), end = v.end();
	while ( i != end ) {
		o << *i++ << endl;
	}
	return o;
}
#endif // def DEBUGFACTOR

////////////////////////////////////////////////////////////////////////////////
// modular univariate polynomial code

typedef cl_UP_MI umod;
typedef vector<umod> umodvec;

#define COPY(to,from) from.ring()->create(degree(from)); \
	for ( int II=0; II<=degree(from); ++II ) to.set_coeff(II, coeff(from, II)); \
 	to.finalize()

#ifdef DEBUGFACTOR
ostream& operator<<(ostream& o, const umodvec& v)
{
	umodvec::const_iterator i = v.begin(), end = v.end();
	while ( i != end ) {
		o << *i++ << " , " << endl;
	}
	return o;
}
#endif // def DEBUGFACTOR

static umod umod_from_ex(const ex& e, const ex& x, const cl_univpoly_modint_ring& UPR)
{
	// assert: e is in Z[x]
	int deg = e.degree(x);
	umod p = UPR->create(deg);
	int ldeg = e.ldegree(x);
	for ( ; deg>=ldeg; --deg ) {
		cl_I coeff = the<cl_I>(ex_to<numeric>(e.coeff(x, deg)).to_cl_N());
		p.set_coeff(deg, UPR->basering()->canonhom(coeff));
	}
	for ( ; deg>=0; --deg ) {
		p.set_coeff(deg, UPR->basering()->zero());
	}
	p.finalize();
	return p;
}

static umod umod_from_ex(const ex& e, const ex& x, const cl_modint_ring& R)
{
	return umod_from_ex(e, x, find_univpoly_ring(R));
}

static umod umod_from_ex(const ex& e, const ex& x, const cl_I& modulus)
{
	return umod_from_ex(e, x, find_modint_ring(modulus));
}

static umod umod_from_modvec(const mvec& mv)
{
	size_t n = mv.size(); // assert: n>0
	while ( n && zerop(mv[n-1]) ) --n;
	cl_univpoly_modint_ring UPR = find_univpoly_ring(mv.front().ring());
	if ( n == 0 ) {
		umod p = UPR->create(-1);
		p.finalize();
		return p;
	}
	umod p = UPR->create(n-1);
	for ( size_t i=0; i<n; ++i ) {
		p.set_coeff(i, mv[i]);
	}
	p.finalize();
	return p;
}

static umod divide(const umod& a, const cl_I& x)
{
	DCOUT(divide);
	DCOUTVAR(a);
	cl_univpoly_modint_ring UPR = a.ring();
	cl_modint_ring R = UPR->basering();
	int deg = degree(a);
	umod newa = UPR->create(deg);
	for ( int i=0; i<=deg; ++i ) {
		cl_I c = R->retract(coeff(a, i));
		newa.set_coeff(i, cl_MI(R, the<cl_I>(c / x)));
	}
	newa.finalize();
	DCOUT(END divide);
	return newa;
}

static ex umod_to_ex(const umod& a, const ex& x)
{
	ex e;
	cl_modint_ring R = a.ring()->basering();
	cl_I mod = R->modulus;
	cl_I halfmod = (mod-1) >> 1;
	for ( int i=degree(a); i>=0; --i ) {
		cl_I n = R->retract(coeff(a, i));
		if ( n > halfmod ) {
			e += numeric(n-mod) * pow(x, i);
		} else {
			e += numeric(n) * pow(x, i);
		}
	}
	return e;
}

static void unit_normal(umod& a)
{
	int deg = degree(a);
	if ( deg >= 0 ) {
		cl_MI lc = coeff(a, deg);
		cl_MI one = a.ring()->basering()->one();
		if ( lc != one ) {
			umod newa = a.ring()->create(deg);
			newa.set_coeff(deg, one);
			for ( --deg; deg>=0; --deg ) {
				cl_MI nc = div(coeff(a, deg), lc);
				newa.set_coeff(deg, nc);
			}
			newa.finalize();
			a = newa;
		}
	}
}

static umod rem(const umod& a, const umod& b)
{
	int k, n;
	n = degree(b);
	k = degree(a) - n;
	if ( k < 0 ) {
		umod c = COPY(c, a);
		return c;
	}

	umod c = COPY(c, a);
	do {
		cl_MI qk = div(coeff(c, n+k), coeff(b, n));
		if ( !zerop(qk) ) {
			unsigned int j;
			for ( int i=0; i<n; ++i ) {
				j = n + k - 1 - i;
				c.set_coeff(j, coeff(c, j) - qk * coeff(b, j-k));
			}
		}
	} while ( k-- );

	cl_MI zero = a.ring()->basering()->zero();
	for ( int i=degree(a); i>=n; --i ) {
		c.set_coeff(i, zero);
	}

	c.finalize();
	return c;
}

static umod div(const umod& a, const umod& b)
{
	int k, n;
	n = degree(b);
	k = degree(a) - n;
	if ( k < 0 ) {
		umod q = a.ring()->create(-1);
		q.finalize();
		return q;
	}

	umod c = COPY(c, a);
	umod q = a.ring()->create(k);
	do {
		cl_MI qk = div(coeff(c, n+k), coeff(b, n));
		if ( !zerop(qk) ) {
			q.set_coeff(k, qk);
			unsigned int j;
			for ( int i=0; i<n; ++i ) {
				j = n + k - 1 - i;
				c.set_coeff(j, coeff(c, j) - qk * coeff(b, j-k));
			}
		}
	} while ( k-- );

	q.finalize();
	return q;
}

static umod remdiv(const umod& a, const umod& b, umod& q)
{
	int k, n;
	n = degree(b);
	k = degree(a) - n;
	if ( k < 0 ) {
		q = a.ring()->create(-1);
		q.finalize();
		umod c = COPY(c, a);
		return c;
	}

	umod c = COPY(c, a);
	q = a.ring()->create(k);
	do {
		cl_MI qk = div(coeff(c, n+k), coeff(b, n));
		if ( !zerop(qk) ) {
			q.set_coeff(k, qk);
			unsigned int j;
			for ( int i=0; i<n; ++i ) {
				j = n + k - 1 - i;
				c.set_coeff(j, coeff(c, j) - qk * coeff(b, j-k));
			}
		}
	} while ( k-- );

	cl_MI zero = a.ring()->basering()->zero();
	for ( int i=degree(a); i>=n; --i ) {
		c.set_coeff(i, zero);
	}

	q.finalize();
	c.finalize();
	return c;
}

static umod gcd(const umod& a, const umod& b)
{
	if ( degree(a) < degree(b) ) return gcd(b, a);

	umod c = COPY(c, a);
	unit_normal(c);
	umod d = COPY(d, b);
	unit_normal(d);
	while ( !zerop(d) ) {
		umod r = rem(c, d);
		c = COPY(c, d);
		d = COPY(d, r);
	}
	unit_normal(c);
	return c;
}

static bool squarefree(const umod& a)
{
	umod b = deriv(a);
	if ( zerop(b) ) {
		return false;
	}
	umod one = a.ring()->one();
	umod c = gcd(a, b);
	return c == one;
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

static void q_matrix(const umod& a, modular_matrix& Q)
{
	int n = degree(a);
	unsigned int q = cl_I_to_uint(a.ring()->basering()->modulus);
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
	cl_MI one = a.ring()->basering()->one();
	for ( int i=0; i<n; ++i ) {
		umod qk = a.ring()->create(i*q);
		qk.set_coeff(i*q, one);
		qk.finalize();
		umod r = rem(qk, a);
		mvec rvec;
		for ( int j=0; j<n; ++j ) {
			rvec.push_back(coeff(r, j));
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

static void berlekamp(const umod& a, umodvec& upv)
{
	cl_modint_ring R = a.ring()->basering();
	const umod one = a.ring()->one();

	modular_matrix Q(degree(a), degree(a), R->zero());
	q_matrix(a, Q);
	vector<mvec> nu;
	nullspace(Q, nu);
	const unsigned int k = nu.size();
	if ( k == 1 ) {
		return;
	}

	list<umod> factors;
	factors.push_back(a);
	unsigned int size = 1;
	unsigned int r = 1;
	unsigned int q = cl_I_to_uint(R->modulus);

	list<umod>::iterator u = factors.begin();

	while ( true ) {
		for ( unsigned int s=0; s<q; ++s ) {
			umod nur = umod_from_modvec(nu[r]);
			cl_MI buf = coeff(nur, 0) - cl_MI(R, s);
			nur.set_coeff(0, buf);
			nur.finalize();
			umod g = gcd(nur, *u);
			if ( g != one && g != *u ) {
				umod uo = div(*u, g);
				if ( uo == one ) {
					throw logic_error("berlekamp: unexpected divisor.");
				}
				else {
					*u = COPY((*u), uo);
				}
				factors.push_back(g);
				size = 0;
				list<umod>::const_iterator i = factors.begin(), end = factors.end();
				while ( i != end ) {
					if ( degree(*i) ) ++size; 
					++i;
				}
				if ( size == k ) {
					list<umod>::const_iterator i = factors.begin(), end = factors.end();
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

static umod rem_xq(int q, const umod& b)
{
	cl_univpoly_modint_ring UPR = b.ring();
	cl_modint_ring R = UPR->basering();

	int n = degree(b);
	if ( n > q ) {
		umod c = UPR->create(q);
		c.set_coeff(q, R->one());
		c.finalize();
		return c;
	}

	mvec c(n+1, R->zero());
	int k = q-n;
	c[n] = R->one();
	DCOUTVAR(k);

	int ofs = 0;
	do {
		cl_MI qk = div(c[n-ofs], coeff(b, n));
		if ( !zerop(qk) ) {
			for ( int i=1; i<=n; ++i ) {
				c[n-i+ofs] = c[n-i] - qk * coeff(b, n-i);
			}
			ofs = ofs ? 0 : 1;
			DCOUTVAR(ofs);
			DCOUTVAR(c);
		}
	} while ( k-- );

	if ( ofs ) {
		c.pop_back();
	}
	else {
		c.erase(c.begin());
	}
	umod res = umod_from_modvec(c);
	return res;
}

static void distinct_degree_factor(const umod& a_, umodvec& result)
{
	umod a = COPY(a, a_);

	DCOUT(distinct_degree_factor);
	DCOUTVAR(a);

	cl_univpoly_modint_ring UPR = a.ring();
	cl_modint_ring R = UPR->basering();
	int q = cl_I_to_int(R->modulus);
	int n = degree(a);
	size_t nhalf = n/2;


	size_t i = 1;
	umod w = UPR->create(1);
	w.set_coeff(1, R->one());
	w.finalize();
	umod x = COPY(x, w);

	umodvec ai;

	while ( i <= nhalf ) {
		w = expt_pos(w, q);
		w = rem(w, a);

		ai.push_back(gcd(a, w-x));

		if ( ai.back() != UPR->one() ) {
			a = div(a, ai.back());
			w = rem(w, a);
		}

		++i;
	}

	result = ai;
	DCOUTVAR(result);
	DCOUT(END distinct_degree_factor);
}

static void same_degree_factor(const umod& a, umodvec& result)
{
	DCOUT(same_degree_factor);

	cl_univpoly_modint_ring UPR = a.ring();
	cl_modint_ring R = UPR->basering();
	int deg = degree(a);

	umodvec buf;
	distinct_degree_factor(a, buf);
	int degsum = 0;

	for ( size_t i=0; i<buf.size(); ++i ) {
		if ( buf[i] != UPR->one() ) {
			degsum += degree(buf[i]);
			umodvec upv;
			berlekamp(buf[i], upv);
			for ( size_t j=0; j<upv.size(); ++j ) {
				result.push_back(upv[j]);
			}
		}
	}

	if ( degsum < deg ) {
		result.push_back(a);
	}

	DCOUTVAR(result);
	DCOUT(END same_degree_factor);
}

static void distinct_degree_factor_BSGS(const umod& a, umodvec& result)
{
	DCOUT(distinct_degree_factor_BSGS);
	DCOUTVAR(a);

	cl_univpoly_modint_ring UPR = a.ring();
	cl_modint_ring R = UPR->basering();
	int q = cl_I_to_int(R->modulus);
	int n = degree(a);

	cl_N pm = 0.3;
	int l = cl_I_to_int(ceiling1(the<cl_F>(expt(n, pm))));
	DCOUTVAR(l);
	umodvec h(l+1, UPR->create(-1));
	umod qk = UPR->create(1);
	qk.set_coeff(1, R->one());
	qk.finalize();
	h[0] = qk;
	DCOUTVAR(h[0]);
	for ( int i=1; i<=l; ++i ) {
		qk = expt_pos(h[i-1], q);
		h[i] = rem(qk, a);
		DCOUTVAR(i);
		DCOUTVAR(h[i]);
	}

	int m = std::ceil(((double)n)/2/l);
	DCOUTVAR(m);
	umodvec H(m, UPR->create(-1));
	int ql = std::pow(q, l);
	H[0] = COPY(H[0], h[l]);
	DCOUTVAR(H[0]);
	for ( int i=1; i<m; ++i ) {
		qk = expt_pos(H[i-1], ql);
		H[i] = rem(qk, a);
		DCOUTVAR(i);
		DCOUTVAR(H[i]);
	}

	umodvec I(m, UPR->create(-1));
	for ( int i=0; i<m; ++i ) {
		I[i] = UPR->one();
		for ( int j=0; j<l; ++j ) {
			I[i] = I[i] * (H[i] - h[j]);
		}
		DCOUTVAR(i);
		DCOUTVAR(I[i]);
		I[i] = rem(I[i], a);
		DCOUTVAR(I[i]);
	}

	umodvec F(m, UPR->one());
	umod f = COPY(f, a);
	for ( int i=0; i<m; ++i ) {
		DCOUTVAR(i);
		umod g = gcd(f, I[i]); 
		if ( g == UPR->one() ) continue;
		F[i] = g;
		f = div(f, g);
		DCOUTVAR(F[i]);
	}

	result.resize(n, UPR->one());
	if ( f != UPR->one() ) {
		result[n] = f;
	}
	for ( int i=0; i<m; ++i ) {
		DCOUTVAR(i);
		umod f = COPY(f, F[i]);
		for ( int j=l-1; j>=0; --j ) {
			umod g = gcd(f, H[i]-h[j]);
			result[l*(i+1)-j-1] = g;
			f = div(f, g);
		}
	}

	DCOUTVAR(result);
	DCOUT(END distinct_degree_factor_BSGS);
}

static void cantor_zassenhaus(const umod& a, umodvec& result)
{
}

static void factor_modular(const umod& p, umodvec& upv)
{
	//same_degree_factor(p, upv);
	berlekamp(p, upv);
	return;
}

static void exteuclid(const umod& a, const umod& b, umod& g, umod& s, umod& t)
{
	if ( degree(a) < degree(b) ) {
		exteuclid(b, a, g, t, s);
		return;
	}
	umod c = COPY(c, a); unit_normal(c);
	umod d = COPY(d, b); unit_normal(d);
	umod c1 = a.ring()->one();
	umod c2 = a.ring()->create(-1);
	umod d1 = a.ring()->create(-1);
	umod d2 = a.ring()->one();
	while ( !zerop(d) ) {
		umod q = div(c, d);
		umod r = c - q * d;
		umod r1 = c1 - q * d1;
		umod r2 = c2 - q * d2;
		c = COPY(c, d);
		c1 = COPY(c1, d1);
		c2 = COPY(c2, d2);
		d = COPY(d, r);
		d1 = COPY(d1, r1);
		d2 = COPY(d2, r2);
	}
	g = COPY(g, c); unit_normal(g);
	s = COPY(s, c1);
	for ( int i=0; i<=degree(s); ++i ) {
		s.set_coeff(i, coeff(s, i) * recip(coeff(a, degree(a)) * coeff(c, degree(c))));
	}
	s.finalize();
	t = COPY(t, c2);
	for ( int i=0; i<=degree(t); ++i ) {
		t.set_coeff(i, coeff(t, i) * recip(coeff(b, degree(b)) * coeff(c, degree(c))));
	}
	t.finalize();
}

static ex replace_lc(const ex& poly, const ex& x, const ex& lc)
{
	ex r = expand(poly + (lc - poly.lcoeff(x)) * pow(x, poly.degree(x)));
	return r;
}

static ex hensel_univar(const ex& a_, const ex& x, unsigned int p, const umod& u1_, const umod& w1_, const ex& gamma_ = 0)
{
	ex a = a_;
	const cl_univpoly_modint_ring& UPR = u1_.ring();
	const cl_modint_ring& R = UPR->basering();

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
	umod nu1 = COPY(nu1, u1_);
	unit_normal(nu1);
	umod nw1 = COPY(nw1, w1_);
	unit_normal(nw1);
	ex phi;
	phi = gamma * umod_to_ex(nu1, x);
	umod u1 = umod_from_ex(phi, x, R);
	phi = alpha * umod_to_ex(nw1, x);
	umod w1 = umod_from_ex(phi, x, R);

	// step 2
	umod g = UPR->create(-1);
	umod s = UPR->create(-1);
	umod t = UPR->create(-1);
	exteuclid(u1, w1, g, s, t);

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
		umod sigmatilde = umod_from_ex(phi, x, R);
		phi = expand(umod_to_ex(t, x) * c);
		umod tautilde = umod_from_ex(phi, x, R);
		umod q = UPR->create(-1);
		umod r = remdiv(sigmatilde, w1, q);
		umod sigma = COPY(sigma, r);
		phi = expand(umod_to_ex(tautilde, x) + umod_to_ex(q, x) * umod_to_ex(u1, x));
		umod tau = umod_from_ex(phi, x, R);
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

static void split(const umodvec& factors, const Partition& part, umod& a, umod& b)
{
	a = factors.front().ring()->one();
	b = factors.front().ring()->one();
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
	umodvec factors;
};

static ex factor_univariate(const ex& poly, const ex& x)
{
	DCOUT(factor_univariate);
	DCOUTVAR(poly);

	ex unit, cont, prim;
	poly.unitcontprim(x, unit, cont, prim);

	// determine proper prime and minimize number of modular factors
	unsigned int p = 3, lastp = 3;
	cl_modint_ring R;
	unsigned int trials = 0;
	unsigned int minfactors = 0;
	numeric lcoeff = ex_to<numeric>(prim.lcoeff(x));
	umodvec factors;
	while ( trials < 2 ) {
		while ( true ) {
			p = next_prime(p);
			if ( irem(lcoeff, p) != 0 ) {
				R = find_modint_ring(p);
				umod modpoly = umod_from_ex(prim, x, R);
				if ( squarefree(modpoly) ) break;
			}
		}

		// do modular factorization
		umod modpoly = umod_from_ex(prim, x, R);
		umodvec trialfactors;
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
			umod a = UPR->create(-1);
			umod b = UPR->create(-1);
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
					umodvec newfactors1(part.size_first(), UPR->create(-1)), newfactors2(part.size_second(), UPR->create(-1));
					umodvec::iterator i1 = newfactors1.begin(), i2 = newfactors2.begin();
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

	DCOUT(END factor_univariate);
	return unit * cont * result;
}

struct EvalPoint
{
	ex x;
	int evalpoint;
};

// MARK

// forward declaration
vector<ex> multivar_diophant(const vector<ex>& a_, const ex& x, const ex& c, const vector<EvalPoint>& I, unsigned int d, unsigned int p, unsigned int k);

umodvec multiterm_eea_lift(const umodvec& a, const ex& x, unsigned int p, unsigned int k)
{
	DCOUT(multiterm_eea_lift);
	DCOUTVAR(a);
	DCOUTVAR(p);
	DCOUTVAR(k);

	const size_t r = a.size();
	DCOUTVAR(r);
	cl_modint_ring R = find_modint_ring(expt_pos(cl_I(p),k));
	cl_univpoly_modint_ring UPR = find_univpoly_ring(R);
	umodvec q(r-1, UPR->create(-1));
	q[r-2] = a[r-1];
	for ( size_t j=r-2; j>=1; --j ) {
		q[j-1] = a[j] * q[j];
	}
	DCOUTVAR(q);
	umod beta = UPR->one();
	umodvec s;
	for ( size_t j=1; j<r; ++j ) {
		DCOUTVAR(j);
		DCOUTVAR(beta);
		vector<ex> mdarg(2);
		mdarg[0] = umod_to_ex(q[j-1], x);
		mdarg[1] = umod_to_ex(a[j-1], x);
		vector<EvalPoint> empty;
		vector<ex> exsigma = multivar_diophant(mdarg, x, umod_to_ex(beta, x), empty, 0, p, k);
		umod sigma1 = umod_from_ex(exsigma[0], x, R);
		umod sigma2 = umod_from_ex(exsigma[1], x, R);
		beta = COPY(beta, sigma1);
		s.push_back(sigma2);
	}
	s.push_back(beta);

	DCOUTVAR(s);
	DCOUT(END multiterm_eea_lift);
	return s;
}

void change_modulus(umod& out, const umod& in)
{
	// ASSERT: out and in have same degree
	if ( out.ring() == in.ring() ) {
		out = COPY(out, in);
	}
	else {
		for ( int i=0; i<=degree(in); ++i ) {
			out.set_coeff(i, out.ring()->basering()->canonhom(in.ring()->basering()->retract(coeff(in, i))));
		}
		out.finalize();
	}
}

void eea_lift(const umod& a, const umod& b, const ex& x, unsigned int p, unsigned int k, umod& s_, umod& t_)
{
	DCOUT(eea_lift);

	cl_modint_ring R = find_modint_ring(p);
	cl_univpoly_modint_ring UPR = find_univpoly_ring(R);
	umod amod = UPR->create(degree(a));
	change_modulus(amod, a);
	umod bmod = UPR->create(degree(b));
	change_modulus(bmod, b);

	umod g = UPR->create(-1);
	umod smod = UPR->create(-1);
	umod tmod = UPR->create(-1);
	exteuclid(amod, bmod, g, smod, tmod);
	
	cl_modint_ring Rpk = find_modint_ring(expt_pos(cl_I(p),k));
	cl_univpoly_modint_ring UPRpk = find_univpoly_ring(Rpk);
	umod s = UPRpk->create(degree(smod));
	change_modulus(s, smod);
	umod t = UPRpk->create(degree(tmod));
	change_modulus(t, tmod);

	cl_I modulus(p);
	umod one = UPRpk->one();
	for ( size_t j=1; j<k; ++j ) {
		umod e = one - a * s - b * t;
		e = divide(e, modulus);
		umod c = UPR->create(degree(e));
		change_modulus(c, e);
		umod sigmabar = smod * c;
		umod taubar = tmod * c;
		umod q = UPR->create(-1);
		umod sigma = remdiv(sigmabar, bmod, q);
		umod tau = taubar + q * amod;
		umod sadd = UPRpk->create(degree(sigma));
		change_modulus(sadd, sigma);
		cl_MI modmodulus(Rpk, modulus);
		s = s + sadd * modmodulus;
		umod tadd = UPRpk->create(degree(tau));
		change_modulus(tadd, tau);
		t = t + tadd * modmodulus;
		modulus = modulus * p;
	}

	s_ = s; t_ = t;

	DCOUT2(check, a*s + b*t);
	DCOUT(END eea_lift);
}

umodvec univar_diophant(const umodvec& a, const ex& x, unsigned int m, unsigned int p, unsigned int k)
{
	DCOUT(univar_diophant);
	DCOUTVAR(a);
	DCOUTVAR(x);
	DCOUTVAR(m);
	DCOUTVAR(p);
	DCOUTVAR(k);

	cl_modint_ring R = find_modint_ring(expt_pos(cl_I(p),k));
	cl_univpoly_modint_ring UPR = find_univpoly_ring(R);

	const size_t r = a.size();
	umodvec result;
	if ( r > 2 ) {
		umodvec s = multiterm_eea_lift(a, x, p, k);
		for ( size_t j=0; j<r; ++j ) {
			ex phi = expand(pow(x,m) * umod_to_ex(s[j], x));
			umod bmod = umod_from_ex(phi, x, R);
			umod buf = rem(bmod, a[j]);
			result.push_back(buf);
		}
	}
	else {
		umod s = UPR->create(-1);
		umod t = UPR->create(-1);
		eea_lift(a[1], a[0], x, p, k, s, t);
		ex phi = expand(pow(x,m) * umod_to_ex(s, x));
		umod bmod = umod_from_ex(phi, x, R);
		umod q = UPR->create(-1);
		umod buf = remdiv(bmod, a[0], q);
		result.push_back(buf);
		phi = expand(pow(x,m) * umod_to_ex(t, x));
		umod t1mod = umod_from_ex(phi, x, R);
		umod buf2 = t1mod + q * a[1];
		result.push_back(buf2);
	}

	DCOUTVAR(result);
	DCOUT(END univar_diophant);
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

	DCOUT(multivar_diophant);
#ifdef DEBUGFACTOR
	cout << "a ";
	for ( size_t i=0; i<a.size(); ++i ) {
		cout << a[i] << " ";
	}
	cout << endl;
#endif
	DCOUTVAR(x);
	DCOUTVAR(c);
#ifdef DEBUGFACTOR
	cout << "I ";
	for ( size_t i=0; i<I.size(); ++i ) {
		cout << I[i].x << "=" << I[i].evalpoint << " ";
	}
	cout << endl;
#endif
	DCOUTVAR(d);
	DCOUTVAR(p);
	DCOUTVAR(k);

	const cl_modint_ring R = find_modint_ring(expt_pos(cl_I(p),k));
	const size_t r = a.size();
	const size_t nu = I.size() + 1;
	DCOUTVAR(r);
	DCOUTVAR(nu);

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
		DCOUTVAR(sigma);

		ex buf = c;
		for ( size_t i=0; i<r; ++i ) {
			buf -= sigma[i] * b[i];
		}
		ex e = make_modular(buf, R);

		DCOUTVAR(e);
		DCOUTVAR(d);
		ex monomial = 1;
		for ( size_t m=1; m<=d; ++m ) {
			DCOUTVAR(m);
			while ( !e.is_zero() && e.has(xnu) ) {
				monomial *= (xnu - alphanu);
				monomial = expand(monomial);
				DCOUTVAR(monomial);
				DCOUTVAR(xnu);
				DCOUTVAR(alphanu);
				ex cm = e.diff(ex_to<symbol>(xnu), m).subs(xnu==alphanu) / factorial(m);
				cm = make_modular(cm, R);
				DCOUTVAR(cm);
				if ( !cm.is_zero() ) {
					vector<ex> delta_s = multivar_diophant(anew, x, cm, Inew, d, p, k);
					DCOUTVAR(delta_s);
					ex buf = e;
					for ( size_t j=0; j<delta_s.size(); ++j ) {
						delta_s[j] *= monomial;
						sigma[j] += delta_s[j];
						buf -= delta_s[j] * b[j];
					}
					e = make_modular(buf, R);
					DCOUTVAR(e);
				}
			}
		}
	}
	else {
		DCOUT(uniterm left);
		umodvec amod;
		for ( size_t i=0; i<a.size(); ++i ) {
			umod up = umod_from_ex(a[i], x, R);
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
		DCOUTVAR(nterms);
		for ( size_t i=0; i<nterms; ++i ) {
			DCOUTVAR(z);
			int m = z.degree(x);
			DCOUTVAR(m);
			cl_I cm = the<cl_I>(ex_to<numeric>(z.lcoeff(x)).to_cl_N());
			DCOUTVAR(cm);
			umodvec delta_s = univar_diophant(amod, x, m, p, k);
			cl_MI modcm;
			cl_I poscm = cm;
			while ( poscm < 0 ) {
				poscm = poscm + expt_pos(cl_I(p),k);
			}
			modcm = cl_MI(R, poscm);
			DCOUTVAR(modcm);
			for ( size_t j=0; j<delta_s.size(); ++j ) {
				delta_s[j] = delta_s[j] * modcm;
				sigma[j] = sigma[j] + umod_to_ex(delta_s[j], x);
			}
			DCOUTVAR(delta_s);
#ifdef DEBUGFACTOR
			cout << "STEP " << i << " sigma ";
			for ( size_t p=0; p<sigma.size(); ++p ) {
				cout << sigma[p] << " ";
			}
			cout << endl;
#endif
			if ( nterms > 1 ) {
				z = c.op(i+1);
			}
		}
	}
#ifdef DEBUGFACTOR
	cout << "sigma ";
	for ( size_t i=0; i<sigma.size(); ++i ) {
		cout << sigma[i] << " ";
	}
	cout << endl;
#endif

	for ( size_t i=0; i<sigma.size(); ++i ) {
		sigma[i] = make_modular(sigma[i], R);
	}

#ifdef DEBUGFACTOR
	cout << "sigma ";
	for ( size_t i=0; i<sigma.size(); ++i ) {
		cout << sigma[i] << " ";
	}
	cout << endl;
#endif
	DCOUT(END multivar_diophant);
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


ex hensel_multivar(const ex& a, const ex& x, const vector<EvalPoint>& I, unsigned int p, const cl_I& l, const umodvec& u, const vector<ex>& lcU)
{
	DCOUT(hensel_multivar);
	DCOUTVAR(a);
	DCOUTVAR(x);
	DCOUTVAR(I);
	DCOUTVAR(p);
	DCOUTVAR(l);
	DCOUTVAR(u);
	DCOUTVAR(lcU);
	const size_t nu = I.size() + 1;
	const cl_modint_ring R = find_modint_ring(expt_pos(cl_I(p),l));

	DCOUTVAR(nu);
	
	vector<ex> A(nu);
	A[nu-1] = a;

	for ( size_t j=nu; j>=2; --j ) {
		ex x = I[j-2].x;
		int alpha = I[j-2].evalpoint;
		A[j-2] = A[j-1].subs(x==alpha);
		A[j-2] = make_modular(A[j-2], R);
	}

#ifdef DEBUGFACTOR
	cout << "A ";
	for ( size_t i=0; i<A.size(); ++i) cout << A[i] << " ";
	cout << endl;
#endif

	int maxdeg = a.degree(I.front().x);
	for ( size_t i=1; i<I.size(); ++i ) {
		int maxdeg2 = a.degree(I[i].x);
		if ( maxdeg2 > maxdeg ) maxdeg = maxdeg2;
	}
	DCOUTVAR(maxdeg);

	const size_t n = u.size();
	DCOUTVAR(n);
	vector<ex> U(n);
	for ( size_t i=0; i<n; ++i ) {
		U[i] = umod_to_ex(u[i], x);
	}
#ifdef DEBUGFACTOR
	cout << "U ";
	for ( size_t i=0; i<U.size(); ++i) cout << U[i] << " ";
	cout << endl;
#endif

	for ( size_t j=2; j<=nu; ++j ) {
		DCOUTVAR(j);
		vector<ex> U1 = U;
		ex monomial = 1;
		DCOUTVAR(U);
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
		DCOUTVAR(U);
		ex Uprod = 1;
		for ( size_t i=0; i<n; ++i ) {
			Uprod *= U[i];
		}
		ex e = expand(A[j-1] - Uprod);
		DCOUTVAR(e);

		vector<EvalPoint> newI;
		for ( size_t i=1; i<=j-2; ++i ) {
			newI.push_back(I[i-1]);
		}
		DCOUTVAR(newI);

		ex xj = I[j-2].x;
		int alphaj = I[j-2].evalpoint;
		size_t deg = A[j-1].degree(xj);
		DCOUTVAR(deg);
		for ( size_t k=1; k<=deg; ++k ) {
			DCOUTVAR(k);
			if ( !e.is_zero() ) {
				DCOUTVAR(xj);
				DCOUTVAR(alphaj);
				monomial *= (xj - alphaj);
				monomial = expand(monomial);
				DCOUTVAR(monomial);
				ex dif = e.diff(ex_to<symbol>(xj), k);
				DCOUTVAR(dif);
				ex c = dif.subs(xj==alphaj) / factorial(k);
				DCOUTVAR(c);
				if ( !c.is_zero() ) {
					vector<ex> deltaU = multivar_diophant(U1, x, c, newI, maxdeg, p, cl_I_to_uint(l));
					for ( size_t i=0; i<n; ++i ) {
						DCOUTVAR(i);
						DCOUTVAR(deltaU[i]);
						deltaU[i] *= monomial;
						U[i] += deltaU[i];
						U[i] = make_modular(U[i], R);
						DCOUTVAR(U[i]);
					}
					ex Uprod = 1;
					for ( size_t i=0; i<n; ++i ) {
						Uprod *= U[i];
					}
					DCOUTVAR(Uprod.expand());
					DCOUTVAR(A[j-1]);
					e = A[j-1] - Uprod;
					e = make_modular(e, R);
					DCOUTVAR(e);
				}
			}
		}
	}

	ex acand = 1;
	for ( size_t i=0; i<U.size(); ++i ) {
		acand *= U[i];
	}
	DCOUTVAR(acand);
	if ( expand(a-acand).is_zero() ) {
		lst res;
		for ( size_t i=0; i<U.size(); ++i ) {
			res.append(U[i]);
		}
		DCOUTVAR(res);
		DCOUT(END hensel_multivar);
		return res;
	}
	else {
		lst res;
		DCOUTVAR(res);
		DCOUT(END hensel_multivar);
		return lst();
	}
}

static ex put_factors_into_lst(const ex& e)
{
	DCOUT(put_factors_into_lst);
	DCOUTVAR(e);

	lst result;

	if ( is_a<numeric>(e) ) {
		result.append(e);
		DCOUT(END put_factors_into_lst);
		DCOUTVAR(result);
		return result;
	}
	if ( is_a<power>(e) ) {
		result.append(1);
		result.append(e.op(0));
		result.append(e.op(1));
		DCOUT(END put_factors_into_lst);
		DCOUTVAR(result);
		return result;
	}
	if ( is_a<symbol>(e) || is_a<add>(e) ) {
		result.append(1);
		result.append(e);
		result.append(1);
		DCOUT(END put_factors_into_lst);
		DCOUTVAR(result);
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
		DCOUT(END put_factors_into_lst);
		DCOUTVAR(result);
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
	DCOUT(checkdivisors);
	const int k = f.nops()-2;
	DCOUTVAR(k);
	DCOUTVAR(d.size());
	numeric q, r;
	d[0] = ex_to<numeric>(f.op(0) * f.op(f.nops()-1));
	if ( d[0] == 1 && k == 1 && abs(f.op(1)) != 1 ) {
		DCOUT(false);
		DCOUT(END checkdivisors);
		return false;
	}
	DCOUTVAR(d[0]);
	for ( int i=1; i<=k; ++i ) {
		DCOUTVAR(i);
		DCOUTVAR(abs(f.op(i)));
		q = ex_to<numeric>(abs(f.op(i)));
		DCOUTVAR(q);
		for ( int j=i-1; j>=0; --j ) {
			r = d[j];
			DCOUTVAR(r);
			do {
				r = gcd(r, q);
				DCOUTVAR(r);
				q = q/r;
				DCOUTVAR(q);
			} while ( r != 1 );
			if ( q == 1 ) {
				DCOUT(true);
				DCOUT(END checkdivisors);
				return true;
			}
		}
		d[i] = q;
	}
	DCOUT(false);
	DCOUT(END checkdivisors);
	return false;
}

static bool generate_set(const ex& u, const ex& vn, const exset& syms, const ex& f, const numeric& modulus, vector<numeric>& a, vector<numeric>& d)
{
	// computation of d is actually not necessary
	DCOUT(generate_set);
	DCOUTVAR(u);
	DCOUTVAR(vn);
	DCOUTVAR(f);
	DCOUTVAR(modulus);
	const ex& x = *syms.begin();
	bool trying = true;
	do {
		ex u0 = u;
		ex vna = vn;
		ex vnatry;
		exset::const_iterator s = syms.begin();
		++s;
		for ( size_t i=0; i<a.size(); ++i ) {
			DCOUTVAR(*s);
			do {
				a[i] = mod(numeric(rand()), 2*modulus) - modulus;
				vnatry = vna.subs(*s == a[i]);
			} while ( vnatry == 0 );
			vna = vnatry;
			u0 = u0.subs(*s == a[i]);
			++s;
		}
		DCOUTVAR(a);
		DCOUTVAR(u0);
		if ( gcd(u0,u0.diff(ex_to<symbol>(x))) != 1 ) {
			continue;
		}
		if ( is_a<numeric>(vn) ) {
			trying = false;
		}
		else {
			DCOUT(do substitution);
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
			DCOUTVAR(fnum);
			trying = checkdivisors(fnum, d);
		}
	} while ( trying );
	DCOUT(END generate_set);
	return false;
}

static ex factor_multivariate(const ex& poly, const exset& syms)
{
	DCOUT(factor_multivariate);
	DCOUTVAR(poly);

	exset::const_iterator s;
	const ex& x = *syms.begin();
	DCOUTVAR(x);

	/* make polynomial primitive */
	ex p = poly.expand().collect(x);
	DCOUTVAR(p);
	ex cont = p.lcoeff(x);
	for ( numeric i=p.degree(x)-1; i>=p.ldegree(x); --i ) {
		cont = gcd(cont, p.coeff(x,ex_to<numeric>(i).to_int()));
		if ( cont == 1 ) break;
	}
	DCOUTVAR(cont);
	ex pp = expand(normal(p / cont));
	DCOUTVAR(pp);
	if ( !is_a<numeric>(cont) ) {
#ifdef DEBUGFACTOR
		return ::factor(cont) * ::factor(pp);
#else
		return factor(cont) * factor(pp);
#endif
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
#ifdef DEBUGFACTOR
		ex vnfactors = ::factor(vn);
#else
		ex vnfactors = factor(vn);
#endif
		vnlst = put_factors_into_lst(vnfactors);
	}
	DCOUTVAR(vnlst);

	const numeric maxtrials = 3;
	numeric modulus = (vnlst.nops()-1 > 3) ? vnlst.nops()-1 : 3;
	DCOUTVAR(modulus);
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
			DCOUTVAR(problem);
			if ( problem ) {
				++modulus;
				continue;
			}
			DCOUTVAR(a);
			DCOUTVAR(d);
			u = pp;
			s = syms.begin();
			++s;
			for ( size_t i=0; i<a.size(); ++i ) {
				u = u.subs(*s == a[i]);
				++s;
			}
			delta = u.content(x);
			DCOUTVAR(u);

			// determine proper prime
			prime = 3;
			DCOUTVAR(prime);
			cl_modint_ring R = find_modint_ring(prime);
			DCOUTVAR(u.lcoeff(x));
			while ( true ) {
				if ( irem(ex_to<numeric>(u.lcoeff(x)), prime) != 0 ) {
					umod modpoly = umod_from_ex(u, x, R);
					if ( squarefree(modpoly) ) break;
				}
				prime = next_prime(prime);
				DCOUTVAR(prime);
				R = find_modint_ring(prime);
			}

#ifdef DEBUGFACTOR
			ufac = ::factor(u);
#else
			ufac = factor(u);
#endif
			DCOUTVAR(ufac);
			ufaclst = put_factors_into_lst(ufac);
			DCOUTVAR(ufaclst);
			factor_count = (ufaclst.nops()-1)/2;
			DCOUTVAR(factor_count);

			if ( factor_count <= 1 ) {
				DCOUTVAR(poly);
				DCOUT(END factor_multivariate);
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
			DCOUTVAR(trialcount);
			DCOUTVAR(minimalr);
			if ( minimalr <= 1 ) {
				DCOUTVAR(poly);
				DCOUT(END factor_multivariate);
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
		DCOUTVAR(ftilde);

		vector<bool> used_flag((vnlst.nops()-1)/2+1, false);
		vector<ex> D(factor_count, 1);
		for ( size_t i=0; i<=factor_count; ++i ) {
			DCOUTVAR(i);
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
			DCOUTVAR(prefac);
			for ( size_t j=(vnlst.nops()-1)/2+1; j>0; --j ) {
				DCOUTVAR(j);
				DCOUTVAR(prefac);
				DCOUTVAR(ftilde[j-1]);
				if ( abs(ftilde[j-1]) == 1 ) {
					used_flag[j-1] = true;
					continue;
				}
				numeric g = gcd(prefac, ftilde[j-1]);
				DCOUTVAR(g);
				if ( g != 1 ) {
					DCOUT(has_common_prime);
					prefac = prefac / g;
					numeric count = abs(iquo(g, ftilde[j-1]));
					DCOUTVAR(count);
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
						DCOUT(BREAK);
						DCOUTVAR(ftilde[j-1]);
						break;
					}
					++j;
				}
			}
		}
		DCOUTVAR(D);

		bool some_factor_unused = false;
		for ( size_t i=0; i<used_flag.size(); ++i ) {
			if ( !used_flag[i] ) {
				some_factor_unused = true;
				break;
			}
		}
		if ( some_factor_unused ) {
			DCOUT(some factor unused!);
			continue;
		}

		vector<ex> C(factor_count);
		DCOUTVAR(C);
		DCOUTVAR(delta);
		if ( delta == 1 ) {
			for ( size_t i=0; i<D.size(); ++i ) {
				ex Dtilde = D[i];
				s = syms.begin();
				++s;
				for ( size_t j=0; j<a.size(); ++j ) {
					Dtilde = Dtilde.subs(*s == a[j]);
					++s;
				}
				DCOUTVAR(Dtilde);
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
		DCOUTVAR(C);

		EvalPoint ep;
		vector<EvalPoint> epv;
		s = syms.begin();
		++s;
		for ( size_t i=0; i<a.size(); ++i ) {
			ep.x = *s++;
			ep.evalpoint = a[i].to_int();
			epv.push_back(ep);
		}
		DCOUTVAR(epv);

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

		umodvec uvec;
		cl_modint_ring R = find_modint_ring(expt_pos(cl_I(prime),l));
		for ( size_t i=0; i<(ufaclst.nops()-1)/2; ++i ) {
			umod newu = umod_from_ex(ufaclst.op(i*2+1), x, R);
			uvec.push_back(newu);
		}
		DCOUTVAR(uvec);

		ex res = hensel_multivar(ufaclst.op(0)*pp, x, epv, prime, l, uvec, C);
		if ( res != lst() ) {
			ex result = cont * ufaclst.op(0);
			for ( size_t i=0; i<res.nops(); ++i ) {
				result *= res.op(i).content(x) * res.op(i).unit(x);
				result *= res.op(i).primpart(x);
			}
			DCOUTVAR(result);
			DCOUT(END factor_multivariate);
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
	DCOUT(factor_sqrfree);

	// determine all symbols in poly
	find_symbols_map findsymbols;
	findsymbols(poly);
	if ( findsymbols.syms.size() == 0 ) {
		DCOUT(END factor_sqrfree);
		return poly;
	}

	if ( findsymbols.syms.size() == 1 ) {
		// univariate case
		const ex& x = *(findsymbols.syms.begin());
		if ( poly.ldegree(x) > 0 ) {
			int ld = poly.ldegree(x);
			ex res = factor_univariate(expand(poly/pow(x, ld)), x);
			DCOUT(END factor_sqrfree);
			return res * pow(x,ld);
		}
		else {
			ex res = factor_univariate(poly, x);
			DCOUT(END factor_sqrfree);
			return res;
		}
	}

	// multivariate case
	ex res = factor_multivariate(poly, findsymbols.syms);
	DCOUT(END factor_sqrfree);
	return res;
}

struct apply_factor_map : public map_function {
	unsigned options;
	apply_factor_map(unsigned options_) : options(options_) { }
	ex operator()(const ex& e)
	{
		if ( e.info(info_flags::polynomial) ) {
#ifdef DEBUGFACTOR
			return ::factor(e, options);
#else
			return factor(e, options);
#endif
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
#ifdef DEBUGFACTOR
			return ::factor(s1, options) + s2.map(*this);
#else
			return factor(s1, options) + s2.map(*this);
#endif
		}
		return e.map(*this);
	}
};

} // anonymous namespace

#ifdef DEBUGFACTOR
ex factor(const ex& poly, unsigned options = 0)
#else
ex factor(const ex& poly, unsigned options)
#endif
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
