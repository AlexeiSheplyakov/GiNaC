/** @file clifford.cpp
 *
 *  Implementation of GiNaC's clifford algebra (Dirac gamma) objects. */

/*
 *  GiNaC Copyright (C) 1999-2004 Johannes Gutenberg University Mainz, Germany
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

#include "clifford.h"

#include "ex.h"
#include "idx.h"
#include "ncmul.h"
#include "symbol.h"
#include "numeric.h" // for I
#include "symmetry.h"
#include "lst.h"
#include "relational.h"
#include "operators.h"
#include "add.h"
#include "mul.h"
#include "power.h"
#include "matrix.h"
#include "archive.h"
#include "utils.h"

namespace GiNaC {

GINAC_IMPLEMENT_REGISTERED_CLASS_OPT(clifford, indexed,
  print_func<print_dflt>(&clifford::do_print_dflt).
  print_func<print_latex>(&clifford::do_print_latex))

GINAC_IMPLEMENT_REGISTERED_CLASS_OPT(diracone, tensor,
  print_func<print_dflt>(&diracone::do_print).
  print_func<print_latex>(&diracone::do_print_latex))

GINAC_IMPLEMENT_REGISTERED_CLASS_OPT(cliffordunit, tensor,
  print_func<print_dflt>(&cliffordunit::do_print).
  print_func<print_latex>(&cliffordunit::do_print_latex))

GINAC_IMPLEMENT_REGISTERED_CLASS_OPT(diracgamma, cliffordunit,
  print_func<print_dflt>(&diracgamma::do_print).
  print_func<print_latex>(&diracgamma::do_print_latex))

GINAC_IMPLEMENT_REGISTERED_CLASS_OPT(diracgamma5, tensor,
  print_func<print_dflt>(&diracgamma5::do_print).
  print_func<print_latex>(&diracgamma5::do_print_latex))

GINAC_IMPLEMENT_REGISTERED_CLASS_OPT(diracgammaL, tensor,
  print_func<print_context>(&diracgammaL::do_print).
  print_func<print_latex>(&diracgammaL::do_print_latex))

GINAC_IMPLEMENT_REGISTERED_CLASS_OPT(diracgammaR, tensor,
  print_func<print_context>(&diracgammaR::do_print).
  print_func<print_latex>(&diracgammaR::do_print_latex))

//////////
// default constructors
//////////

clifford::clifford() : representation_label(0), metric(lorentz_g(varidx((new symbol)->setflag(status_flags::dynallocated), 4), varidx((new symbol)->setflag(status_flags::dynallocated), 4)))
{
	tinfo_key = TINFO_clifford;
}

DEFAULT_CTOR(diracone)
DEFAULT_CTOR(cliffordunit)
DEFAULT_CTOR(diracgamma)
DEFAULT_CTOR(diracgamma5)
DEFAULT_CTOR(diracgammaL)
DEFAULT_CTOR(diracgammaR)

//////////
// other constructors
//////////

/** Construct object without any indices. This constructor is for internal
 *  use only. Use the dirac_ONE() function instead.
 *  @see dirac_ONE */
clifford::clifford(const ex & b, unsigned char rl) : inherited(b), representation_label(rl), metric(0)
{
	tinfo_key = TINFO_clifford;
}

/** Construct object with one Lorentz index. This constructor is for internal
 *  use only. Use the clifford_unit() or dirac_gamma() functions instead.
 *  @see clifford_unit
 *  @see dirac_gamma */
clifford::clifford(const ex & b, const ex & mu, const ex & metr, unsigned char rl) : inherited(b, mu), representation_label(rl), metric(metr)
{
	GINAC_ASSERT(is_a<varidx>(mu));
	tinfo_key = TINFO_clifford;
}

clifford::clifford(unsigned char rl, const ex & metr, const exvector & v, bool discardable) : inherited(not_symmetric(), v, discardable), representation_label(rl), metric(metr)
{
	tinfo_key = TINFO_clifford;
}

clifford::clifford(unsigned char rl, const ex & metr, std::auto_ptr<exvector> vp) : inherited(not_symmetric(), vp), representation_label(rl), metric(metr)
{
	tinfo_key = TINFO_clifford;
}

//////////
// archiving
//////////

clifford::clifford(const archive_node &n, lst &sym_lst) : inherited(n, sym_lst)
{
	unsigned rl;
	n.find_unsigned("label", rl);
	representation_label = rl;
	n.find_ex("metric", metric, sym_lst);
}

void clifford::archive(archive_node &n) const
{
	inherited::archive(n);
	n.add_unsigned("label", representation_label);
	n.add_ex("metric", metric);
}

