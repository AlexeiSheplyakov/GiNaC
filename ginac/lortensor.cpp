/** @file lortensor.cpp
 *
 *  Implementation of GiNaC´s lortensor objects.
 *  No real implementation yet, do be done.      */

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
#include <list>
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <map>

#include "basic.h"
#include "add.h"
#include "mul.h"
#include "debugmsg.h"
#include "flags.h"
#include "lst.h"
#include "lortensor.h"
#include "operators.h"
#include "tinfos.h"
#include "power.h"
#include "symbol.h"
#include "utils.h"

#ifndef NO_GINAC_NAMESPACE
namespace GiNaC {
#endif // ndef NO_GINAC_NAMESPACE

//////////
// default constructor, destructor, copy constructor assignment operator and helpers
//////////

// public

lortensor::lortensor()
{
    debugmsg("lortensor default constructor",LOGLEVEL_CONSTRUCT);
    serial=next_serial++;
    name=autoname_prefix()+ToString(serial);
    tinfo_key=TINFO_lortensor;
}

lortensor::~lortensor()
{
    debugmsg("lortensor destructor",LOGLEVEL_DESTRUCT);
    destroy(0);
}

lortensor::lortensor(lortensor const & other)
{
    debugmsg("lortensor copy constructor",LOGLEVEL_CONSTRUCT);
    copy (other);
}

lortensor const & lortensor::operator=(lortensor const & other)
{
    debugmsg("lortensor operator=",LOGLEVEL_ASSIGNMENT);
    if (this != & other) {
        destroy(1);
        copy(other);
    }
    return *this;
}

//protected

void lortensor::copy(lortensor const & other)
{
    indexed::copy(other);
    type=other.type;
    name=other.name;
    serial=other.serial;
}

void lortensor::destroy(bool call_parent)
{
    if (call_parent) {
        indexed::destroy(call_parent);
    }
}

//////////
// other constructors
//////////

// protected

lortensor::lortensor(lortensor_types const lt, string const & n) : type(lt), name(n)
{
    debugmsg("lortensor constructor from lortensor_types,string",LOGLEVEL_CONSTRUCT);
    serial=next_serial++;
    tinfo_key=TINFO_lortensor;
}

lortensor::lortensor(lortensor_types const lt, string const & n, ex const & mu) : indexed(mu), type(lt), name(n)
{
    debugmsg("lortensor constructor from lortensor_types,string,ex",LOGLEVEL_CONSTRUCT);
    serial=next_serial++;    
    GINAC_ASSERT(all_of_type_lorentzidx());
    tinfo_key=TINFO_lortensor;
}

lortensor::lortensor(lortensor_types const lt, string const & n, ex const & mu, ex const & nu) : indexed(mu,nu), type(lt), name(n)
{
    debugmsg("lortensor constructor from lortensor_types,string,ex,ex",LOGLEVEL_CONSTRUCT);
    serial=next_serial++;
    GINAC_ASSERT(all_of_type_lorentzidx());
    tinfo_key=TINFO_lortensor;
}

lortensor::lortensor(lortensor_types const lt, string const & n, ex const & mu, ex const & nu, ex const & rho) : indexed(mu,nu,rho), type(lt), name(n)
{
    debugmsg("lortensor constructor from lortensor_types,string,ex,ex,ex",LOGLEVEL_CONSTRUCT);
    serial=next_serial++;
    GINAC_ASSERT(all_of_type_lorentzidx());
    tinfo_key=TINFO_lortensor;
}

lortensor::lortensor(lortensor_types const lt, string const & n, ex const & mu, ex const & nu, ex const & rho, ex const & sigma) : indexed(mu,nu,rho,sigma), type(lt), name(n)
{
    debugmsg("lortensor constructor from lortensor_types,string,ex,ex,ex,ex",LOGLEVEL_CONSTRUCT);
    serial=next_serial++;
    GINAC_ASSERT(all_of_type_lorentzidx());
    tinfo_key=TINFO_lortensor;
}

lortensor::lortensor(lortensor_types const lt, string const & n, exvector const & iv) : indexed(iv), type(lt), name(n)
{
    debugmsg("lortensor constructor from lortensor_types,string,exvector",LOGLEVEL_CONSTRUCT);
    serial=next_serial++;
    GINAC_ASSERT(all_of_type_lorentzidx());
    tinfo_key=TINFO_lortensor;
}

lortensor::lortensor(lortensor_types const lt, string const & n, unsigned s, exvector const & iv) : indexed(iv), type(lt), name(n), serial(s)
{
    debugmsg("lortensor constructor from lortensor_types,string,unsigned,exvector",LOGLEVEL_CONSTRUCT);
    GINAC_ASSERT(all_of_type_lorentzidx());
    tinfo_key=TINFO_lortensor;
}

lortensor::lortensor(lortensor_types const lt, string const & n, unsigned s, exvector *ivp) : indexed(ivp), type(lt), name(n), serial(s)
{
    debugmsg("lortensor constructor from lortensor_types,string,unsigned,exvector",LOGLEVEL_CONSTRUCT);
    GINAC_ASSERT(all_of_type_lorentzidx());
    tinfo_key=TINFO_lortensor;
}

//////////
// functions overriding virtual functions from bases classes
//////////

//public

basic * lortensor::duplicate() const
{
    debugmsg("lortensor duplicate",LOGLEVEL_DUPLICATE);
    return new lortensor(*this);
}

void lortensor::printraw(ostream & os) const
{
    debugmsg("lortensor printraw",LOGLEVEL_PRINT);
    os << "lortensor(type=" << (unsigned)type
       << ",indices=";
    printrawindices(os);
    os << ",hash=" << hashvalue << ",flags=" << flags << ")";
}

void lortensor::printtree(ostream & os, unsigned indent) const
{
    debugmsg("lortensor printtree",LOGLEVEL_PRINT);
    os << string(indent,' ') <<"lortensor object: "
       << "type=" << (unsigned)type << ","
       << seq.size() << " indices" << endl;
    printtreeindices(os,indent);
    os << string(indent,' ') << "hash=" << hashvalue
       << " (0x" << hex << hashvalue << dec << ")"
       << ", flags=" << flags << endl;
}

void lortensor::print(ostream & os, unsigned upper_precedence) const
{
    debugmsg("lortensor print",LOGLEVEL_PRINT);
    switch (type) {
    case lortensor_g:
        os << "g";
        break;
    case lortensor_rankn:
        os << name;
        break;
    case lortensor_rank1:
        os << name;
        break;
    case lortensor_rank2:
        os << name;
        break;
    case lortensor_epsilon:
        os << "epsilon";
        break;
    case invalid:
    default:
        os << "INVALID_LORTENSOR_OBJECT";
        break;
    }
    printindices(os);
}

void lortensor::printcsrc(ostream & os, unsigned type, unsigned upper_precedence) const
{
    debugmsg("lortensor print csrc",LOGLEVEL_PRINT);
    print(os,upper_precedence);
}

bool lortensor::info(unsigned inf) const
{
    return indexed::info(inf);
}

ex lortensor::eval(int level) const
{
    if (type==lortensor_g) {
        // canonicalize indices
        exvector iv=seq;
        int sig=canonicalize_indices(iv,false); //symmetric
        if (sig!=INT_MAX) {
            //something has changed while sorting indices, more evaluations later
            return ex(sig) *lortensor(type,name,iv);
        }
        lorentzidx const & idx1=ex_to_lorentzidx(seq[0]);
        lorentzidx const & idx2=ex_to_lorentzidx(seq[1]);
        if ((!idx1.is_symbolic()) && (!idx2.is_symbolic())) {
            //both indices are numeric
            if ((idx1.get_value()==idx2.get_value())) {
                //both on diagonal
                if (idx1.get_value()==0){
                    // (0,0)
                    return _ex1();
                } else {
                    if (idx1.is_covariant() != idx2.is_covariant()) {
                        // (_i,~i) or (~i,_i), i = 1...3
                        return _ex1();
                    } else {
                        // (_i,_i) or (~i,~i), i= 1...3
                        return _ex_1();
                    }
                }
            } else {
                // at least one off-diagonal
                return _ex0();
            }
        } else if (idx1.is_symbolic() && idx1.is_co_contra_pair(idx2)) {
            return Dim()-idx1.get_dim_parallel_space();
        }
    }
    return this -> hold();
}

//protected

int lortensor::compare_same_type(basic const & other) const
{
    GINAC_ASSERT(is_of_type(other,lortensor));
    const lortensor *o = static_cast <const lortensor *> (&other);
    if (type==o->type) {
        if (type==lortensor_rankn) {
            if (serial!=o->serial) {
                return serial < o->serial ? -1 : 1;
            }
        }
        return indexed::compare_same_type(other);
    }
    return type < o->type ? -1 : 1;            
}

bool lortensor::is_equal_same_type(basic const & other) const
{
    GINAC_ASSERT(is_of_type(other,lortensor));
    const lortensor *o=static_cast<const lortensor *> (&other);
    if (type!=o->type) return false;
    if (type==lortensor_rankn) {
        if (serial!=o->serial) return false;
    }
    return indexed::is_equal_same_type(other);            
}

unsigned lortensor::return_type(void) const
{
    return return_types::commutative;
}
unsigned lortensor::return_type_tinfo(void) const
{
    return tinfo_key;
}
ex lortensor::thisexprseq(exvector const & v) const
{
    return lortensor(type,name,serial,v);
}
ex lortensor::thisexprseq(exvector *vp) const
{
    return lortensor(type,name,serial,vp);
}
    
//////////
// non-virtual functions in this class
//////////

// protected

void lortensor::setname(string const & n)
{
    name=n;
}

bool lortensor::all_of_type_lorentzidx(void) const
{
    for (exvector::const_iterator cit=seq.begin(); cit!=seq.end(); ++ cit) {
        if (!is_ex_of_type(*cit,lorentzidx)) {
            return false;
        }
    }
    return true;
}

// private

string & lortensor::autoname_prefix(void)
{
    static string * s=new string("lortensor");
    return *s;
}

//////////
// static member variables
//////////

// private

unsigned lortensor::next_serial=0;

//////////
// friend functions
//////////

lortensor lortensor_g(ex const & mu, ex const & nu)
{
    return lortensor(lortensor::lortensor_g,"",mu,nu);
}

lortensor lortensor_epsilon(ex const & mu, ex const & nu, ex const & rho, ex const & sigma)
{
    return lortensor(lortensor::lortensor_epsilon,"",mu,nu,rho,sigma);
}

lortensor lortensor_rank1(string const & n, ex const & mu)
{
    return lortensor(lortensor::lortensor_rank1,n,mu);
}

lortensor lortensor_rank2(string const & n, ex const & mu, ex const & nu)
{
    return lortensor(lortensor::lortensor_rank2,n,mu,nu);
}

ex simplify_lortensor_mul(ex const & m)
{
    GINAC_ASSERT(is_ex_exactly_of_type(m,mul));
    exvector v_contracted;

    // collect factors in an exvector, store squares twice
    int n=m.nops();
    v_contracted.reserve(2*n);
    for (int i=0; i<n; ++i) {
        ex f=m.op(i);
        if (is_ex_exactly_of_type(f,power)&&f.op(1).is_equal(_ex2())) {
            v_contracted.push_back(f.op(0));
            v_contracted.push_back(f.op(0));
        } else {
            v_contracted.push_back(f);
        }
    }

    unsigned replacements;
    bool something_changed=false;

    exvector::iterator it=v_contracted.begin();
    while (it!=v_contracted.end()) {
        // process only lor_g objects
        if (is_ex_exactly_of_type(*it,lortensor) &&
            (ex_to_lortensor(*it).type==lortensor::lortensor_g)) {            
            lortensor const & g=ex_to_lortensor(*it);
            GINAC_ASSERT(g.seq.size()==2);
            idx const & first_idx=ex_to_lorentzidx(g.seq[0]);
            idx const & second_idx=ex_to_lorentzidx(g.seq[1]);
            // g_{mu,mu} should have been contracted in lortensor::eval()
            GINAC_ASSERT(!first_idx.is_equal(second_idx));
            ex saved_g=*it; // save to restore it later

            // try to contract first index
            replacements=0;
            if (first_idx.is_symbolic()) {
                replacements = subs_index_in_exvector(v_contracted,
                                   first_idx.toggle_covariant(),second_idx);
                if (replacements==0) {
                    // not contracted, restore g object
                    *it=saved_g;
                } else {
                    // a contracted index should occur exactly once
                    GINAC_ASSERT(replacements==1);
                    *it=_ex1();
                    something_changed=true;
                }
            }

            // try second index only if first was not contracted
            if ((replacements==0)&&(second_idx.is_symbolic())) {
                // first index not contracted, *it is again the original g object
                replacements = subs_index_in_exvector(v_contracted,
                                   second_idx.toggle_covariant(),first_idx);
                if (replacements==0) {
                    // not contracted except in itself, restore g object
                    *it=saved_g;
                } else {
                    // a contracted index should occur exactly once
                    GINAC_ASSERT(replacements==1);
                    *it=_ex1();
                    something_changed=true;
                }
            }
        }
        ++it;
    }
    if (something_changed) {
        return mul(v_contracted);
    }
    return m;
}

ex simplify_lortensor(ex const & e)
{
    // all simplification is done on expanded objects
    ex e_expanded=e.expand();

    // simplification of sum=sum of simplifications
    if (is_ex_exactly_of_type(e_expanded,add)) {
        ex sum=_ex0();
        for (int i=0; i<e_expanded.nops(); ++i) {
            sum += simplify_lortensor(e_expanded.op(i));
        }
        return sum;
    }

    // simplification of commutative product=commutative product of simplifications
    if (is_ex_exactly_of_type(e_expanded,mul)) {
        return simplify_lortensor_mul(e);
    }

    // cannot do anything
    return e_expanded;
}

ex Dim(void)
{
    static symbol * d=new symbol("dim");
    return *d;
}

#ifndef NO_GINAC_NAMESPACE
} // namespace GiNaC
#endif // ndef NO_GINAC_NAMESPACE
