/** @file ex.h
 *
 *  Interface to GiNaC's light-weight expression handles. */

/*
 *  GiNaC Copyright (C) 1999-2001 Johannes Gutenberg University Mainz, Germany
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

#ifndef __GINAC_EX_H__
#define __GINAC_EX_H__

#include "basic.h"
#include "operators.h"

#include <functional>

namespace GiNaC {

// Sorry, this is the only constant to pollute the global scope, the other ones
// are defined in utils.h and not visible from outside.
class ex;
extern const ex & _ex0(void);     ///<  single ex(numeric(0))

class symbol;
class lst;
class scalar_products;


/** Lightweight wrapper for GiNaC's symbolic objects.  Basically all it does is
 *  to hold a pointer to the other objects, manage the reference counting and
 *  provide methods for manipulation of these objects.  (Some people call such
 *  a thing a proxy class.) */
class ex
{
	friend class basic;
	
// member functions
	
	// default ctor, dtor, copy ctor assignment operator and helpers
public:
	ex();
	~ex();
	ex(const ex & other);
	ex & operator=(const ex & other);
	// other ctors
public:
	ex(const basic & other);
	ex(int i);
	ex(unsigned int i);
	ex(long i);
	ex(unsigned long i);
	ex(double const d);
	/** Construct ex from string and a list of symbols. The input grammar is
	 *  similar to the GiNaC output format. All symbols to be used in the
	 *  expression must be specified in a lst in the second argument. Undefined
	 *  symbols and other parser errors will throw an exception. */
	ex(const std::string &s, const ex &l);
	
	// non-virtual functions in this class
public:
	void swap(ex & other);
	void print(const print_context & c, unsigned level = 0) const;
	void printtree(std::ostream & os) const;
	void dbgprint(void) const;
	void dbgprinttree(void) const;
	bool info(unsigned inf) const { return bp->info(inf); }
	unsigned nops() const { return bp->nops(); }
	ex expand(unsigned options=0) const;
	bool has(const ex & pattern) const { return bp->has(pattern); }
	ex map(map_function & f) const { return bp->map(f); }
	ex map(ex (*f)(const ex & e)) const;
	bool find(const ex & pattern, lst & found) const;
	int degree(const ex & s) const { return bp->degree(s); }
	int ldegree(const ex & s) const { return bp->ldegree(s); }
	ex coeff(const ex & s, int n = 1) const { return bp->coeff(s, n); }
	ex lcoeff(const ex & s) const { return coeff(s, degree(s)); }
	ex tcoeff(const ex & s) const { return coeff(s, ldegree(s)); }
	ex numer(void) const;
	ex denom(void) const;
	ex numer_denom(void) const;
	ex unit(const symbol &x) const;
	ex content(const symbol &x) const;
	numeric integer_content(void) const;
	ex primpart(const symbol &x) const;
	ex primpart(const symbol &x, const ex &cont) const;
	ex normal(int level = 0) const;
	ex to_rational(lst &repl_lst) const { return bp->to_rational(repl_lst); }
	ex smod(const numeric &xi) const { return bp->smod(xi); }
	numeric max_coefficient(void) const;
	ex collect(const ex & s, bool distributed = false) const { return bp->collect(s, distributed); }
	ex eval(int level = 0) const { return bp->eval(level); }
	ex evalf(int level = 0) const { return bp->evalf(level); }
	ex evalm(void) const { return bp->evalm(); }
	ex diff(const symbol & s, unsigned nth = 1) const;
	ex series(const ex & r, int order, unsigned options = 0) const;
	bool match(const ex & pattern) const;
	bool match(const ex & pattern, lst & repl_lst) const { return bp->match(pattern, repl_lst); }
	ex subs(const lst & ls, const lst & lr, bool no_pattern = false) const { return bp->subs(ls, lr, no_pattern); }
	ex subs(const ex & e, bool no_pattern = false) const { return bp->subs(e, no_pattern); }
	exvector get_free_indices(void) const { return bp->get_free_indices(); }
	ex simplify_indexed(void) const;
	ex simplify_indexed(const scalar_products & sp) const;
	ex symmetrize(void) const;
	ex symmetrize(const lst & l) const;
	ex antisymmetrize(void) const;
	ex antisymmetrize(const lst & l) const;
	ex symmetrize_cyclic(void) const;
	ex symmetrize_cyclic(const lst & l) const;
	ex simplify_ncmul(const exvector & v) const { return bp->simplify_ncmul(v); }
	ex operator[](const ex & index) const;
	ex operator[](int i) const;
	ex op(int i) const { return bp->op(i); }
	ex & let_op(int i);
	ex lhs(void) const;
	ex rhs(void) const;
	int compare(const ex & other) const;
	bool is_equal(const ex & other) const;
	bool is_zero(void) const { return is_equal(_ex0()); }
	
