/** @file idx.cpp
 *
 *  Implementation of GiNaC's indices. */

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

#include <stdexcept>

#include "idx.h"
#include "ex.h"
#include "lst.h"
#include "relational.h"
#include "utils.h"
#include "debugmsg.h"

namespace GiNaC {

//////////
// default constructor, destructor, copy constructor assignment operator and helpers
//////////

// public

idx::idx() : basic(TINFO_idx), symbolic(true), covariant(false)
{
    debugmsg("idx default constructor",LOGLEVEL_CONSTRUCT);
    serial=next_serial++;
    name="index"+ToString(serial);
}

idx::~idx() 
{
    debugmsg("idx destructor",LOGLEVEL_DESTRUCT);
    destroy(0);
}

idx::idx(idx const & other)
{
    debugmsg("idx copy constructor",LOGLEVEL_CONSTRUCT);
    copy(other);
}

idx const & idx::operator=(idx const & other)
{
    debugmsg("idx operator=",LOGLEVEL_ASSIGNMENT);
    if (this != &other) {
        destroy(1);
        copy(other);
    }
    return *this;
}

// protected

void idx::copy(idx const & other)
{
    basic::copy(other);
    serial=other.serial;
    symbolic=other.symbolic;
    name=other.name;
    value=other.value;
    covariant=other.covariant;
}

void idx::destroy(bool call_parent)
{
    if (call_parent) basic::destroy(call_parent);
}

//////////
// other constructors
//////////

// public

idx::idx(bool cov) : basic(TINFO_idx), symbolic(true), covariant(cov)
{
    debugmsg("idx constructor from bool",LOGLEVEL_CONSTRUCT);
    serial=next_serial++;
    name="index"+ToString(serial);
}

idx::idx(string const & n, bool cov) : basic(TINFO_idx),  
    symbolic(true), name(n), covariant(cov)
{
    debugmsg("idx constructor from string,bool",LOGLEVEL_CONSTRUCT);
    serial=next_serial++;
}

idx::idx(char const * n, bool cov) : basic(TINFO_idx),  
    symbolic(true), name(n), covariant(cov)
{
    debugmsg("idx constructor from char*,bool",LOGLEVEL_CONSTRUCT);
    serial=next_serial++;
}

idx::idx(unsigned const v, bool cov) : basic(TINFO_idx),
    symbolic(false), value(v), covariant(cov)
{
    debugmsg("idx constructor from unsigned,bool",LOGLEVEL_CONSTRUCT);
    serial=0;
}


//////////
// functions overriding virtual functions from bases classes
//////////

// public

basic * idx::duplicate() const
{
    debugmsg("idx duplicate",LOGLEVEL_DUPLICATE);
    return new idx(*this);
}

void idx::printraw(ostream & os) const
{
    debugmsg("idx printraw",LOGLEVEL_PRINT);

    os << "idx(";

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

    os << ",serial=" << serial;
    os << ",hash=" << hashvalue << ",flags=" << flags;
    os << ")";
}

void idx::printtree(ostream & os, unsigned indent) const
{
    debugmsg("idx printtree",LOGLEVEL_PRINT);

    os << string(indent,' ') << "idx: ";

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

    os << ", serial=" << serial
       << ", hash=" << hashvalue << " (0x" << hex << hashvalue << dec << ")"
       << ", flags=" << flags << endl;
}

void idx::print(ostream & os, unsigned upper_precedence) const
{
    debugmsg("idx print",LOGLEVEL_PRINT);

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

bool idx::info(unsigned inf) const
{
    if (inf==info_flags::idx) return true;
    return basic::info(inf);
}

ex idx::subs(lst const & ls, lst const & lr) const
{
    ASSERT(ls.nops()==lr.nops());
#ifdef DOASSERT
    for (int i=0; i<ls.nops(); i++) {
        ASSERT(is_ex_exactly_of_type(ls.op(i),symbol)||
               is_ex_of_type(ls.op(i),idx));
    }
#endif // def DOASSERT

    for (int i=0; i<ls.nops(); i++) {
        if (is_equal(*(ls.op(i)).bp)) {
            return lr.op(i);
        }
    }
    return *this;
}

// protected

int idx::compare_same_type(basic const & other) const
{
    ASSERT(is_of_type(other,idx));
    idx const & o=static_cast<idx const &>
                             (const_cast<basic &>(other));

    if (covariant!=o.covariant) {
        // different co/contravariant
        return covariant ? -1 : 1;
    }
    if ((!symbolic) && (!o.symbolic)) {
        // non-symbolic, of equal type: compare values
        if (value==o.value) {
            return 0;
        }
        return value<o.value ? -1 : 1;
    }
    if (symbolic && o.symbolic) {
        // both symbolic: compare serials
        if (serial==o.serial) {
            return 0;
        }
        return serial<o.serial ? -1 : 1;
    }
    // one symbolic, one value: value is sorted first
    return o.symbolic ? -1 : 1;
}

bool idx::is_equal_same_type(basic const & other) const
{
    ASSERT(is_of_type(other,idx));
    idx const & o=static_cast<idx const &>
                             (const_cast<basic &>(other));

    if (covariant!=o.covariant) return false;
    if (symbolic!=o.symbolic) return false;
    if (symbolic && o.symbolic) return serial==o.serial;
    return value==o.value;
}    

unsigned idx::calchash(void) const
{
    hashvalue=golden_ratio_hash(golden_ratio_hash(tinfo_key ^ serial));
    setflag(status_flags::hash_calculated);
    return hashvalue;
}

//////////
// new virtual functions which can be overridden by derived classes
//////////

// public

bool idx::is_co_contra_pair(basic const & other) const
{
    // like is_equal_same_type(), but tests for different covariant status
    ASSERT(is_of_type(other,idx));
    idx const & o=static_cast<idx const &>
                             (const_cast<basic &>(other));

    if (covariant==o.covariant) return false;
    if (symbolic!=o.symbolic) return false;
    if (symbolic && o.symbolic) return serial==o.serial;
    return value==o.value;
}    

bool idx::is_symbolic(void) const
{
    return symbolic;
}

unsigned idx::get_value(void) const
{
    return value;
}

bool idx::is_covariant(void) const
{
    return covariant;
}

ex idx::toggle_covariant(void) const
{
    idx * i_copy=static_cast<idx *>(duplicate());
    i_copy->covariant = !i_copy->covariant;
    i_copy->clearflag(status_flags::hash_calculated);
    return i_copy->setflag(status_flags::dynallocated);
}

//////////
// non-virtual functions in this class
//////////

// none

//////////
// static member variables
//////////

// protected

unsigned idx::next_serial=0;

//////////
// global constants
//////////

const idx some_idx;
type_info const & typeid_idx=typeid(some_idx);

//////////
// other functions
//////////

int canonicalize_indices(exvector & iv, bool antisymmetric)
{
    if (iv.size()<2) {
        // nothing do to for 0 or 1 indices
        return INT_MAX;
    }

    bool something_changed=false;
    int sig=1;
    // simple bubble sort algorithm should be sufficient for the small number of indices needed
    exvector::const_iterator last_idx=iv.end();
    exvector::const_iterator next_to_last_idx=iv.end()-1;
    for (exvector::iterator it1=iv.begin(); it1!=next_to_last_idx; ++it1) {
        for (exvector::iterator it2=it1+1; it2!=last_idx; ++it2) {
            int cmpval=(*it1).compare(*it2);
            if (cmpval==1) {
                iter_swap(it1,it2);
                something_changed=true;
                if (antisymmetric) sig=-sig;
            } else if ((cmpval==0) && antisymmetric) {
                something_changed=true;
                sig=0;
            }
        }
    }
    return something_changed ? sig : INT_MAX;
}

exvector idx_intersect(exvector const & iv1, exvector const & iv2)
{
    // build a vector of symbolic indices contained in iv1 and iv2 simultaneously
    // assumes (but does not test) that each index occurs at most twice
    exvector iv_intersect;
    for (exvector::const_iterator cit1=iv1.begin(); cit1!=iv1.end(); ++cit1) {
        ASSERT(is_ex_of_type(*cit1,idx));
        if (ex_to_idx(*cit1).is_symbolic()) {
            for (exvector::const_iterator cit2=iv2.begin(); cit2!=iv2.end(); ++cit2) {
                ASSERT(is_ex_of_type(*cit2,idx));
                if ((*cit1).is_equal(*cit2)) {
                    iv_intersect.push_back(*cit1);
                    break;
                }
            }
        }
    }
    return iv_intersect;
}

#define TEST_PERMUTATION(A,B,C,P) \
    if ((iv3[B].is_equal(iv2[0]))&&(iv3[C].is_equal(iv2[1]))) { \
        if (antisymmetric) *sig=P; \
        return iv3[A]; \
    }

ex permute_free_index_to_front(exvector const & iv3, exvector const & iv2,
                               bool antisymmetric, int * sig)
{
    // match (return value,iv2) to iv3 by permuting indices
    // iv3 is always cyclic

    ASSERT(iv3.size()==3);
    ASSERT(iv2.size()==2);

    *sig=1;
    
    TEST_PERMUTATION(0,1,2,  1);
    TEST_PERMUTATION(0,2,1, -1);
    TEST_PERMUTATION(1,0,2, -1);
    TEST_PERMUTATION(1,2,0,  1);
    TEST_PERMUTATION(2,0,1,  1);
    TEST_PERMUTATION(2,1,0, -1);
    throw(std::logic_error("permute_free_index_to_front(): no valid permutation found"));
}
    
unsigned subs_index_in_exvector(exvector & v, ex const & is, ex const & ir)
{
    exvector::iterator it;
    unsigned replacements=0;
    unsigned current_replacements;

    ASSERT(is_ex_of_type(is,idx));
    ASSERT(is_ex_of_type(ir,idx));
   
    for (it=v.begin(); it!=v.end(); ++it) {
        current_replacements=count_index(*it,is);
        if (current_replacements>0) {
            (*it)=(*it).subs(is==ir);
        }
        replacements += current_replacements;
    }
    return replacements;
}

unsigned count_index(ex const & e, ex const & i)
{
    exvector idxv=e.get_indices();
    unsigned count=0;
    for (exvector::const_iterator cit=idxv.begin(); cit!=idxv.end(); ++cit) {
        if ((*cit).is_equal(i)) count++;
    }
    return count;
}

ex subs_indices(ex const & e, exvector const & idxv_subs,
                exvector const & idxv_repl)
{
    ASSERT(idxv_subs.size()==idxv_repl.size());
    ex res=e;
    for (unsigned i=0; i<idxv_subs.size(); ++i) {
        res=res.subs(idxv_subs[i]==idxv_repl[i]);
    }
    return res;
}

} // namespace GiNaC
