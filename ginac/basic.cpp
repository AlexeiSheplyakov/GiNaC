/** @file basic.cpp
 *
 *  Implementation of GiNaC's ABC. */

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

#include <iostream>
#include <typeinfo>
#include <stdexcept>

#include "basic.h"
#include "ex.h"
#include "numeric.h"
#include "power.h"
#include "symbol.h"
#include "lst.h"
#include "ncmul.h"
#include "archive.h"
#include "utils.h"
#include "debugmsg.h"

#ifndef NO_NAMESPACE_GINAC
namespace GiNaC {
#endif // ndef NO_NAMESPACE_GINAC

GINAC_IMPLEMENT_REGISTERED_CLASS(basic, void)

//////////
// default constructor, destructor, copy constructor assignment operator and helpers
//////////

// public

#ifndef INLINE_BASIC_CONSTRUCTORS
basic::basic() : flags(0), refcount(0), tinfo_key(TINFO_BASIC)
{
    debugmsg("basic default constructor", LOGLEVEL_CONSTRUCT);
    // nothing to do
}

basic::~basic() 
{
    debugmsg("basic destructor", LOGLEVEL_DESTRUCT);
    destroy(0);
    GINAC_ASSERT((!(flags & status_flags::dynallocated))||(refcount==0));
}

basic::basic(const basic & other) : flags(0), refcount(0), tinfo_key(TINFO_BASIC)
{
    debugmsg("basic copy constructor", LOGLEVEL_CONSTRUCT);
    copy(other);
}
#endif

const basic & basic::operator=(const basic & other)
{
    debugmsg("basic operator=", LOGLEVEL_ASSIGNMENT);
    if (this != &other) {
        destroy(1);
        copy(other);
    }
    return *this;
}

// protected

#if 0
void basic::copy(const basic & other)
{
    flags=other.flags & ~ status_flags::dynallocated;
    hashvalue=other.hashvalue;
    tinfo_key=other.tinfo_key;
}
#endif

//////////
// other constructors
//////////

#ifndef INLINE_BASIC_CONSTRUCTORS
basic::basic(unsigned ti) : flags(0), refcount(0), tinfo_key(ti)
{
    debugmsg("basic constructor with tinfo_key", LOGLEVEL_CONSTRUCT);
    // nothing to do
}
#endif

//////////
// archiving
//////////

/** Construct object from archive_node. */
basic::basic(const archive_node &n, const lst &sym_lst) : flags(0), refcount(0)
{
    debugmsg("basic constructor from archive_node", LOGLEVEL_CONSTRUCT);

    // Reconstruct tinfo_key from class name
    string class_name;
    if (n.find_string("class", class_name))
        tinfo_key = find_tinfo_key(class_name);
    else
        throw (std::runtime_error("archive node contains no class name"));
}

/** Unarchive the object. */
ex basic::unarchive(const archive_node &n, const lst &sym_lst)
{
    return (new basic(n, sym_lst))->setflag(status_flags::dynallocated);
}

/** Archive the object. */
void basic::archive(archive_node &n) const
{
    n.add_string("class", class_name());
}

//////////
// functions overriding virtual functions from bases classes
//////////

// none

//////////
// new virtual functions which can be overridden by derived classes
//////////

// public

/** Output to stream formatted to be useful as ginsh input. */
void basic::print(ostream & os, unsigned upper_precedence) const
{
    debugmsg("basic print",LOGLEVEL_PRINT);
    os << "[basic object]";
}

/** Output to stream in ugly raw format, so brave developers can have a look
 * at the underlying structure. */
void basic::printraw(ostream & os) const
{
    debugmsg("basic printraw",LOGLEVEL_PRINT);
    os << "[basic object]";
}

/** Output to stream formatted in tree- (indented-) form, so developers can
 *  have a look at the underlying structure. */
void basic::printtree(ostream & os, unsigned indent) const
{
    debugmsg("basic printtree",LOGLEVEL_PRINT);
    os << string(indent,' ') << "type=" << typeid(*this).name()
       << ", hash=" << hashvalue << " (0x" << hex << hashvalue << dec << ")"
       << ", flags=" << flags
       << ", nops=" << nops() << endl;
    for (unsigned i=0; i<nops(); ++i) {
        op(i).printtree(os,indent+delta_indent);
    }
}

/** Output to stream formatted as C-source.
 *
 *  @param os a stream for output
 *  @param type variable type (one of the csrc_types)
 *  @param upper_precedence operator precedence of caller
 *  @see ex::printcsrc */
void basic::printcsrc(ostream & os, unsigned type, unsigned upper_precedence) const
{
    debugmsg("basic print csrc", LOGLEVEL_PRINT);
}

/** Little wrapper arount print to be called within a debugger. */
void basic::dbgprint(void) const
{
    print(cerr);
    cerr << endl;
}

/** Little wrapper arount printtree to be called within a debugger. */
void basic::dbgprinttree(void) const
{
    printtree(cerr,0);
}

basic * basic::duplicate() const
{
    debugmsg("basic duplicate",LOGLEVEL_DUPLICATE);
    return new basic(*this);
}

/** Information about the object.
 *
 *  @see class info_flags */
bool basic::info(unsigned inf) const
{
    return false; // all possible properties are false for basic objects
}

/** Number of operands/members. */
unsigned basic::nops() const
{
    return 0;
}

/** Return operand/member at position i. */
ex basic::op(int i) const
{
    return (const_cast<basic *>(this))->let_op(i);
}

/** Return modifyable operand/member at position i. */
ex & basic::let_op(int i)
{
    throw(std::out_of_range("op() out of range"));
}

ex basic::operator[](const ex & index) const
{
    if (is_exactly_of_type(*index.bp,numeric)) {
        return op(static_cast<const numeric &>(*index.bp).to_int());
    }
    throw(std::invalid_argument("non-numeric indices not supported by this type"));
}

ex basic::operator[](int i) const
{
    return op(i);
}

bool basic::has(const ex & other) const
{
    GINAC_ASSERT(other.bp!=0);
    if (is_equal(*other.bp)) return true;
    if (nops()>0) {
        for (unsigned i=0; i<nops(); i++) {
            if (op(i).has(other)) return true;
        }
    }
    return false;
}

int basic::degree(const symbol & s) const
{
    return 0;
}

int basic::ldegree(const symbol & s) const
{
    return 0;
}

ex basic::coeff(const symbol & s, int n) const
{
    return n==0 ? *this : _ex0();
}

ex basic::collect(const symbol & s) const
{
    ex x;
    int ldeg=ldegree(s);
    int deg=degree(s);
    for (int n=ldeg; n<=deg; n++) {
        x += coeff(s,n)*power(s,n);
    }
    return x;
}

ex basic::eval(int level) const
{
    return this->hold();
}

ex basic::evalf(int level) const
{
    return *this;
}

ex basic::subs(const lst & ls, const lst & lr) const
{
    return *this;
}

exvector basic::get_indices(void) const
{
    return exvector(); // return an empty exvector
}

ex basic::simplify_ncmul(const exvector & v) const
{
    return simplified_ncmul(v);
}

// protected

int basic::compare_same_type(const basic & other) const
{
    return compare_pointers(this, &other);
}

bool basic::is_equal_same_type(const basic & other) const
{
    return compare_same_type(other)==0;
}

unsigned basic::return_type(void) const
{
    return return_types::commutative;
}

unsigned basic::return_type_tinfo(void) const
{
    return tinfo();
}

unsigned basic::calchash(void) const
{
    unsigned v=golden_ratio_hash(tinfo());
    for (unsigned i=0; i<nops(); i++) {
        v=rotate_left_31(v);
        v ^= (const_cast<basic *>(this))->op(i).gethash();
    }

    v = v & 0x7FFFFFFFU;
    
    // store calculated hash value only if object is already evaluated
    if (flags & status_flags::evaluated) {
        setflag(status_flags::hash_calculated);
        hashvalue=v;
    }

    return v;
}

ex basic::expand(unsigned options) const
{
    return this->setflag(status_flags::expanded);
}

//////////
// non-virtual functions in this class
//////////

// public

ex basic::subs(const ex & e) const
{
    // accept 2 types of replacement expressions:
    //   - symbol==ex
    //   - lst(symbol1==ex1,symbol2==ex2,...)
    // convert to subs(lst(symbol1,symbol2,...),lst(ex1,ex2,...))
    // additionally, idx can be used instead of symbol
    if (e.info(info_flags::relation_equal)) {
        return subs(lst(e));
    }
    if (!e.info(info_flags::list)) {
        throw(std::invalid_argument("basic::subs(ex): argument must be a list"));
    }
    lst ls;
    lst lr;
    for (unsigned i=0; i<e.nops(); i++) {
        if (!e.op(i).info(info_flags::relation_equal)) {
            throw(std::invalid_argument("basic::subs(ex): argument must be a list or equations"));
        }
        if (!e.op(i).op(0).info(info_flags::symbol)) {
            if (!e.op(i).op(0).info(info_flags::idx)) {
                throw(std::invalid_argument("basic::subs(ex): lhs must be a symbol or an idx"));
            }
        }
        ls.append(e.op(i).op(0));
        lr.append(e.op(i).op(1));
    }
    return subs(ls,lr);
}

/** Compare objects to establish canonical order.
 *  All compare functions return: -1 for *this less than other, 0 equal,
 *  1 greater. */
int basic::compare(const basic & other) const
{
    unsigned hash_this = gethash();
    unsigned hash_other = other.gethash();

    if (hash_this<hash_other) return -1;
    if (hash_this>hash_other) return 1;

    unsigned typeid_this = tinfo();
    unsigned typeid_other = other.tinfo();

    if (typeid_this<typeid_other) {
        /*
        cout << "hash collision, different types: " 
             << *this << " and " << other << endl;
        this->printraw(cout);
        cout << " and ";
        other.printraw(cout);
        cout << endl;
        */
        return -1;
    }
    if (typeid_this>typeid_other) {
        /*
        cout << "hash collision, different types: " 
             << *this << " and " << other << endl;
        this->printraw(cout);
        cout << " and ";
        other.printraw(cout);
        cout << endl;
        */
        return 1;
    }

    GINAC_ASSERT(typeid(*this)==typeid(other));

    int cmpval=compare_same_type(other);
    if ((cmpval!=0)&&(hash_this<0x80000000U)) {
        /*
        cout << "hash collision, same type: " 
             << *this << " and " << other << endl;
        this->printraw(cout);
        cout << " and ";
        other.printraw(cout);
        cout << endl;
        */
    }
    return cmpval;
}

bool basic::is_equal(const basic & other) const
{
    unsigned hash_this = gethash();
    unsigned hash_other = other.gethash();

    if (hash_this!=hash_other) return false;

    unsigned typeid_this = tinfo();
    unsigned typeid_other = other.tinfo();

    if (typeid_this!=typeid_other) return false;

    GINAC_ASSERT(typeid(*this)==typeid(other));

    return is_equal_same_type(other);
}

// protected

const basic & basic::hold(void) const
{
    return setflag(status_flags::evaluated);
}

void basic::ensure_if_modifiable(void) const
{
    if (refcount>1) {
        throw(std::runtime_error("cannot modify multiply referenced object"));
    }
}

//////////
// static member variables
//////////

// protected

unsigned basic::precedence=70;
unsigned basic::delta_indent=4;

//////////
// global constants
//////////

const basic some_basic;
const type_info & typeid_basic=typeid(some_basic);

//////////
// global variables
//////////

int max_recursion_level=1024;

#ifndef NO_NAMESPACE_GINAC
} // namespace GiNaC
#endif // ndef NO_NAMESPACE_GINAC
