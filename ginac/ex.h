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
#include <ginac/basic.h>
#include <ginac/operators.h>

#ifndef NO_GINAC_NAMESPACE
namespace GiNaC {
#endif // ndef NO_GINAC_NAMESPACE

class ex;
class expand_options;
class status_flags;

class symbol;
class lst;

extern const ex & _ex0(void);  /* FIXME: should this pollute headers? */

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
    
    ex(int i);
    ex(unsigned int i);
    ex(long i);
    ex(unsigned long i);
    ex(double const d);

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
    int degree(const symbol & s) const;
    int ldegree(const symbol & s) const;
    ex coeff(const symbol & s, int n=1) const;
    ex lcoeff(const symbol & s) const { return coeff(s, degree(s)); }
    ex tcoeff(const symbol & s) const { return coeff(s, ldegree(s)); }
    ex numer(bool normalize = true) const;
    ex denom(bool normalize = true) const;
    ex unit(const symbol &x) const;
    ex content(const symbol &x) const;
    numeric integer_content(void) const;
    ex primpart(const symbol &x) const;
    ex primpart(const symbol &x, const ex &cont) const;
    ex normal(int level = 0) const;
    ex smod(const numeric &xi) const;
    numeric max_coefficient(void) const;
    ex collect(const symbol & s) const;
    ex eval(int level = 0) const;
    ex evalf(int level = 0) const;
    ex diff(const symbol & s, unsigned nth = 1) const;
    ex series(const symbol & s, const ex & point, int order = 6) const;
    ex subs(const lst & ls, const lst & lr) const;
    ex subs(const ex & e) const;
    exvector get_indices(void) const;
    ex simplify_ncmul(const exvector & v) const;
    ex operator[](const ex & index) const;
    ex operator[](int i) const;
    ex op(int i) const;
    ex & let_op(int i);
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
    bool is_zero(void) const {return compare(_ex0())==0;};
        
    unsigned return_type(void) const;
    unsigned return_type_tinfo(void) const;
    unsigned gethash(void) const;

    ex exadd(const ex & rh) const;
    ex exmul(const ex & rh) const;
    ex exncmul(const ex & rh) const;
private:
    void construct_from_basic(const basic & other);
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
        }
#endif // def OBSCURE_CINT_HACK

// member variables

public:
    basic *bp;
#ifdef OBSCURE_CINT_HACK
    static basic *last_created_or_assigned_bp;
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

inline ex numer(const ex & thisex, bool normalize = true)
{ return thisex.numer(normalize); }

inline ex denom(const ex & thisex, bool normalize = true)
{ return thisex.denom(normalize); }

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

inline ex series(const ex & thisex, const symbol & s, const ex & point, int order = 6)
{ return thisex.series(s, point, order); }

inline ex subs(const ex & thisex, const ex & e)
{ return thisex.subs(e); }

inline ex subs(const ex & thisex, const lst & ls, const lst & lr)
{ return thisex.subs(ls, lr); }

inline void swap(ex & e1, ex & e2)
{ e1.swap(e2); }

#ifndef NO_GINAC_NAMESPACE
} // namespace GiNaC
#endif // ndef NO_GINAC_NAMESPACE

#endif // ndef __GINAC_EX_H__

