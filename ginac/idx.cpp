/** @file idx.cpp
 *
 *  Implementation of GiNaC's indices. */

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

#include <stdexcept>

#include "idx.h"
#include "ex.h"
#include "lst.h"
#include "relational.h"
#include "archive.h"
#include "utils.h"
#include "debugmsg.h"

#ifndef NO_NAMESPACE_GINAC
namespace GiNaC {
#endif // ndef NO_NAMESPACE_GINAC

GINAC_IMPLEMENT_REGISTERED_CLASS(idx, basic)

//////////
// default constructor, destructor, copy constructor assignment operator and helpers
//////////

// public

idx::idx() : inherited(TINFO_idx), symbolic(true), covariant(false)
{
	debugmsg("idx default constructor",LOGLEVEL_CONSTRUCT);
	serial=next_serial++;
	name=autoname_prefix()+ToString(serial);
}

// protected

void idx::copy(const idx & other)
{
	inherited::copy(other);
	serial=other.serial;
	symbolic=other.symbolic;
	name=other.name;
	value=other.value;
	covariant=other.covariant;
}

void idx::destroy(bool call_parent)
{
	if (call_parent) inherited::destroy(call_parent);
}

//////////
// other constructors
//////////

// public

/** Construct symbolic index, using an automatically generated unique name.
 *
 *  @param cov Index is covariant (contravariant otherwise)
 *  @return newly constructed index */
idx::idx(bool cov) : inherited(TINFO_idx), symbolic(true), covariant(cov)
{
	debugmsg("idx constructor from bool",LOGLEVEL_CONSTRUCT);
	serial = next_serial++;
	name = autoname_prefix()+ToString(serial);
}

/** Construct symbolic index with specified name.
 *
 *  @param n Symbolic index name
 *  @param cov Index is covariant (contravariant otherwise)
 *  @return newly constructed index */
idx::idx(const std::string & n, bool cov) : inherited(TINFO_idx),  
	symbolic(true), name(n), covariant(cov)
{
	debugmsg("idx constructor from string,bool",LOGLEVEL_CONSTRUCT);
	serial = next_serial++;
}

/** Construct symbolic index with specified name.
 *
 *  @param n Symbolic index name
 *  @param cov Index is covariant (contravariant otherwise)
 *  @return newly constructed index */
idx::idx(const char * n, bool cov) : inherited(TINFO_idx), symbolic(true), name(n), covariant(cov)
{
	debugmsg("idx constructor from char*,bool",LOGLEVEL_CONSTRUCT);
	serial = next_serial++;
}

/** Construct numeric index with specified value.
 *
 *  @param v Numeric index value
 *  @param cov Index is covariant (contravariant otherwise)
 *  @return newly constructed index */
idx::idx(unsigned v, bool cov) : inherited(TINFO_idx), symbolic(false), value(v), covariant(cov)
{
	debugmsg("idx constructor from unsigned,bool",LOGLEVEL_CONSTRUCT);
	serial = 0;
}

//////////
// archiving
//////////

/** Construct object from archive_node. */
idx::idx(const archive_node &n, const lst &sym_lst) : inherited(n, sym_lst)
{
	debugmsg("idx constructor from archive_node", LOGLEVEL_CONSTRUCT);
	n.find_bool("symbolic", symbolic);
	n.find_bool("covariant", covariant);
	if (symbolic) {
		serial = next_serial++;
		if (!(n.find_string("name", name)))
			name = autoname_prefix() + ToString(serial);
	} else {
		serial = 0;
		n.find_unsigned("value", value);
	}
}

/** Unarchive the object. */
ex idx::unarchive(const archive_node &n, const lst &sym_lst)
{
	ex s = (new idx(n, sym_lst))->setflag(status_flags::dynallocated);

	if (ex_to_idx(s).symbolic) {
		// If idx is in sym_lst, return the existing idx
		for (unsigned i=0; i<sym_lst.nops(); i++) {
			if (is_ex_of_type(sym_lst.op(i), idx) && (ex_to_idx(sym_lst.op(i)).name == ex_to_idx(s).name))
				return sym_lst.op(i);
		}
	}
	return s;
}

/** Archive the object. */
void idx::archive(archive_node &n) const
{
	inherited::archive(n);
	n.add_bool("symbolic", symbolic);
	n.add_bool("covariant", covariant);
	if (symbolic)
		n.add_string("name", name);
	else
		n.add_unsigned("value", value);
}

//////////
// functions overriding virtual functions from bases classes
//////////

// public

void idx::printraw(std::ostream & os) const
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

void idx::printtree(std::ostream & os, unsigned indent) const
{
	debugmsg("idx printtree",LOGLEVEL_PRINT);

	os << std::string(indent,' ') << "idx: ";

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
	   << ", hash=" << hashvalue
	   << " (0x" << std::hex << hashvalue << std::dec << ")"
	   << ", flags=" << flags << std::endl;
}

void idx::print(std::ostream & os, unsigned upper_precedence) const
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
	return inherited::info(inf);
}