DEFAULT_UNARCHIVE(clifford)
DEFAULT_ARCHIVING(diracone)
DEFAULT_ARCHIVING(cliffordunit)
DEFAULT_ARCHIVING(diracgamma)
DEFAULT_ARCHIVING(diracgamma5)
DEFAULT_ARCHIVING(diracgammaL)
DEFAULT_ARCHIVING(diracgammaR)

//////////
// functions overriding virtual functions from base classes
//////////

ex clifford::get_metric(const ex & i, const ex & j) const
{
	return metric.subs(metric.op(1) == i).subs(metric.op(2) == j);
}

bool clifford::same_metric(const ex & other) const
{
	if (is_a<clifford>(other)) {
		ex m = get_metric();
		return m.is_equal(ex_to<clifford>(other).get_metric(m.op(1), m.op(2)));
	} else if (is_a<indexed>(other)) {
		ex m = get_metric(other.op(1), other.op(2));
		return m.is_equal(other);
	} else
		return false;
}

int clifford::compare_same_type(const basic & other) const
{
	GINAC_ASSERT(is_a<clifford>(other));
	const clifford &o = static_cast<const clifford &>(other);

	if (representation_label != o.representation_label) {
		// different representation label
		return representation_label < o.representation_label ? -1 : 1;
	}

	return inherited::compare_same_type(other);
}

bool clifford::match_same_type(const basic & other) const
{
	GINAC_ASSERT(is_a<clifford>(other));
	const clifford &o = static_cast<const clifford &>(other);

	return (representation_label == o.representation_label) && same_metric(o);
}

static bool is_dirac_slash(const ex & seq0)
{
	return !is_a<diracgamma5>(seq0) && !is_a<diracgammaL>(seq0) &&
	       !is_a<diracgammaR>(seq0) && !is_a<cliffordunit>(seq0) &&
	       !is_a<diracone>(seq0);
}

void clifford::do_print_dflt(const print_dflt & c, unsigned level) const
{
	// dirac_slash() object is printed differently
	if (is_dirac_slash(seq[0])) {
		seq[0].print(c, level);
		c.s << "\\";
	} else
		this->print_dispatch<inherited>(c, level);
}

void clifford::do_print_latex(const print_latex & c, unsigned level) const
{
	// dirac_slash() object is printed differently
	if (is_dirac_slash(seq[0])) {
		c.s << "{";
		seq[0].print(c, level);
		c.s << "\\hspace{-1.0ex}/}";
	} else
		this->print_dispatch<inherited>(c, level);
}

DEFAULT_COMPARE(diracone)
DEFAULT_COMPARE(cliffordunit)
DEFAULT_COMPARE(diracgamma)
DEFAULT_COMPARE(diracgamma5)
DEFAULT_COMPARE(diracgammaL)
DEFAULT_COMPARE(diracgammaR)

DEFAULT_PRINT_LATEX(diracone, "ONE", "\\mathbb{1}")
DEFAULT_PRINT_LATEX(cliffordunit, "e", "e")
DEFAULT_PRINT_LATEX(diracgamma, "gamma", "\\gamma")
DEFAULT_PRINT_LATEX(diracgamma5, "gamma5", "{\\gamma^5}")
DEFAULT_PRINT_LATEX(diracgammaL, "gammaL", "{\\gamma_L}")
DEFAULT_PRINT_LATEX(diracgammaR, "gammaR", "{\\gamma_R}")

/** This function decomposes gamma~mu -> (1, mu) and a\ -> (a.ix, ix) */
static void base_and_index(const ex & c, ex & b, ex & i)
{
	GINAC_ASSERT(is_a<clifford>(c));
	GINAC_ASSERT(c.nops() == 2);

	if (is_a<cliffordunit>(c.op(0))) { // proper dirac gamma object or clifford unit
		i = c.op(1);
		b = _ex1;
	} else if (is_a<diracgamma5>(c.op(0)) || is_a<diracgammaL>(c.op(0)) || is_a<diracgammaR>(c.op(0))) { // gamma5/L/R
		i = _ex0;
		b = _ex1;
	} else { // slash object, generate new dummy index
		varidx ix((new symbol)->setflag(status_flags::dynallocated), ex_to<idx>(c.op(1)).get_dim());
		b = indexed(c.op(0), ix.toggle_variance());
		i = ix;
	}
}

