/** @file flags.h
 *
 *  Collection of all flags used through the GiNaC framework. */

#ifndef _FLAGS_H_
#define _FLAGS_H_

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

class info_flags {
public:
    enum { 
           // answered by class numeric
           numeric,
           real,
           rational,
           integer,
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
           rational_polynomial,
           rational_function,

           // answered by class ex
           normal_form,
           
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

#endif // ndef _FLAGS_H_
