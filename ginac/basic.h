/** @file basic.h
 *
 *  Interface to GiNaC's ABC. */

#ifndef _BASIC_H_
#define _BASIC_H_

#include <iostream>
#include <typeinfo>
#include <vector>

#include "flags.h"
#include "tinfos.h"
#include "debugmsg.h"

class basic;
class ex;
class symbol;
class lst;
class numeric;

typedef vector<ex> exvector;

#define INLINE_BASIC_CONSTRUCTORS

/** This class is the ABC (abstract base class) of GiNaC's class hierarchy.
 *  It is responsible for the reference counting. */
class basic
{
    friend class ex;

// member functions

    // default constructor, destructor, copy constructor assignment operator and helpers
public:
    basic()
#ifdef INLINE_BASIC_CONSTRUCTORS
    : tinfo_key(TINFO_BASIC), flags(0), refcount(0)
    {
        debugmsg("basic default constructor",LOGLEVEL_CONSTRUCT);
        // nothing to do
    }
#else
;
#endif // def INLINE_BASIC_CONSTRUCTORS

    virtual ~basic()
#ifdef INLINE_BASIC_CONSTRUCTORS
    {
        debugmsg("basic destructor",LOGLEVEL_DESTRUCT);
        destroy(0);
        ASSERT((!(flags & status_flags::dynallocated))||(refcount==0));
    }
#else
;
#endif // def INLINE_BASIC_CONSTRUCTORS

    basic(basic const & other)
#ifdef INLINE_BASIC_CONSTRUCTORS
    {
        debugmsg("basic copy constructor",LOGLEVEL_CONSTRUCT);
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
        debugmsg("basic constructor with tinfo_key",LOGLEVEL_CONSTRUCT);
        // nothing to do
    }
#else
;
#endif // def INLINE_BASIC_CONSTRUCTORS

    // functions overriding virtual functions from bases classes
    // none
    
    // new virtual functions which can be overridden by derived classes
public: // only const functions please (may break reference counting)
    virtual basic * duplicate() const;
    virtual void printraw(ostream & os) const;
    virtual void printtree(ostream & os, unsigned indent) const;
    virtual void print(ostream & os,unsigned upper_precedence=0) const;
    virtual void printcsrc(ostream & os, unsigned type, unsigned upper_precedence=0) const;
    virtual void dbgprint(void) const;
    virtual void dbgprinttree(void) const;
    virtual bool info(unsigned inf) const;
    virtual int nops() const;
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
    virtual ex smod(numeric const &xi) const;
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

/*
#ifndef _DEBUG
*/
#define is_of_type(OBJ,TYPE) \
    (dynamic_cast<TYPE *>(const_cast<basic *>(&OBJ))!=0)

/*
#define is_exactly_of_type(OBJ,TYPE) \
    (typeid(OBJ)==typeid(some_##TYPE))
*/
#define is_exactly_of_type(OBJ,TYPE) \
    ((OBJ).tinfo()==(some_##TYPE).tinfo())


    /*
#else 
#define is_of_type(OBJ,TYPE)                               \
    (ASSERT(typeid(OBJ)!=typeid(exZERO())),                \
     (dynamic_cast<TYPE *>(const_cast<basic *>(&OBJ))!=0))

#define is_exactly_of_type(OBJ,TYPE)                       \
    (ASSERT(typeid(OBJ)!=typeid(exZERO())),                \
     (typeid(OBJ)==typeid(some_##TYPE))
#endif // ndef _DEBUG
*/

#define is_ex_of_type(OBJ,TYPE) \
    (dynamic_cast<TYPE *>(const_cast<basic *>((OBJ).bp))!=0)

/*
#define is_ex_exactly_of_type(OBJ,TYPE) \
    (typeid(*(OBJ).bp)==typeid(some_##TYPE))
*/

#define is_ex_exactly_of_type(OBJ,TYPE) \
    ((*(OBJ).bp).tinfo()==(some_##TYPE).tinfo())

#define are_ex_trivially_equal(EX1,EX2) \
    ((EX1).bp==(EX2).bp)

// global functions

inline unsigned rotate_left_31(unsigned n)
{
    // clear highest bit and shift 1 bit to the left
    n=(n & 0x7FFFFFFFU) << 1;

    // overflow? clear highest bit and set lowest bit
    if (n & 0x80000000U) {
        n=(n & 0x7FFFFFFFU) | 0x00000001U;
    }

    ASSERT(n<0x80000000U);

    return n;
}

inline unsigned golden_ratio_hash(unsigned n)
{
#if 0
	// This requires ´long long´ (or an equivalent 64 bit type)---which is,
    // unfortunately, not ANSI-compliant:
	unsigned long long l = n * 0x4f1bbcddLL;
	return (l & 0x7fffffffU) ^ (l >> 32);
#else
	// This requires ´long double´ to have a mantissa of at least 64 bit---
    // which is not guaranteed by any standard:
    const static long double golden_ratio=.618033988749894848204586834370;
    long double m=golden_ratio*n;
    return unsigned((m-int(m))*0x80000000);
#endif
}

#endif // ndef _BASIC_H_
