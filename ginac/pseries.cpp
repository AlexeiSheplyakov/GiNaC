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
 *  @param var_  series variable (must hold a symbol)
 *  @param point_  expansion point
 *  @param ops_  vector of {coefficient, power} pairs (coefficient must not be zero)
 *  @return newly constructed pseries */
pseries::pseries(const ex &var_, const ex &point_, const epvector &ops_)
    : basic(TINFO_pseries), seq(ops_), var(var_), point(point_)
{
    debugmsg("pseries constructor from ex,ex,epvector", LOGLEVEL_CONSTRUCT);
    GINAC_ASSERT(is_ex_exactly_of_type(var_, symbol));
}


/*
 *  Archiving
 */

/** Construct object from archive_node. */
pseries::pseries(const archive_node &n, const lst &sym_lst) : inherited(n, sym_lst)
{
    debugmsg("pseries constructor from archive_node", LOGLEVEL_CONSTRUCT);
    for (unsigned int i=0; true; i++) {
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
        i++;
    }
    n.add_ex("var", var);
    n.add_ex("point", point);
}


/*
 *  Functions overriding virtual functions from base classes
 */

basic *pseries::duplicate() const
{
    debugmsg("pseries duplicate", LOGLEVEL_DUPLICATE);
    return new pseries(*this);
}

void pseries::print(ostream &os, unsigned upper_precedence) const
{
    debugmsg("pseries print", LOGLEVEL_PRINT);
    convert_to_poly().print(os, upper_precedence);
}

void pseries::printraw(ostream &os) const
{
	debugmsg("pseries printraw", LOGLEVEL_PRINT);
	os << "pseries(" << var << ";" << point << ";";
	for (epvector::const_iterator i=seq.begin(); i!=seq.end(); i++) {
		os << "(" << (*i).rest << "," << (*i).coeff << "),";
	}
	os << ")";
}

unsigned pseries::nops(void) const
{
    return seq.size();
}

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
            it++;
        }
        return max_pow;
    }
}

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
            it++;
        }
        return min_pow;
    }
}

ex pseries::coeff(const symbol &s, int n) const
{
    if (var.is_equal(s)) {
        epvector::const_iterator it = seq.begin(), itend = seq.end();
        while (it != itend) {
            int pow = ex_to_numeric(it->coeff).to_int();
            if (pow == n)
                return it->rest;
            if (pow > n)
                return _ex0();
            it++;
        }
        return _ex0();
    } else
        return convert_to_poly().coeff(s, n);
}

ex pseries::eval(int level) const
{
    if (level == 1)
        return this->hold();
    
    // Construct a new series with evaluated coefficients
    epvector new_seq;
    new_seq.reserve(seq.size());
    epvector::const_iterator it = seq.begin(), itend = seq.end();
    while (it != itend) {
        new_seq.push_back(expair(it->rest.eval(level-1), it->coeff));
        it++;
    }
    return (new pseries(var, point, new_seq))->setflag(status_flags::dynallocated | status_flags::evaluated);
}

/** Evaluate numerically.  The order term is dropped. */
ex pseries::evalf(int level) const
{
    return convert_to_poly().evalf(level);
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
	epvector new_seq;
	new_seq.reserve(seq.size());
	epvector::const_iterator it = seq.begin(), itend = seq.end();
	while (it != itend) {
		new_seq.push_back(expair(it->rest.subs(ls, lr), it->coeff));
		it++;
	}
    return (new pseries(var, point.subs(ls, lr), new_seq))->setflag(status_flags::dynallocated);
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
        it++;
    }
    return e;
}


/*
 *  Implementation of series expansion
 */

/** Default implementation of ex::series(). This performs Taylor expansion.
 *  @see ex::series */
ex basic::series(const symbol & s, const ex & point, int order) const
{
    epvector seq;
    numeric fac(1);
    ex deriv = *this;
    ex coeff = deriv.subs(s == point);
    if (!coeff.is_zero())
        seq.push_back(expair(coeff, numeric(0)));
    
    int n;
    for (n=1; n<order; n++) {
        fac = fac.mul(numeric(n));
        deriv = deriv.diff(s).expand();
        if (deriv.is_zero()) {
            // Series terminates
            return pseries(s, point, seq);
        }
        coeff = fac.inverse() * deriv.subs(s == point);
        if (!coeff.is_zero())
            seq.push_back(expair(coeff, numeric(n)));
    }
    
    // Higher-order terms, if present
    deriv = deriv.diff(s);
    if (!deriv.is_zero())
        seq.push_back(expair(Order(_ex1()), numeric(n)));
    return pseries(s, point, seq);
}


/** Implementation of ex::series() for symbols.
 *  @see ex::series */
