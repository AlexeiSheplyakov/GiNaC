#include <algorithm>
#include <cstddef>
#include "optimal_vars_finder.h"
#include "add.h"
#include "mul.h"
#include "power.h"
#include "symbol.h"
#include "numeric.h"

namespace GiNaC
{
namespace
{
// XXX: copy-pasted from normal.cpp.
/*
 *  Statistical information about symbols in polynomials
 */

/** This structure holds information about the highest and lowest degrees
 *  in which a symbol appears in two multivariate polynomials "a" and "b".
 *  A vector of these structures with information about all symbols in
 *  two polynomials can be created with the function get_symbol_stats().
 *
 *  @see get_symbol_stats */
struct sym_desc 
{
	/** Reference to symbol */
	ex sym;

	/** Highest degree of symbol in polynomial "a" */
	int deg_a;

	/** Highest degree of symbol in polynomial "b" */
	int deg_b;

	/** Lowest degree of symbol in polynomial "a" */
	int ldeg_a;

	/** Lowest degree of symbol in polynomial "b" */
	int ldeg_b;

	/** Maximum of deg_a and deg_b (Used for sorting) */
	int max_deg;

	/** Maximum number of terms of leading coefficient of symbol in both polynomials */
	std::size_t max_lcnops;

	/** Commparison operator for sorting */
	bool operator<(const sym_desc &x) const
	{
		if (max_deg == x.max_deg)
			return max_lcnops < x.max_lcnops;
		else
			return max_deg < x.max_deg;
	}
};

// Vector of sym_desc structures
typedef std::vector<sym_desc> sym_desc_vec;

// Add symbol the sym_desc_vec (used internally by get_symbol_stats())
static void add_symbol(const ex &s, sym_desc_vec &v)
{
	sym_desc_vec::const_iterator it = v.begin(), itend = v.end();
	while (it != itend) {
		if (it->sym.is_equal(s))  // If it's already in there, don't add it a second time
			return;
		++it;
	}
	sym_desc d;
	d.sym = s;
	v.push_back(d);
}

// Collect all symbols of an expression (used internally by get_symbol_stats())
static void collect_symbols(const ex &e, sym_desc_vec &v)
{
	if (is_a<symbol>(e)) {
		add_symbol(e, v);
	} else if (is_exactly_a<add>(e) || is_exactly_a<mul>(e)) {
		for (size_t i=0; i<e.nops(); i++)
			collect_symbols(e.op(i), v);
	} else if (is_exactly_a<power>(e)) {
		collect_symbols(e.op(0), v);
	}
}

/**
 * @brief Find the order of variables which is optimal for GCD computation.
 *
 * Collects statistical information about the highest and lowest degrees
 * of all variables that appear in two polynomials. Sorts the variables
 * by minimum degree (lowest to highest). The information gathered by
 * this function is used by GCD routines to find out the main variable
 * for GCD computation.
 *
 *  @param a  first multivariate polynomial
 *  @param b  second multivariate polynomial
 *  @param v  vector of sym_desc structs (filled in) */
static void get_symbol_stats(const ex &a, const ex &b, sym_desc_vec &v)
{
	collect_symbols(a, v);
	collect_symbols(b, v);
	sym_desc_vec::iterator it = v.begin(), itend = v.end();
	while (it != itend) {
		int deg_a = a.degree(it->sym);
		int deg_b = b.degree(it->sym);
		it->deg_a = deg_a;
		it->deg_b = deg_b;
		it->max_deg = std::max(deg_a, deg_b);
		it->max_lcnops = std::max(a.lcoeff(it->sym).nops(), b.lcoeff(it->sym).nops());
		it->ldeg_a = a.ldegree(it->sym);
		it->ldeg_b = b.ldegree(it->sym);
		++it;
	}
	std::sort(v.begin(), v.end());
}
// XXX: end copy-paste from normal.cpp

} // anonymous namespace of helper functions

exvector gcd_optimal_variables_order(const ex& a, const ex& b)
{
	sym_desc_vec v;
	get_symbol_stats(a, b, v);
	exvector vars;
	vars.reserve(v.size());
	for (std::size_t i = v.size(); i-- != 0; )
		vars.push_back(v[i].sym);
	return vars;
}

} // namespace GiNaC

