/** @file relational.cpp
 *
 *  Implementation of relations between expressions */

#include <stdexcept>

#include "ginac.h"

//////////
// default constructor, destructor, copy constructor assignment operator and helpers
//////////

// public

relational::relational() : basic(TINFO_RELATIONAL)
{
    debugmsg("relational default constructor",LOGLEVEL_CONSTRUCT);
}

relational::~relational()
{
    debugmsg("relational destructor",LOGLEVEL_DESTRUCT);
    destroy(0);
}

relational::relational(relational const & other)
{
    debugmsg("relational copy constructor",LOGLEVEL_CONSTRUCT);
    copy(other);
}

relational const & relational::operator=(relational const & other)
{
    debugmsg("relational operator=",LOGLEVEL_ASSIGNMENT);
    if (this != &other) {
        destroy(1);
        copy(other);
    }
    return *this;
}

// protected

void relational::copy(relational const & other)
{
    basic::copy(other);
    lh=other.lh;
    rh=other.rh;
    o=other.o;
}

void relational::destroy(bool call_parent)
{
    if (call_parent) basic::destroy(call_parent);
}

//////////
// other constructors
//////////

// public

relational::relational(ex const & lhs, ex const & rhs, operators oper) : basic(TINFO_RELATIONAL)
{
    debugmsg("relational constructor ex,ex,operator",LOGLEVEL_CONSTRUCT);
    lh=lhs;
    rh=rhs;
    o=oper;
}

//////////
// functions overriding virtual functions from bases classes
//////////

// public

basic * relational::duplicate() const
{
    debugmsg("relational duplicate",LOGLEVEL_DUPLICATE);
    return new relational(*this);
}

bool relational::info(unsigned inf) const
{
    switch (inf) {
    case info_flags::relation:
        return 1;
    case info_flags::relation_equal:
        return o==equal;
    case info_flags::relation_not_equal:
        return o==not_equal;
    case info_flags::relation_less:
        return o==less;
    case info_flags::relation_less_or_equal:
        return o==less_or_equal;
    case info_flags::relation_greater:
        return o==greater;
    case info_flags::relation_greater_or_equal:
        return o==greater_or_equal;
    }
    return 0;
}

int relational::nops() const
{
    return 2;
}

ex & relational::let_op(int const i)
{
    ASSERT(i>=0);
    ASSERT(i<2);

    return i==0 ? lh : rh;
}
    
ex relational::eval(int level) const
{
    if (level==1) {
        return this->hold();
    }
    if (level == -max_recursion_level) {
        throw(std::runtime_error("max recursion level reached"));
    }
    return (new relational(lh.eval(level-1),rh.eval(level-1),o))->
            setflag(status_flags::dynallocated  |
                    status_flags::evaluated );
}

ex relational::evalf(int level) const
{
    if (level==1) {
        return *this;
    }
    if (level == -max_recursion_level) {
        throw(std::runtime_error("max recursion level reached"));
    }
    return (new relational(lh.eval(level-1),rh.eval(level-1),o))->
            setflag(status_flags::dynallocated);
}

ex relational::simplify_ncmul(exvector const & v) const
{
    return lh.simplify_ncmul(v);
}

// protected

int relational::compare_same_type(basic const & other) const
{
    ASSERT(is_exactly_of_type(other, relational));
    relational const & oth=static_cast<relational const &>(const_cast<basic &>(other));
    
    int cmpval;
    
    if (o == oth.o) {
        cmpval=lh.compare(oth.lh);
        if (cmpval==0) {
            return rh.compare(oth.rh);
        } else {
            return cmpval;
        }
    }
    if (o<oth.o) {
        return -1;
    } else {
        return 1;
    }
}

unsigned relational::return_type(void) const
{
    ASSERT(lh.return_type()==rh.return_type());
    return lh.return_type();
}
   
unsigned relational::return_type_tinfo(void) const
{
    ASSERT(lh.return_type_tinfo()==rh.return_type_tinfo());
    return lh.return_type_tinfo();
}

//////////
// new virtual functions which can be overridden by derived classes
//////////

// none

//////////
// non-virtual functions in this class
//////////

#include <iostream>

relational::operator bool() const
{
    // please note that (a<b) == false does not imply (a>=b) == true
    // a false result means the comparison is either false or undecidable
    // (except for !=, where true means unequal or undecidable)
    ex df=lh-rh;
    if (!is_ex_exactly_of_type(df,numeric)) {
        return o==not_equal ? true : false; // cannot decide on non-numerical results
    }
    int cmpval=ex_to_numeric(df).compare(numZERO());
    switch (o) {
    case equal:
        return cmpval==0;
        break;
    case not_equal:
        return cmpval!=0;
        break;
    case less:
        return cmpval<0;
        break;
    case less_or_equal:
        return cmpval<=0;
        break;
    case greater:
        return cmpval>0;
        break;
    case greater_or_equal:
        return cmpval>=0;
        break;
    default:
        throw(std::logic_error("invalid relational operator"));
    }
    return 0;
}

//////////
// static member variables
//////////

// protected

unsigned relational::precedence=20;

//////////
// global constants
//////////

const relational some_relational;
type_info const & typeid_relational=typeid(some_relational);

