/** @file coloridx.cpp
 *
 *  Implementation of GiNaC's color indices. */

#include <stdexcept>

#include "ginac.h"
#include "utils.h"

//////////
// default constructor, destructor, copy constructor assignment operator and helpers
//////////

// public

coloridx::coloridx()
{
    debugmsg("coloridx default constructor",LOGLEVEL_CONSTRUCT);
    // serial is incremented in idx::idx()
    name="color"+ToString(serial);
    tinfo_key=TINFO_COLORIDX;
}

coloridx::~coloridx() 
{
    debugmsg("coloridx destructor",LOGLEVEL_DESTRUCT);
    destroy(0);
}

coloridx::coloridx(coloridx const & other)
{
    debugmsg("coloridx copy constructor",LOGLEVEL_CONSTRUCT);
    copy(other);
}

coloridx const & coloridx::operator=(coloridx const & other)
{
    debugmsg("coloridx operator=",LOGLEVEL_ASSIGNMENT);
    if (this != &other) {
        destroy(1);
        copy(other);
    }
    return *this;
}

// protected

void coloridx::copy(coloridx const & other)
{
    idx::copy(other);
}

void coloridx::destroy(bool call_parent)
{
    if (call_parent) idx::destroy(call_parent);
}

//////////
// other constructors
//////////

// public

coloridx::coloridx(bool cov) : idx(cov)
{
    debugmsg("coloridx constructor from bool",LOGLEVEL_CONSTRUCT);
    // serial is incremented in idx::idx(bool)
    name="color"+ToString(serial);
    tinfo_key=TINFO_COLORIDX;
}

coloridx::coloridx(string const & n, bool cov) : idx(n,cov)
{
    debugmsg("coloridx constructor from string,bool",LOGLEVEL_CONSTRUCT);
    tinfo_key=TINFO_COLORIDX;
}

coloridx::coloridx(char const * n, bool cov) : idx(n,cov)
{
    debugmsg("coloridx constructor from char*,bool",LOGLEVEL_CONSTRUCT);
    tinfo_key=TINFO_COLORIDX;
}

coloridx::coloridx(unsigned const v, bool cov) : idx(v,cov)
{
    debugmsg("coloridx constructor from unsigned,bool",LOGLEVEL_CONSTRUCT);
    tinfo_key=TINFO_COLORIDX;
}

//////////
// functions overriding virtual functions from bases classes
//////////

// public

basic * coloridx::duplicate() const
{
    debugmsg("coloridx duplicate",LOGLEVEL_DUPLICATE);
    return new coloridx(*this);
}

void coloridx::printraw(ostream & os) const
{
    debugmsg("coloridx printraw",LOGLEVEL_PRINT);

    os << "coloridx(";

    if (symbolic) {
        os << "symbolic,name=" << name;
    } else {
        os << "non symbolic,value=" << value;
    }

    if (covariant) {
        os << ",covariant";
    } else {
        os << ",contravariant";
    }

    os << ",serial=" << serial;
    os << ",hash=" << hashvalue << ",flags=" << flags;
    os << ")";
}

void coloridx::printtree(ostream & os, unsigned indent) const
{
    debugmsg("coloridx printtree",LOGLEVEL_PRINT);

    os << string(indent,' ') << "coloridx: ";

    if (symbolic) {
        os << "symbolic,name=" << name;
    } else {
        os << "non symbolic,value=" << value;
    }

    if (covariant) {
        os << ",covariant";
    } else {
        os << ",contravariant";
    }

    os << ", serial=" << serial
       << ", hash=" << hashvalue << " (0x" << hex << hashvalue << dec << ")"
       << ", flags=" << flags << endl;
}

void coloridx::print(ostream & os, unsigned upper_precedence) const
{
    debugmsg("coloridx print",LOGLEVEL_PRINT);

    if (covariant) {
        os << "_";
    } else {
        os << "~";
    }
    if (symbolic) {
        os << name;
    } else {
        os << value;
    }
}

bool coloridx::info(unsigned inf) const
{
    if (inf==info_flags::coloridx) return true;
    return idx::info(inf);
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

const coloridx some_coloridx;
type_info const & typeid_coloridx=typeid(some_coloridx);



