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
#include "exprseq.h"

#ifndef NO_NAMESPACE_GINAC
namespace GiNaC {
#endif // ndef NO_NAMESPACE_GINAC

/** Base class for non-commutative indexed objects */
class indexed : public exprseq
{
    GINAC_DECLARE_REGISTERED_CLASS(indexed, exprseq)

// member functions

    // default constructor, destructor, copy constructor assignment operator and helpers
public:
    indexed();
    ~indexed();
    indexed(const indexed & other);
    const indexed & operator=(const indexed & other);
protected:
    void copy(const indexed & other); 
    void destroy(bool call_parent);

    // other constructors
public:
    indexed(const ex & i1);
    indexed(const ex & i1, const ex & i2);
    indexed(const ex & i1, const ex & i2, const ex & i3);
    indexed(const ex & i1, const ex & i2, const ex & i3, const ex & i4);
    indexed(const exvector & iv);
    indexed(exvector * iv);

    // functions overriding virtual functions from base classes
public:
    basic * duplicate() const;
    void printraw(ostream & os) const;
    void printtree(ostream & os, unsigned indent) const;
    void print(ostream & os, unsigned upper_precedence=0) const;
    void printcsrc(ostream & os, unsigned type, unsigned upper_precedence) const;
    bool info(unsigned inf) const;
    ex diff(const symbol & s) const;
    exvector get_indices(void) const;
protected:
    int compare_same_type(const basic & other) const;
    bool is_equal_same_type(const basic & other) const;
    unsigned return_type(void) const;
    unsigned return_type_tinfo(void) const;
    ex thisexprseq(const exvector & v) const;
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
extern const type_info & typeid_indexed;

// utility functions
inline const indexed &ex_to_indexed(const ex &e)
{
	return static_cast<const indexed &>(*e.bp);
}

#ifndef NO_NAMESPACE_GINAC
} // namespace GiNaC
#endif // ndef NO_NAMESPACE_GINAC

#endif // ndef __GINAC_INDEXED_H__
