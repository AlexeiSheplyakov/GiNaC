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

#include <iostream>
#include <vector>
#include <stdexcept>

#include "mul.h"
#include "add.h"
#include "power.h"
#include "matrix.h"
#include "archive.h"
#include "utils.h"

namespace GiNaC {

GINAC_IMPLEMENT_REGISTERED_CLASS(mul, expairseq)

//////////
// default ctor, dtor, copy ctor, assignment operator and helpers
//////////

mul::mul()
{
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
	tinfo_key = TINFO_mul;
	overall_coeff = _ex1;
	construct_from_2_ex(lh,rh);
	GINAC_ASSERT(is_canonical());
}

mul::mul(const exvector & v)
{
	tinfo_key = TINFO_mul;
	overall_coeff = _ex1;
	construct_from_exvector(v);
	GINAC_ASSERT(is_canonical());
}

mul::mul(const epvector & v)
{
	tinfo_key = TINFO_mul;
	overall_coeff = _ex1;
	construct_from_epvector(v);
	GINAC_ASSERT(is_canonical());
}

mul::mul(const epvector & v, const ex & oc)
{
	tinfo_key = TINFO_mul;
	overall_coeff = oc;
	construct_from_epvector(v);
	GINAC_ASSERT(is_canonical());
}

mul::mul(epvector * vp, const ex & oc)
{
	tinfo_key = TINFO_mul;
	GINAC_ASSERT(vp!=0);
	overall_coeff = oc;
	construct_from_epvector(*vp);
	delete vp;
	GINAC_ASSERT(is_canonical());
}

mul::mul(const ex & lh, const ex & mh, const ex & rh)
{
	tinfo_key = TINFO_mul;
	exvector factors;
	factors.reserve(3);
	factors.push_back(lh);
	factors.push_back(mh);
	factors.push_back(rh);
	overall_coeff = _ex1;
	construct_from_exvector(factors);
	GINAC_ASSERT(is_canonical());
}

//////////
// archiving
//////////

DEFAULT_ARCHIVING(mul)

//////////
// functions overriding virtual functions from base classes
//////////

// public

void mul::print(const print_context & c, unsigned level) const
{
	if (is_a<print_tree>(c)) {

		inherited::print(c, level);

	} else if (is_a<print_csrc>(c)) {

		if (precedence() <= level)
			c.s << "(";

		if (!overall_coeff.is_equal(_ex1)) {
			overall_coeff.print(c, precedence());
			c.s << "*";
		}

		// Print arguments, separated by "*" or "/"
		epvector::const_iterator it = seq.begin(), itend = seq.end();
		while (it != itend) {

			// If the first argument is a negative integer power, it gets printed as "1.0/<expr>"
			if (it == seq.begin() && ex_to<numeric>(it->coeff).is_integer() && it->coeff.compare(_num0) < 0) {
				if (is_a<print_csrc_cl_N>(c))
					c.s << "recip(";
				else
					c.s << "1.0/";
			}

			// If the exponent is 1 or -1, it is left out
			if (it->coeff.compare(_ex1) == 0 || it->coeff.compare(_num_1) == 0)
				it->rest.print(c, precedence());
			else {
				// Outer parens around ex needed for broken gcc-2.95 parser:
				(ex(power(it->rest, abs(ex_to<numeric>(it->coeff))))).print(c, level);
			}

			// Separator is "/" for negative integer powers, "*" otherwise
			++it;
			if (it != itend) {
				if (ex_to<numeric>(it->coeff).is_integer() && it->coeff.compare(_num0) < 0)
					c.s << "/";
				else
					c.s << "*";
			}
		}

		if (precedence() <= level)
			c.s << ")";

	} else if (is_a<print_python_repr>(c)) {
		c.s << class_name() << '(';
		unsigned end = nops();
		if (end)
			op(0).print(c);
		for (unsigned i=1; i<end; ++i) {
			c.s << ',';
			op(i).print(c);
		}
		c.s << ')';
	} else {

		if (precedence() <= level) {
			if (is_a<print_latex>(c))
				c.s << "{(";
			else
				c.s << "(";
		}

		bool first = true;

		// First print the overall numeric coefficient
		numeric coeff = ex_to<numeric>(overall_coeff);
		if (coeff.csgn() == -1)
			c.s << '-';
		if (!coeff.is_equal(_num1) &&
			!coeff.is_equal(_num_1)) {
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
			if (is_a<print_latex>(c))
				c.s << ' ';
			else
				c.s << '*';
		}

		// Then proceed with the remaining factors
		epvector::const_iterator it = seq.begin(), itend = seq.end();
		while (it != itend) {
			if (!first) {
				if (is_a<print_latex>(c))
					c.s << ' ';
				else
					c.s << '*';
			} else {
				first = false;
			}
			recombine_pair_to_ex(*it).print(c, precedence());
			++it;
		}

		if (precedence() <= level) {
			if (is_a<print_latex>(c))
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
			epvector::const_iterator i = seq.begin(), end = seq.end();
			while (i != end) {
				if (!(recombine_pair_to_ex(*i).info(inf)))
					return false;
				++i;
			}
			return overall_coeff.info(inf);
		}
		case info_flags::algebraic: {
			epvector::const_iterator i = seq.begin(), end = seq.end();
			while (i != end) {
				if ((recombine_pair_to_ex(*i).info(inf)))
					return true;
				++i;
			}
			return false;
		}
	}
	return inherited::info(inf);
}

int mul::degree(const ex & s) const
{
	// Sum up degrees of factors
	int deg_sum = 0;
	epvector::const_iterator i = seq.begin(), end = seq.end();
	while (i != end) {
		if (ex_to<numeric>(i->coeff).is_integer())
			deg_sum += i->rest.degree(s) * ex_to<numeric>(i->coeff).to_int();
		++i;
	}
	return deg_sum;
}

int mul::ldegree(const ex & s) const
{
	// Sum up degrees of factors
	int deg_sum = 0;
	epvector::const_iterator i = seq.begin(), end = seq.end();
	while (i != end) {
		if (ex_to<numeric>(i->coeff).is_integer())
			deg_sum += i->rest.ldegree(s) * ex_to<numeric>(i->coeff).to_int();
		++i;
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
		epvector::const_iterator i = seq.begin(), end = seq.end();
		while (i != end) {
			coeffseq.push_back(recombine_pair_to_ex(*i).coeff(s,n));
			++i;
		}
		coeffseq.push_back(overall_coeff);
		return (new mul(coeffseq))->setflag(status_flags::dynallocated);
	}
	
	epvector::const_iterator i = seq.begin(), end = seq.end();
	bool coeff_found = false;
	while (i != end) {
		ex t = recombine_pair_to_ex(*i);
		ex c = t.coeff(s, n);
		if (!c.is_zero()) {
			coeffseq.push_back(c);
			coeff_found = 1;
		} else {
			coeffseq.push_back(t);
		}
		++i;
	}
	if (coeff_found) {
		coeffseq.push_back(overall_coeff);
		return (new mul(coeffseq))->setflag(status_flags::dynallocated);
	}
	
	return _ex0;
}

/** Perform automatic term rewriting rules in this class.  In the following
 *  x, x1, x2,... stand for a symbolic variables of type ex and c, c1, c2...
 *  stand for such expressions that contain a plain number.
 *  - *(...,x;0) -> 0
 *  - *(+(x1,x2,...);c) -> *(+(*(x1,c),*(x2,c),...))
 *  - *(x;1) -> x
 *  - *(;c) -> c
 *
 *  @param level cut-off in recursive evaluation */
ex mul::eval(int level) const
{
	epvector *evaled_seqp = evalchildren(level);
	if (evaled_seqp) {
		// do more evaluation later
		return (new mul(evaled_seqp,overall_coeff))->
		           setflag(status_flags::dynallocated);
	}
	
#ifdef DO_GINAC_ASSERT
	epvector::const_iterator i = seq.begin(), end = seq.end();
	while (i != end) {
		GINAC_ASSERT((!is_exactly_a<mul>(i->rest)) ||
		             (!(ex_to<numeric>(i->coeff).is_integer())));
		GINAC_ASSERT(!(i->is_canonical_numeric()));
		if (is_ex_exactly_of_type(recombine_pair_to_ex(*i), numeric))
		    print(print_tree(std::cerr));
		GINAC_ASSERT(!is_exactly_a<numeric>(recombine_pair_to_ex(*i)));
		/* for paranoia */
		expair p = split_ex_to_pair(recombine_pair_to_ex(*i));
		GINAC_ASSERT(p.rest.is_equal(i->rest));
		GINAC_ASSERT(p.coeff.is_equal(i->coeff));
		/* end paranoia */
		++i;
	}
#endif // def DO_GINAC_ASSERT
	
	if (flags & status_flags::evaluated) {
		GINAC_ASSERT(seq.size()>0);
		GINAC_ASSERT(seq.size()>1 || !overall_coeff.is_equal(_ex1));
		return *this;
	}
	
	int seq_size = seq.size();
	if (overall_coeff.is_zero()) {
		// *(...,x;0) -> 0
		return _ex0;
	} else if (seq_size==0) {
		// *(;c) -> c
		return overall_coeff;
	} else if (seq_size==1 && overall_coeff.is_equal(_ex1)) {
		// *(x;1) -> x
		return recombine_pair_to_ex(*(seq.begin()));
	} else if ((seq_size==1) &&
	           is_ex_exactly_of_type((*seq.begin()).rest,add) &&
	           ex_to<numeric>((*seq.begin()).coeff).is_equal(_num1)) {
		// *(+(x,y,...);c) -> +(*(x,c),*(y,c),...) (c numeric(), no powers of +())
		const add & addref = ex_to<add>((*seq.begin()).rest);
		epvector *distrseq = new epvector();
		distrseq->reserve(addref.seq.size());
		epvector::const_iterator i = addref.seq.begin(), end = addref.seq.end();
		while (i != end) {
			distrseq->push_back(addref.combine_pair_with_coeff_to_pair(*i, overall_coeff));
			++i;
		}
		return (new add(distrseq,
		                ex_to<numeric>(addref.overall_coeff).
		                mul_dyn(ex_to<numeric>(overall_coeff))))
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
	
	epvector *s = new epvector();
	s->reserve(seq.size());

	--level;
	epvector::const_iterator i = seq.begin(), end = seq.end();
	while (i != end) {
		s->push_back(combine_ex_with_coeff_to_pair(i->rest.evalf(level),
		                                           i->coeff));
		++i;
	}
	return mul(s, overall_coeff.evalf(level));
}

ex mul::evalm(void) const
{
	// numeric*matrix
	if (seq.size() == 1 && seq[0].coeff.is_equal(_ex1)
	 && is_ex_of_type(seq[0].rest, matrix))
		return ex_to<matrix>(seq[0].rest).mul(ex_to<numeric>(overall_coeff));

	// Evaluate children first, look whether there are any matrices at all
	// (there can be either no matrices or one matrix; if there were more
	// than one matrix, it would be a non-commutative product)
	epvector *s = new epvector;
	s->reserve(seq.size());

	bool have_matrix = false;
	epvector::iterator the_matrix;

	epvector::const_iterator i = seq.begin(), end = seq.end();
	while (i != end) {
		const ex &m = recombine_pair_to_ex(*i).evalm();
		s->push_back(split_ex_to_pair(m));
		if (is_ex_of_type(m, matrix)) {
			have_matrix = true;
			the_matrix = s->end() - 1;
		}
		++i;
	}

	if (have_matrix) {

		// The product contained a matrix. We will multiply all other factors
		// into that matrix.
		matrix m = ex_to<matrix>(the_matrix->rest);
		s->erase(the_matrix);
		ex scalar = (new mul(s, overall_coeff))->setflag(status_flags::dynallocated);
		return m.mul_scalar(scalar);

	} else
		return (new mul(s, overall_coeff))->setflag(status_flags::dynallocated);
}

ex mul::simplify_ncmul(const exvector & v) const
{
	if (seq.empty())
		return inherited::simplify_ncmul(v);

	// Find first noncommutative element and call its simplify_ncmul()
	epvector::const_iterator i = seq.begin(), end = seq.end();
	while (i != end) {
		if (i->rest.return_type() == return_types::noncommutative)
			return i->rest.simplify_ncmul(v);
		++i;
	}
	return inherited::simplify_ncmul(v);
}

// protected

/** Implementation of ex::diff() for a product.  It applies the product rule.
 *  @see ex::diff */
ex mul::derivative(const symbol & s) const
{
	unsigned num = seq.size();
	exvector addseq;
	addseq.reserve(num);
	
	// D(a*b*c) = D(a)*b*c + a*D(b)*c + a*b*D(c)
	epvector mulseq = seq;
	epvector::const_iterator i = seq.begin(), end = seq.end();
	epvector::iterator i2 = mulseq.begin();
	while (i != end) {
		expair ep = split_ex_to_pair(power(i->rest, i->coeff - _ex1) *
		                             i->rest.diff(s));
		ep.swap(*i2);
		addseq.push_back((new mul(mulseq, overall_coeff * i->coeff))->setflag(status_flags::dynallocated));
		ep.swap(*i2);
		++i; ++i2;
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
	if (seq.empty()) {
		// mul without factors: should not happen, but commutes
		return return_types::commutative;
	}
	
	bool all_commutative = true;
	epvector::const_iterator noncommutative_element; // point to first found nc element
	
	epvector::const_iterator i = seq.begin(), end = seq.end();
	while (i != end) {
		unsigned rt = i->rest.return_type();
		if (rt == return_types::noncommutative_composite)
			return rt; // one ncc -> mul also ncc
		if ((rt == return_types::noncommutative) && (all_commutative)) {
			// first nc element found, remember position
			noncommutative_element = i;
			all_commutative = false;
		}
		if ((rt == return_types::noncommutative) && (!all_commutative)) {
			// another nc element found, compare type_infos
			if (noncommutative_element->rest.return_type_tinfo() != i->rest.return_type_tinfo()) {
				// diffent types -> mul is ncc
				return return_types::noncommutative_composite;
			}
		}
		++i;
	}
	// all factors checked
	return all_commutative ? return_types::commutative : return_types::noncommutative;
}
   
unsigned mul::return_type_tinfo(void) const
{
	if (seq.empty())
		return tinfo_key;  // mul without factors: should not happen
	
	// return type_info of first noncommutative element
	epvector::const_iterator i = seq.begin(), end = seq.end();
	while (i != end) {
		if (i->rest.return_type() == return_types::noncommutative)
			return i->rest.return_type_tinfo();
		++i;
	}
	// no noncommutative element found, should not happen
	return tinfo_key;
}

ex mul::thisexpairseq(const epvector & v, const ex & oc) const
{
	return (new mul(v, oc))->setflag(status_flags::dynallocated);
}

ex mul::thisexpairseq(epvector * vp, const ex & oc) const
{
	return (new mul(vp, oc))->setflag(status_flags::dynallocated);
}

expair mul::split_ex_to_pair(const ex & e) const
{
	if (is_ex_exactly_of_type(e,power)) {
		const power & powerref = ex_to<power>(e);
		if (is_ex_exactly_of_type(powerref.exponent,numeric))
			return expair(powerref.basis,powerref.exponent);
	}
	return expair(e,_ex1);
}
	
expair mul::combine_ex_with_coeff_to_pair(const ex & e,
                                          const ex & c) const
{
	// to avoid duplication of power simplification rules,
	// we create a temporary power object
	// otherwise it would be hard to correctly simplify
	// expression like (4^(1/3))^(3/2)
	if (are_ex_trivially_equal(c,_ex1))
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
	if (are_ex_trivially_equal(c,_ex1))
		return p;
	
	return split_ex_to_pair(power(recombine_pair_to_ex(p),c));
}
	
ex mul::recombine_pair_to_ex(const expair & p) const
{
	if (ex_to<numeric>(p.coeff).is_equal(_num1)) 
		return p.rest;
	else
		return power(p.rest,p.coeff);
}

bool mul::expair_needs_further_processing(epp it)
{
	if (is_ex_exactly_of_type((*it).rest,mul) &&
		ex_to<numeric>((*it).coeff).is_integer()) {
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
		if (ex_to<numeric>((*it).coeff).is_equal(_num1)) {
			// combined pair has coeff 1 and must be moved to the end
			return true;
		}
	}
	return false;
}       

ex mul::default_overall_coeff(void) const
{
	return _ex1;
}

void mul::combine_overall_coeff(const ex & c)
{
	GINAC_ASSERT(is_exactly_a<numeric>(overall_coeff));
	GINAC_ASSERT(is_exactly_a<numeric>(c));
	overall_coeff = ex_to<numeric>(overall_coeff).mul_dyn(ex_to<numeric>(c));
}

void mul::combine_overall_coeff(const ex & c1, const ex & c2)
{
	GINAC_ASSERT(is_exactly_a<numeric>(overall_coeff));
	GINAC_ASSERT(is_exactly_a<numeric>(c1));
	GINAC_ASSERT(is_exactly_a<numeric>(c2));
	overall_coeff = ex_to<numeric>(overall_coeff).mul_dyn(ex_to<numeric>(c1).power(ex_to<numeric>(c2)));
}

bool mul::can_make_flat(const expair & p) const
{
	GINAC_ASSERT(is_exactly_a<numeric>(p.coeff));
	// this assertion will probably fail somewhere
	// it would require a more careful make_flat, obeying the power laws
	// probably should return true only if p.coeff is integer
	return ex_to<numeric>(p.coeff).is_equal(_num1);
}

ex mul::expand(unsigned options) const
{
	// First, expand the children
	epvector * expanded_seqp = expandchildren(options);
	const epvector & expanded_seq = (expanded_seqp == NULL) ? seq : *expanded_seqp;

	// Now, look for all the factors that are sums and multiply each one out
	// with the next one that is found while collecting the factors which are
	// not sums
	int number_of_adds = 0;
	ex last_expanded = _ex1;
	epvector non_adds;
	non_adds.reserve(expanded_seq.size());
	epvector::const_iterator cit = expanded_seq.begin(), last = expanded_seq.end();
	while (cit != last) {
		if (is_ex_exactly_of_type(cit->rest, add) &&
			(cit->coeff.is_equal(_ex1))) {
			++number_of_adds;
			if (is_ex_exactly_of_type(last_expanded, add)) {
				const add & add1 = ex_to<add>(last_expanded);
				const add & add2 = ex_to<add>(cit->rest);
				int n1 = add1.nops();
				int n2 = add2.nops();
				exvector distrseq;
				distrseq.reserve(n1*n2);
				for (int i1=0; i1<n1; ++i1) {
					for (int i2=0; i2<n2; ++i2) {
						distrseq.push_back(add1.op(i1) * add2.op(i2));
					}
				}
				last_expanded = (new add(distrseq))->
				                 setflag(status_flags::dynallocated | (options == 0 ? status_flags::expanded : 0));
			} else {
				non_adds.push_back(split_ex_to_pair(last_expanded));
				last_expanded = cit->rest;
			}
		} else {
			non_adds.push_back(*cit);
		}
		++cit;
	}
	if (expanded_seqp)
		delete expanded_seqp;
	
	// Now the only remaining thing to do is to multiply the factors which
	// were not sums into the "last_expanded" sum
	if (is_ex_exactly_of_type(last_expanded, add)) {
		const add & finaladd = ex_to<add>(last_expanded);
		exvector distrseq;
		int n = finaladd.nops();
		distrseq.reserve(n);
		for (int i=0; i<n; ++i) {
			epvector factors = non_adds;
			factors.push_back(split_ex_to_pair(finaladd.op(i)));
			distrseq.push_back((new mul(factors, overall_coeff))->
			                    setflag(status_flags::dynallocated | (options == 0 ? status_flags::expanded : 0)));
		}
		return ((new add(distrseq))->
		        setflag(status_flags::dynallocated | (options == 0 ? status_flags::expanded : 0)));
	}
	non_adds.push_back(split_ex_to_pair(last_expanded));
	return (new mul(non_adds, overall_coeff))->
	        setflag(status_flags::dynallocated | (options == 0 ? status_flags::expanded : 0));
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
	const epvector::const_iterator last = seq.end();
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