	unsigned return_type(void) const { return bp->return_type(); }
	unsigned return_type_tinfo(void) const { return bp->return_type_tinfo(); }
	unsigned gethash(void) const { return bp->gethash(); }
private:
	void construct_from_basic(const basic & other);
	void construct_from_int(int i);
	void construct_from_uint(unsigned int i);
	void construct_from_long(long i);
	void construct_from_ulong(unsigned long i);
	void construct_from_double(double d);
	void construct_from_string_and_lst(const std::string &s, const ex &l);
	void makewriteable();

#ifdef OBSCURE_CINT_HACK
public:
	static bool last_created_or_assigned_bp_can_be_converted_to_ex(void)
	{
		if (last_created_or_assigned_bp==0) return false;
		if ((last_created_or_assigned_bp->flags &
			 status_flags::dynallocated)==0) return false;
		if ((last_created_or_assigned_bp->flags &
			 status_flags::evaluated)==0) return false;
		return true;
	}
protected:
	void update_last_created_or_assigned_bp(void)
	{
		if (last_created_or_assigned_bp!=0) {
			if (--last_created_or_assigned_bp->refcount == 0) {
				delete last_created_or_assigned_bp;
			}
		}
		last_created_or_assigned_bp = bp;
		++last_created_or_assigned_bp->refcount;
		last_created_or_assigned_exp = (long)(void *)(this);
	}
#endif // def OBSCURE_CINT_HACK

// member variables

public:
	basic *bp;      ///< pointer to basic object managed by this
#ifdef OBSCURE_CINT_HACK
	static basic * last_created_or_assigned_bp;
	static basic * dummy_bp;
	static long last_created_or_assigned_exp;
#endif // def OBSCURE_CINT_HACK
};


// performance-critical inlined method implementations

inline
ex::ex() : bp(_ex0().bp)
{
	/*debugmsg("ex default ctor",LOGLEVEL_CONSTRUCT);*/
	GINAC_ASSERT(_ex0().bp!=0);
	GINAC_ASSERT(_ex0().bp->flags & status_flags::dynallocated);
	GINAC_ASSERT(bp!=0);
	++bp->refcount;
#ifdef OBSCURE_CINT_HACK
	update_last_created_or_assigned_bp();
#endif // def OBSCURE_CINT_HACK
}

inline
ex::~ex()
{
	/*debugmsg("ex dtor",LOGLEVEL_DESTRUCT);*/
	GINAC_ASSERT(bp!=0);
	GINAC_ASSERT(bp->flags & status_flags::dynallocated);
	if (--bp->refcount == 0)
		delete bp;
}

inline
ex::ex(const ex & other) : bp(other.bp)
{
	/*debugmsg("ex copy ctor",LOGLEVEL_CONSTRUCT);*/
	GINAC_ASSERT(bp!=0);
	GINAC_ASSERT((bp->flags) & status_flags::dynallocated);
	++bp->refcount;
#ifdef OBSCURE_CINT_HACK
	update_last_created_or_assigned_bp();
#endif // def OBSCURE_CINT_HACK
}

inline
ex & ex::operator=(const ex & other)
{
	/*debugmsg("ex operator=",LOGLEVEL_ASSIGNMENT);*/
	GINAC_ASSERT(bp!=0);
	GINAC_ASSERT(bp->flags & status_flags::dynallocated);
	GINAC_ASSERT(other.bp!=0);
	GINAC_ASSERT(other.bp->flags & status_flags::dynallocated);
	// NB: must first increment other.bp->refcount, since other might be *this.
	++other.bp->refcount;
	if (--bp->refcount==0)
		delete bp;
	bp = other.bp;
#ifdef OBSCURE_CINT_HACK
	update_last_created_or_assigned_bp();
#endif // def OBSCURE_CINT_HACK
	return *this;
}

