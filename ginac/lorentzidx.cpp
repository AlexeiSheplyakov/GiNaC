/** @file lorentzidx.cpp
 *
 *  Implementation of GiNaC's lorentz indices. */

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

#include <stdexcept>

#include "lorentzidx.h"
#include "lst.h"
#include "archive.h"
#include "utils.h"
#include "debugmsg.h"

#ifndef NO_GINAC_NAMESPACE
namespace GiNaC {
#endif // ndef NO_GINAC_NAMESPACE

GINAC_IMPLEMENT_REGISTERED_CLASS(lorentzidx, idx)

//////////
// default constructor, destructor, copy constructor assignment operator and helpers
//////////

// public

lorentzidx::lorentzidx() : orthogonal_only(false), dim_parallel_space(0)
{
    debugmsg("lorentzidx default constructor",LOGLEVEL_CONSTRUCT);
    // serial is incremented in idx::idx()
    name="mu"+ToString(serial);
    tinfo_key=TINFO_lorentzidx;
}

lorentzidx::~lorentzidx() 
{
    debugmsg("lorentzidx destructor",LOGLEVEL_DESTRUCT);
    destroy(0);
}

lorentzidx::lorentzidx(const lorentzidx & other)
{
    debugmsg("lorentzidx copy constructor",LOGLEVEL_CONSTRUCT);
    copy(other);
}

const lorentzidx & lorentzidx::operator=(const lorentzidx & other)
{
    debugmsg("lorentzidx operator=",LOGLEVEL_ASSIGNMENT);
    if (this != &other) {
        destroy(1);
        copy(other);
    }
    return *this;
}

// protected

void lorentzidx::copy(const lorentzidx & other)
{
    inherited::copy(other);
    orthogonal_only=other.orthogonal_only;
    dim_parallel_space=other.dim_parallel_space;
}

void lorentzidx::destroy(bool call_parent)
{
    if (call_parent) inherited::destroy(call_parent);
}

//////////
// other constructors
//////////

// public

lorentzidx::lorentzidx(bool cov, bool oonly, unsigned dimp) :
    idx(cov), orthogonal_only(oonly), dim_parallel_space(dimp)
{
    debugmsg("lorentzidx constructor from bool",LOGLEVEL_CONSTRUCT);
    // serial is incremented in idx::idx(bool)
    if (oonly) {
        name="muorth"+ToString(serial);
    } else {
        name="mu"+ToString(serial);
    }
    tinfo_key=TINFO_lorentzidx;
}

lorentzidx::lorentzidx(const string & n, bool cov, bool oonly, unsigned dimp)
    : idx(n,cov), orthogonal_only(oonly), dim_parallel_space(dimp)
{
    debugmsg("lorentzidx constructor from string,bool,bool,unsigned",
             LOGLEVEL_CONSTRUCT);
    tinfo_key=TINFO_lorentzidx;
}

lorentzidx::lorentzidx(const char * n, bool cov, bool oonly, unsigned dimp)
    : idx(n,cov), orthogonal_only(oonly), dim_parallel_space(dimp)
{
    debugmsg("lorentzidx constructor from char*,bool,bool,unsigned",
             LOGLEVEL_CONSTRUCT);
    tinfo_key=TINFO_lorentzidx;
}

lorentzidx::lorentzidx(unsigned v, bool cov) : idx(v,cov),
    orthogonal_only(false), dim_parallel_space(0)
{
    debugmsg("lorentzidx constructor from unsigned,bool",LOGLEVEL_CONSTRUCT);
    tinfo_key=TINFO_lorentzidx;
}

//////////
// archiving
//////////

/** Construct object from archive_node. */
lorentzidx::lorentzidx(const archive_node &n, const lst &sym_lst) : inherited(n, sym_lst)
{
    debugmsg("lorentzidx constructor from archive_node", LOGLEVEL_CONSTRUCT);
    n.find_bool("orthogonal_only", orthogonal_only);
	if (orthogonal_only)
		n.find_unsigned("pdim", dim_parallel_space);
}

/** Unarchive the object. */
ex lorentzidx::unarchive(const archive_node &n, const lst &sym_lst)
{
    ex s = (new lorentzidx(n, sym_lst))->setflag(status_flags::dynallocated);

    if (ex_to_lorentzidx(s).symbolic) {
        // If lorentzidx is in sym_lst, return the existing lorentzidx
        for (int i=0; i<sym_lst.nops(); i++) {
            if (is_ex_of_type(sym_lst.op(i), lorentzidx) && (ex_to_lorentzidx(sym_lst.op(i)).name == ex_to_lorentzidx(s).name))
                return sym_lst.op(i);
        }
    }
    return s;
}

/** Archive the object. */
void lorentzidx::archive(archive_node &n) const
{
    inherited::archive(n);
	n.add_bool("orthogonal_only", orthogonal_only);
	if (orthogonal_only)
		n.add_unsigned("pdim", dim_parallel_space);
}

//////////
// functions overriding virtual functions from bases classes
//////////

// public

basic * lorentzidx::duplicate() const
{
    debugmsg("lorentzidx duplicate",LOGLEVEL_DUPLICATE);
    return new lorentzidx(*this);
}

void lorentzidx::printraw(ostream & os) const
{
    debugmsg("lorentzidx printraw",LOGLEVEL_PRINT);

    os << "lorentzidx(";

    if (symbolic) {
        os << "symbolic,name=" << name;
    } else {
        os << "non symbolic,value=" << value;
    }

    if (covariant) {
        os << ",covariant";
    } else {
        os << ",contravariant";
    }

    if (orthogonal_only) {
        os << ",only orthogonal components at " << dim_parallel_space
           << " parallel dimensions";
    } else {
        os << ",parallel and orthogonal components";
    }

    os << ",serial=" << serial;
    os << ",hash=" << hashvalue << ",flags=" << flags;
    os << ")";
}

void lorentzidx::printtree(ostream & os, unsigned indent) const
{
    debugmsg("lorentzidx printtree",LOGLEVEL_PRINT);

    os << string(indent,' ') << "lorentzidx: ";

    if (symbolic) {
        os << "symbolic,name=" << name;
    } else {
        os << "non symbolic,value=" << value;
    }

    if (covariant) {
        os << ",covariant";
    } else {
        os << ",contravariant";
    }

    if (orthogonal_only) {
        os << ",only orthogonal components at " << dim_parallel_space
           << " parallel dimensions";
    } else {
        os << ",parallel and orthogonal components";
    }

    os << ", serial=" << serial
       << ", hash=" << hashvalue << " (0x" << hex << hashvalue << dec << ")"
       << ", flags=" << flags << endl;
}

void lorentzidx::print(ostream & os, unsigned upper_precedence) const
{
    debugmsg("lorentzidx print",LOGLEVEL_PRINT);

    if (covariant) {
        os << "_";
    } else {
        os << "~";
    }
    if (symbolic) {
        os << name;
    } else {
        os << value;
    }
}

bool lorentzidx::info(unsigned inf) const
{
    if (inf==info_flags::lorentzidx) return true;
    return inherited::info(inf);
}

//////////
// new virtual functions which can be overridden by derived classes
//////////

// none

//////////
// non-virtual functions in this class
//////////

// public

lorentzidx lorentzidx::create_anonymous_representative(void) const
{
    GINAC_ASSERT(is_symbolic());
    lorentzidx i_copy(*this);
    i_copy.serial=0;
    i_copy.name="anonymous_representative";
    i_copy.covariant=false;
    i_copy.clearflag(status_flags::dynallocated|
                     status_flags::hash_calculated);
    return i_copy;
}

//////////
// static member variables
//////////

// none

//////////
// global constants
//////////

const lorentzidx some_lorentzidx;
const type_info & typeid_lorentzidx=typeid(some_lorentzidx);

#ifndef NO_GINAC_NAMESPACE
} // namespace GiNaC
#endif // ndef NO_GINAC_NAMESPACE
