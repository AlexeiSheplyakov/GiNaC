/** @file mul.cpp
 *
 *  Implementation of GiNaC's products of expressions. */

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

#include <vector>
#include <stdexcept>

#include "mul.h"
#include "add.h"
#include "power.h"
#include "matrix.h"
#include "archive.h"
#include "debugmsg.h"
#include "utils.h"

namespace GiNaC {

GINAC_IMPLEMENT_REGISTERED_CLASS(mul, expairseq)

//////////
// default ctor, dctor, copy ctor assignment operator and helpers
//////////

mul::mul()
{
	debugmsg("mul default ctor",LOGLEVEL_CONSTRUCT);
	tinfo_key = TINFO_mul;
}

DEFAULT_COPY(mul)
DEFAULT_DESTROY(mul)

//////////
// other ctors
//////////

// public

mul::mul(const ex & lh, const ex & rh)
{
	debugmsg("mul ctor from ex,ex",LOGLEVEL_CONSTRUCT);
	tinfo_key = TINFO_mul;
	overall_coeff = _ex1();
	construct_from_2_ex(lh,rh);
	GINAC_ASSERT(is_canonical());
}

mul::mul(const exvector & v)
{
	debugmsg("mul ctor from exvector",LOGLEVEL_CONSTRUCT);
	tinfo_key = TINFO_mul;
	overall_coeff = _ex1();
	construct_from_exvector(v);
	GINAC_ASSERT(is_canonical());
}

mul::mul(const epvector & v)
{
	debugmsg("mul ctor from epvector",LOGLEVEL_CONSTRUCT);
	tinfo_key = TINFO_mul;
	overall_coeff = _ex1();
	construct_from_epvector(v);
	GINAC_ASSERT(is_canonical());
}

mul::mul(const epvector & v, const ex & oc)
{
	debugmsg("mul ctor from epvector,ex",LOGLEVEL_CONSTRUCT);
	tinfo_key = TINFO_mul;
	overall_coeff = oc;
	construct_from_epvector(v);
	GINAC_ASSERT(is_canonical());
}

mul::mul(epvector * vp, const ex & oc)
{
	debugmsg("mul ctor from epvector *,ex",LOGLEVEL_CONSTRUCT);
	tinfo_key = TINFO_mul;
	GINAC_ASSERT(vp!=0);
	overall_coeff = oc;
	construct_from_epvector(*vp);
	delete vp;
	GINAC_ASSERT(is_canonical());
}

mul::mul(const ex & lh, const ex & mh, const ex & rh)
{
	debugmsg("mul ctor from ex,ex,ex",LOGLEVEL_CONSTRUCT);
	tinfo_key = TINFO_mul;
	exvector factors;
	factors.reserve(3);
	factors.push_back(lh);
	factors.push_back(mh);
	factors.push_back(rh);
	overall_coeff = _ex1();
	construct_from_exvector(factors);
	GINAC_ASSERT(is_canonical());
}

//////////
// archiving
//////////

DEFAULT_ARCHIVING(mul)

//////////
// functions overriding virtual functions from bases classes
//////////

// public

void mul::print(const print_context & c, unsigned level) const
{
	debugmsg("mul print", LOGLEVEL_PRINT);

	if (is_of_type(c, print_tree)) {

		inherited::print(c, level);

	} else if (is_of_type(c, print_csrc)) {

		if (precedence() <= level)
			c.s << "(";

		if (!overall_coeff.is_equal(_ex1())) {
			overall_coeff.bp->print(c, precedence());
			c.s << "*";
		}

		// Print arguments, separated by "*" or "/"
		epvector::const_iterator it = seq.begin(), itend = seq.end();
		while (it != itend) {

			// If the first argument is a negative integer power, it gets printed as "1.0/<expr>"
			if (it == seq.begin() && ex_to_numeric(it->coeff).is_integer() && it->coeff.compare(_num0()) < 0) {
				if (is_of_type(c, print_csrc_cl_N))
					c.s << "recip(";
				else
					c.s << "1.0/";
			}

			// If the exponent is 1 or -1, it is left out
			if (it->coeff.compare(_ex1()) == 0 || it->coeff.compare(_num_1()) == 0)
				it->rest.print(c, precedence());
			else {
				// Outer parens around ex needed for broken gcc-2.95 parser:
				(ex(power(it->rest, abs(ex_to_numeric(it->coeff))))).print(c, level);
			}

			// Separator is "/" for negative integer powers, "*" otherwise
			++it;
			if (it != itend) {
				if (ex_to_numeric(it->coeff).is_integer() && it->coeff.compare(_num0()) < 0)
					c.s << "/";
				else
					c.s << "*";
			}
		}

		if (precedence() <= level)
			c.s << ")";

	} else {

		if (precedence() <= level) {
			if (is_of_type(c, print_latex))
				c.s << "{(";
			else
				c.s << "(";
		}

		bool first = true;

		// First print the overall numeric coefficient
		numeric coeff = ex_to_numeric(overall_coeff);
		if (coeff.csgn() == -1)
			c.s << '-';
		if (!coeff.is_equal(_num1()) &&
			!coeff.is_equal(_num_1())) {
			if (coeff.is_rational()) {
				if (coeff.is_negative())
					(-coeff).print(c);
				else
					coeff.print(c);
			} else {
				if (coeff.csgn() == -1)
					(-coeff).print(c, precedence());
				else
					coeff.print(c, precedence());
			}
			if (is_of_type(c, print_latex))
				c.s << ' ';
			else
				c.s << '*';
		}

		// Then proceed with the remaining factors
		epvector::const_iterator it = seq.begin(), itend = seq.end();
		while (it != itend) {
			if (!first) {
				if (is_of_type(c, print_latex))
					c.s << ' ';
				else
					c.s << '*';
			} else {
				first = false;
			}
			recombine_pair_to_ex(*it).print(c, precedence());
			it++;
		}

		if (precedence() <= level) {
			if (is_of_type(c, print_latex))
				c.s << ")}";
			else
				c.s << ")";
		}
	}
}

bool mul::info(unsigned inf) const
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

int mul::degree(const ex & s) const
{
	int deg_sum = 0;
	for (epvector::const_iterator cit=seq.begin(); cit!=seq.end(); ++cit) {
		if (ex_to_numeric(cit->coeff).is_integer())
			deg_sum+=cit->rest.degree(s) * ex_to_numeric(cit->coeff).to_int();
	}
	return deg_sum;
}

int mul::ldegree(const ex & s) const
{
	int deg_sum = 0;
	for (epvector::const_iterator cit=seq.begin(); cit!=seq.end(); ++cit) {
		if (ex_to_numeric(cit->coeff).is_integer())
			deg_sum+=cit->rest.ldegree(s) * ex_to_numeric(cit->coeff).to_int();
	}
	return deg_sum;
}

ex mul::coeff(const ex & s, int n) const
{
	exvector coeffseq;
	coeffseq.reserve(seq.size()+1);
	
	if (n==0) {
		// product of individual coeffs
		// if a non-zero power of s is found, the resulting product will be 0
		epvector::const_iterator it = seq.begin();
		while (it!=seq.end()) {
			coeffseq.push_back(recombine_pair_to_ex(*it).coeff(s,n));
			++it;
		}
		coeffseq.push_back(overall_coeff);
		return (new mul(coeffseq))->setflag(status_flags::dynallocated);
	}
	
	epvector::const_iterator it=seq.begin();
	bool coeff_found = 0;
	while (it!=seq.end()) {
		ex t = recombine_pair_to_ex(*it);
		ex c = t.coeff(s,n);
		if (!c.is_zero()) {
			coeffseq.push_back(c);
			coeff_found = 1;
		} else {
			coeffseq.push_back(t);
		}
		++it;
	}
	if (coeff_found) {
		coeffseq.push_back(overall_coeff);
		return (new mul(coeffseq))->setflag(status_flags::dynallocated);
	}
	
	return _ex0();
}

ex mul::eval(int level) const
{
	// simplifications  *(...,x;0) -> 0
	//                  *(+(x,y,...);c) -> *(+(*(x,c),*(y,c),...)) (c numeric())
	//                  *(x;1) -> x
	//                  *(;c) -> c
	
	debugmsg("mul eval",LOGLEVEL_MEMBER_FUNCTION);
	
	epvector * evaled_seqp = evalchildren(level);
	if (evaled_seqp!=0) {
		// do more evaluation later
		return (new mul(evaled_seqp,overall_coeff))->
		           setflag(status_flags::dynallocated);
	}
	
#ifdef DO_GINAC_ASSERT
	for (epvector::const_iterator cit=seq.begin(); cit!=seq.end(); ++cit) {
		GINAC_ASSERT((!is_ex_exactly_of_type((*cit).rest,mul)) ||
		             (!(ex_to_numeric((*cit).coeff).is_integer())));
		GINAC_ASSERT(!(cit->is_canonical_numeric()));
		if (is_ex_exactly_of_type(recombine_pair_to_ex(*cit),numeric))
		    print(print_tree(std::cerr));
		GINAC_ASSERT(!is_ex_exactly_of_type(recombine_pair_to_ex(*cit),numeric));
		/* for paranoia */
		expair p = split_ex_to_pair(recombine_pair_to_ex(*cit));
		GINAC_ASSERT(p.rest.is_equal((*cit).rest));
		GINAC_ASSERT(p.coeff.is_equal((*cit).coeff));
		/* end paranoia */
	}
#endif // def DO_GINAC_ASSERT
	
	if (flags & status_flags::evaluated) {
		GINAC_ASSERT(seq.size()>0);
		GINAC_ASSERT(seq.size()>1 || !overall_coeff.is_equal(_ex1()));
		return *this;
	}
	
	int seq_size = seq.size();
	if (overall_coeff.is_equal(_ex0())) {
		// *(...,x;0) -> 0
		return _ex0();
	} else if (seq_size==0) {
		// *(;c) -> c
		return overall_coeff;
	} else if (seq_size==1 && overall_coeff.is_equal(_ex1())) {
		// *(x;1) -> x
		return recombine_pair_to_ex(*(seq.begin()));
	} else if ((seq_size==1) &&
	           is_ex_exactly_of_type((*seq.begin()).rest,add) &&
	           ex_to_numeric((*seq.begin()).coeff).is_equal(_num1())) {
		// *(+(x,y,...);c) -> +(*(x,c),*(y,c),...) (c numeric(), no powers of +())
		const add & addref = ex_to_add((*seq.begin()).rest);
		epvector distrseq;
		distrseq.reserve(addref.seq.size());
		for (epvector::const_iterator cit=addref.seq.begin(); cit!=addref.seq.end(); ++cit) {
			distrseq.push_back(addref.combine_pair_with_coeff_to_pair(*cit, overall_coeff));
		}
		return (new add(distrseq,
		                ex_to_numeric(addref.overall_coeff).
		                mul_dyn(ex_to_numeric(overall_coeff))))
		      ->setflag(status_flags::dynallocated | status_flags::evaluated);
	}
	return this->hold();
}

ex mul::evalf(int level) const
{
	if (level==1)
		return mul(seq,overall_coeff);
	
	if (level==-max_recursion_level)
		throw(std::runtime_error("max recursion level reached"));
	
	epvector s;
	s.reserve(seq.size());
	
	--level;
	for (epvector::const_iterator it=seq.begin(); it!=seq.end(); ++it) {
		s.push_back(combine_ex_with_coeff_to_pair((*it).rest.evalf(level),
		                                          (*it).coeff));
	}
	return mul(s,overall_coeff.evalf(level));
}

ex mul::evalm(void) const
{
	// numeric*matrix
	if (seq.size() == 1 && is_ex_of_type(seq[0].rest, matrix))
		return ex_to_matrix(seq[0].rest).mul(ex_to_numeric(overall_coeff));

	// Evaluate children first, look whether there are any matrices at all
	// (there can be either no matrices or one matrix; if there were more
	// than one matrix, it would be a non-commutative product)
	epvector *s = new epvector;
	s->reserve(seq.size());

	bool have_matrix = false;
	epvector::iterator the_matrix;

	epvector::const_iterator it = seq.begin(), itend = seq.end();
	while (it != itend) {
		const ex &m = recombine_pair_to_ex(*it).evalm();
		s->push_back(split_ex_to_pair(m));
		if (is_ex_of_type(m, matrix)) {
			have_matrix = true;
			the_matrix = s->end() - 1;
		}
		it++;
	}

	if (have_matrix) {

		// The product contained a matrix. We will multiply all other factors
		// into that matrix.
		matrix m = ex_to_matrix(the_matrix->rest);
		s->erase(the_matrix);
		ex scalar = (new mul(s, overall_coeff))->setflag(status_flags::dynallocated);
		return m.mul_scalar(scalar);

	} else
		return (new mul(s, overall_coeff))->setflag(status_flags::dynallocated);
}

ex mul::simplify_ncmul(const exvector & v) const
{
	if (seq.size()==0) {
		return inherited::simplify_ncmul(v);
	}

	// Find first noncommutative element and call its simplify_ncmul()
	for (epvector::const_iterator cit=seq.begin(); cit!=seq.end(); ++cit) {
		if (cit->rest.return_type() == return_types::noncommutative)
			return cit->rest.simplify_ncmul(v);
	}
	return inherited::simplify_ncmul(v);
}

// protected

/** Implementation of ex::diff() for a product.  It applies the product rule.
 *  @see ex::diff */
ex mul::derivative(const symbol & s) const
{
	exvector addseq;
	addseq.reserve(seq.size());
	
	// D(a*b*c) = D(a)*b*c + a*D(b)*c + a*b*D(c)
	for (unsigned i=0; i!=seq.size(); ++i) {
		epvector mulseq = seq;
		mulseq[i] = split_ex_to_pair(power(seq[i].rest,seq[i].coeff - _ex1()) *
		                             seq[i].rest.diff(s));
		addseq.push_back((new mul(mulseq,overall_coeff*seq[i].coeff))->setflag(status_flags::dynallocated));
	}
	return (new add(addseq))->setflag(status_flags::dynallocated);
}

int mul::compare_same_type(const basic & other) const
{
	return inherited::compare_same_type(other);
}

bool mul::is_equal_same_type(const basic & other) const
{
	return inherited::is_equal_same_type(other);
}

unsigned mul::return_type(void) const
{
	if (seq.size()==0) {
		// mul without factors: should not happen, but commutes
		return return_types::commutative;
	}
	
	bool all_commutative = 1;
	unsigned rt;
	epvector::const_iterator cit_noncommutative_element; // point to first found nc element
	
	for (epvector::const_iterator cit=seq.begin(); cit!=seq.end(); ++cit) {
		rt=(*cit).rest.return_type();
		if (rt==return_types::noncommutative_composite) return rt; // one ncc -> mul also ncc
		if ((rt==return_types::noncommutative)&&(all_commutative)) {
			// first nc element found, remember position
			cit_noncommutative_element = cit;
			all_commutative = 0;
		}
		if ((rt==return_types::noncommutative)&&(!all_commutative)) {
			// another nc element found, compare type_infos
			if ((*cit_noncommutative_element).rest.return_type_tinfo()!=(*cit).rest.return_type_tinfo()) {
				// diffent types -> mul is ncc
				return return_types::noncommutative_composite;
			}
		}
	}
	// all factors checked
	return all_commutative ? return_types::commutative : return_types::noncommutative;
}
   
unsigned mul::return_type_tinfo(void) const
{
	if (seq.size()==0)
		return tinfo_key;  // mul without factors: should not happen
	
	// return type_info of first noncommutative element
	for (epvector::const_iterator cit=seq.begin(); cit!=seq.end(); ++cit) {
		if ((*cit).rest.return_type()==return_types::noncommutative)
			return (*cit).rest.return_type_tinfo();
	}
	// no noncommutative element found, should not happen
	return tinfo_key;
}

ex mul::thisexpairseq(const epvector & v, const ex & oc) const
{
	return (new mul(v,oc))->setflag(status_flags::dynallocated);
}

ex mul::thisexpairseq(epvector * vp, const ex & oc) const
{
	return (new mul(vp,oc))->setflag(status_flags::dynallocated);
}

expair mul::split_ex_to_pair(const ex & e) const
{
	if (is_ex_exactly_of_type(e,power)) {
		const power & powerref = ex_to_power(e);
		if (is_ex_exactly_of_type(powerref.exponent,numeric))
			return expair(powerref.basis,powerref.exponent);
	}
	return expair(e,_ex1());
}
	
expair mul::combine_ex_with_coeff_to_pair(const ex & e,
                                          const ex & c) const
{
	// to avoid duplication of power simplification rules,
	// we create a temporary power object
	// otherwise it would be hard to correctly simplify
	// expression like (4^(1/3))^(3/2)
	if (are_ex_trivially_equal(c,_ex1()))
		return split_ex_to_pair(e);
	
	return split_ex_to_pair(power(e,c));
}
	
expair mul::combine_pair_with_coeff_to_pair(const expair & p,
                                            const ex & c) const
{
	// to avoid duplication of power simplification rules,
	// we create a temporary power object
	// otherwise it would be hard to correctly simplify
	// expression like (4^(1/3))^(3/2)
	if (are_ex_trivially_equal(c,_ex1()))
		return p;
	
	return split_ex_to_pair(power(recombine_pair_to_ex(p),c));
}
	
ex mul::recombine_pair_to_ex(const expair & p) const
{
	if (ex_to_numeric(p.coeff).is_equal(_num1())) 
		return p.rest;
	else
		return power(p.rest,p.coeff);
}

bool mul::expair_needs_further_processing(epp it)
{
	if (is_ex_exactly_of_type((*it).rest,mul) &&
		ex_to_numeric((*it).coeff).is_integer()) {
		// combined pair is product with integer power -> expand it
		*it = split_ex_to_pair(recombine_pair_to_ex(*it));
		return true;
	}
	if (is_ex_exactly_of_type((*it).rest,numeric)) {
		expair ep=split_ex_to_pair(recombine_pair_to_ex(*it));
		if (!ep.is_equal(*it)) {
			// combined pair is a numeric power which can be simplified
			*it = ep;
			return true;
		}
		if (ex_to_numeric((*it).coeff).is_equal(_num1())) {
			// combined pair has coeff 1 and must be moved to the end
			return true;
		}
	}
	return false;
}       

ex mul::default_overall_coeff(void) const
{
	return _ex1();
}

void mul::combine_overall_coeff(const ex & c)
{
	GINAC_ASSERT(is_ex_exactly_of_type(overall_coeff,numeric));
	GINAC_ASSERT(is_ex_exactly_of_type(c,numeric));
	overall_coeff = ex_to_numeric(overall_coeff).mul_dyn(ex_to_numeric(c));
}

void mul::combine_overall_coeff(const ex & c1, const ex & c2)
{
	GINAC_ASSERT(is_ex_exactly_of_type(overall_coeff,numeric));
	GINAC_ASSERT(is_ex_exactly_of_type(c1,numeric));
	GINAC_ASSERT(is_ex_exactly_of_type(c2,numeric));
	overall_coeff = ex_to_numeric(overall_coeff).mul_dyn(ex_to_numeric(c1).power(ex_to_numeric(c2)));
}

bool mul::can_make_flat(const expair & p) const
{
	GINAC_ASSERT(is_ex_exactly_of_type(p.coeff,numeric));
	// this assertion will probably fail somewhere
	// it would require a more careful make_flat, obeying the power laws
	// probably should return true only if p.coeff is integer
	return ex_to_numeric(p.coeff).is_equal(_num1());
}

ex mul::expand(unsigned options) const
{
	if (flags & status_flags::expanded)
		return *this;
	
	exvector sub_expanded_seq;
	
	epvector * expanded_seqp = expandchildren(options);
	
	const epvector & expanded_seq = expanded_seqp==0 ? seq : *expanded_seqp;
	
	int number_of_adds = 0;
	epvector non_adds;
	non_adds.reserve(expanded_seq.size());
	epvector::const_iterator cit = expanded_seq.begin();
	epvector::const_iterator last = expanded_seq.end();
	ex last_expanded = _ex1();
	while (cit!=last) {
		if (is_ex_exactly_of_type((*cit).rest,add) &&
			((*cit).coeff.is_equal(_ex1()))) {
			++number_of_adds;
			if (is_ex_exactly_of_type(last_expanded,add)) {
				// expand adds
				const add & add1 = ex_to_add(last_expanded);
				const add & add2 = ex_to_add((*cit).rest);
				int n1 = add1.nops();
				int n2 = add2.nops();
				exvector distrseq;
				distrseq.reserve(n1*n2);
				for (int i1=0; i1<n1; ++i1) {
					for (int i2=0; i2<n2; ++i2) {
						distrseq.push_back(add1.op(i1)*add2.op(i2));
					}
				}
				last_expanded = (new add(distrseq))->setflag(status_flags::dynallocated | status_flags::expanded);
			} else {
				non_adds.push_back(split_ex_to_pair(last_expanded));
				last_expanded = (*cit).rest;
			}
		} else {
			non_adds.push_back(*cit);
		}
		++cit;
	}
	if (expanded_seqp)
		delete expanded_seqp;

	if (is_ex_exactly_of_type(last_expanded,add)) {
		add const & finaladd = ex_to_add(last_expanded);
		exvector distrseq;
		int n = finaladd.nops();
		distrseq.reserve(n);
		for (int i=0; i<n; ++i) {
			epvector factors = non_adds;
			factors.push_back(split_ex_to_pair(finaladd.op(i)));
			distrseq.push_back((new mul(factors,overall_coeff))->setflag(status_flags::dynallocated | status_flags::expanded));
		}
		return ((new add(distrseq))->
		        setflag(status_flags::dynallocated | status_flags::expanded));
	}
	non_adds.push_back(split_ex_to_pair(last_expanded));
	return (new mul(non_adds,overall_coeff))->
	        setflag(status_flags::dynallocated | status_flags::expanded);
}

  
//////////
// new virtual functions which can be overridden by derived classes
//////////

// none

//////////
// non-virtual functions in this class
//////////


/** Member-wise expand the expairs representing this sequence.  This must be
 *  overridden from expairseq::expandchildren() and done iteratively in order
 *  to allow for early cancallations and thus safe memory.
 *
 *  @see mul::expand()
 *  @return pointer to epvector containing expanded representation or zero
 *  pointer, if sequence is unchanged. */
epvector * mul::expandchildren(unsigned options) const
{
	epvector::const_iterator last = seq.end();
	epvector::const_iterator cit = seq.begin();
	while (cit!=last) {
		const ex & factor = recombine_pair_to_ex(*cit);
		const ex & expanded_factor = factor.expand(options);
		if (!are_ex_trivially_equal(factor,expanded_factor)) {
			
			// something changed, copy seq, eval and return it
			epvector *s = new epvector;
			s->reserve(seq.size());
			
			// copy parts of seq which are known not to have changed
			epvector::const_iterator cit2 = seq.begin();
			while (cit2!=cit) {
				s->push_back(*cit2);
				++cit2;
			}
			// copy first changed element
			s->push_back(split_ex_to_pair(expanded_factor));
			++cit2;
			// copy rest
			while (cit2!=last) {
				s->push_back(split_ex_to_pair(recombine_pair_to_ex(*cit2).expand(options)));
				++cit2;
			}
			return s;
		}
		++cit;
	}
	
	return 0; // nothing has changed
}

} // namespace GiNaC