inline
ex::ex(const basic & other)
{
	/*debugmsg("ex ctor from basic",LOGLEVEL_CONSTRUCT);*/
	construct_from_basic(other);
#ifdef OBSCURE_CINT_HACK
	update_last_created_or_assigned_bp();
#endif // def OBSCURE_CINT_HACK
}

inline
ex::ex(int i)
{
	/*debugmsg("ex ctor from int",LOGLEVEL_CONSTRUCT);*/
	construct_from_int(i);
#ifdef OBSCURE_CINT_HACK
	update_last_created_or_assigned_bp();
#endif // def OBSCURE_CINT_HACK
}

inline
ex::ex(unsigned int i)
{
	/*debugmsg("ex ctor from unsigned int",LOGLEVEL_CONSTRUCT);*/
	construct_from_uint(i);
#ifdef OBSCURE_CINT_HACK
	update_last_created_or_assigned_bp();
#endif // def OBSCURE_CINT_HACK
}

inline
ex::ex(long i)
{
	/*debugmsg("ex ctor from long",LOGLEVEL_CONSTRUCT);*/
	construct_from_long(i);
#ifdef OBSCURE_CINT_HACK
	update_last_created_or_assigned_bp();
#endif // def OBSCURE_CINT_HACK
}

inline
ex::ex(unsigned long i)
{
	/*debugmsg("ex ctor from unsigned long",LOGLEVEL_CONSTRUCT);*/
	construct_from_ulong(i);
#ifdef OBSCURE_CINT_HACK
	update_last_created_or_assigned_bp();
#endif // def OBSCURE_CINT_HACK
}

inline
ex::ex(double const d)
{
	/*debugmsg("ex ctor from double",LOGLEVEL_CONSTRUCT);*/
	construct_from_double(d);
#ifdef OBSCURE_CINT_HACK
	update_last_created_or_assigned_bp();
#endif // def OBSCURE_CINT_HACK
}

inline
ex::ex(const std::string &s, const ex &l)
{
	/*debugmsg("ex ctor from string,lst",LOGLEVEL_CONSTRUCT);*/
	construct_from_string_and_lst(s, l);
#ifdef OBSCURE_CINT_HACK
	update_last_created_or_assigned_bp();
#endif // def OBSCURE_CINT_HACK
}

inline
int ex::compare(const ex & other) const
{
	GINAC_ASSERT(bp!=0);
	GINAC_ASSERT(other.bp!=0);
	if (bp==other.bp)  // trivial case: both expressions point to same basic
		return 0;
	return bp->compare(*other.bp);
}

inline
bool ex::is_equal(const ex & other) const
{
	GINAC_ASSERT(bp!=0);
	GINAC_ASSERT(other.bp!=0);
	if (bp==other.bp)  // trivial case: both expressions point to same basic
		return true;
	return bp->is_equal(*other.bp);
}


// utility functions
inline bool are_ex_trivially_equal(const ex &e1, const ex &e2)
{
	return e1.bp == e2.bp;
}

// wrapper functions around member functions
inline unsigned nops(const ex & thisex)
{ return thisex.nops(); }

inline ex expand(const ex & thisex, unsigned options = 0)
{ return thisex.expand(options); }

inline bool has(const ex & thisex, const ex & pattern)
{ return thisex.has(pattern); }

inline bool find(const ex & thisex, const ex & pattern, lst & found)
{ return thisex.find(pattern, found); }

inline int degree(const ex & thisex, const ex & s)
{ return thisex.degree(s); }

inline int ldegree(const ex & thisex, const ex & s)
{ return thisex.ldegree(s); }

inline ex coeff(const ex & thisex, const ex & s, int n=1)
{ return thisex.coeff(s, n); }

inline ex numer(const ex & thisex)
{ return thisex.numer(); }

inline ex denom(const ex & thisex)
{ return thisex.denom(); }

inline ex numer_denom(const ex & thisex)
{ return thisex.numer_denom(); }

inline ex normal(const ex & thisex, int level=0)
{ return thisex.normal(level); }

inline ex to_rational(const ex & thisex, lst & repl_lst)
{ return thisex.to_rational(repl_lst); }

inline ex collect(const ex & thisex, const ex & s, bool distributed = false)
{ return thisex.collect(s, distributed); }

inline ex eval(const ex & thisex, int level = 0)
{ return thisex.eval(level); }

inline ex evalf(const ex & thisex, int level = 0)
{ return thisex.evalf(level); }