/** Contraction of a gamma matrix with something else. */
bool diracgamma::contract_with(exvector::iterator self, exvector::iterator other, exvector & v) const
{
	GINAC_ASSERT(is_a<clifford>(*self));
	GINAC_ASSERT(is_a<indexed>(*other));
	GINAC_ASSERT(is_a<diracgamma>(self->op(0)));
	unsigned char rl = ex_to<clifford>(*self).get_representation_label();

	ex dim = ex_to<idx>(self->op(1)).get_dim();
	if (other->nops() > 1)
		dim = minimal_dim(dim, ex_to<idx>(other->op(1)).get_dim());

	if (is_a<clifford>(*other)) {

		// Contraction only makes sense if the represenation labels are equal
		if (ex_to<clifford>(*other).get_representation_label() != rl)
			return false;

		// gamma~mu gamma.mu = dim ONE
		if (other - self == 1) {
			*self = dim;
			*other = dirac_ONE(rl);
			return true;

		// gamma~mu gamma~alpha gamma.mu = (2-dim) gamma~alpha
		} else if (other - self == 2
		        && is_a<clifford>(self[1])) {
			*self = 2 - dim;
			*other = _ex1;
			return true;

		// gamma~mu gamma~alpha gamma~beta gamma.mu = 4 g~alpha~beta + (dim-4) gamam~alpha gamma~beta
		} else if (other - self == 3
		        && is_a<clifford>(self[1])
		        && is_a<clifford>(self[2])) {
			ex b1, i1, b2, i2;
			base_and_index(self[1], b1, i1);
			base_and_index(self[2], b2, i2);
			*self = 4 * lorentz_g(i1, i2) * b1 * b2 * dirac_ONE(rl) + (dim - 4) * self[1] * self[2];
			self[1] = _ex1;
			self[2] = _ex1;
			*other = _ex1;
			return true;

		// gamma~mu gamma~alpha gamma~beta gamma~delta gamma.mu = -2 gamma~delta gamma~beta gamma~alpha - (dim-4) gamam~alpha gamma~beta gamma~delta
		} else if (other - self == 4
		        && is_a<clifford>(self[1])
		        && is_a<clifford>(self[2])
		        && is_a<clifford>(self[3])) {
			*self = -2 * self[3] * self[2] * self[1] - (dim - 4) * self[1] * self[2] * self[3];
			self[1] = _ex1;
			self[2] = _ex1;
			self[3] = _ex1;
			*other = _ex1;
			return true;

		// gamma~mu S gamma~alpha gamma.mu = 2 gamma~alpha S - gamma~mu S gamma.mu gamma~alpha
		// (commutate contracted indices towards each other, simplify_indexed()
		// will re-expand and re-run the simplification)
		} else {
			exvector::iterator it = self + 1, next_to_last = other - 1;
			while (it != other) {
				if (!is_a<clifford>(*it))
					return false;
				++it;
			}

			it = self + 1;
			ex S = _ex1;
			while (it != next_to_last) {
				S *= *it;
				*it++ = _ex1;
			}

			*self = 2 * (*next_to_last) * S - (*self) * S * (*other) * (*next_to_last);
			*next_to_last = _ex1;
			*other = _ex1;
			return true;
		}

	} else if (is_a<symbol>(other->op(0)) && other->nops() == 2) {

		// x.mu gamma~mu -> x-slash
		*self = dirac_slash(other->op(0), dim, rl);
		*other = _ex1;
		return true;
	}

	return false;
}

/** An utility function looking for a given metric within an exvector,
 *  used in cliffordunit::contract_with(). */
static int find_same_metric(exvector & v, ex & c)
{
	for (int i=0; i<v.size();i++) {
		if (!is_a<clifford>(v[i]) && is_a<indexed>(v[i])
		    && ex_to<clifford>(c).same_metric(v[i]) 
		    && (ex_to<varidx>(c.op(1)) == ex_to<indexed>(v[i]).get_indices()[0]
			|| ex_to<varidx>(c.op(1)).toggle_variance() == ex_to<indexed>(v[i]).get_indices()[0])) {
			return ++i; // next to found
		}
	}
	return 0; //nothing found
}

