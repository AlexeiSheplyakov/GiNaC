/** @file utils.cpp
 *
 *  Implementation of several small and furry utilities. */

/*
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

#include "utils.h"

#ifndef NO_GINAC_NAMESPACE
namespace GiNaC {
#endif // ndef NO_GINAC_NAMESPACE

/** Integer binary logarithm */
unsigned log2(unsigned n)
{
    unsigned k;
    for (k = 0; n > 1; n >>= 1) ++k;
    return k;
}

/** Compare two pointers (just to establish some sort of canonical order).
 *  @return -1, 0, or 1 */
int compare_pointers(void const * a, void const * b)
{
    if (a<b) {
        return -1;
    } else if (a>b) {
        return 1;
    }
    return 0;
}


// comment skeleton for header files


// member functions

    // default constructor, destructor, copy constructor assignment operator and helpers
    // none

    // other constructors
    // none

    // functions overriding virtual functions from bases classes
    // none
    
    // new virtual functions which can be overridden by derived classes
    // none

    // non-virtual functions in this class
    // none

// member variables
// none
    


// comment skeleton for implementation files


//////////
// default constructor, destructor, copy constructor assignment operator and helpers
//////////

// public
// protected

//////////
// other constructors
//////////

// public
// none

//////////
// functions overriding virtual functions from bases classes
//////////

// public
// protected
// none

//////////
// new virtual functions which can be overridden by derived classes
//////////

// public
// protected
// none

//////////
// non-virtual functions in this class
//////////

// public
// protected
// none

//////////
// static member variables
//////////

// protected
// private
// none

#ifndef NO_GINAC_NAMESPACE
} // namespace GiNaC
#endif // ndef NO_GINAC_NAMESPACE