ex idx::subs(const lst & ls, const lst & lr) const
{
	GINAC_ASSERT(ls.nops()==lr.nops());
#ifdef DO_GINAC_ASSERT
	for (unsigned i=0; i<ls.nops(); i++) {
		GINAC_ASSERT(is_ex_exactly_of_type(ls.op(i),symbol)||
			   is_ex_of_type(ls.op(i),idx));
	}
#endif // def DO_GINAC_ASSERT

	for (unsigned i=0; i<ls.nops(); i++) {
		if (is_equal(*(ls.op(i)).bp)) {
			return lr.op(i);
		}
	}
	return *this;
}

// protected

int idx::compare_same_type(const basic & other) const
{
	GINAC_ASSERT(is_of_type(other,idx));
	const idx &o = static_cast<const idx &>(other);

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

bool idx::is_equal_same_type(const basic & other) const
{
	GINAC_ASSERT(is_of_type(other,idx));
	const idx &o = static_cast<const idx &>(other);

	if (covariant != o.covariant) return false;
	if (symbolic != o.symbolic) return false;
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

/** Check whether the index forms a co-/contravariant pair with another
 *  index (i.e. same name/value but opposite co-/contravariance). */
bool idx::is_co_contra_pair(const basic & other) const
{
	// like is_equal_same_type(), but tests for different covariant status
	GINAC_ASSERT(is_of_type(other,idx));
	const idx & o=static_cast<const idx &>(const_cast<basic &>(other));

	if (covariant==o.covariant) return false;
	if (symbolic!=o.symbolic) return false;
	if (symbolic && o.symbolic) return serial==o.serial;
	return value==o.value;
}    

/** Toggle co-/contravariance of index. */
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

// private

std::string & idx::autoname_prefix(void)
{
	static std::string * s = new std::string("index");
	return *s;
}

//////////
// static member variables
//////////

// protected

unsigned idx::next_serial=0;

//////////
// other functions
//////////

/** Bring a vector of indices into a canonic order. This operation only makes
 *  sense if the object carrying these indices is either symmetric or totally
 *  antisymmetric with respect to the indices.
 *
 *  @param iv Index vector
 *  @param antisymmetric Whether the object carrying the indices is antisymmetric (symmetric otherwise)
 *  @return the sign introduced by the reordering of the indices. For symmetric
 *          objects this is always +1. For antisymmetric objects this is either
 *          +1 or -1 or 0 (if two equal indices were encountered). If the index
 *          vector was unchanged this function returns INT_MAX. */
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

/** Build a vector of indices as the set intersection of two other index
 *  vectors (i.e. the returned vector contains the indices which appear in
 *  both source vectors). */
exvector idx_intersect(const exvector & iv1, const exvector & iv2)
{
	// Create union vector
	exvector iv_union;
	iv_union.reserve(iv1.size() + iv2.size());
	iv_union.insert(iv_union.end(), iv1.begin(), iv1.end());
	iv_union.insert(iv_union.end(), iv2.begin(), iv2.end());

	// Sort it
	canonicalize_indices(iv_union);

	// Look for duplicates
	exvector iv_intersect;
	exvector::const_iterator cit = iv_union.begin(), citend = iv_union.end();
	ex e;
	if (cit != citend)
		e = *cit++;
	while (cit != citend) {
		if (e.is_equal(*cit)) {
			iv_intersect.push_back(e);
			do {
				cit++;
			} while (cit != citend && e.is_equal(*cit));
			if (cit == citend)
				break;
		}
		e = *cit++;
	}
	return iv_intersect;
}

/** Given a vector iv3 of three indices and a vector iv2 of two indices
 *  where iv2 is a subset of iv3, return the (free) index that is in iv3
 *  but not in iv2 and the sign introduced by permuting that index to the
 *  front.
 *
 *  @param iv3 Vector of 3 indices
 *  @param iv2 Vector of 2 indices, must be a subset of iv3
 *  @param sig Returns the sign introduced by permuting the free index to the
 *             front if the object carrying the indices was antisymmetric (if
 *             it's symmetric, you can just ignore the returned value).
 *  @return the free index (the one that is in iv3 but not in iv2) */
ex permute_free_index_to_front(const exvector & iv3, const exvector & iv2, int * sig)
{
	// match (return value,iv2) to iv3 by permuting indices
	// iv3 is always cyclic

	GINAC_ASSERT(iv3.size()==3);
	GINAC_ASSERT(iv2.size()==2);

	*sig=1;

#define TEST_PERMUTATION(A,B,C,P) \
	if ((iv3[B].is_equal(iv2[0]))&&(iv3[C].is_equal(iv2[1]))) { \
		*sig=P; \
		return iv3[A]; \
	}
	
	TEST_PERMUTATION(0,1,2,  1);
	TEST_PERMUTATION(0,2,1, -1);
	TEST_PERMUTATION(1,0,2, -1);
	TEST_PERMUTATION(1,2,0,  1);
	TEST_PERMUTATION(2,0,1,  1);
	TEST_PERMUTATION(2,1,0, -1);
	throw(std::logic_error("permute_free_index_to_front(): no valid permutation found"));
}
	
/** Substitute one index in a vector of expressions.
 *
 *  @param v Vector to substitute in (will be modified)
 *  @param is Index being substituted
 *  @param ir Index to replace by
 *  @return number of performed substitutions */
unsigned subs_index_in_exvector(exvector & v, const ex & is, const ex & ir)
{
	exvector::iterator it;
	unsigned replacements=0;
	unsigned current_replacements;

	GINAC_ASSERT(is_ex_of_type(is,idx));
	GINAC_ASSERT(is_ex_of_type(ir,idx));
   
	for (it=v.begin(); it!=v.end(); ++it) {
		current_replacements=count_index(*it,is);
		if (current_replacements>0) {
			(*it)=(*it).subs(is==ir);
		}
		replacements += current_replacements;
	}
	return replacements;
}

/** Count number of times a given index appears in the index vector of an
 *  indexed object.
 *
 *  @param e Indexed object
 *  @param i Index to look for
 *  @return number of times the index was found */
unsigned count_index(const ex & e, const ex & i)
{
	exvector idxv=e.get_indices();
	unsigned count=0;
	for (exvector::const_iterator cit=idxv.begin(); cit!=idxv.end(); ++cit) {
		if ((*cit).is_equal(i)) count++;
	}
	return count;
}

/** Substitute multiple indices in an expression.
 *
 *  @param e Expression to substitute in
 *  @param idxv_subs Vector of indices being substituted
 *  @param idxv_repl Vector of indices to replace by (1:1 correspondence to idxv_subs)
 *  @return expression with substituted indices */
ex subs_indices(const ex & e, const exvector & idxv_subs, const exvector & idxv_repl)
{
	GINAC_ASSERT(idxv_subs.size()==idxv_repl.size());
	ex res=e;
	for (unsigned i=0; i<idxv_subs.size(); ++i) {
		res=res.subs(idxv_subs[i]==idxv_repl[i]);
	}
	return res;
}

#ifndef NO_NAMESPACE_GINAC
} // namespace GiNaC
#endif // ndef NO_NAMESPACE_GINAC
