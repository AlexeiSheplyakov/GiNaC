/** @file isospin.cpp
 *
 *  Implementation of GiNaC's isospin objects.
 *  No real implementation yet, to be done.     */

#include <string>

#include "ginac.h"
#include "utils.h"

//////////
// default constructor, destructor, copy constructor assignment operator and helpers
//////////

// public

isospin::isospin()
{
    debugmsg("isospin default constructor",LOGLEVEL_CONSTRUCT);
    serial=next_serial++;
    name=autoname_prefix()+ToString(serial);
    tinfo_key=TINFO_ISOSPIN;
}

isospin::~isospin()
{
    debugmsg("isospin destructor",LOGLEVEL_DESTRUCT);
    destroy(0);
}

isospin::isospin(isospin const & other)
{
    debugmsg("isospin copy constructor",LOGLEVEL_CONSTRUCT);
    copy (other);
}

isospin const & isospin::operator=(isospin const & other)
{
    debugmsg("isospin operator=",LOGLEVEL_ASSIGNMENT);
    if (this != &other) {
        destroy(1);
        copy(other);
    }
    return *this;
}

// protected

void isospin::copy(isospin const & other)
{
    indexed::copy(other);
    name=other.name;
    serial=other.serial;
}

void isospin::destroy(bool call_parent)
{
    if (call_parent) {
        indexed::destroy(call_parent);
    }
}

//////////
// other constructors
//////////

// public

isospin::isospin(string const & initname)
{
    debugmsg("isospin constructor from string",LOGLEVEL_CONSTRUCT);
    name=initname;
    serial=next_serial++;
    tinfo_key=TINFO_ISOSPIN;
}

//////////
// functions overriding virtual functions from bases classes
//////////

// public

basic * isospin::duplicate() const
{
    debugmsg("isospin duplicate",LOGLEVEL_DUPLICATE);
    return new isospin(*this);
}

void isospin::printraw(ostream & os) const
{
    debugmsg("isospin printraw",LOGLEVEL_PRINT);
    os << "isospin(" << "name=" << name << ",serial=" << serial
       << ",indices=";
    printrawindices(os);
    os << ",hash=" << hashvalue << ",flags=" << flags << ")";
}

void isospin::printtree(ostream & os, unsigned indent) const
{
    debugmsg("isospin printtree",LOGLEVEL_PRINT);
    os << string(indent,' ') << name << " (isospin): "
       << "serial=" << serial << ","
       << seq.size() << "indices=";
    printtreeindices(os,indent);
    os << ", hash=" << hashvalue << " (0x" << hex << hashvalue << dec << ")"
       << ", flags=" << flags << endl;
}

void isospin::print(ostream & os, unsigned upper_precedence) const
{
    debugmsg("isospin print",LOGLEVEL_PRINT);
    os << name;
    printindices(os);
}

void isospin::printcsrc(ostream & os, unsigned type, unsigned upper_precedence) const
{
    debugmsg("isospin print csrc",LOGLEVEL_PRINT);
    print(os,upper_precedence);
}

bool isospin::info(unsigned inf) const
{
    return indexed::info(inf);
}

// protected

int isospin::compare_same_type(basic const & other) const
{
    ASSERT(other.tinfo() == TINFO_ISOSPIN);
    const isospin *o = static_cast<const isospin *>(&other);
    if (serial==o->serial) {
        return indexed::compare_same_type(other);
    }
    return serial < o->serial ? -1 : 1;
}

ex isospin::simplify_ncmul(exvector const & v) const
{
    return simplified_ncmul(v);
}

unsigned isospin::calchash(void) const
{
    hashvalue=golden_ratio_hash(golden_ratio_hash(0x55555556U ^
                                                  golden_ratio_hash(tinfo_key) ^
                                                  serial));
    setflag(status_flags::hash_calculated);
    return hashvalue;
}

//////////
// virtual functions which can be overridden by derived classes
//////////

// none

//////////
// non-virtual functions in this class
//////////

void isospin::setname(string const & n)
{
    name=n;
}

// private

string & isospin::autoname_prefix(void)
{
    static string * s=new string("isospin");
    return *s;
}

//////////
// static member variables
//////////

// private

unsigned isospin::next_serial=0;

//////////
// global constants
//////////

const isospin some_isospin;
type_info const & typeid_isospin=typeid(some_isospin);

