/** @file clifford.cpp
 *
 *  Implementation of GiNaC's clifford objects.
 *  No real implementation yet, to be done.     */

#include <string>

#include "ginac.h"
#include "utils.h"

//////////
// default constructor, destructor, copy constructor assignment operator and helpers
//////////

// public

clifford::clifford()
{
    debugmsg("clifford default constructor",LOGLEVEL_CONSTRUCT);
    serial=next_serial++;
    name=autoname_prefix()+ToString(serial);
    tinfo_key=TINFO_CLIFFORD;
}

clifford::~clifford()
{
    debugmsg("clifford destructor",LOGLEVEL_DESTRUCT);
    destroy(0);
}

clifford::clifford(clifford const & other)
{
    debugmsg("clifford copy constructor",LOGLEVEL_CONSTRUCT);
    copy (other);
}

clifford const & clifford::operator=(clifford const & other)
{
    debugmsg("clifford operator=",LOGLEVEL_ASSIGNMENT);
    if (this != &other) {
        destroy(1);
        copy(other);
    }
    return *this;
}

// protected

void clifford::copy(clifford const & other)
{
    indexed::copy(other);
    name=other.name;
    serial=other.serial;
}

void clifford::destroy(bool call_parent)
{
    if (call_parent) {
        indexed::destroy(call_parent);
    }
}

//////////
// other constructors
//////////

// public

clifford::clifford(string const & initname)
{
    debugmsg("clifford constructor from string",LOGLEVEL_CONSTRUCT);
    name=initname;
    serial=next_serial++;
    tinfo_key=TINFO_CLIFFORD;
}

//////////
// functions overriding virtual functions from bases classes
//////////

// public

basic * clifford::duplicate() const
{
    debugmsg("clifford duplicate",LOGLEVEL_DUPLICATE);
    return new clifford(*this);
}

void clifford::printraw(ostream & os) const
{
    debugmsg("clifford printraw",LOGLEVEL_PRINT);
    os << "clifford(" << "name=" << name << ",serial=" << serial
       << ",indices=";
    printrawindices(os);
    os << ",hash=" << hashvalue << ",flags=" << flags << ")";
}

void clifford::printtree(ostream & os, unsigned indent) const
{
    debugmsg("clifford printtree",LOGLEVEL_PRINT);
    os << string(indent,' ') << name << " (clifford): "
       << "serial=" << serial << ","
       << seq.size() << "indices=";
    printtreeindices(os,indent);
    os << ", hash=" << hashvalue << " (0x" << hex << hashvalue << dec << ")"
       << ", flags=" << flags << endl;
}

void clifford::print(ostream & os, unsigned upper_precedence) const
{
    debugmsg("clifford print",LOGLEVEL_PRINT);
    os << name;
    printindices(os);
}

void clifford::printcsrc(ostream & os, unsigned type, unsigned upper_precedence) const
{
    debugmsg("clifford print csrc",LOGLEVEL_PRINT);
    print(os,upper_precedence);
}

bool clifford::info(unsigned inf) const
{
    return indexed::info(inf);
}

// protected

int clifford::compare_same_type(basic const & other) const
{
    ASSERT(other.tinfo() == TINFO_CLIFFORD);
    const clifford *o = static_cast<const clifford *>(&other);
    if (serial==o->serial) {
        return indexed::compare_same_type(other);
    }
    return serial < o->serial ? -1 : 1;
}

ex clifford::simplify_ncmul(exvector const & v) const
{
    return simplified_ncmul(v);
}

unsigned clifford::calchash(void) const
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

void clifford::setname(string const & n)
{
    name=n;
}

// private

string & clifford::autoname_prefix(void)
{
    static string * s=new string("clifford");
    return *s;
}

//////////
// static member variables
//////////

// private

unsigned clifford::next_serial=0;

//////////
// global constants
//////////

const clifford some_clifford;
type_info const & typeid_clifford=typeid(some_clifford);

