/** @file constant.cpp
 *
 *  Implementation of GiNaC's constant types and some special constants.
 *
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

#include "ginac.h"

//////////
// default constructor, destructor, copy constructor assignment operator and helpers
//////////

// public

constant::~constant()
{
    debugmsg("constant destructor",LOGLEVEL_DESTRUCT);
    destroy(0);
}

constant::constant(constant const & other)
{
    debugmsg("constant copy constructor",LOGLEVEL_CONSTRUCT);
    copy(other);
}

// protected

void constant::copy(constant const & other)
{
    basic::copy(other);
    name=other.name;
    serial=other.serial;
    ef=other.ef;
    if (other.number != 0) {
        number = new numeric(*other.number);
    } else {
        number = 0;
    }
    fct_assigned=other.fct_assigned;
}

void constant::destroy(bool call_parent)
{
    delete number;
    if (call_parent)
        basic::destroy(call_parent);
}

//////////
// other constructors
//////////

// public

constant::constant(string const & initname, ex (*efun)()) :
    basic(TINFO_CONSTANT), name(initname), ef(efun),
    number(0), fct_assigned(true), serial(next_serial++)
{
    debugmsg("constant constructor from string, function",LOGLEVEL_CONSTRUCT);
}

constant::constant(string const & initname, numeric const & initnumber) :
    basic(TINFO_CONSTANT), name(initname), ef(0),
    number(new numeric(initnumber)), fct_assigned(false), serial(next_serial++)
{
    debugmsg("constant constructor from string, numeric",LOGLEVEL_CONSTRUCT);
}

//////////
// functions overriding virtual functions from bases classes
//////////

// public

basic * constant::duplicate() const
{
    debugmsg("constant duplicate",LOGLEVEL_DUPLICATE);
    return new constant(*this);
}

ex constant::evalf(int level) const
{
    if (fct_assigned) {
        return ef();
    } else if (number != 0) {
        return *number;
    }
    return *this;
}

// protected

int constant::compare_same_type(basic const & other) const
{
    ASSERT(is_exactly_of_type(other, constant));
    // constant const & o=static_cast<constant &>(const_cast<basic &>(other));
    // return name.compare(o.name);
    const constant *o = static_cast<const constant *>(&other);
    if (serial==o->serial) return 0;
    return serial < o->serial ? -1 : 1;
}

bool constant::is_equal_same_type(basic const & other) const
{
    ASSERT(is_exactly_of_type(other, constant));
    const constant *o = static_cast<const constant *>(&other);
    return serial==o->serial;
}

//////////
// new virtual functions which can be overridden by derived classes
//////////

// none

//////////
// non-virtual functions in this class
//////////

// none

//////////
// static member variables
//////////

unsigned constant::next_serial=0;

//////////
// global constants
//////////

const constant some_constant("",0);
type_info const & typeid_constant=typeid(some_constant);

/**  Pi. (3.14159...)  Diverts straight into CLN for evalf(). */
const constant Pi("Pi", PiEvalf);
/** Catalan's constant. (0.91597...)  Diverts straight into CLN for evalf(). */
const constant EulerGamma("EulerGamma", EulerGammaEvalf);
/** Euler's constant. (0.57721...)  Sometimes called Euler-Mascheroni constant.
 *  Diverts straight into CLN for evalf(). */
const constant Catalan("Catalan", CatalanEvalf);
