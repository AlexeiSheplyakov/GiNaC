/** @file symbol.cpp
 *
 *  Implementation of GiNaC's symbolic objects. */

/*
 *  GiNaC Copyright (C) 1999 Johannes Gutenberg University Mainz, Germany
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

#include <string>
#include <stdexcept>

#include "symbol.h"
#include "lst.h"
#include "utils.h"
#include "idx.h"
#include "debugmsg.h"

#ifndef NO_GINAC_NAMESPACE
namespace GiNaC {
#endif // ndef NO_GINAC_NAMESPACE

//////////
// default constructor, destructor, copy constructor assignment operator and helpers
//////////

symbol::symbol() : basic(TINFO_symbol)
{
    debugmsg("symbol default constructor",LOGLEVEL_CONSTRUCT);
    serial=next_serial++;
    name=autoname_prefix()+ToString(serial);
    asexinfop=new assigned_ex_info;
    setflag(status_flags::evaluated);
}

symbol::~symbol()
{
    debugmsg("symbol destructor",LOGLEVEL_DESTRUCT);
    destroy(0);
}

symbol::symbol(symbol const & other)
{
    debugmsg("symbol copy constructor",LOGLEVEL_CONSTRUCT);
    copy(other);
}

void symbol::copy(symbol const & other)
{
    basic::copy(other);
    name=other.name;
    serial=other.serial;
    asexinfop=other.asexinfop;
    ++asexinfop->refcount;
}

void symbol::destroy(bool call_parent)
{
    if (--asexinfop->refcount == 0) {
        delete asexinfop;
    }
    if (call_parent) {
        basic::destroy(call_parent);
    }
}

// how should the following be interpreted?
// symbol x;
// symbol y;
// x=y;
// probably as: x=ex(y);

//////////
// other constructors
//////////

// public

symbol::symbol(string const & initname) : basic(TINFO_symbol)
{
    debugmsg("symbol constructor from string",LOGLEVEL_CONSTRUCT);
    name=initname;
    serial=next_serial++;
    asexinfop=new assigned_ex_info;
    setflag(status_flags::evaluated);
}

//////////
// functions overriding virtual functions from bases classes
//////////

// public

basic * symbol::duplicate() const
{
    debugmsg("symbol duplicate",LOGLEVEL_DUPLICATE);
    return new symbol(*this);
}

void symbol::print(ostream & os, unsigned upper_precedence) const
{
    debugmsg("symbol print",LOGLEVEL_PRINT);
    os << name;
}

void symbol::printraw(ostream & os) const
{
    debugmsg("symbol printraw",LOGLEVEL_PRINT);
    os << "symbol(" << "name=" << name << ",serial=" << serial
       << ",hash=" << hashvalue << ",flags=" << flags << ")";
}

void symbol::printtree(ostream & os, unsigned indent) const
{
    debugmsg("symbol printtree",LOGLEVEL_PRINT);
    os << string(indent,' ') << name << " (symbol): "
       << "serial=" << serial
       << ", hash=" << hashvalue << " (0x" << hex << hashvalue << dec << ")"
       << ", flags=" << flags << endl;
}

void symbol::printcsrc(ostream & os, unsigned type, unsigned upper_precedence) const
{
    debugmsg("symbol print csrc", LOGLEVEL_PRINT);
    os << name;
}

bool symbol::info(unsigned inf) const
{
    if (inf==info_flags::symbol) return true;
    if (inf==info_flags::polynomial ||
        inf==info_flags::integer_polynomial ||
        inf==info_flags::cinteger_polynomial ||
        inf==info_flags::rational_polynomial ||
        inf==info_flags::crational_polynomial ||
        inf==info_flags::rational_function) {
        return true;
    } else {
        return basic::info(inf);
    }
}

ex symbol::expand(unsigned options) const
{
    return this->hold();
}

bool symbol::has(ex const & other) const
{
    if (is_equal(*other.bp)) return true;
    return false;
}

int symbol::degree(symbol const & s) const
{
    return compare_same_type(s)==0 ? 1 : 0;
}

int symbol::ldegree(symbol const & s) const
{
    return compare_same_type(s)==0 ? 1 : 0;
}

ex symbol::coeff(symbol const & s, int const n) const
{
    if (compare_same_type(s)==0) {
        return n==1 ? exONE() : exZERO();
    } else {
        return n==0 ? *this : exZERO();
    }
}

ex symbol::eval(int level) const
{
    if (level == -max_recursion_level) {
        throw(std::runtime_error("max recursion level reached"));
    }
    
    if (asexinfop->is_assigned) {
        setflag(status_flags::evaluated);
        if (level==1) {
            return (asexinfop->assigned_expression);
        } else {
            return (asexinfop->assigned_expression).eval(level);
        }
    } else {
        return this->hold();
    }
}

ex symbol::subs(lst const & ls, lst const & lr) const
{
    GINAC_ASSERT(ls.nops()==lr.nops());
#ifdef DO_GINAC_ASSERT
    for (int i=0; i<ls.nops(); i++) {
        GINAC_ASSERT(is_ex_exactly_of_type(ls.op(i),symbol)||
               is_ex_of_type(ls.op(i),idx));
    }
#endif // def DO_GINAC_ASSERT

    for (int i=0; i<ls.nops(); i++) {
        if (is_ex_exactly_of_type(ls.op(i),symbol)) {
            if (compare_same_type(ex_to_symbol(ls.op(i)))==0) return lr.op(i);
        }
    }
    return *this;
}

// protected

int symbol::compare_same_type(basic const & other) const
{
    GINAC_ASSERT(is_of_type(other,symbol));
    const symbol *o = static_cast<const symbol *>(&other);
    if (serial==o->serial) return 0;
    return serial < o->serial ? -1 : 1;
}

bool symbol::is_equal_same_type(basic const & other) const
{
    GINAC_ASSERT(is_of_type(other,symbol));
    const symbol *o = static_cast<const symbol *>(&other);
    return serial==o->serial;
}

unsigned symbol::return_type(void) const
{
    return return_types::commutative;
}
   
unsigned symbol::return_type_tinfo(void) const
{
    return tinfo_key;
}

unsigned symbol::calchash(void) const
{
    // return golden_ratio_hash(tinfo()) ^ serial;
    hashvalue=golden_ratio_hash(golden_ratio_hash(0x55555555U ^ serial));
    setflag(status_flags::hash_calculated);
    return hashvalue;
}

//////////
// virtual functions which can be overridden by derived classes
//////////

// none

//////////
// non-virtual functions in this class
//////////

// public

void symbol::assign(ex const & value)
{
    asexinfop->is_assigned=1;
    asexinfop->assigned_expression=value;
    clearflag(status_flags::evaluated);
}

void symbol::unassign(void)
{
    if (asexinfop->is_assigned) {
        asexinfop->is_assigned=0;
        asexinfop->assigned_expression=exZERO();
    }
    setflag(status_flags::evaluated);
}

// private

string & symbol::autoname_prefix(void)
{
    static string * s=new string("symbol");
    return *s;
}

//////////
// static member variables
//////////

// private

unsigned symbol::next_serial=0;

// string const symbol::autoname_prefix="symbol";

//////////
// global constants
//////////

const symbol some_symbol;
type_info const & typeid_symbol=typeid(some_symbol);

//////////
// subclass assigned_ex_info
//////////

/** Default ctor.  Defaults to unassigned. */
symbol::assigned_ex_info::assigned_ex_info(void) : is_assigned(0), refcount(1)
{
}

#ifndef NO_GINAC_NAMESPACE
} // namespace GiNaC
#endif // ndef NO_GINAC_NAMESPACE