/** Contraction of a Clifford unit with something else. */
bool cliffordunit::contract_with(exvector::iterator self, exvector::iterator other, exvector & v) const
{
	GINAC_ASSERT(is_a<clifford>(*self));
	GINAC_ASSERT(is_a<indexed>(*other));
	GINAC_ASSERT(is_a<cliffordunit>(self->op(0)));
	clifford unit = ex_to<clifford>(*self);
	unsigned char rl = unit.get_representation_label();

	if (is_a<clifford>(*other)) {
		// Contraction only makes sense if the represenation labels are equal
		// and the metrics are the same
		if ((ex_to<clifford>(*other).get_representation_label() != rl) 
		    && unit.same_metric(*other))
			return false;

		// Find if a previous contraction produces the square of self
		int prev_square = find_same_metric(v, self[0]);
		varidx d((new symbol)->setflag(status_flags::dynallocated), ex_to<idx>(ex_to<idx>(self->op(1)).get_dim()));
		ex squared_metric = unit.get_metric(self->op(1), d)*unit.get_metric(d.toggle_variance(), other->op(1));

		// e~mu e.mu = Tr ONE
		if (other - self == 1) {
			if (prev_square != 0) {
				*self = squared_metric;
				v[prev_square-1] = _ex1;
			} else
				*self = unit.get_metric(self->op(1), other->op(1));
			*other = dirac_ONE(rl);
			return true;

		// e~mu e~alpha e.mu = (2e~alpha^2-Tr) e~alpha
		} else if (other - self == 2
		        && is_a<clifford>(self[1])) {

			const ex & ia = self[1].op(1);
			const ex & ib = self[1].op(1);
			if (is_a<tensmetric>(unit.get_metric().op(0)))
				*self = 2 - unit.get_metric(self->op(1), other->op(1));
			else if (prev_square != 0) {
				*self = 2-squared_metric;
				v[prev_square-1] = _ex1;
			} else 
				*self = 2*unit.get_metric(ia, ib) - unit.get_metric(self->op(1), other->op(1));
			*other = _ex1;
			return true;

		// e~mu S e~alpha e.mu = 2 e~alpha^3 S - e~mu S e.mu e~alpha
		// (commutate contracted indices towards each other, simplify_indexed()
		// will re-expand and re-run the simplification)
		} else {
			exvector::iterator it = self + 1, next_to_last = other - 1;
			while (it != other) {
				if (!is_a<clifford>(*it))
					return false;
				++it;
			}

			it = self + 1;
			ex S = _ex1;
			while (it != next_to_last) {
				S *= *it;
				*it++ = _ex1;
			}

			const ex & ia = next_to_last->op(1);
			const ex & ib = next_to_last->op(1);
			if (is_a<tensmetric>(unit.get_metric().op(0)))
				*self = 2 * (*next_to_last) * S - (*self) * S * (*other) * (*next_to_last);
			else if (prev_square != 0) {
				*self = 2 * (*next_to_last) * S  - (*self) * S * (*other) * (*next_to_last)*unit.get_metric(self->op(1),self->op(1));
				v[prev_square-1] = _ex1;
			} else 
				*self = 2 * (*next_to_last) * S* unit.get_metric(ia,ib) - (*self) * S * (*other) * (*next_to_last);
			*next_to_last = _ex1;
			*other = _ex1;
			return true;
		}

	} 

	return false;
}

/** Perform automatic simplification on noncommutative product of clifford
 *  objects. This removes superfluous ONEs, permutes gamma5/L/R's to the front
 *  and removes squares of gamma objects. */