ex symbol::series(const symbol & s, const ex & point, int order) const
{
	epvector seq;
	if (is_equal(s)) {
		if (order > 0 && !point.is_zero())
			seq.push_back(expair(point, _ex0()));
		if (order > 1)
			seq.push_back(expair(_ex1(), _ex1()));
		else
			seq.push_back(expair(Order(_ex1()), numeric(order)));
	} else
		seq.push_back(expair(*this, _ex0()));
	return pseries(s, point, seq);
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
        return pseries(var, point, nul);
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
                b++;
            }
            break;
        } else
            pow_a = ex_to_numeric((*a).coeff).to_int();
        
        // If b is empty, fill up with elements from a and stop
        if (b == b_end) {
            while (a != a_end) {
                new_seq.push_back(*a);
                a++;
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
            a++;
        } else if (pow_b < pow_a) {
            // b has lesser power, get coefficient from b
            new_seq.push_back(*b);
            if (is_order_function((*b).rest))
                break;
            b++;
        } else {
            // Add coefficient of a and b
            if (is_order_function((*a).rest) || is_order_function((*b).rest)) {
                new_seq.push_back(expair(Order(_ex1()), (*a).coeff));
                break;  // Order term ends the sequence
            } else {
                ex sum = (*a).rest + (*b).rest;
                if (!(sum.is_zero()))
                    new_seq.push_back(expair(sum, numeric(pow_a)));
                a++;
                b++;
            }
        }
    }
    return pseries(var, point, new_seq);
}


/** Implementation of ex::series() for sums. This performs series addition when
 *  adding pseries objects.
 *  @see ex::series */
ex add::series(const symbol & s, const ex & point, int order) const
{
    ex acc; // Series accumulator
    
    // Get first term from overall_coeff
    acc = overall_coeff.series(s, point, order);

    // Add remaining terms
    epvector::const_iterator it = seq.begin();
    epvector::const_iterator itend = seq.end();
    for (; it!=itend; it++) {
        ex op;
        if (is_ex_exactly_of_type(it->rest, pseries))
            op = it->rest;
        else
            op = it->rest.series(s, point, order);
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
        it++;
    }
    return pseries(var, point, new_seq);
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
        return pseries(var, point, nul);
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
    int higher_order_c = min(higher_order_a, higher_order_b);
    if (cdeg_max >= higher_order_c)
        cdeg_max = higher_order_c - 1;
    
    for (int cdeg=cdeg_min; cdeg<=cdeg_max; cdeg++) {
        ex co = _ex0();
        // c(i)=a(0)b(i)+...+a(i)b(0)
        for (int i=a_min; cdeg-i>=b_min; i++) {
            ex a_coeff = coeff(*s, i);
            ex b_coeff = other.coeff(*s, cdeg-i);
            if (!is_order_function(a_coeff) && !is_order_function(b_coeff))
                co += coeff(*s, i) * other.coeff(*s, cdeg-i);
        }
        if (!co.is_zero())
            new_seq.push_back(expair(co, numeric(cdeg)));
    }
    if (higher_order_c < INT_MAX)
        new_seq.push_back(expair(Order(_ex1()), numeric(higher_order_c)));
    return pseries(var, point, new_seq);
}


/** Implementation of ex::series() for product. This performs series
 *  multiplication when multiplying series.
 *  @see ex::series */
ex mul::series(const symbol & s, const ex & point, int order) const
{
    ex acc; // Series accumulator
    
    // Get first term from overall_coeff
    acc = overall_coeff.series(s, point, order);
    
    // Multiply with remaining terms
    epvector::const_iterator it = seq.begin();
    epvector::const_iterator itend = seq.end();
    for (; it!=itend; it++) {
        ex op = it->rest;
        if (op.info(info_flags::numeric)) {
            // series * const (special case, faster)
            ex f = power(op, it->coeff);
            acc = ex_to_pseries(acc).mul_const(ex_to_numeric(f));
            continue;
        } else if (!is_ex_exactly_of_type(op, pseries))
            op = op.series(s, point, order);
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
    for (i=1; i<deg; i++) {
        ex sum = _ex0();
        for (int j=1; j<=i; j++) {
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
    for (i=0; i<deg; i++) {
        if (!co[i].is_zero())
            new_seq.push_back(expair(co[i], numeric(i) + p * ldeg));
        if (is_order_function(co[i])) {
            higher_order = true;
            break;
        }
    }
    if (!higher_order && !all_sums_zero)
        new_seq.push_back(expair(Order(_ex1()), numeric(deg) + p * ldeg));
    return pseries(var, point, new_seq);
}


/** Implementation of ex::series() for powers. This performs Laurent expansion
 *  of reciprocals of series at singularities.
 *  @see ex::series */
ex power::series(const symbol & s, const ex & point, int order) const
{
    ex e;
    if (!is_ex_exactly_of_type(basis, pseries)) {
        // Basis is not a series, may there be a singulary?
        if (!exponent.info(info_flags::negint))
            return basic::series(s, point, order);
        
        // Expression is of type something^(-int), check for singularity
        if (!basis.subs(s == point).is_zero())
            return basic::series(s, point, order);
        
        // Singularity encountered, expand basis into series
        e = basis.series(s, point, order);
    } else {
        // Basis is a series
        e = basis;
    }
    
    // Power e
    return ex_to_pseries(e).power_const(ex_to_numeric(exponent), order);
}


/** Compute the truncated series expansion of an expression.
 *  This function returns an expression containing an object of class pseries to
 *  represent the series. If the series does not terminate within the given
 *  truncation order, the last term of the series will be an order term.
 *
 *  @param s  expansion variable
 *  @param point  expansion point
 *  @param order  truncation order of series calculations
 *  @return an expression holding a pseries object */
ex ex::series(const symbol &s, const ex &point, int order) const
{
    GINAC_ASSERT(bp!=0);
    return bp->series(s, point, order);
}


// Global constants
const pseries some_pseries;
const type_info & typeid_pseries = typeid(some_pseries);

#ifndef NO_NAMESPACE_GINAC
} // namespace GiNaC
#endif // ndef NO_NAMESPACE_GINAC
