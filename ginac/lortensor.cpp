/** @file lortensor.cpp
 *
 *  Implementation of GiNaC's Lorentz tensors. */

/*
 *  GiNaC Copyright (C) 1999-2001 Johannes Gutenberg University Mainz, Germany
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
#include "archive.h"
#include "utils.h"
#include "config.h"

#ifndef NO_NAMESPACE_GINAC
namespace GiNaC {
#endif // ndef NO_NAMESPACE_GINAC

GINAC_IMPLEMENT_REGISTERED_CLASS(lortensor, indexed)

//////////
// default constructor, destructor, copy constructor assignment operator and helpers
//////////

// public

lortensor::lortensor() : inherited(TINFO_lortensor), type(invalid)
{
	debugmsg("lortensor default constructor",LOGLEVEL_CONSTRUCT);
	serial=next_serial++;
	name=autoname_prefix()+ToString(serial);
}

lortensor::~lortensor()
{
	debugmsg("lortensor destructor",LOGLEVEL_DESTRUCT);
	destroy(false);
}

lortensor::lortensor(const lortensor & other)
{
	debugmsg("lortensor copy constructor",LOGLEVEL_CONSTRUCT);
	copy (other);
}

const lortensor & lortensor::operator=(const lortensor & other)
{
	debugmsg("lortensor operator=",LOGLEVEL_ASSIGNMENT);
	if (this != & other) {
		destroy(true);
		copy(other);
	}
	return *this;
}

//protected

void lortensor::copy(const lortensor & other)
{
	inherited::copy(other);
	type=other.type;
	name=other.name;
	serial=other.serial;
}

void lortensor::destroy(bool call_parent)
{
	if (call_parent) inherited::destroy(call_parent);
}

//////////
// other constructors
//////////

// protected

/** Construct object without any Lorentz index. This constructor is for
 *  internal use only. */
lortensor::lortensor(lortensor_types const lt, const std::string & n) : type(lt), name(n)
{
	debugmsg("lortensor constructor from lortensor_types,string",LOGLEVEL_CONSTRUCT);
	if (lt == lortensor_symbolic)
		serial = next_serial++;
	else
		serial = 0;
	tinfo_key = TINFO_lortensor;
}

/** Construct object with one Lorentz index. This constructor is for
 *  internal use only. Use the lortensor_vector() or lortensor_symbolic()
 *  functions instead.
 *  @see lortensor_vector
 *  @see lortensor_symbolic */
lortensor::lortensor(lortensor_types const lt, const std::string & n, const ex & mu) : inherited(mu), type(lt), name(n)
{
	debugmsg("lortensor constructor from lortensor_types,string,ex",LOGLEVEL_CONSTRUCT);
	GINAC_ASSERT(all_of_type_lorentzidx());
	if (lt == lortensor_symbolic)
		serial = next_serial++;
	else
		serial = 0;
	tinfo_key=TINFO_lortensor;
}

/** Construct object with two Lorentz indices. This constructor is for
 *  internal use only. Use the lortensor_g(), lortensor_delta() or
 *  lortensor_symbolic() functions instead.
 *  @see lortensor_g
 *  @see lortensor_delta
 *  @see lortensor_symbolic */
lortensor::lortensor(lortensor_types const lt, const std::string & n, const ex & mu, const ex & nu) : inherited(mu,nu), type(lt), name(n)
{
	debugmsg("lortensor constructor from lortensor_types,string,ex,ex",LOGLEVEL_CONSTRUCT);
	GINAC_ASSERT(all_of_type_lorentzidx());
	if (lt == lortensor_symbolic)
		serial = next_serial++;
	else
		serial = 0;
	tinfo_key=TINFO_lortensor;
}

/** Construct object with three Lorentz indices. This constructor is for
 *  internal use only. Use the lortensor_symbolic() function instead.
 *  @see lortensor_symbolic */
lortensor::lortensor(lortensor_types const lt, const std::string & n, const ex & mu, const ex & nu, const ex & rho) : inherited(mu,nu,rho), type(lt), name(n)
{
	debugmsg("lortensor constructor from lortensor_types,string,ex,ex,ex",LOGLEVEL_CONSTRUCT);
	GINAC_ASSERT(all_of_type_lorentzidx());
	if (lt == lortensor_symbolic)
		serial = next_serial++;
	else
		serial = 0;
	tinfo_key=TINFO_lortensor;
}

/** Construct object with four Lorentz indices. This constructor is for
 *  internal use only. Use the lortensor_epsilon() or lortensor_symbolic()
 *  functions instead.
 *  @see lortensor_epsilon
 *  @see lortensor_symbolic */
lortensor::lortensor(lortensor_types const lt, const std::string & n, const ex & mu, const ex & nu, const ex & rho, const ex & sigma) : inherited(mu,nu,rho,sigma), type(lt), name(n)
{
	debugmsg("lortensor constructor from lortensor_types,string,ex,ex,ex,ex",LOGLEVEL_CONSTRUCT);
	GINAC_ASSERT(all_of_type_lorentzidx());
	if (lt == lortensor_symbolic)
		serial = next_serial++;
	else
		serial = 0;
	tinfo_key=TINFO_lortensor;
}