ex clifford::eval_ncmul(const exvector & v) const
{
	exvector s;
	s.reserve(v.size());

	// Remove superfluous ONEs
	exvector::const_iterator cit = v.begin(), citend = v.end();
	while (cit != citend) {
		if (!is_a<clifford>(*cit) || !is_a<diracone>(cit->op(0)))
			s.push_back(*cit);
		cit++;
	}

	bool something_changed = false;
	int sign = 1;

	// Anticommute gamma5/L/R's to the front
	if (s.size() >= 2) {
		exvector::iterator first = s.begin(), next_to_last = s.end() - 2;
		while (true) {
			exvector::iterator it = next_to_last;
			while (true) {
				exvector::iterator it2 = it + 1;
				if (is_a<clifford>(*it) && is_a<clifford>(*it2)) {
					ex e1 = it->op(0), e2 = it2->op(0);

					if (is_a<diracgamma5>(e2)) {

						if (is_a<diracgammaL>(e1) || is_a<diracgammaR>(e1)) {

							// gammaL/R gamma5 -> gamma5 gammaL/R
							it->swap(*it2);
							something_changed = true;

						} else if (!is_a<diracgamma5>(e1)) {

							// gamma5 gamma5 -> gamma5 gamma5 (do nothing)
							// x gamma5 -> -gamma5 x
							it->swap(*it2);
							sign = -sign;
							something_changed = true;
						}

					} else if (is_a<diracgammaL>(e2)) {

						if (is_a<diracgammaR>(e1)) {

							// gammaR gammaL -> 0
							return _ex0;

						} else if (!is_a<diracgammaL>(e1) && !is_a<diracgamma5>(e1)) {

							// gammaL gammaL -> gammaL gammaL (do nothing)
							// gamma5 gammaL -> gamma5 gammaL (do nothing)
							// x gammaL -> gammaR x
							it->swap(*it2);
							*it = clifford(diracgammaR(), ex_to<clifford>(*it).get_representation_label());
							something_changed = true;
						}

					} else if (is_a<diracgammaR>(e2)) {

						if (is_a<diracgammaL>(e1)) {

							// gammaL gammaR -> 0
							return _ex0;

						} else if (!is_a<diracgammaR>(e1) && !is_a<diracgamma5>(e1)) {

							// gammaR gammaR -> gammaR gammaR (do nothing)
							// gamma5 gammaR -> gamma5 gammaR (do nothing)
							// x gammaR -> gammaL x
							it->swap(*it2);
							*it = clifford(diracgammaL(), ex_to<clifford>(*it).get_representation_label());
							something_changed = true;
						}
					}
				}
				if (it == first)
					break;
				--it;
			}
			if (next_to_last == first)
				break;
			--next_to_last;
		}
	}

	// Remove equal adjacent gammas
	if (s.size() >= 2) {
		exvector::iterator it, itend = s.end() - 1;
		for (it = s.begin(); it != itend; ++it) {
			ex & a = it[0];
			ex & b = it[1];
			if (!is_a<clifford>(a) || !is_a<clifford>(b))
				continue;

			const ex & ag = a.op(0);
			const ex & bg = b.op(0);
			bool a_is_cliffordunit = is_a<cliffordunit>(ag);
			bool b_is_cliffordunit =  is_a<cliffordunit>(bg);

			if (a_is_cliffordunit && b_is_cliffordunit && ex_to<clifford>(a).same_metric(b)) {

				const ex & ia = a.op(1);
				const ex & ib = b.op(1);
				if (ia.is_equal(ib)) { // gamma~alpha gamma~alpha -> g~alpha~alpha
					a = ex_to<clifford>(a).get_metric(ia,ib);
					b = dirac_ONE(representation_label);
					something_changed = true;
				}

			} else if ((is_a<diracgamma5>(ag) && is_a<diracgamma5>(bg))) {

				// Remove squares of gamma5
				a = dirac_ONE(representation_label);
				b = dirac_ONE(representation_label);
				something_changed = true;

			} else if ((is_a<diracgammaL>(ag) && is_a<diracgammaL>(bg))
			        || (is_a<diracgammaR>(ag) && is_a<diracgammaR>(bg))) {

				// Remove squares of gammaL/R
				b = dirac_ONE(representation_label);
				something_changed = true;

			} else if (is_a<diracgammaL>(ag) && is_a<diracgammaR>(bg)) {

				// gammaL and gammaR are orthogonal
				return _ex0;

			} else if (is_a<diracgamma5>(ag) && is_a<diracgammaL>(bg)) {

				// gamma5 gammaL -> -gammaL
				a = dirac_ONE(representation_label);
				sign = -sign;
				something_changed = true;

			} else if (is_a<diracgamma5>(ag) && is_a<diracgammaR>(bg)) {

				// gamma5 gammaR -> gammaR
				a = dirac_ONE(representation_label);
				something_changed = true;

			} else if (!a_is_cliffordunit && !b_is_cliffordunit && ag.is_equal(bg)) {

				// a\ a\ -> a^2
				varidx ix((new symbol)->setflag(status_flags::dynallocated), ex_to<idx>(a.op(1)).minimal_dim(ex_to<idx>(b.op(1))));
				
				a = indexed(ag, ix) * indexed(ag, ix.toggle_variance());
				b = dirac_ONE(representation_label);
				something_changed = true;
			}
		}
	}

	if (s.empty())
		return clifford(diracone(), representation_label) * sign;
	if (something_changed)
		return reeval_ncmul(s) * sign;
	else
		return hold_ncmul(s) * sign;
}

ex clifford::thiscontainer(const exvector & v) const
{
	return clifford(representation_label, get_metric(), v);
}

ex clifford::thiscontainer(std::auto_ptr<exvector> vp) const
{
	return clifford(representation_label, get_metric(), vp);
}

ex diracgamma5::conjugate() const
{	
	return _ex_1 * (*this);
}

ex diracgammaL::conjugate() const
{
	return (new diracgammaR)->setflag(status_flags::dynallocated);
}

ex diracgammaR::conjugate() const
{
	return (new diracgammaL)->setflag(status_flags::dynallocated);
}

//////////
// global functions
//////////

ex dirac_ONE(unsigned char rl)
{
	static ex ONE = (new diracone)->setflag(status_flags::dynallocated);
	return clifford(ONE, rl);
}

ex clifford_unit(const ex & mu, const ex & metr, unsigned char rl)
{
	if (!is_a<varidx>(mu))
		throw(std::invalid_argument("index of Clifford unit must be of type varidx"));
	if (!is_a<indexed>(metr))
		throw(std::invalid_argument("metric for Clifford unit must be of type indexed"));
	exvector d = ex_to<indexed>(metr).get_indices();
	if (d.size() > 2 || ex_to<idx>(d[0]).get_dim() != ex_to<idx>(d[1]).get_dim())
	  //|| ex_to<idx>(d[0]).get_dim() != ex_to<idx>(mu).get_dim())
		throw(std::invalid_argument("metric is not square"));
	else
		ex_to<idx>(mu).replace_dim(ex_to<idx>(d[0]).get_dim());
	return clifford(cliffordunit(), mu, metr, rl);
}

