/** @file indexed.h
 *
 *  Interface to GiNaC's index carrying objects. */

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

#ifndef __GINAC_INDEXED_H__
#define __GINAC_INDEXED_H__

#include <string>
#include <ginac/exprseq.h>

#ifndef NO_GINAC_NAMESPACE
namespace GiNaC {
#endif // ndef NO_GINAC_NAMESPACE

/** Base class for non-commutative indexed objects */
class indexed : public exprseq
{
    GINAC_DECLARE_REGISTERED_CLASS(indexed, exprseq)

// member functions

    // default constructor, destructor, copy constructor assignment operator and helpers
public:
    indexed();
    ~indexed();
    indexed(indexed const & other);
    indexed const & operator=(indexed const & other);
protected:
    void copy(indexed const & other); 
    void destroy(bool call_parent);

    // other constructors
public:
    indexed(ex const & i1);
    indexed(ex const & i1, ex const & i2);
    indexed(ex const & i1, ex const & i2, ex const & i3);
    indexed(ex const & i1, ex const & i2, ex const & i3, ex const & i4);
    indexed(exvector const & iv);
    indexed(exvector * iv);

    // functions overriding virtual functions from base classes
public:
    basic * duplicate() const;
    void printraw(ostream & os) const;
    void printtree(ostream & os, unsigned indent) const;
    void print(ostream & os, unsigned upper_precedence=0) const;
    void printcsrc(ostream & os, unsigned type, unsigned upper_precedence) const;
    bool info(unsigned inf) const;
    ex diff(symbol const & s) const;
    exvector get_indices(void) const;
protected:
    int compare_same_type(basic const & other) const;
    bool is_equal_same_type(basic const & other) const;
    unsigned return_type(void) const;
    unsigned return_type_tinfo(void) const;
    ex thisexprseq(exvector const & v) const;
    ex thisexprseq(exvector * vp) const;

    // new virtual functions which can be overridden by derived classes
    // none
    
    // non-virtual functions in this class
protected:
    void printrawindices(ostream & os) const;
    void printtreeindices(ostream & os, unsigned indent) const;
    void printindices(ostream & os) const;
    bool all_of_type_idx(void) const;

// member variables
    // none
};

// global constants

extern const indexed some_indexed;
extern type_info const & typeid_indexed;

// utility functions
inline const indexed &ex_to_indexed(const ex &e)
{
	return static_cast<const indexed &>(*e.bp);
}

#ifndef NO_GINAC_NAMESPACE
} // namespace GiNaC
#endif // ndef NO_GINAC_NAMESPACE

#endif // ndef __GINAC_INDEXED_H__
