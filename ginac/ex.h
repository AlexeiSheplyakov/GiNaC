/** @file ex.h
 *
 *  Interface to GiNaC's light-weight expression handles. */

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

#ifndef __GINAC_EX_H__
#define __GINAC_EX_H__

#include <iostream>
#include "basic.h"
#include "operators.h"

#ifndef NO_NAMESPACE_GINAC
namespace GiNaC {
#endif // ndef NO_NAMESPACE_GINAC

class ex;
class expand_options;
class status_flags;

class symbol;
class lst;

// Sorry, this is the only constant to pollute the global scope, the other ones
// are defined in utils.h and not visible from outside.
extern const ex & _ex0(void);     //  single ex(numeric(0))

// typedef vector<ex> exvector;

#define INLINE_EX_CONSTRUCTORS

/** Lightweight wrapper for GiNaC's symbolic objects.  Basically all it does is
 *  to hold a pointer to the other objects, manage the reference counting and
 *  provide methods for manipulation of these objects. */
class ex
{
    friend class basic;

// member functions

    // default constructor, destructor, copy constructor assignment operator and helpers
public:
    ex()
#ifdef INLINE_EX_CONSTRUCTORS
    : bp(_ex0().bp)
        {
            GINAC_ASSERT(_ex0().bp!=0);
            GINAC_ASSERT(_ex0().bp->flags & status_flags::dynallocated);
            GINAC_ASSERT(bp!=0);
            ++bp->refcount;
#ifdef OBSCURE_CINT_HACK
            update_last_created_or_assigned_bp();
#endif // def OBSCURE_CINT_HACK
        }
#else
;
#endif // def INLINE_EX_CONSTRUCTORS

    ~ex()
#ifdef INLINE_EX_CONSTRUCTORS
        {
            GINAC_ASSERT(bp!=0);
            GINAC_ASSERT(bp->flags & status_flags::dynallocated);
            if (--bp->refcount == 0) {
                delete bp;
            }
        }
#else
;
#endif // def INLINE_EX_CONSTRUCTORS
        
    ex(const ex & other)
#ifdef INLINE_EX_CONSTRUCTORS
    : bp(other.bp)
        {
            GINAC_ASSERT(bp!=0);
            GINAC_ASSERT((bp->flags) & status_flags::dynallocated);
            ++bp->refcount;
#ifdef OBSCURE_CINT_HACK
            update_last_created_or_assigned_bp();
#endif // def OBSCURE_CINT_HACK
        }
#else
;
#endif // def INLINE_EX_CONSTRUCTORS
        
    const ex & operator=(const ex & other)
#ifdef INLINE_EX_CONSTRUCTORS
        {
            GINAC_ASSERT(bp!=0);
            GINAC_ASSERT(bp->flags & status_flags::dynallocated);
            GINAC_ASSERT(other.bp!=0);
            GINAC_ASSERT(other.bp->flags & status_flags::dynallocated);
            ++other.bp->refcount;
            basic * tmpbp=other.bp;
            if (--bp->refcount==0) {
                delete bp;
            }
            bp=tmpbp;
#ifdef OBSCURE_CINT_HACK
            update_last_created_or_assigned_bp();
#endif // def OBSCURE_CINT_HACK
            return *this;
        }
#else
;
#endif // def INLINE_EX_CONSTRUCTORS

#ifdef CINT_CONVERSION_WORKAROUND
    // workaround to fix the missing automatic derived->basic->ex conversion
    const ex & operator=(const basic & other)
        {
            return *this=ex(other);
        }
#endif // def CINT_CONVERSION_WORKAROUND 

    // other constructors
public:
    ex(const basic & other)
#ifdef INLINE_EX_CONSTRUCTORS
        {
            construct_from_basic(other);
#ifdef OBSCURE_CINT_HACK
            update_last_created_or_assigned_bp();
#endif // def OBSCURE_CINT_HACK
        }
#else
;
#endif // def INLINE_EX_CONSTRUCTORS
    
    ex(int i)
#ifdef INLINE_EX_CONSTRUCTORS
        {
            construct_from_int(i);
#ifdef OBSCURE_CINT_HACK
            update_last_created_or_assigned_bp();
#endif // def OBSCURE_CINT_HACK
        }
#else
;
#endif // def INLINE_EX_CONSTRUCTORS