inline ex evalm(const ex & thisex)
{ return thisex.evalm(); }

inline ex diff(const ex & thisex, const symbol & s, unsigned nth = 1)
{ return thisex.diff(s, nth); }

inline ex series(const ex & thisex, const ex & r, int order, unsigned options = 0)
{ return thisex.series(r, order, options); }

inline bool match(const ex & thisex, const ex & pattern, lst & repl_lst)
{ return thisex.match(pattern, repl_lst); }

inline ex subs(const ex & thisex, const ex & e)
{ return thisex.subs(e); }

inline ex subs(const ex & thisex, const lst & ls, const lst & lr)
{ return thisex.subs(ls, lr); }

inline ex simplify_indexed(const ex & thisex)
{ return thisex.simplify_indexed(); }

inline ex simplify_indexed(const ex & thisex, const scalar_products & sp)
{ return thisex.simplify_indexed(sp); }

inline ex symmetrize(const ex & thisex)
{ return thisex.symmetrize(); }

inline ex symmetrize(const ex & thisex, const lst & l)
{ return thisex.symmetrize(l); }

inline ex antisymmetrize(const ex & thisex)
{ return thisex.antisymmetrize(); }

inline ex antisymmetrize(const ex & thisex, const lst & l)
{ return thisex.antisymmetrize(l); }

inline ex symmetrize_cyclic(const ex & thisex)
{ return thisex.symmetrize_cyclic(); }

inline ex symmetrize_cyclic(const ex & thisex, const lst & l)
{ return thisex.symmetrize_cyclic(l); }

inline ex op(const ex & thisex, int i)
{ return thisex.op(i); }

inline ex lhs(const ex & thisex)
{ return thisex.lhs(); }

inline ex rhs(const ex & thisex)
{ return thisex.rhs(); }

inline bool is_zero(const ex & thisex)
{ return thisex.is_zero(); }

inline void swap(ex & e1, ex & e2)
{ e1.swap(e2); }


/* Function objects for STL sort() etc. */
struct ex_is_less : public std::binary_function<ex, ex, bool> {
	bool operator() (const ex &lh, const ex &rh) const { return lh.compare(rh) < 0; }
};

struct ex_is_equal : public std::binary_function<ex, ex, bool> {
	bool operator() (const ex &lh, const ex &rh) const { return lh.is_equal(rh); }
};

struct ex_swap : public std::binary_function<ex, ex, void> {
	void operator() (ex &lh, ex &rh) const { lh.swap(rh); }
};


/* Convert function pointer to function object suitable for map(). */
class pointer_to_map_function : public map_function {
protected:
	ex (*ptr)(const ex &);
public:
	explicit pointer_to_map_function(ex (*x)(const ex &)) : ptr(x) {}
	ex operator()(const ex & e) { return ptr(e); }
};

template<class T1>
class pointer_to_map_function_1arg : public map_function {
protected:
	ex (*ptr)(const ex &, T1);
	T1 arg1;
public:
	explicit pointer_to_map_function_1arg(ex (*x)(const ex &, T1), T1 a1) : ptr(x), arg1(a1) {}
	ex operator()(const ex & e) { return ptr(e, arg1); }
};

template<class T1, class T2>
class pointer_to_map_function_2args : public map_function {
protected:
	ex (*ptr)(const ex &, T1, T2);
	T1 arg1;
	T2 arg2;
public:
	explicit pointer_to_map_function_2args(ex (*x)(const ex &, T1, T2), T1 a1, T2 a2) : ptr(x), arg1(a1), arg2(a2) {}
	ex operator()(const ex & e) { return ptr(e, arg1, arg2); }
};

template<class T1, class T2, class T3>
class pointer_to_map_function_3args : public map_function {
protected:
	ex (*ptr)(const ex &, T1, T2, T3);
	T1 arg1;
	T2 arg2;
	T3 arg3;
public:
	explicit pointer_to_map_function_3args(ex (*x)(const ex &, T1, T2, T3), T1 a1, T2 a2, T3 a3) : ptr(x), arg1(a1), arg2(a2), arg3(a3) {}
	ex operator()(const ex & e) { return ptr(e, arg1, arg2, arg3); }
};

inline ex ex::map(ex (*f)(const ex & e)) const
{
	pointer_to_map_function fcn(f);
	return bp->map(fcn);
}


} // namespace GiNaC

#endif // ndef __GINAC_EX_H__