ex dirac_gamma(const ex & mu, unsigned char rl)
{
	static ex gamma = (new diracgamma)->setflag(status_flags::dynallocated);

	if (!is_a<varidx>(mu))
		throw(std::invalid_argument("index of Dirac gamma must be of type varidx"));

	ex dim = ex_to<idx>(mu).get_dim();
	return clifford(gamma, mu, lorentz_g(varidx((new symbol)->setflag(status_flags::dynallocated), dim),varidx((new symbol)->setflag(status_flags::dynallocated), dim)), rl);
}

ex dirac_gamma5(unsigned char rl)
{
	static ex gamma5 = (new diracgamma5)->setflag(status_flags::dynallocated);
	return clifford(gamma5, rl);
}

ex dirac_gammaL(unsigned char rl)
{
	static ex gammaL = (new diracgammaL)->setflag(status_flags::dynallocated);
	return clifford(gammaL, rl);
}

ex dirac_gammaR(unsigned char rl)
{
	static ex gammaR = (new diracgammaR)->setflag(status_flags::dynallocated);
	return clifford(gammaR, rl);
}

ex dirac_slash(const ex & e, const ex & dim, unsigned char rl)
{
	// Slashed vectors are actually stored as a clifford object with the
	// vector as its base expression and a (dummy) index that just serves
	// for storing the space dimensionality
	return clifford(e, varidx(0, dim), rl);
}

/** Check whether a given tinfo key (as returned by return_type_tinfo()
 *  is that of a clifford object with the specified representation label. */
static bool is_clifford_tinfo(unsigned ti, unsigned char rl)
{
	return ti == (TINFO_clifford + rl);
}

/** Check whether a given tinfo key (as returned by return_type_tinfo()
 *  is that of a clifford object (with an arbitrary representation label). */
static bool is_clifford_tinfo(unsigned ti)
{
	return (ti & ~0xff) == TINFO_clifford;
}

/** Take trace of a string of an even number of Dirac gammas given a vector
 *  of indices. */
static ex trace_string(exvector::const_iterator ix, size_t num)
{
	// Tr gamma.mu gamma.nu = 4 g.mu.nu
	if (num == 2)
		return lorentz_g(ix[0], ix[1]);

	// Tr gamma.mu gamma.nu gamma.rho gamma.sig = 4 (g.mu.nu g.rho.sig + g.nu.rho g.mu.sig - g.mu.rho g.nu.sig )
	else if (num == 4)
		return lorentz_g(ix[0], ix[1]) * lorentz_g(ix[2], ix[3])
		     + lorentz_g(ix[1], ix[2]) * lorentz_g(ix[0], ix[3])
		     - lorentz_g(ix[0], ix[2]) * lorentz_g(ix[1], ix[3]);

	// Traces of 6 or more gammas are computed recursively:
	// Tr gamma.mu1 gamma.mu2 ... gamma.mun =
	//   + g.mu1.mu2 * Tr gamma.mu3 ... gamma.mun
	//   - g.mu1.mu3 * Tr gamma.mu2 gamma.mu4 ... gamma.mun
	//   + g.mu1.mu4 * Tr gamma.mu3 gamma.mu3 gamma.mu5 ... gamma.mun
	//   - ...
	//   + g.mu1.mun * Tr gamma.mu2 ... gamma.mu(n-1)
	exvector v(num - 2);
	int sign = 1;
	ex result;
	for (size_t i=1; i<num; i++) {
		for (size_t n=1, j=0; n<num; n++) {
			if (n == i)
				continue;
			v[j++] = ix[n];
		}
		result += sign * lorentz_g(ix[0], ix[i]) * trace_string(v.begin(), num-2);
		sign = -sign;
	}
	return result;
}

