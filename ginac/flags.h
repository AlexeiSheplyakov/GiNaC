/** @file flags.h
 *
 *  Collection of all flags used through the GiNaC framework. */

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

#ifndef __GINAC_FLAGS_H__
#define __GINAC_FLAGS_H__

#ifndef NO_NAMESPACE_GINAC
namespace GiNaC {
#endif // ndef NO_NAMESPACE_GINAC

class expand_options {
public:
    enum { expand_trigonometric      = 0x0001
         };
};

class status_flags {
public:
    enum { dynallocated              = 0x0001,
           evaluated                 = 0x0002,
           expanded                  = 0x0004,
           hash_calculated           = 0x0008
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
           idx,
           
           // answered by class coloridx
           coloridx,
           
           // answered by class lorentzidx
           lorentzidx
    };
};

class return_types {
public:
    enum { commutative, noncommutative, noncommutative_composite};
};

class csrc_types {
public:
	enum {
		ctype_float,
		ctype_double,
		ctype_cl_N
	};
};

class remember_strategies {
public:
    enum { delete_never, // let table grow undefinitely, not recommmended, but currently default
           delete_lru,   // least recently used
           delete_lfu,   // least frequently used
           delete_cyclic // first one in list (oldest)
    };
};

#ifndef NO_NAMESPACE_GINAC
} // namespace GiNaC
#endif // ndef NO_NAMESPACE_GINAC

#endif // ndef __GINAC_FLAGS_H__