/** Construct object with arbitrary number of Lorentz indices. This
 *  constructor is for internal use only. Use the lortensor_symbolic()
 *  function instead.
 *
 *  @see lortensor_symbolic */
lortensor::lortensor(lortensor_types const lt, const std::string & n, const exvector & iv) : inherited(iv), type(lt), name(n)
{
	debugmsg("lortensor constructor from lortensor_types,string,exvector",LOGLEVEL_CONSTRUCT);
	GINAC_ASSERT(all_of_type_lorentzidx());
	if (lt == lortensor_symbolic)
		serial = next_serial++;
	else
		serial = 0;
	tinfo_key=TINFO_lortensor;
}

lortensor::lortensor(lortensor_types const lt, const std::string & n, unsigned s, const exvector & iv) : indexed(iv), type(lt), name(n), serial(s)
{
	debugmsg("lortensor constructor from lortensor_types,string,unsigned,exvector",LOGLEVEL_CONSTRUCT);
	GINAC_ASSERT(all_of_type_lorentzidx());
	tinfo_key=TINFO_lortensor;
}

lortensor::lortensor(lortensor_types const lt, const std::string & n, unsigned s, exvector *ivp) : indexed(ivp), type(lt), name(n), serial(s)
{
	debugmsg("lortensor constructor from lortensor_types,string,unsigned,exvector",LOGLEVEL_CONSTRUCT);
	GINAC_ASSERT(all_of_type_lorentzidx());
	tinfo_key=TINFO_lortensor;
}


//////////
// archiving
//////////

/** Construct object from archive_node. */
lortensor::lortensor(const archive_node &n, const lst &sym_lst) : inherited(n, sym_lst)
{
	debugmsg("lortensor constructor from archive_node", LOGLEVEL_CONSTRUCT);
	unsigned int ty;
	if (!(n.find_unsigned("type", ty)))
		throw (std::runtime_error("unknown lortensor type in archive"));
	type = (lortensor_types)ty;
	if (type == lortensor_symbolic) {
		serial = next_serial++;
		if (!(n.find_string("name", name)))
			name = autoname_prefix() + ToString(serial);
	} else
		serial = 0;
}

/** Unarchive the object. */
ex lortensor::unarchive(const archive_node &n, const lst &sym_lst)
{
	ex s = (new lortensor(n, sym_lst))->setflag(status_flags::dynallocated);

	if (ex_to_lortensor(s).type == lortensor_symbolic) {
		// If lortensor is in sym_lst, return the existing lortensor
		for (unsigned i=0; i<sym_lst.nops(); i++) {
			if (is_ex_of_type(sym_lst.op(i), lortensor) && (ex_to_lortensor(sym_lst.op(i)).name == ex_to_lortensor(s).name))
				return sym_lst.op(i);
		}
	}
	return s;
}

/** Archive the object. */
void lortensor::archive(archive_node &n) const
{
	inherited::archive(n);
	n.add_unsigned("type", type);
	if (type == lortensor_symbolic)
		n.add_string("name", name);
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

void lortensor::printraw(std::ostream & os) const
{
	debugmsg("lortensor printraw",LOGLEVEL_PRINT);
	os << "lortensor(type=" << (unsigned)type
	   << ",indices=";
	printrawindices(os);
	os << ",serial=" << serial;
	os << ",hash=" << hashvalue << ",flags=" << flags << ")";
}

void lortensor::printtree(std::ostream & os, unsigned indent) const
{
	debugmsg("lortensor printtree",LOGLEVEL_PRINT);
	os << std::string(indent,' ') <<"lortensor object: "
	   << "type=" << (unsigned)type << ","
	   << seq.size() << " indices" << std::endl;
	printtreeindices(os,indent);
	os << std::string(indent,' ') << "hash=" << hashvalue
	   << " (0x" << std::hex << hashvalue << std::dec << ")"
	   << ", flags=" << flags << std::endl;
}

void lortensor::print(std::ostream & os, unsigned upper_precedence) const
{
	debugmsg("lortensor print",LOGLEVEL_PRINT);
	switch (type) {
	case lortensor_g:
		os << "g";
		break;
	case lortensor_delta:
		os << "delta";
		break;
	case lortensor_epsilon:
		os << "epsilon";
		break;
	case lortensor_symbolic:
		os << name;
		break;
	case invalid:
	default:
		os << "INVALID_LORTENSOR_OBJECT";
		break;
	}
	printindices(os);
}

bool lortensor::info(unsigned inf) const
{
	return inherited::info(inf);
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
		const lorentzidx & idx1=ex_to_lorentzidx(seq[0]);
		const lorentzidx & idx2=ex_to_lorentzidx(seq[1]);
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
			return Dim() - 2;
		}
	}
	return this -> hold();
}

