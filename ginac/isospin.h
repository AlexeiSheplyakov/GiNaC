/** @file isospin.h
 *
 *  Interface to GiNaC's isospin objects. */

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

#ifndef __GINAC_ISOSPIN_H__
#define __GINAC_ISOSPIN_H__

#include <string>
#include "indexed.h"

#ifndef NO_NAMESPACE_GINAC
namespace GiNaC {
#endif // ndef NO_NAMESPACE_GINAC

/** Base class for isospin object */
class isospin : public indexed
{
    GINAC_DECLARE_REGISTERED_CLASS(isospin, indexed)

// member functions

    // default constructor, destructor, copy constructor assignment operator and helpers
public:
    isospin();
    ~isospin();
    isospin(const isospin & other);
    const isospin & operator=(const isospin & other);
protected:
    void copy(const isospin & other); 
    void destroy(bool call_parent);

    // other constructors
public:
    explicit isospin(const std::string & initname);

    // functions overriding virtual functions from base classes
public:
    basic * duplicate() const;
    void printraw(std::ostream & os) const;
    void printtree(std::ostream & os, unsigned indent) const;
    void print(std::ostream & os, unsigned upper_precedence=0) const;
    void printcsrc(std::ostream & os, unsigned type, unsigned upper_precedence=0) const;
    bool info(unsigned inf) const;
protected:
    int compare_same_type(const basic & other) const;
    ex simplify_ncmul(const exvector & v) const;
    unsigned calchash(void) const;

    // new virtual functions which can be overridden by derived classes
    // none
    
    // non-virtual functions in this class
public:
    void setname(const std::string & n);
private:
    std::string & autoname_prefix(void);

// member variables

protected:
    std::string name;
    unsigned serial; // unique serial number for comparision
private:
    static unsigned next_serial;
};

// global constants

extern const isospin some_isospin;
extern const type_info & typeid_isospin;

// utility functions
inline const isospin &ex_to_isospin(const ex &e)
{
	return static_cast<const isospin &>(*e.bp);
}

#ifndef NO_NAMESPACE_GINAC
} // namespace GiNaC
#endif // ndef NO_NAMESPACE_GINAC

#endif // ndef __GINAC_ISOSPIN_H__
