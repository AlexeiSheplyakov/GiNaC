/** @file indexed.cpp
 *
 *  Implementation of GiNaC's index carrying objects. */

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

#include <string>

#include "indexed.h"
#include "ex.h"
#include "idx.h"
#include "debugmsg.h"

#ifndef NO_NAMESPACE_GINAC
namespace GiNaC {
#endif // ndef NO_NAMESPACE_GINAC

GINAC_IMPLEMENT_REGISTERED_CLASS(indexed, exprseq)

//////////
// default constructor, destructor, copy constructor assignment operator and helpers
//////////

// public

indexed::indexed()
{
    debugmsg("indexed default constructor",LOGLEVEL_CONSTRUCT);
    tinfo_key=TINFO_indexed;
}

indexed::~indexed()
{
    debugmsg("indexed destructor",LOGLEVEL_DESTRUCT);
    destroy(0);
}

indexed::indexed(const indexed & other)
{
    debugmsg("indexed copy constructor",LOGLEVEL_CONSTRUCT);
    copy (other);
}

const indexed & indexed::operator=(const indexed & other)
{
    debugmsg("indexed operator=",LOGLEVEL_ASSIGNMENT);
    if (this != &other) {
        destroy(1);
        copy(other);
    }
    return *this;
}

// protected

void indexed::copy(const indexed & other)
{
    inherited::copy(other);
}

void indexed::destroy(bool call_parent)
{
    if (call_parent) {
        inherited::destroy(call_parent);
    }
}

//////////
// other constructors
//////////

// public

indexed::indexed(const ex & i1) : inherited(i1)
{
    debugmsg("indexed constructor from ex",LOGLEVEL_CONSTRUCT);
    tinfo_key=TINFO_indexed;
    GINAC_ASSERT(all_of_type_idx());
}

indexed::indexed(const ex & i1, const ex & i2) : inherited(i1,i2)
{
    debugmsg("indexed constructor from ex,ex",LOGLEVEL_CONSTRUCT);
    tinfo_key=TINFO_indexed;
    GINAC_ASSERT(all_of_type_idx());
}

indexed::indexed(const ex & i1, const ex & i2, const ex & i3)
    : inherited(i1,i2,i3)
{
    debugmsg("indexed constructor from ex,ex,ex",LOGLEVEL_CONSTRUCT);
    tinfo_key=TINFO_indexed;
    GINAC_ASSERT(all_of_type_idx());
}

indexed::indexed(const ex & i1, const ex & i2, const ex & i3, const ex & i4)
    : inherited(i1,i2,i3,i4)
{
    debugmsg("indexed constructor from ex,ex,ex,ex",LOGLEVEL_CONSTRUCT);
    tinfo_key=TINFO_indexed;
    GINAC_ASSERT(all_of_type_idx());
}

indexed::indexed(const exvector & iv) : inherited(iv)
{
    debugmsg("indexed constructor from exvector",LOGLEVEL_CONSTRUCT);
    tinfo_key=TINFO_indexed;
    GINAC_ASSERT(all_of_type_idx());
}

indexed::indexed(exvector * ivp) : inherited(ivp)
{
    debugmsg("indexed constructor from exvector *",LOGLEVEL_CONSTRUCT);
    tinfo_key=TINFO_indexed;
    GINAC_ASSERT(all_of_type_idx());
}

//////////
// archiving
//////////

/** Construct object from archive_node. */
indexed::indexed(const archive_node &n, const lst &sym_lst) : inherited(n, sym_lst)
{
    debugmsg("indexed constructor from archive_node", LOGLEVEL_CONSTRUCT);
    tinfo_key = TINFO_indexed;
}

/** Unarchive the object. */
ex indexed::unarchive(const archive_node &n, const lst &sym_lst)
{
    return (new indexed(n, sym_lst))->setflag(status_flags::dynallocated);
}

/** Archive the object. */
void indexed::archive(archive_node &n) const
{
    inherited::archive(n);
}

//////////
// functions overriding virtual functions from bases classes
//////////

// public

basic * indexed::duplicate() const
{
    debugmsg("indexed duplicate",LOGLEVEL_DUPLICATE);
    return new indexed(*this);
}

void indexed::printraw(ostream & os) const
{
    debugmsg("indexed printraw",LOGLEVEL_PRINT);
    os << "indexed(indices=";
    printrawindices(os);
    os << ",hash=" << hashvalue << ",flags=" << flags << ")";
}

void indexed::printtree(ostream & os, unsigned indent) const
{
    debugmsg("indexed printtree",LOGLEVEL_PRINT);
    os << string(indent,' ') << "indexed: " << seq.size() << " indices";
    os << ",hash=" << hashvalue << ",flags=" << flags << endl;
    printtreeindices(os,indent);
}

void indexed::print(ostream & os, unsigned upper_precedence) const
{
    debugmsg("indexed print",LOGLEVEL_PRINT);
    os << "UNNAMEDINDEX";
    printindices(os);
}

void indexed::printcsrc(ostream & os, unsigned type,
                        unsigned upper_precedence) const
{
    debugmsg("indexed print csrc",LOGLEVEL_PRINT);
    print(os,upper_precedence);
}

bool indexed::info(unsigned inf) const
{
    if (inf==info_flags::indexed) return true;
    if (inf==info_flags::has_indices) return seq.size()!=0;
    return inherited::info(inf);
}

exvector indexed::get_indices(void) const
{
    return seq;

    /*
    idxvector filtered_indices;
    filtered_indices.reserve(indices.size());
    for (idxvector::const_iterator cit=indices.begin(); cit!=indices.end(); ++cit) {
        if ((*cit).get_type()==t) {
            filtered_indices.push_back(*cit);
        }
    }
    return filtered_indices;
    */
}

// protected

/** Implementation of ex::diff() for an indexed object. It always returns 0.
 *  @see ex::diff */
ex indexed::derivative(const symbol & s) const
{
        return _ex0();
}

int indexed::compare_same_type(const basic & other) const
{
    GINAC_ASSERT(is_of_type(other,indexed));
    return inherited::compare_same_type(other);
}

bool indexed::is_equal_same_type(const basic & other) const
{
    GINAC_ASSERT(is_of_type(other,indexed));
    return inherited::is_equal_same_type(other);
}

unsigned indexed::return_type(void) const
{
    return return_types::noncommutative;
}
   
unsigned indexed::return_type_tinfo(void) const
{
    return tinfo_key;
}

ex indexed::thisexprseq(const exvector & v) const
{
    return indexed(v);
}

ex indexed::thisexprseq(exvector * vp) const
{
    return indexed(vp);
}

//////////
// virtual functions which can be overridden by derived classes
//////////

// none

//////////
// non-virtual functions in this class
//////////

// protected

void indexed::printrawindices(ostream & os) const
{
    if (seq.size()!=0) {
        for (exvector::const_iterator cit=seq.begin(); cit!=seq.end(); ++cit) {
            (*cit).printraw(os);
            os << ",";
        }
    }
}

void indexed::printtreeindices(ostream & os, unsigned indent) const
{
    if (seq.size()!=0) {
        for (exvector::const_iterator cit=seq.begin(); cit!=seq.end(); ++cit) {
            os << string(indent+delta_indent,' ');
            (*cit).printraw(os);
            os << endl;
        }
    }
}

void indexed::printindices(ostream & os) const
{
    if (seq.size()!=0) {
        if (seq.size()>1) {
            os << "{";
        }
        exvector::const_iterator last=seq.end()-1;
        exvector::const_iterator cit=seq.begin();
        for (; cit!=last; ++cit) {
            (*cit).print(os);
            os << ",";
        }
        (*cit).print(os);
        if (seq.size()>1) {
            os << "}";
        }
    }
}

bool indexed::all_of_type_idx(void) const
{
    // used only inside of ASSERTs
    for (exvector::const_iterator cit=seq.begin(); cit!=seq.end(); ++cit) {
        if (!is_ex_of_type(*cit,idx)) return false;
    }
    return true;
}

//////////
// static member variables
//////////

// none

//////////
// global constants
//////////

const indexed some_indexed;
const type_info & typeid_indexed=typeid(some_indexed);

#ifndef NO_NAMESPACE_GINAC
} // namespace GiNaC
#endif // ndef NO_NAMESPACE_GINAC