//protected

int lortensor::compare_same_type(const basic & other) const
{
	GINAC_ASSERT(is_of_type(other,lortensor));
	const lortensor &o = static_cast<const lortensor &>(other);

	if (type!=o.type) {
		// different type
		return type < o.type ? -1 : 1;
	}

	if (type == lortensor_symbolic) {
		// symbolic, compare serials
		if (serial != o.serial) {
			return serial < o.serial ? -1 : 1;
		}
	}

	return inherited::compare_same_type(other);
}

bool lortensor::is_equal_same_type(const basic & other) const
{
	GINAC_ASSERT(is_of_type(other,lortensor));
	const lortensor &o = static_cast<const lortensor &>(other);

	if (type != o.type) return false;
	if (type == lortensor_symbolic && serial != o.serial) return false;
	return inherited::is_equal_same_type(other);            
}

unsigned lortensor::return_type(void) const
{
	return return_types::commutative;
}

unsigned lortensor::return_type_tinfo(void) const
{
	return tinfo_key;
}

ex lortensor::thisexprseq(const exvector & v) const
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

/** Check whether all indices are of class lorentzidx or a subclass. This
 *  function is used internally to make sure that all constructed Lorentz
 *  tensors really carry Lorentz indices and not some other classes. */
bool lortensor::all_of_type_lorentzidx(void) const
{
	for (exvector::const_iterator cit=seq.begin(); cit!=seq.end(); ++ cit) {
		if (!is_ex_of_type(*cit,lorentzidx)) return false;
	}
	return true;
}

// private

std::string & lortensor::autoname_prefix(void)
{
	static std::string * s = new std::string("lortensor");
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

/** Construct an object representing the metric tensor g. The indices must
 *  be of class lorentzidx.
 *
 *  @param mu First index
 *  @param nu Second index
 *  @return newly constructed object */
lortensor lortensor_g(const ex & mu, const ex & nu)
{
	return lortensor(lortensor::lortensor_g,"",mu,nu);
}

/** Construct an object representing the unity matrix delta. The indices
 *  must be of class lorentzidx.
 *
 *  @param mu First index
 *  @param nu Second index
 *  @return newly constructed object */
lortensor lortensor_delta(const ex & mu, const ex & nu)
{
	return lortensor(lortensor::lortensor_delta,"",mu,nu);
}

/** Construct an object representing the four-dimensional totally
 *  antisymmetric tensor epsilon. The indices must be of class lorentzidx.
 *
 *  @param mu First index
 *  @param nu Second index
 *  @param rho Third index
 *  @param sigma Fourth index
 *  @return newly constructed object */
lortensor lortensor_epsilon(const ex & mu, const ex & nu, const ex & rho, const ex & sigma)
{
	return lortensor(lortensor::lortensor_epsilon,"",mu,nu,rho,sigma);
}

/** Construct an object representing a symbolic Lorentz vector. The index
 *  must be of class lorentzidx.
 *
 *  @param n Symbolic name
 *  @param mu Index
 *  @return newly constructed object */
lortensor lortensor_vector(const std::string & n, const ex & mu)
{
	return lortensor(lortensor::lortensor_symbolic,n,mu);
}

/** Construct an object representing a symbolic Lorentz tensor of arbitrary
 *  rank. The indices must be of class lorentzidx.
 *
 *  @param n Symbolic name
 *  @param iv Vector of indices
 *  @return newly constructed object */
lortensor lortensor_symbolic(const std::string & n, const exvector & iv)
{
	return lortensor(lortensor::lortensor_symbolic,n,iv);
}

ex simplify_lortensor_mul(const ex & m)
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
			const lortensor & g=ex_to_lortensor(*it);
			GINAC_ASSERT(g.seq.size()==2);
			const idx & first_idx=ex_to_lorentzidx(g.seq[0]);
			const idx & second_idx=ex_to_lorentzidx(g.seq[1]);
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

/** Perform some simplifications on an expression containing Lorentz tensors. */
ex simplify_lortensor(const ex & e)
{
	// all simplification is done on expanded objects
	ex e_expanded=e.expand();

	// simplification of sum=sum of simplifications
	if (is_ex_exactly_of_type(e_expanded,add)) {
		ex sum=_ex0();
		for (unsigned i=0; i<e_expanded.nops(); ++i) {
			sum += simplify_lortensor(e_expanded.op(i));
		}
		return sum;
	}

	// simplification of (commutative) product
	if (is_ex_exactly_of_type(e_expanded,mul)) {
		return simplify_lortensor_mul(e);
	}

	// cannot do anything
	return e_expanded;
}

//////////
// global constants
//////////

const lortensor some_lortensor;
const std::type_info & typeid_lortensor = typeid(some_lortensor);

#ifndef NO_NAMESPACE_GINAC
} // namespace GiNaC
#endif // ndef NO_NAMESPACE_GINAC
