/** @file add.cpp
 *
 *  Implementation of GiNaC's sums of expressions. */

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
#include <stdexcept>

#include "add.h"
#include "mul.h"
#include "archive.h"
#include "debugmsg.h"
#include "utils.h"

#ifndef NO_NAMESPACE_GINAC
namespace GiNaC {
#endif // ndef NO_NAMESPACE_GINAC

GINAC_IMPLEMENT_REGISTERED_CLASS(add, expairseq)

//////////
// default constructor, destructor, copy constructor assignment operator and helpers
//////////

// public

add::add()
{
	debugmsg("add default constructor",LOGLEVEL_CONSTRUCT);
	tinfo_key = TINFO_add;
}

add::~add()
{
	debugmsg("add destructor",LOGLEVEL_DESTRUCT);
	destroy(false);
}

add::add(const add & other)
{
	debugmsg("add copy constructor",LOGLEVEL_CONSTRUCT);
	copy(other);
}

const add & add::operator=(const add & other)
{
	debugmsg("add operator=",LOGLEVEL_ASSIGNMENT);
	if (this != &other) {
		destroy(true);
		copy(other);
	}
	return *this;
}

// protected

void add::copy(const add & other)
{
	inherited::copy(other);
}

void add::destroy(bool call_parent)
{
	if (call_parent) inherited::destroy(call_parent);
}

//////////
// other constructors
//////////

// public

add::add(const ex & lh, const ex & rh)
{
	debugmsg("add constructor from ex,ex",LOGLEVEL_CONSTRUCT);
	tinfo_key = TINFO_add;
	overall_coeff = _ex0();
	construct_from_2_ex(lh,rh);
	GINAC_ASSERT(is_canonical());
}

add::add(const exvector & v)
{
	debugmsg("add constructor from exvector",LOGLEVEL_CONSTRUCT);
	tinfo_key = TINFO_add;
	overall_coeff = _ex0();
	construct_from_exvector(v);
	GINAC_ASSERT(is_canonical());
}

/*
add::add(const epvector & v, bool do_not_canonicalize)
{
	debugmsg("add constructor from epvector,bool",LOGLEVEL_CONSTRUCT);
	tinfo_key = TINFO_add;
	if (do_not_canonicalize) {
		seq=v;
#ifdef EXPAIRSEQ_USE_HASHTAB
		combine_same_terms(); // to build hashtab
#endif // def EXPAIRSEQ_USE_HASHTAB
	} else {
		construct_from_epvector(v);
	}
	GINAC_ASSERT(is_canonical());
}
*/

add::add(const epvector & v)
{
	debugmsg("add constructor from epvector",LOGLEVEL_CONSTRUCT);
	tinfo_key = TINFO_add;
	overall_coeff = _ex0();
	construct_from_epvector(v);
	GINAC_ASSERT(is_canonical());
}

add::add(const epvector & v, const ex & oc)
{
	debugmsg("add constructor from epvector,ex",LOGLEVEL_CONSTRUCT);
	tinfo_key = TINFO_add;
	overall_coeff = oc;
	construct_from_epvector(v);
	GINAC_ASSERT(is_canonical());
}

add::add(epvector * vp, const ex & oc)
{
	debugmsg("add constructor from epvector *,ex",LOGLEVEL_CONSTRUCT);
	tinfo_key = TINFO_add;
	GINAC_ASSERT(vp!=0);
	overall_coeff = oc;
	construct_from_epvector(*vp);
	delete vp;
	GINAC_ASSERT(is_canonical());
}

//////////
// archiving
//////////

/** Construct object from archive_node. */
add::add(const archive_node &n, const lst &sym_lst) : inherited(n, sym_lst)
{
	debugmsg("add constructor from archive_node", LOGLEVEL_CONSTRUCT);
}

/** Unarchive the object. */
ex add::unarchive(const archive_node &n, const lst &sym_lst)
{
	return (new add(n, sym_lst))->setflag(status_flags::dynallocated);
}

/** Archive the object. */
void add::archive(archive_node &n) const
{
	inherited::archive(n);
}

//////////
// functions overriding virtual functions from bases classes
//////////

// public

basic * add::duplicate() const
{
	debugmsg("add duplicate",LOGLEVEL_DUPLICATE);
	return new add(*this);
}

void add::print(std::ostream & os, unsigned upper_precedence) const
{
	debugmsg("add print",LOGLEVEL_PRINT);
	if (precedence<=upper_precedence) os << "(";
	numeric coeff;
	bool first = true;
	// first print the overall numeric coefficient, if present:
	if (!overall_coeff.is_zero()) {
		os << overall_coeff;
		first = false;
	}
	// then proceed with the remaining factors:
	for (epvector::const_iterator cit=seq.begin(); cit!=seq.end(); ++cit) {
		coeff = ex_to_numeric(cit->coeff);
		if (!first) {
			if (coeff.csgn()==-1) os << '-'; else os << '+';
		} else {
			if (coeff.csgn()==-1) os << '-';
			first = false;
		}
		if (!coeff.is_equal(_num1()) &&
			!coeff.is_equal(_num_1())) {
			if (coeff.is_rational()) {
				if (coeff.is_negative())
					os << -coeff;
				else
					os << coeff;
			} else {
				if (coeff.csgn()==-1)
					(-coeff).print(os, precedence);
				else
					coeff.print(os, precedence);
			}
			os << '*';
		}
		os << cit->rest;
	}
	if (precedence<=upper_precedence) os << ")";
}

void add::printraw(std::ostream & os) const
{
	debugmsg("add printraw",LOGLEVEL_PRINT);

	os << "+(";
	for (epvector::const_iterator it=seq.begin(); it!=seq.end(); ++it) {
		os << "(";
		(*it).rest.bp->printraw(os);
		os << ",";
		(*it).coeff.bp->printraw(os);        
		os << "),";
	}
	os << ",hash=" << hashvalue << ",flags=" << flags;
	os << ")";
}

void add::printcsrc(std::ostream & os, unsigned type, unsigned upper_precedence) const
{
	debugmsg("add print csrc", LOGLEVEL_PRINT);
	if (precedence <= upper_precedence)
		os << "(";

	// Print arguments, separated by "+"
	epvector::const_iterator it = seq.begin();
	epvector::const_iterator itend = seq.end();
	while (it != itend) {

		// If the coefficient is -1, it is replaced by a single minus sign
		if (it->coeff.compare(_num1()) == 0) {
			it->rest.bp->printcsrc(os, type, precedence);
		} else if (it->coeff.compare(_num_1()) == 0) {
			os << "-";
			it->rest.bp->printcsrc(os, type, precedence);
		} else if (ex_to_numeric(it->coeff).numer().compare(_num1()) == 0) {
			it->rest.bp->printcsrc(os, type, precedence);
			os << "/";
			ex_to_numeric(it->coeff).denom().printcsrc(os, type, precedence);
		} else if (ex_to_numeric(it->coeff).numer().compare(_num_1()) == 0) {
			os << "-";
			it->rest.bp->printcsrc(os, type, precedence);
			os << "/";
			ex_to_numeric(it->coeff).denom().printcsrc(os, type, precedence);
		} else {
			it->coeff.bp->printcsrc(os, type, precedence);
			os << "*";
			it->rest.bp->printcsrc(os, type, precedence);
		}

		// Separator is "+", except if the following expression would have a leading minus sign
		it++;
		if (it != itend && !(it->coeff.compare(_num0()) < 0 || (it->coeff.compare(_num1()) == 0 && is_ex_exactly_of_type(it->rest, numeric) && it->rest.compare(_num0()) < 0)))
			os << "+";
	}
	
	if (!overall_coeff.is_equal(_ex0())) {
		if (overall_coeff.info(info_flags::positive)) os << '+';
		overall_coeff.bp->printcsrc(os,type,precedence);
	}
	
	if (precedence <= upper_precedence)
		os << ")";
}

bool add::info(unsigned inf) const
{
	switch (inf) {
		case info_flags::polynomial:
		case info_flags::integer_polynomial:
		case info_flags::cinteger_polynomial:
		case info_flags::rational_polynomial:
		case info_flags::crational_polynomial:
		case info_flags::rational_function: {
			for (epvector::const_iterator i=seq.begin(); i!=seq.end(); ++i) {
				if (!(recombine_pair_to_ex(*i).info(inf)))
					return false;
			}
			return overall_coeff.info(inf);
		}
		case info_flags::algebraic: {
			for (epvector::const_iterator i=seq.begin(); i!=seq.end(); ++i) {
				if ((recombine_pair_to_ex(*i).info(inf)))
					return true;
			}
			return false;
		}
	}
	return inherited::info(inf);
}

int add::degree(const symbol & s) const
{
	int deg = INT_MIN;
	if (!overall_coeff.is_equal(_ex0())) {
		deg = 0;
	}
	int cur_deg;
	for (epvector::const_iterator cit=seq.begin(); cit!=seq.end(); ++cit) {
		cur_deg=(*cit).rest.degree(s);
		if (cur_deg>deg) deg=cur_deg;
	}
	return deg;
}

int add::ldegree(const symbol & s) const
{
	int deg = INT_MAX;
	if (!overall_coeff.is_equal(_ex0())) {
		deg = 0;
	}
	int cur_deg;
	for (epvector::const_iterator cit=seq.begin(); cit!=seq.end(); ++cit) {
		cur_deg = (*cit).rest.ldegree(s);
		if (cur_deg<deg) deg=cur_deg;
	}
	return deg;
}

ex add::coeff(const symbol & s, int n) const
{
	epvector coeffseq;
	coeffseq.reserve(seq.size());

	epvector::const_iterator it=seq.begin();
	while (it!=seq.end()) {
		coeffseq.push_back(combine_ex_with_coeff_to_pair((*it).rest.coeff(s,n),
														 (*it).coeff));
		++it;
	}
	if (n==0) {
		return (new add(coeffseq,overall_coeff))->setflag(status_flags::dynallocated);
	}
	return (new add(coeffseq))->setflag(status_flags::dynallocated);
}

ex add::eval(int level) const
{
	// simplifications: +(;c) -> c
	//                  +(x;1) -> x

	debugmsg("add eval",LOGLEVEL_MEMBER_FUNCTION);

	epvector * evaled_seqp=evalchildren(level);
	if (evaled_seqp!=0) {
		// do more evaluation later
		return (new add(evaled_seqp,overall_coeff))->
				   setflag(status_flags::dynallocated);
	}
	
#ifdef DO_GINAC_ASSERT
	for (epvector::const_iterator cit=seq.begin(); cit!=seq.end(); ++cit) {
		GINAC_ASSERT(!is_ex_exactly_of_type((*cit).rest,add));
		if (is_ex_exactly_of_type((*cit).rest,numeric)) {
			dbgprint();
		}
		GINAC_ASSERT(!is_ex_exactly_of_type((*cit).rest,numeric));
	}
#endif // def DO_GINAC_ASSERT
	
	if (flags & status_flags::evaluated) {
		GINAC_ASSERT(seq.size()>0);
		GINAC_ASSERT((seq.size()>1)||!overall_coeff.is_equal(_ex0()));
		return *this;
	}
	
	int seq_size=seq.size();
	if (seq_size==0) {
		// +(;c) -> c
		return overall_coeff;
	} else if ((seq_size==1)&&overall_coeff.is_equal(_ex0())) {
		// +(x;0) -> x
		return recombine_pair_to_ex(*(seq.begin()));
	}
	return this->hold();
}

exvector add::get_indices(void) const
{
	// FIXME: all terms in the sum should have the same indices (compatible
	// tensors) however this is not checked, since there is no function yet
	// which compares indices (idxvector can be unsorted)
	if (seq.size()==0) {
		return exvector();
	}
	return (seq.begin())->rest.get_indices();
}    

ex add::simplify_ncmul(const exvector & v) const
{
	if (seq.size()==0) {
		return inherited::simplify_ncmul(v);
	}
	return (*seq.begin()).rest.simplify_ncmul(v);
}    

// protected

/** Implementation of ex::diff() for a sum. It differentiates each term.
 *  @see ex::diff */
ex add::derivative(const symbol & s) const
{
	// D(a+b+c)=D(a)+D(b)+D(c)
	return (new add(diffchildren(s)))->setflag(status_flags::dynallocated);
}

int add::compare_same_type(const basic & other) const
{
	return inherited::compare_same_type(other);
}

bool add::is_equal_same_type(const basic & other) const
{
	return inherited::is_equal_same_type(other);
}

unsigned add::return_type(void) const
{
	if (seq.size()==0) {
		return return_types::commutative;
	}
	return (*seq.begin()).rest.return_type();
}
   
unsigned add::return_type_tinfo(void) const
{
	if (seq.size()==0) {
		return tinfo_key;
	}
	return (*seq.begin()).rest.return_type_tinfo();
}

ex add::thisexpairseq(const epvector & v, const ex & oc) const
{
	return (new add(v,oc))->setflag(status_flags::dynallocated);
}

ex add::thisexpairseq(epvector * vp, const ex & oc) const
{
	return (new add(vp,oc))->setflag(status_flags::dynallocated);
}

expair add::split_ex_to_pair(const ex & e) const
{
	if (is_ex_exactly_of_type(e,mul)) {
		const mul & mulref=ex_to_mul(e);
		ex numfactor=mulref.overall_coeff;
		// mul * mulcopyp=static_cast<mul *>(mulref.duplicate());
		mul * mulcopyp=new mul(mulref);
		mulcopyp->overall_coeff=_ex1();
		mulcopyp->clearflag(status_flags::evaluated);
		mulcopyp->clearflag(status_flags::hash_calculated);
		return expair(mulcopyp->setflag(status_flags::dynallocated),numfactor);
	}
	return expair(e,_ex1());
}

expair add::combine_ex_with_coeff_to_pair(const ex & e,
										  const ex & c) const
{
	GINAC_ASSERT(is_ex_exactly_of_type(c, numeric));
	ex one = _ex1();
	if (is_ex_exactly_of_type(e, mul)) {
		const mul &mulref = ex_to_mul(e);
		ex numfactor = mulref.overall_coeff;
		mul *mulcopyp = new mul(mulref);
		mulcopyp->overall_coeff = one;
		mulcopyp->clearflag(status_flags::evaluated);
		mulcopyp->clearflag(status_flags::hash_calculated);
		mulcopyp->setflag(status_flags::dynallocated);
		if (are_ex_trivially_equal(c, one)) {
			return expair(*mulcopyp, numfactor);
		} else if (are_ex_trivially_equal(numfactor, one)) {
			return expair(*mulcopyp, c);
		}
		return expair(*mulcopyp, ex_to_numeric(numfactor).mul_dyn(ex_to_numeric(c)));
	} else if (is_ex_exactly_of_type(e, numeric)) {
		if (are_ex_trivially_equal(c, one)) {
			return expair(e, one);
		}
		return expair(ex_to_numeric(e).mul_dyn(ex_to_numeric(c)), one);
	}
	return expair(e, c);
}
	
expair add::combine_pair_with_coeff_to_pair(const expair & p,
											const ex & c) const
{
	GINAC_ASSERT(is_ex_exactly_of_type(p.coeff,numeric));
	GINAC_ASSERT(is_ex_exactly_of_type(c,numeric));

	if (is_ex_exactly_of_type(p.rest,numeric)) {
		GINAC_ASSERT(ex_to_numeric(p.coeff).is_equal(_num1())); // should be normalized
		return expair(ex_to_numeric(p.rest).mul_dyn(ex_to_numeric(c)),_ex1());
	}

	return expair(p.rest,ex_to_numeric(p.coeff).mul_dyn(ex_to_numeric(c)));
}
	
ex add::recombine_pair_to_ex(const expair & p) const
{
	if (ex_to_numeric(p.coeff).is_equal(_num1()))
		return p.rest;
	else
		return p.rest*p.coeff;
}

ex add::expand(unsigned options) const
{
	if (flags & status_flags::expanded)
		return *this;
	
	epvector * vp = expandchildren(options);
	if (vp==0)
		return *this;
	
	return (new add(vp,overall_coeff))->setflag(status_flags::expanded | status_flags::dynallocated);
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

// protected

unsigned add::precedence = 40;

//////////
// global constants
//////////

const add some_add;
const std::type_info & typeid_add = typeid(some_add);

#ifndef NO_NAMESPACE_GINAC
} // namespace GiNaC
#endif // ndef NO_NAMESPACE_GINAC
