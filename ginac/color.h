/** @file color.h
 *
 *  Interface to GiNaC's color objects. */

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

#ifndef __GINAC_COLOR_H__
#define __GINAC_COLOR_H__

#include <string>
#include <vector>
#include <ginac/indexed.h>
#include <ginac/ex.h>

#ifndef NO_GINAC_NAMESPACE
namespace GiNaC {
#endif // ndef NO_GINAC_NAMESPACE

const unsigned MAX_REPRESENTATION_LABELS = 4;
const unsigned COLOR_EIGHT = 8; // N*N-1
const unsigned COLOR_THREE = 3; // N

typedef vector<exvector,malloc_alloc> exvectorvector;

/** Base class for color object */
class color : public indexed
{
// friends

    friend color color_ONE(unsigned const rl);
    friend color color_T(ex const & a, unsigned const rl);
    friend color color_f(ex const & a, ex const & b, ex const & c);
    friend color color_d(ex const & a, ex const & b, ex const & c);
    friend ex color_h(ex const & a, ex const & b, ex const & c);
    friend color color_delta8(ex const & a, ex const & b);
    friend unsigned subs_index_in_exvector(exvector & v, ex const & is, ex const & ir);
    friend void split_color_string_in_parts(exvector const & v, exvector & delta8vec,
                                            exvector & fvec, exvector & dvec,
                                            exvectorvector & Tvecs,
                                            exvectorvector & ONEvecs,
                                            exvector & unknownvec);
    friend exvector recombine_color_string(exvector & delta8vec, exvector & fvec,
                                           exvector & dvec, exvectorvector & Tvecs,
                                           exvectorvector & ONEvecs, exvector & unknownvec);
    friend ex color_trace_of_one_representation_label(exvector const & v);
    friend ex color_trace(exvector const & v, unsigned const rl);
    friend ex simplify_pure_color_string(ex const & e);
    friend ex simplify_color(ex const & e);

    
// types

public:
    typedef enum { invalid, // not properly constructed by one of the friend functions
                   color_T,
                   color_f,
                   color_d,
                   color_delta8,
                   color_ONE
    } color_types;
    
// member functions

    // default constructor, destructor, copy constructor assignment operator and helpers
public:
    color();
    ~color();
    color(color const & other);
    color const & operator=(color const & other);
protected:
    void copy(color const & other); 
    void destroy(bool call_parent);

    // other constructors
protected:
    color(color_types const t, unsigned const rl=0);
    color(color_types const t, ex const & i1, unsigned const rl=0);
    color(color_types const t, ex const & i1, ex const & i2, unsigned const rl=0);
    color(color_types const t, ex const & i1, ex const & i2, ex const & i3,
          unsigned const rl=0);
    color(color_types const t, exvector const & iv, unsigned const rl=0);
    color(color_types const t, exvector * ivp, unsigned const rl=0);
    
    // functions overriding virtual functions from base classes
public:
    basic * duplicate() const;
    void printraw(ostream & os) const;
    void printtree(ostream & os, unsigned indent) const;
    void print(ostream & os, unsigned upper_precedence=0) const;
    void printcsrc(ostream & os, unsigned type, unsigned upper_precedence=0) const;
    bool info(unsigned inf) const;
    ex eval(int level=0) const;
protected:
    int compare_same_type(basic const & other) const;
    bool is_equal_same_type(basic const & other) const;
    ex simplify_ncmul(exvector const & v) const;
    ex thisexprseq(exvector const & v) const;
    ex thisexprseq(exvector * vp) const;

    // new virtual functions which can be overridden by derived classes
    // none
    
    // non-virtual functions in this class
protected:
    bool all_of_type_coloridx(void) const;
    
// member variables

protected:
    color_types type;
    unsigned representation_label; // to distiguish independent color matrices coming from separated fermion lines
};

// global constants

extern const color some_color;
extern type_info const & typeid_color;

// global functions
inline const color &ex_to_color(const ex &e)
{
	return static_cast<const color &>(*e.bp);
}

inline color &ex_to_nonconst_color(const ex &e)
{
	return static_cast<color &>(*e.bp);
}

color color_ONE(unsigned const rl=0);
color color_T(ex const & a, unsigned const rl=0);
color color_f(ex const & a, ex const & b, ex const & c);
color color_d(ex const & a, ex const & b, ex const & c);
ex color_h(ex const & a, ex const & b, ex const & c);
color color_delta8(ex const & a, ex const & b);
void split_color_string_in_parts(exvector const & v, exvector & delta8vec,
                                 exvector & fvec, exvector & dvec,
                                 exvectorvector & Tvecs,
                                 exvectorvector & ONEvecs,
                                 exvector & unknownvec);
exvector recombine_color_string(exvector & delta8vec, exvector & fvec,
                                exvector & dvec, exvectorvector & Tvecs,
                                exvectorvector & ONEvecs, exvector & unknownvec);
ex color_trace_of_one_representation_label(exvector const & v);
ex color_trace(exvector const & v, unsigned const rl=0);
ex simplify_pure_color_string(ex const & e);
ex simplify_color(ex const & e);

ex brute_force_sum_color_indices(ex const & e);

void append_exvector_to_exvector(exvector & dest, exvector const & source);

#ifndef NO_GINAC_NAMESPACE
} // namespace GiNaC
#endif // ndef NO_GINAC_NAMESPACE

#endif // ndef __GINAC_COLOR_H__
