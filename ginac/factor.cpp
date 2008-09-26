/** @file factor.cpp
 *
 *  Polynomial factorization code (Implementation).
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

typedef vector<cl_MI> Vec;
typedef vector<Vec> VecVec;

#ifdef DEBUGFACTOR
ostream& operator<<(ostream& o, const Vec& v)
{
	Vec::const_iterator i = v.begin(), end = v.end();
	while ( i != end ) {
		o << *i++ << " ";
	}
	return o;
}
#endif // def DEBUGFACTOR

#ifdef DEBUGFACTOR
ostream& operator<<(ostream& o, const VecVec& v)
{
	VecVec::const_iterator i = v.begin(), end = v.end();
	while ( i != end ) {
		o << *i++ << endl;
	}
	return o;
}
#endif // def DEBUGFACTOR

struct Term
{
	cl_MI c;          // coefficient
	unsigned int exp; // exponent >=0
};

#ifdef DEBUGFACTOR
ostream& operator<<(ostream& o, const Term& t)
{
	if ( t.exp ) {
		o << "(" << t.c << ")x^" << t.exp;
	}
	else {
		o << "(" << t.c << ")";
	}
	return o;
}
#endif // def DEBUGFACTOR

struct UniPoly
{
	cl_modint_ring R;
	list<Term> terms;  // highest exponent first

	UniPoly(const cl_modint_ring& ring) : R(ring) { }
	UniPoly(const cl_modint_ring& ring, const ex& poly, const ex& x) : R(ring)
	{ 
		// assert: poly is in Z[x]
		Term t;
		for ( int i=poly.degree(x); i>=poly.ldegree(x); --i ) {
			cl_I coeff = the<cl_I>(ex_to<numeric>(poly.coeff(x,i)).to_cl_N());
			if ( !zerop(coeff) ) {
				t.c = R->canonhom(coeff);
				if ( !zerop(t.c) ) {
					t.exp = i;
					terms.push_back(t);
				}
			}
		}
	}
	UniPoly(const cl_modint_ring& ring, const UniPoly& poly) : R(ring)
	{ 
		if ( R->modulus == poly.R->modulus ) {
			terms = poly.terms;
		}
		else {
			list<Term>::const_iterator i=poly.terms.begin(), end=poly.terms.end();
			for ( ; i!=end; ++i ) {
				terms.push_back(*i);
				terms.back().c = R->canonhom(poly.R->retract(i->c));
				if ( zerop(terms.back().c) ) {
					terms.pop_back();
				}
			}
		}
	}
	UniPoly(const cl_modint_ring& ring, const Vec& v) : R(ring)
	{
		Term t;
		for ( unsigned int i=0; i<v.size(); ++i ) {
			if ( !zerop(v[i]) ) {
				t.c = v[i];
				t.exp = i;
				terms.push_front(t);
			}
		}
	}
	unsigned int degree() const
	{
		if ( terms.size() ) {
			return terms.front().exp;
		}
		else {
			return 0;
		}
	}
	bool zero() const { return (terms.size() == 0); }
	const cl_MI operator[](unsigned int deg) const
	{
		list<Term>::const_iterator i = terms.begin(), end = terms.end();
		for ( ; i != end; ++i ) {
			if ( i->exp == deg ) {
				return i->c;
			}
			if ( i->exp < deg ) {
				break;
			}
		}
		return R->zero();
	}
	void set(unsigned int deg, const cl_MI& c)
	{
		list<Term>::iterator i = terms.begin(), end = terms.end();
		while ( i != end ) {
			if ( i->exp == deg ) {
				if ( !zerop(c) ) {
					i->c = c;
				}
				else {
					terms.erase(i);
				}
				return;
			}
			if ( i->exp < deg ) {
				break;
			}
			++i;
		}
		if ( !zerop(c) ) {
			Term t;
			t.c = c;
			t.exp = deg;
			terms.insert(i, t);
		}
	}
	ex to_ex(const ex& x, bool symmetric = true) const
	{
		ex r;
		list<Term>::const_iterator i = terms.begin(), end = terms.end();
		if ( symmetric ) {
			numeric mod(R->modulus);
			numeric halfmod = (mod-1)/2;
			for ( ; i != end; ++i ) {
				numeric n(R->retract(i->c));
				if ( n > halfmod ) {
					r += pow(x, i->exp) * (n-mod);
				}
				else {
					r += pow(x, i->exp) * n;
				}
			}
		}
		else {
			for ( ; i != end; ++i ) {
				r += pow(x, i->exp) * numeric(R->retract(i->c));
			}
		}
		return r;
	}
	void unit_normal()
	{
		if ( terms.size() ) {
			if ( terms.front().c != R->one() ) {
				list<Term>::iterator i = terms.begin(), end = terms.end();
				cl_MI cont = i->c;
				i->c = R->one();
				while ( ++i != end ) {
					i->c = div(i->c, cont);
					if ( zerop(i->c) ) {
						terms.erase(i);
					}
				}
			}
		}
	}
	cl_MI unit() const
	{
		return terms.front().c;
	}
	void divide(const cl_MI& x)
	{
		list<Term>::iterator i = terms.begin(), end = terms.end();
		for ( ; i != end; ++i ) {
			i->c = div(i->c, x);
			if ( zerop(i->c) ) {
				terms.erase(i);
			}
		}
	}
	void divide(const cl_I& x)
	{
		list<Term>::iterator i = terms.begin(), end = terms.end();
		for ( ; i != end; ++i ) {
			i->c = cl_MI(R, the<cl_I>(R->retract(i->c) / x));
		}
	}
	void reduce_exponents(unsigned int prime)
	{
		list<Term>::iterator i = terms.begin(), end = terms.end();
		while ( i != end ) {
			if ( i->exp > 0 ) {
				// assert: i->exp is multiple of prime
				i->exp /= prime;
			}
			++i;
		}
	}
	void deriv(UniPoly& d) const
	{
		list<Term>::const_iterator i = terms.begin(), end = terms.end();
		while ( i != end ) {
			if ( i->exp ) {
				cl_MI newc = i->c * i->exp;
				if ( !zerop(newc) ) {
					Term t;
					t.c = newc;
					t.exp = i->exp-1;
					d.terms.push_back(t);
				}
			}
			++i;
		}
	}
	bool operator<(const UniPoly& o) const
	{
		if ( terms.size() != o.terms.size() ) {
			return terms.size() < o.terms.size();
		}
		list<Term>::const_iterator i1 = terms.begin(), end = terms.end();
		list<Term>::const_iterator i2 = o.terms.begin();
		while ( i1 != end ) {
			if ( i1->exp != i2->exp ) {
				return i1->exp < i2->exp;
			}
			if ( i1->c != i2->c ) {
				return R->retract(i1->c) < R->retract(i2->c);
			}
			++i1; ++i2;
		}
		return true;
	}
	bool operator==(const UniPoly& o) const
	{
		if ( terms.size() != o.terms.size() ) {
			return false;
		}
		list<Term>::const_iterator i1 = terms.begin(), end = terms.end();
		list<Term>::const_iterator i2 = o.terms.begin();
		while ( i1 != end ) {
			if ( i1->exp != i2->exp ) {
				return false;
			}
			if ( i1->c != i2->c ) {
				return false;
			}
			++i1; ++i2;
		}
		return true;
	}
	bool operator!=(const UniPoly& o) const
	{
		bool res = !(*this == o);
		return res;
	}
};

static UniPoly operator*(const UniPoly& a, const UniPoly& b)
{
	unsigned int n = a.degree()+b.degree();
	UniPoly c(a.R);
	Term t;
	for ( unsigned int i=0 ; i<=n; ++i ) {
		t.c = a.R->zero();
		for ( unsigned int j=0 ; j<=i; ++j ) {
			t.c = t.c + a[j] * b[i-j];
		}
		if ( !zerop(t.c) ) {
			t.exp = i;
			c.terms.push_front(t);
		}
	}
	return c;
}

static UniPoly operator-(const UniPoly& a, const UniPoly& b)
{
	list<Term>::const_iterator ia = a.terms.begin(), aend = a.terms.end();
	list<Term>::const_iterator ib = b.terms.begin(), bend = b.terms.end();
	UniPoly c(a.R);
	while ( ia != aend && ib != bend ) {
		if ( ia->exp > ib->exp ) {
			c.terms.push_back(*ia);
			++ia;
		}
		else if ( ia->exp < ib->exp ) {
			c.terms.push_back(*ib);
			c.terms.back().c = -c.terms.back().c;
			++ib;
		}
		else {
			Term t;
			t.exp = ia->exp;
			t.c = ia->c - ib->c;
			if ( !zerop(t.c) ) {
				c.terms.push_back(t);
			}
			++ia; ++ib;
		}
	}
	while ( ia != aend ) {
		c.terms.push_back(*ia);
		++ia;
	}
	while ( ib != bend ) {
		c.terms.push_back(*ib);
		c.terms.back().c = -c.terms.back().c;
		++ib;
	}
	return c;
}

static UniPoly operator*(const UniPoly& a, const cl_MI& fac)
{
	unsigned int n = a.degree();
	UniPoly c(a.R);
	Term t;
	for ( unsigned int i=0 ; i<=n; ++i ) {
		t.c = a[i] * fac;
		if ( !zerop(t.c) ) {
			t.exp = i;
			c.terms.push_front(t);
		}
	}
	return c;
}

static UniPoly operator+(const UniPoly& a, const UniPoly& b)
{
	list<Term>::const_iterator ia = a.terms.begin(), aend = a.terms.end();
	list<Term>::const_iterator ib = b.terms.begin(), bend = b.terms.end();
	UniPoly c(a.R);
	while ( ia != aend && ib != bend ) {
		if ( ia->exp > ib->exp ) {
			c.terms.push_back(*ia);
			++ia;
		}
		else if ( ia->exp < ib->exp ) {
			c.terms.push_back(*ib);
			++ib;
		}
		else {
			Term t;
			t.exp = ia->exp;
			t.c = ia->c + ib->c;
			if ( !zerop(t.c) ) {
				c.terms.push_back(t);
			}
			++ia; ++ib;
		}
	}
	while ( ia != aend ) {
		c.terms.push_back(*ia);
		++ia;
	}
	while ( ib != bend ) {
		c.terms.push_back(*ib);
		++ib;
	}
	return c;
}

// static UniPoly operator-(const UniPoly& a)
// {
// 	list<Term>::const_iterator ia = a.terms.begin(), aend = a.terms.end();
// 	UniPoly c(a.R);
// 	while ( ia != aend ) {
// 		c.terms.push_back(*ia);
// 		c.terms.back().c = -c.terms.back().c;
// 		++ia;
// 	}
// 	return c;
// }

#ifdef DEBUGFACTOR
ostream& operator<<(ostream& o, const UniPoly& t)
{
	list<Term>::const_iterator i = t.terms.begin(), end = t.terms.end();
	if ( i == end ) {
		o << "0";
		return o;
	}
	for ( ; i != end; ) {
		o << *i++;
		if ( i != end ) {
			o << " + ";
		}
	}
	return o;
}
#endif // def DEBUGFACTOR

#ifdef DEBUGFACTOR
ostream& operator<<(ostream& o, const list<UniPoly>& t)
{
	list<UniPoly>::const_iterator i = t.begin(), end = t.end();
	o << "{" << endl;
	for ( ; i != end; ) {
		o << *i++ << endl;
	}
	o << "}" << endl;
	return o;
}
#endif // def DEBUGFACTOR

typedef vector<UniPoly> UniPolyVec;

#ifdef DEBUGFACTOR
ostream& operator<<(ostream& o, const UniPolyVec& v)
{
	UniPolyVec::const_iterator i = v.begin(), end = v.end();
	while ( i != end ) {
		o << *i++ << " , " << endl;
	}
	return o;
}
#endif // def DEBUGFACTOR

struct UniFactor
{
	UniPoly p;
	unsigned int exp;

	UniFactor(const cl_modint_ring& ring) : p(ring) { }
	UniFactor(const UniPoly& p_, unsigned int exp_) : p(p_), exp(exp_) { }
	bool operator<(const UniFactor& o) const
	{
		return p < o.p;
	}
};

struct UniFactorVec
{
	vector<UniFactor> factors;

	void unique()
	{
		sort(factors.begin(), factors.end());
		if ( factors.size() > 1 ) {
			vector<UniFactor>::iterator i = factors.begin();
			vector<UniFactor>::const_iterator cmp = factors.begin()+1;
			vector<UniFactor>::iterator end = factors.end();
			while ( cmp != end ) {
				if ( i->p != cmp->p ) {
					++i;
					++cmp;
				}
				else {
					i->exp += cmp->exp;
					++cmp;
				}
			}
			if ( i != end-1 ) {
				factors.erase(i+1, end);
			}
		}
	}
};

#ifdef DEBUGFACTOR
ostream& operator<<(ostream& o, const UniFactorVec& ufv)
{
	for ( size_t i=0; i<ufv.factors.size(); ++i ) {
		if ( i != ufv.factors.size()-1 ) {
			o << "*";
		}
		else {
			o << " ";
		}
		o << "[ " << ufv.factors[i].p << " ]^" << ufv.factors[i].exp << endl;
	}
	return o;
}
#endif // def DEBUGFACTOR

static void rem(const UniPoly& a_, const UniPoly& b, UniPoly& c)
{
	if ( a_.degree() < b.degree() ) {
		c = a_;
		return;
	}

	unsigned int k, n;
	n = b.degree();
	k = a_.degree() - n;

	if ( n == 0 ) {
		c.terms.clear();
		return;
	}

	c = a_;
	Term termbuf;

	while ( true ) {
		cl_MI qk = div(c[n+k], b[n]);
		if ( !zerop(qk) ) {
			unsigned int j;
			for ( unsigned int i=0; i<n; ++i ) {
				j = n + k - 1 - i;
				c.set(j, c[j] - qk*b[j-k]);
			}
		}
		if ( k == 0 ) break;
		--k;
	}
	list<Term>::iterator i = c.terms.begin(), end = c.terms.end();
	while ( i != end ) {
		if ( i->exp <= n-1 ) {
			break;
		}
		++i;
	}
	c.terms.erase(c.terms.begin(), i);
}

static void div(const UniPoly& a_, const UniPoly& b, UniPoly& q)
{
	if ( a_.degree() < b.degree() ) {
		q.terms.clear();
		return;
	}

	unsigned int k, n;
	n = b.degree();
	k = a_.degree() - n;

	UniPoly c = a_;
	Term termbuf;

	while ( true ) {
		cl_MI qk = div(c[n+k], b[n]);
		if ( !zerop(qk) ) {
			Term t;
			t.c = qk;
			t.exp = k;
			q.terms.push_back(t);
			unsigned int j;
			for ( unsigned int i=0; i<n; ++i ) {
				j = n + k - 1 - i;
				c.set(j, c[j] - qk*b[j-k]);
			}
		}
		if ( k == 0 ) break;
		--k;
	}
}

static void gcd(const UniPoly& a, const UniPoly& b, UniPoly& c)
{
	c = a;
	c.unit_normal();
	UniPoly d = b;
	d.unit_normal();

	if ( c.degree() < d.degree() ) {
		gcd(b, a, c);
		return;
	}

	while ( !d.zero() ) {
		UniPoly r(a.R);
		rem(c, d, r);
		c = d;
		d = r;
	}
	c.unit_normal();
}

static bool is_one(const UniPoly& w)
{
	if ( w.terms.size() == 1 && w[0] == w.R->one() ) {
		return true;
	}
	return false;
}

static void sqrfree_main(const UniPoly& a, UniFactorVec& fvec)
{
	unsigned int i = 1;
	UniPoly b(a.R);
	a.deriv(b);
	if ( !b.zero() ) {
		UniPoly c(a.R), w(a.R);
		gcd(a, b, c);
		div(a, c, w);
		while ( !is_one(w) ) {
			UniPoly y(a.R), z(a.R);
			gcd(w, c, y);
			div(w, y, z);
			if ( !is_one(z) ) {
				UniFactor uf(z, i);
				fvec.factors.push_back(uf);
			}
			++i;
			w = y;
			UniPoly cbuf(a.R);
			div(c, y, cbuf);
			c = cbuf;
		}
		if ( !is_one(c) ) {
			unsigned int prime = cl_I_to_uint(c.R->modulus);
			c.reduce_exponents(prime);
			unsigned int pos = fvec.factors.size();
			sqrfree_main(c, fvec);
			for ( unsigned int p=pos; p<fvec.factors.size(); ++p ) {
				fvec.factors[p].exp *= prime;
			}
			return;
		}
	}
	else {
		unsigned int prime = cl_I_to_uint(a.R->modulus);
		UniPoly amod = a;
		amod.reduce_exponents(prime);
		unsigned int pos = fvec.factors.size();
		sqrfree_main(amod, fvec);
		for ( unsigned int p=pos; p<fvec.factors.size(); ++p ) {
			fvec.factors[p].exp *= prime;
		}
		return;
	}
}

static void squarefree(const UniPoly& a, UniFactorVec& fvec)
{
	sqrfree_main(a, fvec);
	fvec.unique();
}

class Matrix
{
	friend ostream& operator<<(ostream& o, const Matrix& m);
public:
	Matrix(size_t r_, size_t c_, const cl_MI& init) : r(r_), c(c_)
	{
		m.resize(c*r, init);
	}
	size_t rowsize() const { return r; }
	size_t colsize() const { return c; }
	cl_MI& operator()(size_t row, size_t col) { return m[row*c + col]; }
	cl_MI operator()(size_t row, size_t col) const { return m[row*c + col]; }
	void mul_col(size_t col, const cl_MI x)
	{
		Vec::iterator i = m.begin() + col;
		for ( size_t rc=0; rc<r; ++rc ) {
			*i = *i * x;
			i += c;
		}
	}
	void sub_col(size_t col1, size_t col2, const cl_MI fac)
	{
		Vec::iterator i1 = m.begin() + col1;
		Vec::iterator i2 = m.begin() + col2;
		for ( size_t rc=0; rc<r; ++rc ) {
			*i1 = *i1 - *i2 * fac;
			i1 += c;
			i2 += c;
		}
	}
	void switch_col(size_t col1, size_t col2)
	{
		cl_MI buf;
		Vec::iterator i1 = m.begin() + col1;
		Vec::iterator i2 = m.begin() + col2;
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
		Vec::const_iterator i = m.begin() + col;
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
		Vec::const_iterator i = m.begin() + row*c;
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
		Vec::iterator i1 = m.begin() + row*c;
		Vec::const_iterator i2 = newrow.begin(), end = newrow.end();
		for ( ; i2 != end; ++i1, ++i2 ) {
			*i1 = *i2;
		}
	}
	Vec::const_iterator row_begin(size_t row) const { return m.begin()+row*c; }
	Vec::const_iterator row_end(size_t row) const { return m.begin()+row*c+r; }
private:
	size_t r, c;
	Vec m;
};

#ifdef DEBUGFACTOR
Matrix operator*(const Matrix& m1, const Matrix& m2)
{
	const unsigned int r = m1.rowsize();
	const unsigned int c = m2.colsize();
	Matrix o(r,c,m1(0,0));

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

ostream& operator<<(ostream& o, const Matrix& m)
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

static void q_matrix(const UniPoly& a, Matrix& Q)
{
	unsigned int n = a.degree();
	unsigned int q = cl_I_to_uint(a.R->modulus);
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
	for ( size_t i=0; i<n; ++i ) {
		UniPoly qk(a.R);
		qk.set(i*q, a.R->one());
		UniPoly r(a.R);
		rem(qk, a, r);
		Vec rvec;
		for ( size_t j=0; j<n; ++j ) {
			rvec.push_back(r[j]);
		}
		Q.set_row(i, rvec);
	}
}

static void nullspace(Matrix& M, vector<Vec>& basis)
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
			Vec nu(M.row_begin(i), M.row_end(i));
			basis.push_back(nu);
		}
	}
}

static void berlekamp(const UniPoly& a, UniPolyVec& upv)
{
	Matrix Q(a.degree(), a.degree(), a.R->zero());
	q_matrix(a, Q);
	VecVec nu;
	nullspace(Q, nu);
	const unsigned int k = nu.size();
	if ( k == 1 ) {
		return;
	}

	list<UniPoly> factors;
	factors.push_back(a);
	unsigned int size = 1;
	unsigned int r = 1;
	unsigned int q = cl_I_to_uint(a.R->modulus);

	list<UniPoly>::iterator u = factors.begin();

	while ( true ) {
		for ( unsigned int s=0; s<q; ++s ) {
			UniPoly g(a.R);
			UniPoly nur(a.R, nu[r]);
			nur.set(0, nur[0] - cl_MI(a.R, s));
			gcd(nur, *u, g);
			if ( !is_one(g) && g != *u ) {
				UniPoly uo(a.R);
				div(*u, g, uo);
				if ( is_one(uo) ) {
					throw logic_error("berlekamp: unexpected divisor.");
				}
				else {
					*u = uo;
				}
				factors.push_back(g);
				size = 0;
				list<UniPoly>::const_iterator i = factors.begin(), end = factors.end();
				while ( i != end ) {
					if ( i->degree() ) ++size; 
					++i;
				}
				if ( size == k ) {
					list<UniPoly>::const_iterator i = factors.begin(), end = factors.end();
					while ( i != end ) {
						upv.push_back(*i++);
					}
					return;
				}
//				if ( u->degree() < nur.degree() ) {
//					break;
//				}
			}
		}
		if ( ++r == k ) {
			r = 1;
			++u;
		}
	}
}

static void factor_modular(const UniPoly& p, UniPolyVec& upv)
{
	berlekamp(p, upv);
	return;
}

static void exteuclid(const UniPoly& a, const UniPoly& b, UniPoly& g, UniPoly& s, UniPoly& t)
{
	if ( a.degree() < b.degree() ) {
		exteuclid(b, a, g, t, s);
		return;
	}
	UniPoly c1(a.R), c2(a.R), d1(a.R), d2(a.R), q(a.R), r(a.R), r1(a.R), r2(a.R);
	UniPoly c = a; c.unit_normal();
	UniPoly d = b; d.unit_normal();
	c1.set(0, a.R->one());
	d2.set(0, a.R->one());
	while ( !d.zero() ) {
		q.terms.clear();
		div(c, d, q);
		r = c - q * d;
		r1 = c1 - q * d1;
		r2 = c2 - q * d2;
		c = d;
		c1 = d1;
		c2 = d2;
		d = r;
		d1 = r1;
		d2 = r2;
	}
	g = c; g.unit_normal();
	s = c1;
	s.divide(a.unit());
	s.divide(c.unit());
	t = c2;
	t.divide(b.unit());
	t.divide(c.unit());
}

static ex replace_lc(const ex& poly, const ex& x, const ex& lc)
{
	ex r = expand(poly + (lc - poly.lcoeff(x)) * pow(x, poly.degree(x)));
	return r;
}

static ex hensel_univar(const ex& a_, const ex& x, unsigned int p, const UniPoly& u1_, const UniPoly& w1_, const ex& gamma_ = 0)
{
	ex a = a_;
	const cl_modint_ring& R = u1_.R;

	// calc bound B
	ex maxcoeff;
	for ( int i=a.degree(x); i>=a.ldegree(x); --i ) {
		maxcoeff += pow(abs(a.coeff(x, i)),2);
	}
	cl_I normmc = ceiling1(the<cl_R>(cln::sqrt(ex_to<numeric>(maxcoeff).to_cl_N())));
	cl_I maxdegree = (u1_.degree() > w1_.degree()) ? u1_.degree() : w1_.degree();
	cl_I B = normmc * expt_pos(cl_I(2), maxdegree);

	// step 1
	ex alpha = a.lcoeff(x);
	ex gamma = gamma_;
	if ( gamma == 0 ) {
		gamma = alpha;
	}
	numeric gamma_ui = ex_to<numeric>(abs(gamma));
	a = a * gamma;
	UniPoly nu1 = u1_;
	nu1.unit_normal();
	UniPoly nw1 = w1_;
	nw1.unit_normal();
	ex phi;
	phi = expand(gamma * nu1.to_ex(x));
	UniPoly u1(R, phi, x);
	phi = expand(alpha * nw1.to_ex(x));
	UniPoly w1(R, phi, x);

	// step 2
	UniPoly s(R), t(R), g(R);
	exteuclid(u1, w1, g, s, t);

	// step 3
	ex u = replace_lc(u1.to_ex(x), x, gamma);
	ex w = replace_lc(w1.to_ex(x), x, alpha);
	ex e = expand(a - u * w);
	numeric modulus = p;
	const numeric maxmodulus = 2*numeric(B)*gamma_ui;

	// step 4
	while ( !e.is_zero() && modulus < maxmodulus ) {
		ex c = e / modulus;
		phi = expand(s.to_ex(x)*c);
		UniPoly sigmatilde(R, phi, x);
		phi = expand(t.to_ex(x)*c);
		UniPoly tautilde(R, phi, x);
		UniPoly q(R), r(R);
		div(sigmatilde, w1, q);
		rem(sigmatilde, w1, r);
		UniPoly sigma = r;
		phi = expand(tautilde.to_ex(x) + q.to_ex(x) * u1.to_ex(x));
		UniPoly tau(R, phi, x);
		u = expand(u + tau.to_ex(x) * modulus);
		w = expand(w + sigma.to_ex(x) * modulus);
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

static void split(const UniPolyVec& factors, const Partition& part, UniPoly& a, UniPoly& b)
{
	a.set(0, a.R->one());
	b.set(0, a.R->one());
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
	UniPolyVec factors;
};

static ex factor_univariate(const ex& poly, const ex& x)
{
	ex unit, cont, prim;
	poly.unitcontprim(x, unit, cont, prim);

	// determine proper prime
	unsigned int p = 3;
	cl_modint_ring R = find_modint_ring(p);
	while ( true ) {
		if ( irem(ex_to<numeric>(prim.lcoeff(x)), p) != 0 ) {
			UniPoly modpoly(R, prim, x);
			UniFactorVec sqrfree_ufv;
			squarefree(modpoly, sqrfree_ufv);
			if ( sqrfree_ufv.factors.size() == 1 && sqrfree_ufv.factors.front().exp == 1 ) break;
		}
		p = next_prime(p);
		R = find_modint_ring(p);
	}

	// do modular factorization
	UniPoly modpoly(R, prim, x);
	UniPolyVec factors;
	factor_modular(modpoly, factors);
	if ( factors.size() <= 1 ) {
		// irreducible for sure
		return poly;
	}

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
			UniPoly a(R), b(R);
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
					UniPolyVec newfactors1(part.size_first(), R), newfactors2(part.size_second(), R);
					UniPolyVec::iterator i1 = newfactors1.begin(), i2 = newfactors2.begin();
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

UniPolyVec multiterm_eea_lift(const UniPolyVec& a, const ex& x, unsigned int p, unsigned int k)
{
	DCOUT(multiterm_eea_lift);
	DCOUTVAR(a);
	DCOUTVAR(p);
	DCOUTVAR(k);

	const size_t r = a.size();
	DCOUTVAR(r);
	cl_modint_ring R = find_modint_ring(expt_pos(cl_I(p),k));
	UniPoly fill(R);
	UniPolyVec q(r-1, fill);
	q[r-2] = a[r-1];
	for ( size_t j=r-2; j>=1; --j ) {
		q[j-1] = a[j] * q[j];
	}
	DCOUTVAR(q);
	UniPoly beta(R);
	beta.set(0, R->one());
	UniPolyVec s;
	for ( size_t j=1; j<r; ++j ) {
		DCOUTVAR(j);
		DCOUTVAR(beta);
		vector<ex> mdarg(2);
		mdarg[0] = q[j-1].to_ex(x);
		mdarg[1] = a[j-1].to_ex(x);
		vector<EvalPoint> empty;
		vector<ex> exsigma = multivar_diophant(mdarg, x, beta.to_ex(x), empty, 0, p, k);
		UniPoly sigma1(R, exsigma[0], x);
		UniPoly sigma2(R, exsigma[1], x);
		beta = sigma1;
		s.push_back(sigma2);
	}
	s.push_back(beta);

	DCOUTVAR(s);
	DCOUT(END multiterm_eea_lift);
	return s;
}

void eea_lift(const UniPoly& a, const UniPoly& b, const ex& x, unsigned int p, unsigned int k, UniPoly& s_, UniPoly& t_)
{
	DCOUT(eea_lift);
	DCOUTVAR(a);
	DCOUTVAR(b);
	DCOUTVAR(x);
	DCOUTVAR(p);
	DCOUTVAR(k);

	cl_modint_ring R = find_modint_ring(p);
	UniPoly amod(R, a);
	UniPoly bmod(R, b);
	DCOUTVAR(amod);
	DCOUTVAR(bmod);

	UniPoly smod(R), tmod(R), g(R);
	exteuclid(amod, bmod, g, smod, tmod);
	
	DCOUTVAR(smod);
	DCOUTVAR(tmod);
	DCOUTVAR(g);

	cl_modint_ring Rpk = find_modint_ring(expt_pos(cl_I(p),k));
	UniPoly s(Rpk, smod);
	UniPoly t(Rpk, tmod);
	DCOUTVAR(s);
	DCOUTVAR(t);

	cl_I modulus(p);

	UniPoly one(Rpk);
	one.set(0, Rpk->one());
	for ( size_t j=1; j<k; ++j ) {
		UniPoly e = one - a * s - b * t;
		e.divide(modulus);
		UniPoly c(R, e);
		UniPoly sigmabar(R);
		sigmabar = smod * c;
		UniPoly taubar(R);
		taubar = tmod * c;
		UniPoly q(R);
		div(sigmabar, bmod, q);
		UniPoly sigma(R);
		rem(sigmabar, bmod, sigma);
		UniPoly tau(R);
		tau = taubar + q * amod;
		UniPoly sadd(Rpk, sigma);
		cl_MI modmodulus(Rpk, modulus);
		s = s + sadd * modmodulus;
		UniPoly tadd(Rpk, tau);
		t = t + tadd * modmodulus;
		modulus = modulus * p;
	}

	s_ = s; t_ = t;

	DCOUTVAR(s);
	DCOUTVAR(t);
	DCOUT2(check, a*s + b*t);
	DCOUT(END eea_lift);
}

UniPolyVec univar_diophant(const UniPolyVec& a, const ex& x, unsigned int m, unsigned int p, unsigned int k)
{
	DCOUT(univar_diophant);
	DCOUTVAR(a);
	DCOUTVAR(x);
	DCOUTVAR(m);
	DCOUTVAR(p);
	DCOUTVAR(k);

	cl_modint_ring R = find_modint_ring(expt_pos(cl_I(p),k));

	const size_t r = a.size();
	UniPolyVec result;
	if ( r > 2 ) {
		UniPolyVec s = multiterm_eea_lift(a, x, p, k);
		for ( size_t j=0; j<r; ++j ) {
			ex phi = expand(pow(x,m)*s[j].to_ex(x));
			UniPoly bmod(R, phi, x);
			UniPoly buf(R);
			rem(bmod, a[j], buf);
			result.push_back(buf);
		}
	}
	else {
		UniPoly s(R), t(R);
		eea_lift(a[1], a[0], x, p, k, s, t);
		ex phi = expand(pow(x,m)*s.to_ex(x));
		UniPoly bmod(R, phi, x);
		UniPoly buf(R);
		rem(bmod, a[0], buf);
		result.push_back(buf);
		UniPoly q(R);
		div(bmod, a[0], q);
		phi = expand(pow(x,m)*t.to_ex(x));
		UniPoly t1mod(R, phi, x);
		buf = t1mod + q * a[1];
		result.push_back(buf);
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
	return map(e);
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
		ex e = buf;
		e = make_modular(e, R);

		e = e.expand();
		DCOUTVAR(e);
		DCOUTVAR(d);
		ex monomial = 1;
		for ( size_t m=1; m<=d; ++m ) {
			DCOUTVAR(m);
			while ( !e.is_zero() && e.has(xnu) ) {
				monomial *= (xnu - alphanu);
				monomial = expand(monomial);
				DCOUTVAR(xnu);
				DCOUTVAR(alphanu);
				ex cm = e.diff(ex_to<symbol>(xnu), m).subs(xnu==alphanu) / factorial(m);
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
					e = buf.expand();
					e = make_modular(e, R);
				}
			}
		}
	}
	else {
		DCOUT(uniterm left);
		UniPolyVec amod;
		for ( size_t i=0; i<a.size(); ++i ) {
			UniPoly up(R, a[i], x);
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
			UniPolyVec delta_s = univar_diophant(amod, x, m, p, k);
			cl_MI modcm;
			cl_I poscm = cm;
			while ( poscm < 0 ) {
				poscm = poscm + expt_pos(cl_I(p),k);
			}
			modcm = cl_MI(R, poscm);
			DCOUTVAR(modcm);
			for ( size_t j=0; j<delta_s.size(); ++j ) {
				delta_s[j] = delta_s[j] * modcm;
				sigma[j] = sigma[j] + delta_s[j].to_ex(x);
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


ex hensel_multivar(const ex& a, const ex& x, const vector<EvalPoint>& I, unsigned int p, const cl_I& l, const UniPolyVec& u, const vector<ex>& lcU)
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
		U[i] = u[i].to_ex(x);
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
				coef = expand(coef);
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
						U[i] = U[i].expand();
						DCOUTVAR(U[i]);
					}
					ex Uprod = 1;
					for ( size_t i=0; i<n; ++i ) {
						Uprod *= U[i];
					}
					DCOUTVAR(Uprod.expand());
					DCOUTVAR(A[j-1]);
					e = expand(A[j-1] - Uprod);
					e = make_modular(e, R);
					DCOUTVAR(e);
				}
				else {
					break;
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
		unsigned int prime;
		size_t factor_count;
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
					UniPoly modpoly(R, u, x);
					UniFactorVec sqrfree_ufv;
					squarefree(modpoly, sqrfree_ufv);
					DCOUTVAR(sqrfree_ufv);
					if ( sqrfree_ufv.factors.size() == 1 && sqrfree_ufv.factors.front().exp == 1 ) break;
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

		UniPolyVec uvec;
		cl_modint_ring R = find_modint_ring(expt_pos(cl_I(prime),l));
		for ( size_t i=0; i<(ufaclst.nops()-1)/2; ++i ) {
			UniPoly newu(R, ufaclst.op(i*2+1), x);
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
