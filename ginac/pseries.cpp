/** @file pseries.cpp
 *
 *  Implementation of class for extended truncated power series and
 *  methods for series expansion. */

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

#include "pseries.h"
#include "add.h"
#include "inifcns.h"
#include "lst.h"
#include "mul.h"
#include "power.h"
#include "relational.h"
#include "symbol.h"
#include "archive.h"
#include "utils.h"
#include "debugmsg.h"

#ifndef NO_NAMESPACE_GINAC
namespace GiNaC {
#endif // ndef NO_NAMESPACE_GINAC

GINAC_IMPLEMENT_REGISTERED_CLASS(pseries, basic)

/*
 *  Default constructor, destructor, copy constructor, assignment operator and helpers
 */

pseries::pseries() : basic(TINFO_pseries)
{
	debugmsg("pseries default constructor", LOGLEVEL_CONSTRUCT);
}

pseries::~pseries()
{
	debugmsg("pseries destructor", LOGLEVEL_DESTRUCT);
	destroy(false);
}

pseries::pseries(const pseries &other)
{
	debugmsg("pseries copy constructor", LOGLEVEL_CONSTRUCT);
	copy(other);
}

const pseries &pseries::operator=(const pseries & other)
{
	debugmsg("pseries operator=", LOGLEVEL_ASSIGNMENT);
	if (this != &other) {
		destroy(true);
		copy(other);
	}
	return *this;
}

void pseries::copy(const pseries &other)
{
	inherited::copy(other);
	seq = other.seq;
	var = other.var;
	point = other.point;
}

void pseries::destroy(bool call_parent)
{
	if (call_parent)
		inherited::destroy(call_parent);
}


/*
 *  Other constructors
 */

/** Construct pseries from a vector of coefficients and powers.
 *  expair.rest holds the coefficient, expair.coeff holds the power.
 *  The powers must be integers (positive or negative) and in ascending order;
 *  the last coefficient can be Order(_ex1()) to represent a truncated,
 *  non-terminating series.
 *
 *  @param rel_  expansion variable and point (must hold a relational)
 *  @param ops_  vector of {coefficient, power} pairs (coefficient must not be zero)
 *  @return newly constructed pseries */
pseries::pseries(const ex &rel_, const epvector &ops_) : basic(TINFO_pseries), seq(ops_)
{
	debugmsg("pseries constructor from ex,epvector", LOGLEVEL_CONSTRUCT);
	GINAC_ASSERT(is_ex_exactly_of_type(rel_, relational));
	GINAC_ASSERT(is_ex_exactly_of_type(rel_.lhs(),symbol));
	point = rel_.rhs();
	var = *static_cast<symbol *>(rel_.lhs().bp);
}


/*
 *  Archiving
 */

/** Construct object from archive_node. */
pseries::pseries(const archive_node &n, const lst &sym_lst) : inherited(n, sym_lst)
{
	debugmsg("pseries constructor from archive_node", LOGLEVEL_CONSTRUCT);
	for (unsigned int i=0; true; ++i) {
		ex rest;
		ex coeff;
		if (n.find_ex("coeff", rest, sym_lst, i) && n.find_ex("power", coeff, sym_lst, i))
			seq.push_back(expair(rest, coeff));
		else
			break;
	}
	n.find_ex("var", var, sym_lst);
	n.find_ex("point", point, sym_lst);
}

/** Unarchive the object. */
ex pseries::unarchive(const archive_node &n, const lst &sym_lst)
{
	return (new pseries(n, sym_lst))->setflag(status_flags::dynallocated);
}

/** Archive the object. */
void pseries::archive(archive_node &n) const
{
	inherited::archive(n);
	epvector::const_iterator i = seq.begin(), iend = seq.end();
	while (i != iend) {
		n.add_ex("coeff", i->rest);
		n.add_ex("power", i->coeff);
		++i;
	}
	n.add_ex("var", var);
	n.add_ex("point", point);
}

//////////
// functions overriding virtual functions from bases classes
//////////

basic *pseries::duplicate() const
{
	debugmsg("pseries duplicate", LOGLEVEL_DUPLICATE);
	return new pseries(*this);
}

void pseries::print(std::ostream &os, unsigned upper_precedence) const
{
	debugmsg("pseries print", LOGLEVEL_PRINT);
	if (precedence<=upper_precedence) os << "(";
	for (epvector::const_iterator i=seq.begin(); i!=seq.end(); ++i) {
		// omit zero terms
		if (i->rest.is_zero())
			continue;
		// print a sign, if needed
		if (i!=seq.begin())
			os << '+';
		if (!is_order_function(i->rest)) {
			// print 'rest', i.e. the expansion coefficient
			if (i->rest.info(info_flags::numeric) &&
				i->rest.info(info_flags::positive)) {
				os << i->rest;
			} else
				os << "(" << i->rest << ')';
			// print 'coeff', something like (x-1)^42
			if (!i->coeff.is_zero()) {
				os << '*';
				if (!point.is_zero())
					os << '(' << var-point << ')';
				else
					os << var;
				if (i->coeff.compare(_ex1())) {
					os << '^';
					if (i->coeff.info(info_flags::negative))
						os << '(' << i->coeff << ')';
					else
						os << i->coeff;
				}
			}
		} else {
			os << Order(power(var-point,i->coeff));
		}
	}
	if (precedence<=upper_precedence) os << ")";
}


void pseries::printraw(std::ostream &os) const
{
	debugmsg("pseries printraw", LOGLEVEL_PRINT);
	os << "pseries(" << var << ";" << point << ";";
	for (epvector::const_iterator i=seq.begin(); i!=seq.end(); ++i) {
		os << "(" << (*i).rest << "," << (*i).coeff << "),";
	}
	os << ")";
}


void pseries::printtree(std::ostream & os, unsigned indent) const
{
	debugmsg("pseries printtree",LOGLEVEL_PRINT);
	os << std::string(indent,' ') << "pseries " 
	   << ", hash=" << hashvalue
	   << " (0x" << std::hex << hashvalue << std::dec << ")"
	   << ", flags=" << flags << std::endl;
	for (unsigned i=0; i<seq.size(); ++i) {
		seq[i].rest.printtree(os,indent+delta_indent);
		seq[i].coeff.printtree(os,indent+delta_indent);
		if (i!=seq.size()-1)
			os << std::string(indent+delta_indent,' ') << "-----" << std::endl;
	}
	var.printtree(os, indent+delta_indent);
	point.printtree(os, indent+delta_indent);
}

/** Return the number of operands including a possible order term. */
unsigned pseries::nops(void) const
{
	return seq.size();
}


/** Return the ith term in the series when represented as a sum. */
ex pseries::op(int i) const
{
	if (i < 0 || unsigned(i) >= seq.size())
		throw (std::out_of_range("op() out of range"));
	return seq[i].rest * power(var - point, seq[i].coeff);
}


ex &pseries::let_op(int i)
{
	throw (std::logic_error("let_op not defined for pseries"));
}


/** Return degree of highest power of the series.  This is usually the exponent
 *  of the Order term.  If s is not the expansion variable of the series, the
 *  series is examined termwise. */
int pseries::degree(const symbol &s) const
{
	if (var.is_equal(s)) {
		// Return last exponent
		if (seq.size())
			return ex_to_numeric((*(seq.end() - 1)).coeff).to_int();
		else
			return 0;
	} else {
		epvector::const_iterator it = seq.begin(), itend = seq.end();
		if (it == itend)
			return 0;
		int max_pow = INT_MIN;
		while (it != itend) {
			int pow = it->rest.degree(s);
			if (pow > max_pow)
				max_pow = pow;
			++it;
		}
		return max_pow;
	}
}

/** Return degree of lowest power of the series.  This is usually the exponent
 *  of the leading term.  If s is not the expansion variable of the series, the
 *  series is examined termwise.  If s is the expansion variable but the
 *  expansion point is not zero the series is not expanded to find the degree.
 *  I.e.: (1-x) + (1-x)^2 + Order((1-x)^3) has ldegree(x) 1, not 0. */
int pseries::ldegree(const symbol &s) const
{
	if (var.is_equal(s)) {
		// Return first exponent
		if (seq.size())
			return ex_to_numeric((*(seq.begin())).coeff).to_int();
		else
			return 0;
	} else {
		epvector::const_iterator it = seq.begin(), itend = seq.end();
		if (it == itend)
			return 0;
		int min_pow = INT_MAX;
		while (it != itend) {
			int pow = it->rest.ldegree(s);
			if (pow < min_pow)
				min_pow = pow;
			++it;
		}
		return min_pow;
	}
}

ex pseries::coeff(const symbol &s, int n) const
{
	if (var.is_equal(s)) {
		if (seq.size() == 0)
			return _ex0();
		
		// Binary search in sequence for given power
		numeric looking_for = numeric(n);
		int lo = 0, hi = seq.size() - 1;
		while (lo <= hi) {
			int mid = (lo + hi) / 2;
			GINAC_ASSERT(is_ex_exactly_of_type(seq[mid].coeff, numeric));
			int cmp = ex_to_numeric(seq[mid].coeff).compare(looking_for);
			switch (cmp) {
				case -1:
					lo = mid + 1;
					break;
				case 0:
					return seq[mid].rest;
				case 1:
					hi = mid - 1;
					break;
				default:
					throw(std::logic_error("pseries::coeff: compare() didn't return -1, 0 or 1"));
			}
		}
		return _ex0();
	} else
		return convert_to_poly().coeff(s, n);
}


ex pseries::collect(const symbol &s) const
{
	return *this;
}


/** Evaluate coefficients. */
ex pseries::eval(int level) const
{
	if (level == 1)
		return this->hold();
	
	if (level == -max_recursion_level)
		throw (std::runtime_error("pseries::eval(): recursion limit exceeded"));
	
	// Construct a new series with evaluated coefficients
	epvector new_seq;
	new_seq.reserve(seq.size());
	epvector::const_iterator it = seq.begin(), itend = seq.end();
	while (it != itend) {
		new_seq.push_back(expair(it->rest.eval(level-1), it->coeff));
		++it;
	}
	return (new pseries(relational(var,point), new_seq))->setflag(status_flags::dynallocated | status_flags::evaluated);
}


/** Evaluate coefficients numerically. */
ex pseries::evalf(int level) const
{
	if (level == 1)
		return *this;
	
	if (level == -max_recursion_level)
		throw (std::runtime_error("pseries::evalf(): recursion limit exceeded"));
	
	// Construct a new series with evaluated coefficients
	epvector new_seq;
	new_seq.reserve(seq.size());
	epvector::const_iterator it = seq.begin(), itend = seq.end();
	while (it != itend) {
		new_seq.push_back(expair(it->rest.evalf(level-1), it->coeff));
		++it;
	}
	return (new pseries(relational(var,point), new_seq))->setflag(status_flags::dynallocated | status_flags::evaluated);
}


ex pseries::subs(const lst & ls, const lst & lr) const
{
	// If expansion variable is being substituted, convert the series to a
	// polynomial and do the substitution there because the result might
	// no longer be a power series
	if (ls.has(var))
		return convert_to_poly(true).subs(ls, lr);
	
	// Otherwise construct a new series with substituted coefficients and
	// expansion point
	epvector newseq;
	newseq.reserve(seq.size());
	epvector::const_iterator it = seq.begin(), itend = seq.end();
	while (it != itend) {
		newseq.push_back(expair(it->rest.subs(ls, lr), it->coeff));
		++it;
	}
	return (new pseries(relational(var,point.subs(ls, lr)), newseq))->setflag(status_flags::dynallocated);
}


/** Implementation of ex::expand() for a power series.  It expands all the
 *  terms individually and returns the resulting series as a new pseries.
 *  @see ex::diff */
ex pseries::expand(unsigned options) const
{
	epvector newseq;
	newseq.reserve(seq.size());
	for (epvector::const_iterator i=seq.begin(); i!=seq.end(); ++i)
		newseq.push_back(expair(i->rest.expand(), i->coeff));
	return (new pseries(relational(var,point), newseq))
	        ->setflag(status_flags::dynallocated | status_flags::expanded);
}


/** Implementation of ex::diff() for a power series.  It treats the series as a
 *  polynomial.
 *  @see ex::diff */
ex pseries::derivative(const symbol & s) const
{
	if (s == var) {
		epvector new_seq;
		epvector::const_iterator it = seq.begin(), itend = seq.end();
		
		// FIXME: coeff might depend on var
		while (it != itend) {
			if (is_order_function(it->rest)) {
				new_seq.push_back(expair(it->rest, it->coeff - 1));
			} else {
				ex c = it->rest * it->coeff;
				if (!c.is_zero())
					new_seq.push_back(expair(c, it->coeff - 1));
			}
			++it;
		}
		return pseries(relational(var,point), new_seq);
	} else {
		return *this;
	}
}


/*
 *  Construct ordinary polynomial out of series
 */

/** Convert a pseries object to an ordinary polynomial.
 *
 *  @param no_order flag: discard higher order terms */
ex pseries::convert_to_poly(bool no_order) const
{
	ex e;
	epvector::const_iterator it = seq.begin(), itend = seq.end();
	
	while (it != itend) {
		if (is_order_function(it->rest)) {
			if (!no_order)
				e += Order(power(var - point, it->coeff));
		} else
			e += it->rest * power(var - point, it->coeff);
		++it;
	}
	return e;
}

/** Returns true if there is no order term, i.e. the series terminates and
 *  false otherwise. */
bool pseries::is_terminating(void) const
{
	return seq.size() == 0 || !is_order_function((seq.end()-1)->rest);
}


/*
 *  Implementation of series expansion
 */

/** Default implementation of ex::series(). This performs Taylor expansion.
 *  @see ex::series */
ex basic::series(const relational & r, int order, unsigned options) const
{
	epvector seq;
	numeric fac(1);
	ex deriv = *this;
	ex coeff = deriv.subs(r);
	const symbol *s = static_cast<symbol *>(r.lhs().bp);
	
	if (!coeff.is_zero())
		seq.push_back(expair(coeff, numeric(0)));
	
	int n;
	for (n=1; n<order; ++n) {
		fac = fac.mul(numeric(n));
		deriv = deriv.diff(*s).expand();
		if (deriv.is_zero()) {
			// Series terminates
			return pseries(r, seq);
		}
		coeff = deriv.subs(r);
		if (!coeff.is_zero())
			seq.push_back(expair(fac.inverse() * coeff, numeric(n)));
	}
	
	// Higher-order terms, if present
	deriv = deriv.diff(*s);
	if (!deriv.expand().is_zero())
		seq.push_back(expair(Order(_ex1()), numeric(n)));
	return pseries(r, seq);
}


/** Implementation of ex::series() for symbols.
 *  @see ex::series */
ex symbol::series(const relational & r, int order, unsigned options) const
{
	epvector seq;
	const ex point = r.rhs();
	GINAC_ASSERT(is_ex_exactly_of_type(r.lhs(),symbol));
	const symbol *s = static_cast<symbol *>(r.lhs().bp);
	
	if (this->is_equal(*s)) {
		if (order > 0 && !point.is_zero())
			seq.push_back(expair(point, _ex0()));
		if (order > 1)
			seq.push_back(expair(_ex1(), _ex1()));
		else
			seq.push_back(expair(Order(_ex1()), numeric(order)));
	} else
		seq.push_back(expair(*this, _ex0()));
	return pseries(r, seq);
}


/** Add one series object to another, producing a pseries object that
 *  represents the sum.
 *
 *  @param other  pseries object to add with
 *  @return the sum as a pseries */
ex pseries::add_series(const pseries &other) const
{
	// Adding two series with different variables or expansion points
	// results in an empty (constant) series 
	if (!is_compatible_to(other)) {
		epvector nul;
		nul.push_back(expair(Order(_ex1()), _ex0()));
		return pseries(relational(var,point), nul);
	}
	
	// Series addition
	epvector new_seq;
	epvector::const_iterator a = seq.begin();
	epvector::const_iterator b = other.seq.begin();
	epvector::const_iterator a_end = seq.end();
	epvector::const_iterator b_end = other.seq.end();
	int pow_a = INT_MAX, pow_b = INT_MAX;
	for (;;) {
		// If a is empty, fill up with elements from b and stop
		if (a == a_end) {
			while (b != b_end) {
				new_seq.push_back(*b);
				++b;
			}
			break;
		} else
			pow_a = ex_to_numeric((*a).coeff).to_int();
		
		// If b is empty, fill up with elements from a and stop
		if (b == b_end) {
			while (a != a_end) {
				new_seq.push_back(*a);
				++a;
			}
			break;
		} else
			pow_b = ex_to_numeric((*b).coeff).to_int();
		
		// a and b are non-empty, compare powers
		if (pow_a < pow_b) {
			// a has lesser power, get coefficient from a
			new_seq.push_back(*a);
			if (is_order_function((*a).rest))
				break;
			++a;
		} else if (pow_b < pow_a) {
			// b has lesser power, get coefficient from b
			new_seq.push_back(*b);
			if (is_order_function((*b).rest))
				break;
			++b;
		} else {
			// Add coefficient of a and b
			if (is_order_function((*a).rest) || is_order_function((*b).rest)) {
				new_seq.push_back(expair(Order(_ex1()), (*a).coeff));
				break;  // Order term ends the sequence
			} else {
				ex sum = (*a).rest + (*b).rest;
				if (!(sum.is_zero()))
					new_seq.push_back(expair(sum, numeric(pow_a)));
				++a;
				++b;
			}
		}
	}
	return pseries(relational(var,point), new_seq);
}


/** Implementation of ex::series() for sums. This performs series addition when
 *  adding pseries objects.
 *  @see ex::series */
ex add::series(const relational & r, int order, unsigned options) const
{
	ex acc; // Series accumulator
	
	// Get first term from overall_coeff
	acc = overall_coeff.series(r, order, options);
	
	// Add remaining terms
	epvector::const_iterator it = seq.begin();
	epvector::const_iterator itend = seq.end();
	for (; it!=itend; ++it) {
		ex op;
		if (is_ex_exactly_of_type(it->rest, pseries))
			op = it->rest;
		else
			op = it->rest.series(r, order, options);
		if (!it->coeff.is_equal(_ex1()))
			op = ex_to_pseries(op).mul_const(ex_to_numeric(it->coeff));
		
		// Series addition
		acc = ex_to_pseries(acc).add_series(ex_to_pseries(op));
	}
	return acc;
}


/** Multiply a pseries object with a numeric constant, producing a pseries
 *  object that represents the product.
 *
 *  @param other  constant to multiply with
 *  @return the product as a pseries */
ex pseries::mul_const(const numeric &other) const
{
	epvector new_seq;
	new_seq.reserve(seq.size());
	
	epvector::const_iterator it = seq.begin(), itend = seq.end();
	while (it != itend) {
		if (!is_order_function(it->rest))
			new_seq.push_back(expair(it->rest * other, it->coeff));
		else
			new_seq.push_back(*it);
		++it;
	}
	return pseries(relational(var,point), new_seq);
}


/** Multiply one pseries object to another, producing a pseries object that
 *  represents the product.
 *
 *  @param other  pseries object to multiply with
 *  @return the product as a pseries */
ex pseries::mul_series(const pseries &other) const
{
	// Multiplying two series with different variables or expansion points
	// results in an empty (constant) series 
	if (!is_compatible_to(other)) {
		epvector nul;
		nul.push_back(expair(Order(_ex1()), _ex0()));
		return pseries(relational(var,point), nul);
	}
	
	// Series multiplication
	epvector new_seq;
	
	const symbol *s = static_cast<symbol *>(var.bp);
	int a_max = degree(*s);
	int b_max = other.degree(*s);
	int a_min = ldegree(*s);
	int b_min = other.ldegree(*s);
	int cdeg_min = a_min + b_min;
	int cdeg_max = a_max + b_max;
	
	int higher_order_a = INT_MAX;
	int higher_order_b = INT_MAX;
	if (is_order_function(coeff(*s, a_max)))
		higher_order_a = a_max + b_min;
	if (is_order_function(other.coeff(*s, b_max)))
		higher_order_b = b_max + a_min;
	int higher_order_c = std::min(higher_order_a, higher_order_b);
	if (cdeg_max >= higher_order_c)
		cdeg_max = higher_order_c - 1;
	
	for (int cdeg=cdeg_min; cdeg<=cdeg_max; ++cdeg) {
		ex co = _ex0();
		// c(i)=a(0)b(i)+...+a(i)b(0)
		for (int i=a_min; cdeg-i>=b_min; ++i) {
			ex a_coeff = coeff(*s, i);
			ex b_coeff = other.coeff(*s, cdeg-i);
			if (!is_order_function(a_coeff) && !is_order_function(b_coeff))
				co += a_coeff * b_coeff;
		}
		if (!co.is_zero())
			new_seq.push_back(expair(co, numeric(cdeg)));
	}
	if (higher_order_c < INT_MAX)
		new_seq.push_back(expair(Order(_ex1()), numeric(higher_order_c)));
	return pseries(relational(var,point), new_seq);
}


/** Implementation of ex::series() for product. This performs series
 *  multiplication when multiplying series.
 *  @see ex::series */
ex mul::series(const relational & r, int order, unsigned options) const
{
	ex acc; // Series accumulator
	
	// Get first term from overall_coeff
	acc = overall_coeff.series(r, order, options);
	
	// Multiply with remaining terms
	epvector::const_iterator it = seq.begin();
	epvector::const_iterator itend = seq.end();
	for (; it!=itend; ++it) {
		ex op = it->rest;
		if (op.info(info_flags::numeric)) {
			// series * const (special case, faster)
			ex f = power(op, it->coeff);
			acc = ex_to_pseries(acc).mul_const(ex_to_numeric(f));
			continue;
		} else if (!is_ex_exactly_of_type(op, pseries))
			op = op.series(r, order, options);
		if (!it->coeff.is_equal(_ex1()))
			op = ex_to_pseries(op).power_const(ex_to_numeric(it->coeff), order);

		// Series multiplication
		acc = ex_to_pseries(acc).mul_series(ex_to_pseries(op));
	}
	return acc;
}


/** Compute the p-th power of a series.
 *
 *  @param p  power to compute
 *  @param deg  truncation order of series calculation */
ex pseries::power_const(const numeric &p, int deg) const
{
	int i;
	const symbol *s = static_cast<symbol *>(var.bp);
	int ldeg = ldegree(*s);
	
	// Calculate coefficients of powered series
	exvector co;
	co.reserve(deg);
	ex co0;
	co.push_back(co0 = power(coeff(*s, ldeg), p));
	bool all_sums_zero = true;
	for (i=1; i<deg; ++i) {
		ex sum = _ex0();
		for (int j=1; j<=i; ++j) {
			ex c = coeff(*s, j + ldeg);
			if (is_order_function(c)) {
				co.push_back(Order(_ex1()));
				break;
			} else
				sum += (p * j - (i - j)) * co[i - j] * c;
		}
		if (!sum.is_zero())
			all_sums_zero = false;
		co.push_back(co0 * sum / numeric(i));
	}
	
	// Construct new series (of non-zero coefficients)
	epvector new_seq;
	bool higher_order = false;
	for (i=0; i<deg; ++i) {
		if (!co[i].is_zero())
			new_seq.push_back(expair(co[i], numeric(i) + p * ldeg));
		if (is_order_function(co[i])) {
			higher_order = true;
			break;
		}
	}
	if (!higher_order && !all_sums_zero)
		new_seq.push_back(expair(Order(_ex1()), numeric(deg) + p * ldeg));
	return pseries(relational(var,point), new_seq);
}


/** Return a new pseries object with the powers shifted by deg. */
pseries pseries::shift_exponents(int deg) const
{
	epvector newseq(seq);
	for (epvector::iterator i=newseq.begin(); i!=newseq.end(); ++i)
		i->coeff = i->coeff + deg;
	return pseries(relational(var, point), newseq);
}


/** Implementation of ex::series() for powers. This performs Laurent expansion
 *  of reciprocals of series at singularities.
 *  @see ex::series */
ex power::series(const relational & r, int order, unsigned options) const
{
	ex e;
	if (!is_ex_exactly_of_type(basis, pseries)) {
		// Basis is not a series, may there be a singulary?
		if (!exponent.info(info_flags::negint))
			return basic::series(r, order, options);
		
		// Expression is of type something^(-int), check for singularity
		if (!basis.subs(r).is_zero())
			return basic::series(r, order, options);
		
		// Singularity encountered, expand basis into series
		e = basis.series(r, order, options);
	} else {
		// Basis is a series
		e = basis;
	}
	
	// Power e
	return ex_to_pseries(e).power_const(ex_to_numeric(exponent), order);
}


/** Re-expansion of a pseries object. */
ex pseries::series(const relational & r, int order, unsigned options) const
{
	const ex p = r.rhs();
	GINAC_ASSERT(is_ex_exactly_of_type(r.lhs(),symbol));
	const symbol *s = static_cast<symbol *>(r.lhs().bp);
	
	if (var.is_equal(*s) && point.is_equal(p)) {
		if (order > degree(*s))
			return *this;
		else {
			epvector new_seq;
			epvector::const_iterator it = seq.begin(), itend = seq.end();
			while (it != itend) {
				int o = ex_to_numeric(it->coeff).to_int();
				if (o >= order) {
					new_seq.push_back(expair(Order(_ex1()), o));
					break;
				}
				new_seq.push_back(*it);
				++it;
			}
			return pseries(r, new_seq);
		}
	} else
		return convert_to_poly().series(r, order, options);
}


/** Compute the truncated series expansion of an expression.
 *  This function returns an expression containing an object of class pseries 
 *  to represent the series. If the series does not terminate within the given
 *  truncation order, the last term of the series will be an order term.
 *
 *  @param r  expansion relation, lhs holds variable and rhs holds point
 *  @param order  truncation order of series calculations
 *  @param options  of class series_options
 *  @return an expression holding a pseries object */
ex ex::series(const ex & r, int order, unsigned options) const
{
	GINAC_ASSERT(bp!=0);
	ex e;
	relational rel_;
	
	if (is_ex_exactly_of_type(r,relational))
		rel_ = ex_to_relational(r);
	else if (is_ex_exactly_of_type(r,symbol))
		rel_ = relational(r,_ex0());
	else
		throw (std::logic_error("ex::series(): expansion point has unknown type"));
	
	try {
		e = bp->series(rel_, order, options);
	} catch (std::exception &x) {
		throw (std::logic_error(std::string("unable to compute series (") + x.what() + ")"));
	}
	return e;
}

//////////
// static member variables
//////////

// protected

unsigned pseries::precedence = 38;  // for clarity just below add::precedence

//////////
// global constants
//////////

const pseries some_pseries;
const std::type_info & typeid_pseries = typeid(some_pseries);

#ifndef NO_NAMESPACE_GINAC
} // namespace GiNaC
#endif // ndef NO_NAMESPACE_GINAC
