/** @file flags.h
 *
 *  Collection of all flags used through the GiNaC framework. */

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

#ifndef __GINAC_FLAGS_H__
#define __GINAC_FLAGS_H__

namespace GiNaC {

class expand_options {
public:
	enum {
		expand_trigonometric = 0x0001,
		expand_indexed = 0x0002,
		expand_function_args = 0x0004
	};
};

/** Flags to control series expansion. */
class series_options {
public:
	enum {
		suppress_branchcut = 0x0001
	};
};

/** Switch to control algorithm for determinant computation. */
class determinant_algo {
public:
	enum {
		automatic,                      ///< Let the system choose
		gauss,                          ///< Gauss elimiation
		divfree,                        ///< Division-free elimination
		laplace,                        ///< Laplace (or minor) elimination
		bareiss                         ///< Bareiss fraction-free elimination
	};
};

/** Switch to control algorithm for linear system solving. */
class solve_algo {
public:
	enum {
		automatic,                      ///< Let the system choose
		gauss,                          ///< Gauss elimiation
		divfree,                        ///< Division-free elimination
		bareiss                         ///< Bareiss fraction-free elimination
	};
};

/** Flags to store information about the state of an object.
 *  @see basic::flags */
class status_flags {
public:
	enum {
		dynallocated    = 0x0001,       ///< Heap-allocated (i.e. created by new if we want to be clever and bypass the stack, @see ex::construct_from_basic() )
		evaluated       = 0x0002,       ///< .eval() has already done its job
		expanded        = 0x0004,       ///< .expand() has already done its job
		hash_calculated = 0x0008        ///< .calchash() has already done its job
	};
};

/** Possible attributes an object can have. */
class info_flags {
public:
	enum {
		// answered by class numeric
		numeric,
		real,
		rational,
		integer,
		crational,
		cinteger,
		positive,
		negative,
		nonnegative,
		posint,
		negint,
		nonnegint,
		even,
		odd,
		prime,

		// answered by class relation
		relation,
		relation_equal,
		relation_not_equal,
		relation_less,
		relation_less_or_equal,
		relation_greater,
		relation_greater_or_equal,

		// answered by class symbol
		symbol,

		// answered by class lst
		list,

		// answered by class exprseq
		exprseq,

		// answered by classes numeric, symbol, add, mul, power
		polynomial,
		integer_polynomial,
		cinteger_polynomial,
		rational_polynomial,
		crational_polynomial,
		rational_function,
		algebraic,

		// answered by class indexed
		indexed,      // class can carry indices
		has_indices,  // object has at least one index

		// answered by class idx
		idx
	};
};

class return_types {
public:
	enum {
		commutative,
		noncommutative,
		noncommutative_composite
	};
};

/** Strategies how to clean up the function remember cache.
 *  @see remember_table */
class remember_strategies {
public:
	enum {
		delete_never,   ///< Let table grow undefinitely
		delete_lru,     ///< Least recently used
		delete_lfu,     ///< Least frequently used
		delete_cyclic   ///< First (oldest) one in list
	};
};

} // namespace GiNaC

#endif // ndef __GINAC_FLAGS_H__