    ex(unsigned int i)
#ifdef INLINE_EX_CONSTRUCTORS
        {
            construct_from_uint(i);
#ifdef OBSCURE_CINT_HACK
            update_last_created_or_assigned_bp();
#endif // def OBSCURE_CINT_HACK
        }
#else
;
#endif // def INLINE_EX_CONSTRUCTORS
    
    ex(long i)
#ifdef INLINE_EX_CONSTRUCTORS
        {
            construct_from_long(i);
#ifdef OBSCURE_CINT_HACK
            update_last_created_or_assigned_bp();
#endif // def OBSCURE_CINT_HACK
        }
#else
;
#endif // def INLINE_EX_CONSTRUCTORS

    ex(unsigned long i)
#ifdef INLINE_EX_CONSTRUCTORS
        {
            construct_from_ulong(i);
#ifdef OBSCURE_CINT_HACK
            update_last_created_or_assigned_bp();
#endif // def OBSCURE_CINT_HACK
        }
#else
;
#endif // def INLINE_EX_CONSTRUCTORS
    
    ex(double const d)
#ifdef INLINE_EX_CONSTRUCTORS
        {
            construct_from_double(d);
#ifdef OBSCURE_CINT_HACK
            update_last_created_or_assigned_bp();
#endif // def OBSCURE_CINT_HACK
        }
#else
;
#endif // def INLINE_EX_CONSTRUCTORS

    
    // functions overriding virtual functions from bases classes
    // none
    
    // new virtual functions which can be overridden by derived classes
    // none

    // non-virtual functions in this class
public:
    void swap(ex & other);
    void printraw(ostream & os) const;
    void printtree(ostream & os, unsigned indent=0) const;
    void print(ostream & os, unsigned upper_precedence=0) const;
    void printcsrc(ostream & os, unsigned type, const char *var_name) const;
    void dbgprint(void) const;
    void dbgprinttree(void) const;
    bool info(unsigned inf) const;
    unsigned nops() const;
    ex expand(unsigned options=0) const;
    bool has(const ex & other) const;
#ifdef CINT_CONVERSION_WORKAROUND
    bool has(basic const & other) const { return has(ex(other)); }
#endif // def CINT_CONVERSION_WORKAROUND
    int degree(const symbol & s) const;
    int ldegree(const symbol & s) const;
    ex coeff(const symbol & s, int n=1) const;
    ex lcoeff(const symbol & s) const { return coeff(s, degree(s)); }
    ex tcoeff(const symbol & s) const { return coeff(s, ldegree(s)); }
    ex numer(void) const;
    ex denom(void) const;
    ex unit(const symbol &x) const;
    ex content(const symbol &x) const;
    numeric integer_content(void) const;
    ex primpart(const symbol &x) const;
    ex primpart(const symbol &x, const ex &cont) const;
#ifdef CINT_CONVERSION_WORKAROUND
    ex primpart(const symbol &x, const basic &cont) const { return primpart(x,ex(cont)); }
#endif // def CINT_CONVERSION_WORKAROUND
    ex normal(int level = 0) const;
    ex smod(const numeric &xi) const;
    numeric max_coefficient(void) const;
    ex collect(const symbol & s) const;
    ex eval(int level = 0) const;
    ex evalf(int level = 0) const;
    ex diff(const symbol & s, unsigned nth = 1) const;
    ex series(const ex & r, int order) const;
#ifdef CINT_CONVERSION_WORKAROUND
    ex series(const basic & r, int order) const { return series(ex(r),order); }
#endif // def CINT_CONVERSION_WORKAROUND
    ex subs(const lst & ls, const lst & lr) const;
    ex subs(const ex & e) const;
#ifdef CINT_CONVERSION_WORKAROUND
    ex subs(const basic & e) const { return subs(ex(e)); }
#endif // def CINT_CONVERSION_WORKAROUND
    exvector get_indices(void) const;
    ex simplify_ncmul(const exvector & v) const;
    ex operator[](const ex & index) const;
#ifdef CINT_CONVERSION_WORKAROUND
    ex operator[](const basic & index) const { return operator[](ex(index)); }
#endif // def CINT_CONVERSION_WORKAROUND
    ex operator[](int i) const;
    ex op(int i) const;
    ex & let_op(int i);
    ex lhs(void) const;
    ex rhs(void) const;
    int compare(const ex & other) const
#ifdef INLINE_EX_CONSTRUCTORS
        {
            GINAC_ASSERT(bp!=0);
            GINAC_ASSERT(other.bp!=0);
            if (bp==other.bp) {
                // special case: both expression point to same basic, trivially equal
                return 0; 
            }
            return bp->compare(*other.bp);
        }
#else
;
#endif // def INLINE_EX_CONSTRUCTORS
#ifdef CINT_CONVERSION_WORKAROUND
    int compare(const basic & other) const { return compare(ex(other)); }
#endif // def CINT_CONVERSION_WORKAROUND
    bool is_equal(const ex & other) const
#ifdef INLINE_EX_CONSTRUCTORS
        {
            GINAC_ASSERT(bp!=0);
            GINAC_ASSERT(other.bp!=0);
            if (bp==other.bp) {
                // special case: both expression point to same basic, trivially equal
                return true; 
            }
            return bp->is_equal(*other.bp);
        }
#else
;
#endif // def INLINE_EX_CONSTRUCTORS
#ifdef CINT_CONVERSION_WORKAROUND
    bool is_equal(const basic & other) const { return is_equal(ex(other)); }
#endif // def CINT_CONVERSION_WORKAROUND
    bool is_zero(void) const {return compare(_ex0())==0;};
        
