/** @file utils.cpp
 *
 *  Implementation of several small and furry utilities. */

unsigned log2(unsigned n)
{
    unsigned k;
    for (k = 0; n > 1; n >>= 1) ++k;
    return k;
}

int compare_pointers(void const * a, void const * b)
{
    if (a<b) {
        return -1;
    } else if (a>b) {
        return 1;
    }
    return 0;
}

// comment skeletton for header files


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
    


// comment skeletton for implementation files


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




