/** @file basic.h
 *
 *  Interface to GiNaC's ABC. */

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

#ifndef __GINAC_BASIC_H__
#define __GINAC_BASIC_H__

#include <iostream>
#include <typeinfo>
#include <vector>

// CINT needs <algorithm> to work properly with <vector> 
#include <algorithm>

#include <ginac/flags.h>
#include <ginac/tinfos.h>
#include <ginac/assertion.h>
#include <ginac/registrar.h>

#ifndef NO_GINAC_NAMESPACE
namespace GiNaC {
#endif // ndef NO_GINAC_NAMESPACE

class basic;
class ex;
class symbol;
class lst;
class numeric;
class archive_node;

//typedef vector<ex> exvector;
typedef vector<ex,malloc_alloc> exvector; // CINT does not like vector<...,default_alloc>

#define INLINE_BASIC_CONSTRUCTORS

/** This class is the ABC (abstract base class) of GiNaC's class hierarchy.
 *  It is responsible for the reference counting. */
class basic
{
    GINAC_DECLARE_REGISTERED_CLASS(basic, void)

    friend class ex;

// member functions

    // default constructor, destructor, copy constructor assignment operator and helpers
public:
    basic()
#ifdef INLINE_BASIC_CONSTRUCTORS
    : tinfo_key(TINFO_basic), flags(0), refcount(0)
    {
    }
#else
;
#endif // def INLINE_BASIC_CONSTRUCTORS

    virtual ~basic()
#ifdef INLINE_BASIC_CONSTRUCTORS
    {
        destroy(0);
        GINAC_ASSERT((!(flags & status_flags::dynallocated))||(refcount==0));
    }
#else
;
#endif // def INLINE_BASIC_CONSTRUCTORS

    basic(basic const & other)
#ifdef INLINE_BASIC_CONSTRUCTORS
    {
        copy(other);
    }
#else
;
#endif // def INLINE_BASIC_CONSTRUCTORS

    virtual basic const & operator=(basic const & other);
    
protected:
    void copy(basic const & other)
    {
        flags = other.flags & ~status_flags::dynallocated;
        hashvalue = other.hashvalue;
        tinfo_key = other.tinfo_key;
    }
    void destroy(bool call_parent) {}

    // other constructors
    basic(unsigned ti)
#ifdef INLINE_BASIC_CONSTRUCTORS
    : tinfo_key(ti), flags(0), refcount(0)
    {
    }
#else
;
#endif // def INLINE_BASIC_CONSTRUCTORS

    // functions overriding virtual functions from bases classes
    // none
    
    // new virtual functions which can be overridden by derived classes
public: // only const functions please (may break reference counting)
    virtual basic * duplicate() const;
    virtual void print(ostream & os,unsigned upper_precedence=0) const;
    virtual void printraw(ostream & os) const;
    virtual void printtree(ostream & os, unsigned indent) const;
    virtual void printcsrc(ostream & os, unsigned type, unsigned upper_precedence=0) const;
    virtual void dbgprint(void) const;
    virtual void dbgprinttree(void) const;
    virtual bool info(unsigned inf) const;
    virtual unsigned nops() const;
    virtual ex op(int const i) const;
    virtual ex & let_op(int const i);
    virtual ex operator[](ex const & index) const;
    virtual ex operator[](int const i) const;
    virtual bool has(ex const & other) const;
    virtual int degree(symbol const & s) const;
    virtual int ldegree(symbol const & s) const;
    virtual ex coeff(symbol const & s, int const n=1) const;
    virtual ex collect(symbol const & s) const;
    virtual ex eval(int level=0) const;
    virtual ex evalf(int level=0) const;
    virtual ex diff(symbol const & s) const;
    virtual ex series(symbol const & s, ex const & point, int order) const;
    virtual ex subs(lst const & ls, lst const & lr) const;
    virtual ex normal(lst &sym_lst, lst &repl_lst, int level=0) const;
    virtual numeric integer_content(void) const;
    virtual ex smod(const numeric &xi) const;
    virtual numeric max_coefficient(void) const;
    virtual exvector get_indices(void) const;
    virtual ex simplify_ncmul(exvector const & v) const;
protected: // non-const functions should be called from class ex only
    virtual int compare_same_type(basic const & other) const;
    virtual bool is_equal_same_type(basic const & other) const;
    virtual unsigned return_type(void) const;
    virtual unsigned return_type_tinfo(void) const;
    virtual unsigned calchash(void) const;
    virtual ex expand(unsigned options=0) const;

    // non-virtual functions in this class
public:
    ex subs(ex const & e) const;
    int compare(basic const & other) const;
    bool is_equal(basic const & other) const;
    basic const & hold(void) const;
    unsigned gethash(void) const {if (flags & status_flags::hash_calculated) return hashvalue; else return calchash();}
    unsigned tinfo(void) const {return tinfo_key;}
protected:
    basic const & setflag(unsigned f) const {flags |= f; return *this;}
    basic const & clearflag(unsigned f) const {flags &= ~f; return *this;}
    void ensure_if_modifiable(void) const;

// member variables
    
protected:
    unsigned tinfo_key;
    mutable unsigned flags;
    mutable unsigned hashvalue;
    static unsigned precedence;
    static unsigned delta_indent;
private:
    unsigned refcount;
};

// global constants

extern const basic some_basic;
extern type_info const & typeid_basic;

// global variables

extern int max_recursion_level;

// convenience macros

#ifndef NO_GINAC_NAMESPACE

#define is_of_type(OBJ,TYPE) \
    (dynamic_cast<TYPE *>(const_cast<GiNaC::basic *>(&OBJ))!=0)

#define is_exactly_of_type(OBJ,TYPE) \
    ((OBJ).tinfo()==GiNaC::TINFO_##TYPE)

#define is_ex_of_type(OBJ,TYPE) \
    (dynamic_cast<TYPE *>(const_cast<GiNaC::basic *>((OBJ).bp))!=0)

#define is_ex_exactly_of_type(OBJ,TYPE) \
    ((*(OBJ).bp).tinfo()==GiNaC::TINFO_##TYPE)

#else // ndef NO_GINAC_NAMESPACE

#define is_of_type(OBJ,TYPE) \
    (dynamic_cast<TYPE *>(const_cast<basic *>(&OBJ))!=0)

#define is_exactly_of_type(OBJ,TYPE) \
    ((OBJ).tinfo()==TINFO_##TYPE)

#define is_ex_of_type(OBJ,TYPE) \
    (dynamic_cast<TYPE *>(const_cast<basic *>((OBJ).bp))!=0)

#define is_ex_exactly_of_type(OBJ,TYPE) \
    ((*(OBJ).bp).tinfo()==TINFO_##TYPE)

#endif // ndef NO_GINAC_NAMESPACE

#ifndef NO_GINAC_NAMESPACE
} // namespace GiNaC
#endif // ndef NO_GINAC_NAMESPACE

#endif // ndef __GINAC_BASIC_H__