    unsigned return_type(void) const;
    unsigned return_type_tinfo(void) const;
    unsigned gethash(void) const;

    ex exadd(const ex & rh) const;
#ifdef CINT_CONVERSION_WORKAROUND
    ex exadd(const basic & rh) const { return exadd(ex(rh)); }
#endif // def CINT_CONVERSION_WORKAROUND
    ex exmul(const ex & rh) const;
#ifdef CINT_CONVERSION_WORKAROUND
    ex exmul(const basic & rh) const { return exmul(ex(rh)); }
#endif // def CINT_CONVERSION_WORKAROUND
    ex exncmul(const ex & rh) const;
#ifdef CINT_CONVERSION_WORKAROUND
    ex exncmul(const basic & rh) const { return exncmul(ex(rh)); }
#endif // def CINT_CONVERSION_WORKAROUND
private:
    void construct_from_basic(const basic & other);
    void construct_from_int(int i);
    void construct_from_uint(unsigned int i);
    void construct_from_long(long i);
    void construct_from_ulong(unsigned long i);
    void construct_from_double(double d);
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
            last_created_or_assigned_bp=bp;
            ++last_created_or_assigned_bp->refcount;
            last_created_or_assigned_exp=(long)(void *)(this);
        }
#endif // def OBSCURE_CINT_HACK

// member variables

public:
    basic *bp;
#ifdef OBSCURE_CINT_HACK
    static basic * last_created_or_assigned_bp;
    static basic * dummy_bp;
    static long last_created_or_assigned_exp;
#endif // def OBSCURE_CINT_HACK
};

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

inline bool has(const ex & thisex, const ex & other)
{ return thisex.has(other); }

inline int degree(const ex & thisex, const symbol & s)
{ return thisex.degree(s); }

inline int ldegree(const ex & thisex, const symbol & s)
{ return thisex.ldegree(s); }

inline ex coeff(const ex & thisex, const symbol & s, int n=1)
{ return thisex.coeff(s, n); }

inline ex numer(const ex & thisex)
{ return thisex.numer(); }

inline ex denom(const ex & thisex)
{ return thisex.denom(); }

inline ex normal(const ex & thisex, int level=0)
{ return thisex.normal(level); }

inline ex collect(const ex & thisex, const symbol & s)
{ return thisex.collect(s); }

inline ex eval(const ex & thisex, int level = 0)
{ return thisex.eval(level); }

inline ex evalf(const ex & thisex, int level = 0)
{ return thisex.evalf(level); }

inline ex diff(const ex & thisex, const symbol & s, unsigned nth = 1)
{ return thisex.diff(s, nth); }

inline ex series(const ex & thisex, const ex & r, int order)
{ return thisex.series(r, order); }

inline ex subs(const ex & thisex, const ex & e)
{ return thisex.subs(e); }

inline ex subs(const ex & thisex, const lst & ls, const lst & lr)
{ return thisex.subs(ls, lr); }

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

#ifndef NO_NAMESPACE_GINAC
} // namespace GiNaC
#endif // ndef NO_NAMESPACE_GINAC

#endif // ndef __GINAC_EX_H__

