/** @file fail.h
 *
 *  Interface to class signaling failure of operation. Considered obsolete all
 *  this stuff ought to be replaced by exceptions.
 *
 *  GiNaC Copyright (C) 1999 Johannes Gutenberg University Mainz, Germany
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

#ifndef __GINAC_FAIL_H__
#define __GINAC_FAIL_H__

class fail : public basic
{
// member functions

    // default constructor, destructor, copy constructor assignment operator and helpers
public:
    fail();
    ~fail();
    fail(fail const & other);
    fail const & operator=(fail const & other);
protected:
    void copy(fail const & other);
    void destroy(bool call_parent);

    // other constructors
    // none

    // functions overriding virtual functions from bases classes
public:
    basic * duplicate() const;
    void printraw(ostream & os) const;
    void print(ostream & os, unsigned upper_precedence=0) const;
protected:
    int compare_same_type(basic const & other) const;
    unsigned return_type(void) const { return return_types::noncommutative_composite; };
    
    // new virtual functions which can be overridden by derived classes
    // none

    // non-virtual functions in this class
    // none

// member variables
// none
};

// global constants

extern const fail some_fail;
extern type_info const & typeid_fail;

#endif // ndef __GINAC_FAIL_H__