ex dirac_trace(const ex & e, unsigned char rl, const ex & trONE)
{
	if (is_a<clifford>(e)) {

		if (!ex_to<clifford>(e).get_representation_label() == rl)
			return _ex0;
		const ex & g = e.op(0);
		if (is_a<diracone>(g))
			return trONE;
		else if (is_a<diracgammaL>(g) || is_a<diracgammaR>(g))
			return trONE/2;
		else
			return _ex0;

	} else if (is_exactly_a<mul>(e)) {

		// Trace of product: pull out non-clifford factors
		ex prod = _ex1;
		for (size_t i=0; i<e.nops(); i++) {
			const ex &o = e.op(i);
			if (is_clifford_tinfo(o.return_type_tinfo(), rl))
				prod *= dirac_trace(o, rl, trONE);
			else
				prod *= o;
		}
		return prod;

	} else if (is_exactly_a<ncmul>(e)) {

		if (!is_clifford_tinfo(e.return_type_tinfo(), rl))
			return _ex0;

		// Substitute gammaL/R and expand product, if necessary
		ex e_expanded = e.subs(lst(
			dirac_gammaL(rl) == (dirac_ONE(rl)-dirac_gamma5(rl))/2,
			dirac_gammaR(rl) == (dirac_ONE(rl)+dirac_gamma5(rl))/2
		), subs_options::no_pattern).expand();
		if (!is_a<ncmul>(e_expanded))
			return dirac_trace(e_expanded, rl, trONE);

		// gamma5 gets moved to the front so this check is enough
		bool has_gamma5 = is_a<diracgamma5>(e.op(0).op(0));
		size_t num = e.nops();

		if (has_gamma5) {

			// Trace of gamma5 * odd number of gammas and trace of
			// gamma5 * gamma.mu * gamma.nu are zero
			if ((num & 1) == 0 || num == 3)
				return _ex0;

			// Tr gamma5 gamma.mu gamma.nu gamma.rho gamma.sigma = 4I * epsilon(mu, nu, rho, sigma)
			// (the epsilon is always 4-dimensional)
			if (num == 5) {
				ex b1, i1, b2, i2, b3, i3, b4, i4;
				base_and_index(e.op(1), b1, i1);
				base_and_index(e.op(2), b2, i2);
				base_and_index(e.op(3), b3, i3);
				base_and_index(e.op(4), b4, i4);
				return trONE * I * (lorentz_eps(ex_to<idx>(i1).replace_dim(_ex4), ex_to<idx>(i2).replace_dim(_ex4), ex_to<idx>(i3).replace_dim(_ex4), ex_to<idx>(i4).replace_dim(_ex4)) * b1 * b2 * b3 * b4).simplify_indexed();
			}

	   		// Tr gamma5 S_2k =
			//   I/4! * epsilon0123.mu1.mu2.mu3.mu4 * Tr gamma.mu1 gamma.mu2 gamma.mu3 gamma.mu4 S_2k
			// (the epsilon is always 4-dimensional)
			exvector ix(num-1), bv(num-1);
			for (size_t i=1; i<num; i++)
				base_and_index(e.op(i), bv[i-1], ix[i-1]);
			num--;
			int *iv = new int[num];
			ex result;
			for (size_t i=0; i<num-3; i++) {
				ex idx1 = ix[i];
				for (size_t j=i+1; j<num-2; j++) {
					ex idx2 = ix[j];
					for (size_t k=j+1; k<num-1; k++) {
						ex idx3 = ix[k];
						for (size_t l=k+1; l<num; l++) {
							ex idx4 = ix[l];
							iv[0] = i; iv[1] = j; iv[2] = k; iv[3] = l;
							exvector v;
							v.reserve(num - 4);
							for (size_t n=0, t=4; n<num; n++) {
								if (n == i || n == j || n == k || n == l)
									continue;
								iv[t++] = n;
								v.push_back(ix[n]);
							}
							int sign = permutation_sign(iv, iv + num);
							result += sign * lorentz_eps(ex_to<idx>(idx1).replace_dim(_ex4), ex_to<idx>(idx2).replace_dim(_ex4), ex_to<idx>(idx3).replace_dim(_ex4), ex_to<idx>(idx4).replace_dim(_ex4))
							        * trace_string(v.begin(), num - 4);
						}
					}
				}
			}
			delete[] iv;
			return trONE * I * result * mul(bv);

		} else { // no gamma5

			// Trace of odd number of gammas is zero
			if ((num & 1) == 1)
				return _ex0;

			// Tr gamma.mu gamma.nu = 4 g.mu.nu
			if (num == 2) {
				ex b1, i1, b2, i2;
				base_and_index(e.op(0), b1, i1);
				base_and_index(e.op(1), b2, i2);
				return trONE * (lorentz_g(i1, i2) * b1 * b2).simplify_indexed();
			}

			exvector iv(num), bv(num);
			for (size_t i=0; i<num; i++)
				base_and_index(e.op(i), bv[i], iv[i]);

			return trONE * (trace_string(iv.begin(), num) * mul(bv)).simplify_indexed();
		}

	} else if (e.nops() > 0) {

		// Trace maps to all other container classes (this includes sums)
		pointer_to_map_function_2args<unsigned char, const ex &> fcn(dirac_trace, rl, trONE);
		return e.map(fcn);

	} else
		return _ex0;
}

