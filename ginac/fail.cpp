/** @file fail.cpp
 *
 *  Implementation of class signaling failure of operation. Considered
 *  obsolete all this stuff ought to be replaced by exceptions. */

#include "ginac.h"

//////////
// default constructor, destructor, copy constructor assignment operator and helpers
//////////

// public

fail::fail() : basic(TINFO_FAIL)
{
    debugmsg("fail default constructor",LOGLEVEL_CONSTRUCT);
}

fail::~fail()
{
    debugmsg("fail destructor",LOGLEVEL_DESTRUCT);
    destroy(0);
}

fail::fail(fail const & other)
{
    debugmsg("fail copy constructor",LOGLEVEL_CONSTRUCT);
    copy(other);
}

fail const & fail::operator=(fail const & other)
{
    debugmsg("fail operator=",LOGLEVEL_ASSIGNMENT);
    if (this != &other) {
        destroy(1);
        copy(other);
    }
    return *this;
}

// protected

void fail::copy(fail const & other)
{
    basic::copy(other);
}

void fail::destroy(bool call_parent)
{
    if (call_parent) basic::destroy(call_parent);
}


//////////
// other constructors
//////////

// none

//////////
// functions overriding virtual functions from bases classes
//////////

// public

basic * fail::duplicate() const
{
    debugmsg("fail duplicate",LOGLEVEL_DUPLICATE);
    return new fail(*this);
}

// protected

int fail::compare_same_type(basic const & other) const
{
	// two fails are always identical
    return 0;
}

//////////
// new virtual functions which can be overridden by derived classes
//////////

// none

//////////
// non-virtual functions in this class
//////////

// none

//////////
// static member variables
//////////

// none

//////////
// global constants
//////////

const fail some_fail;
type_info const & typeid_fail=typeid(some_fail);

