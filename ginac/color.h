/** @file color.h
 *
 *  Interface to GiNaC's color objects. */

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

#ifndef __GINAC_COLOR_H__
#define __GINAC_COLOR_H__

#include <string>
#include <vector>
#include "indexed.h"
#include "ex.h"

#ifndef NO_NAMESPACE_GINAC
namespace GiNaC {
#endif // ndef NO_NAMESPACE_GINAC

const unsigned MAX_REPRESENTATION_LABELS = 4;
const unsigned COLOR_EIGHT = 8; // N*N-1
const unsigned COLOR_THREE = 3; // N

// Cint doesn't like vector<..,default_alloc> but malloc_alloc is
// unstandardized and not supported by newer GCCs.
#if defined(__GNUC__) && ((__GNUC__ == 2) && (__GNUC_MINOR__ < 97))
typedef std::vector<exvector,malloc_alloc> exvectorvector;
#else
typedef std::vector<exvector> exvectorvector;
#endif


/** This class holds an object carrying color indices (of class coloridx). 
 *  It can represent the generators T_a and structure constants f_abc/d_abc
 *  of SU(3), the unity element ONE of the Lie algebra of SU(3), or the
 *  unity matrix delta8. Together, these objects are used to implement the
 *  Lie algebra su(3), as required for calculations in quantum
 *  chromodynamics. A representation label (an unsigned integer) is used to
 *  distinguish elements from different Lie algebras (only objects with the
 *  same representation label "interact" with each other). */
class color : public indexed
{
	GINAC_DECLARE_REGISTERED_CLASS(color, indexed)

// friends

	friend color color_ONE(unsigned rl);
	friend color color_T(const ex & a, unsigned rl);
	friend color color_f(const ex & a, const ex & b, const ex & c);
	friend color color_d(const ex & a, const ex & b, const ex & c);
	friend ex color_h(const ex & a, const ex & b, const ex & c);
	friend color color_delta8(const ex & a, const ex & b);
	friend unsigned subs_index_in_exvector(exvector & v, const ex & is, const ex & ir);
	friend void split_color_string_in_parts(const exvector & v, exvector & delta8vec,
	                                        exvector & fvec, exvector & dvec,
	                                        exvectorvector & Tvecs,
	                                        exvectorvector & ONEvecs,
	                                        exvector & unknownvec);
	friend exvector recombine_color_string(exvector & delta8vec, exvector & fvec,
	                                       exvector & dvec, exvectorvector & Tvecs,
	                                       exvectorvector & ONEvecs, exvector & unknownvec);
	friend ex color_trace_of_one_representation_label(const exvector & v);
	friend ex color_trace(const exvector & v, unsigned rl);
	friend ex simplify_pure_color_string(const ex & e);
	friend ex simplify_color(const ex & e);

// types

public:
	/** Type of object */
	typedef enum {
		invalid,      /**< not properly constructed */
		color_T,      /**< one of the generators T_a of SU(3) (these are non-commutative) */
		color_f,      /**< one of the antisymmetric structure constants f_abc of SU(3) */
		color_d,      /**< one of the symmetric structure constants d_abc of SU(3) */
		color_delta8, /**< the unity matrix */
		color_ONE     /**< the unity element of su(3) */
	} color_types;
	
// member functions

	// default constructor, destructor, copy constructor assignment operator and helpers
public:
	color();
	~color();
	color(const color & other);
	const color & operator=(const color & other);
protected:
	void copy(const color & other); 
	void destroy(bool call_parent);

	// other constructors
protected:
	color(color_types const t, unsigned rl=0);
	color(color_types const t, const ex & i1, unsigned rl=0);
	color(color_types const t, const ex & i1, const ex & i2, unsigned rl=0);
	color(color_types const t, const ex & i1, const ex & i2, const ex & i3,
	      unsigned rl=0);
	color(color_types const t, const exvector & iv, unsigned rl=0);
	color(color_types const t, exvector * ivp, unsigned rl=0);
	
	// functions overriding virtual functions from base classes
public:
	basic * duplicate() const;
	void printraw(std::ostream & os) const;
	void printtree(std::ostream & os, unsigned indent) const;
	void print(std::ostream & os, unsigned upper_precedence=0) const;
	bool info(unsigned inf) const;
	ex eval(int level=0) const;
protected:
	int compare_same_type(const basic & other) const;
	bool is_equal_same_type(const basic & other) const;
	ex simplify_ncmul(const exvector & v) const;
	ex thisexprseq(const exvector & v) const;
	ex thisexprseq(exvector * vp) const;

	// new virtual functions which can be overridden by derived classes
	// none
	
	// non-virtual functions in this class
protected:
	bool all_of_type_coloridx(void) const;
	
// member variables

protected:
	color_types type; /**< Type of object (generator, structure constant etc.) */
	unsigned representation_label; /**< Representation label to distiguish independent color matrices coming from separated fermion lines */
};

// global constants

extern const color some_color;
extern const std::type_info & typeid_color;

// global functions
inline const color &ex_to_color(const ex &e)
{
	return static_cast<const color &>(*e.bp);
}

inline color &ex_to_nonconst_color(const ex &e)
{
	return static_cast<color &>(*e.bp);
}

color color_ONE(unsigned rl=0);
color color_T(const ex & a, unsigned rl=0);
color color_f(const ex & a, const ex & b, const ex & c);
color color_d(const ex & a, const ex & b, const ex & c);
ex color_h(const ex & a, const ex & b, const ex & c);
color color_delta8(const ex & a, const ex & b);
void split_color_string_in_parts(const exvector & v, exvector & delta8vec,
                                 exvector & fvec, exvector & dvec,
                                 exvectorvector & Tvecs,
                                 exvectorvector & ONEvecs,
                                 exvector & unknownvec);
exvector recombine_color_string(exvector & delta8vec, exvector & fvec,
                                exvector & dvec, exvectorvector & Tvecs,
                                exvectorvector & ONEvecs, exvector & unknownvec);
ex color_trace_of_one_representation_label(const exvector & v);
ex color_trace(const exvector & v, unsigned rl=0);
ex simplify_pure_color_string(const ex & e);
ex simplify_color(const ex & e);

ex brute_force_sum_color_indices(const ex & e);

#ifndef NO_NAMESPACE_GINAC
} // namespace GiNaC
#endif // ndef NO_NAMESPACE_GINAC

#endif // ndef __GINAC_COLOR_H__