ex canonicalize_clifford(const ex & e)
{
	// Scan for any ncmul objects
	exmap srl;
	ex aux = e.to_rational(srl);
	for (exmap::iterator i = srl.begin(); i != srl.end(); ++i) {

		ex lhs = i->first;
		ex rhs = i->second;

		if (is_exactly_a<ncmul>(rhs)
		 && rhs.return_type() == return_types::noncommutative
		 && is_clifford_tinfo(rhs.return_type_tinfo())) {

			// Expand product, if necessary
			ex rhs_expanded = rhs.expand();
			if (!is_a<ncmul>(rhs_expanded)) {
				i->second = canonicalize_clifford(rhs_expanded);
				continue;

			} else if (!is_a<clifford>(rhs.op(0)))
				continue;

			exvector v;
			v.reserve(rhs.nops());
			for (size_t j=0; j<rhs.nops(); j++)
				v.push_back(rhs.op(j));

			// Stupid recursive bubble sort because we only want to swap adjacent gammas
			exvector::iterator it = v.begin(), next_to_last = v.end() - 1;
			if (is_a<diracgamma5>(it->op(0)) || is_a<diracgammaL>(it->op(0)) || is_a<diracgammaR>(it->op(0)))
				++it;
			while (it != next_to_last) {
				if (it[0].compare(it[1]) > 0) {
					ex save0 = it[0], save1 = it[1];
					ex b1, i1, b2, i2;
					base_and_index(it[0], b1, i1);
					base_and_index(it[1], b2, i2);
 					it[0] = (ex_to<clifford>(save0).get_metric(i1, i2) * b1 * b2).simplify_indexed();
					it[1] = v.size() == 2 ? _ex2 * dirac_ONE(ex_to<clifford>(it[1]).get_representation_label()) : _ex2;
					ex sum = ncmul(v);
					it[0] = save1;
					it[1] = save0;
					sum -= ncmul(v, true);
					i->second = canonicalize_clifford(sum);
					goto next_sym;
				}
				++it;
			}
next_sym:	;
		}
	}
	return aux.subs(srl, subs_options::no_pattern).simplify_indexed();
}

ex clifford_prime(const ex &e)
{
	pointer_to_map_function fcn(clifford_prime);
	if (is_a<clifford>(e) && is_a<cliffordunit>(e.op(0))) {
		return -e;
	} else if (is_a<add>(e)) {
		return e.map(fcn);
	} else if (is_a<ncmul>(e)) {
		return e.map(fcn);
	} else if (is_a<power>(e)) {
		return pow(clifford_prime(e.op(0)), e.op(1));
	} else
		return e;
}

ex delete_ONE(const ex &e)
{
	pointer_to_map_function fcn(delete_ONE);
	if (is_a<clifford>(e) && is_a<diracone>(e.op(0))) {
		return 1;
	} else if (is_a<add>(e)) {
		return e.map(fcn);
	} else if (is_a<ncmul>(e)) {
		return e.map(fcn);
	} else if (is_a<mul>(e)) {
		return e.map(fcn);
	} else if (is_a<power>(e)) {
		return pow(delete_ONE(e.op(0)), e.op(1));
	} else
		return e;
}

ex clifford_norm(const ex &e)
{
	return sqrt(delete_ONE((e * clifford_bar(e)).simplify_indexed()));
}

ex clifford_inverse(const ex &e)
{
	ex norm = clifford_norm(e);
	if (!norm.is_zero())
		return clifford_bar(e) / pow(norm, 2);
}

ex lst_to_clifford(const ex & v, const ex & mu, const ex & metr, unsigned char rl)
{
	unsigned min, max;
	if (!ex_to<idx>(mu).is_dim_numeric())
		throw(std::invalid_argument("Index should have a numeric dimension"));
	unsigned dim = (ex_to<numeric>(ex_to<idx>(mu).get_dim())).to_int();
	ex c = clifford_unit(mu, metr, rl);

	if (is_a<matrix>(v)) {
		if (ex_to<matrix>(v).cols() > ex_to<matrix>(v).rows()) {
			min = ex_to<matrix>(v).rows();
			max = ex_to<matrix>(v).cols();
		} else {
			min = ex_to<matrix>(v).cols();
			max = ex_to<matrix>(v).rows();
		}
		if (min == 1) {
			if (dim == max)
				if (is_a<varidx>(mu)) // need to swap variance
					return indexed(v, ex_to<varidx>(mu).toggle_variance()) * c;
				else
					return indexed(v, mu) * c;
			else
				throw(std::invalid_argument("Dimensions of vector and clifford unit mismatch"));
		} else
			throw(std::invalid_argument("First argument should be a vector vector"));
	} else if (is_a<lst>(v)) {
		if (dim == ex_to<lst>(v).nops())
			return indexed(matrix(dim, 1, ex_to<lst>(v)), ex_to<varidx>(mu).toggle_variance()) * c;
		else
			throw(std::invalid_argument("List length and dimension of clifford unit mismatch"));
	} else
		throw(std::invalid_argument("Cannot construct from anything but list or vector"));
}
 
} // namespace GiNaC
