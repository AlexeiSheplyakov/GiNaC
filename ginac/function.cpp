/** @file function.cpp
 *
 *  Implementation of class function.
 *
 *  This file was generated automatically by function.pl.
 *  Please do not modify it directly, edit the perl script instead!
 *  function.pl options: $maxargs=10 */

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

#include <string>
#include <stdexcept>

#include "function.h"
#include "ex.h"

//////////
// default constructor, destructor, copy constructor assignment operator and helpers
//////////

// public

function::function() : serial(0)
{
    debugmsg("function default constructor",LOGLEVEL_CONSTRUCT);
    tinfo_key = TINFO_function;
}

function::~function()
{
    debugmsg("function destructor",LOGLEVEL_DESTRUCT);
    destroy(0);
}

function::function(function const & other)
{
    debugmsg("function copy constructor",LOGLEVEL_CONSTRUCT);
    copy(other);
}

function const & function::operator=(function const & other)
{
    debugmsg("function operator=",LOGLEVEL_ASSIGNMENT);
    if (this != &other) {
        destroy(1);
        copy(other);
    }
    return *this;
}

// protected

void function::copy(function const & other)
{
    exprseq::copy(other);
    serial=other.serial;
}

void function::destroy(bool call_parent)
{
    if (call_parent) exprseq::destroy(call_parent);
}

//////////
// other constructors
//////////

// public

function::function(unsigned ser) : serial(ser)
{
    debugmsg("function constructor from unsigned",LOGLEVEL_CONSTRUCT);
    tinfo_key = TINFO_function;
}

// the following lines have been generated for max. 10 parameters
function::function(unsigned ser, ex const & param1)
    : exprseq(param1), serial(ser)
{
    debugmsg("function constructor from unsigned,1*ex",LOGLEVEL_CONSTRUCT);
    tinfo_key = TINFO_function;
}
function::function(unsigned ser, ex const & param1, ex const & param2)
    : exprseq(param1, param2), serial(ser)
{
    debugmsg("function constructor from unsigned,2*ex",LOGLEVEL_CONSTRUCT);
    tinfo_key = TINFO_function;
}
function::function(unsigned ser, ex const & param1, ex const & param2, ex const & param3)
    : exprseq(param1, param2, param3), serial(ser)
{
    debugmsg("function constructor from unsigned,3*ex",LOGLEVEL_CONSTRUCT);
    tinfo_key = TINFO_function;
}
function::function(unsigned ser, ex const & param1, ex const & param2, ex const & param3, ex const & param4)
    : exprseq(param1, param2, param3, param4), serial(ser)
{
    debugmsg("function constructor from unsigned,4*ex",LOGLEVEL_CONSTRUCT);
    tinfo_key = TINFO_function;
}
function::function(unsigned ser, ex const & param1, ex const & param2, ex const & param3, ex const & param4, ex const & param5)
    : exprseq(param1, param2, param3, param4, param5), serial(ser)
{
    debugmsg("function constructor from unsigned,5*ex",LOGLEVEL_CONSTRUCT);
    tinfo_key = TINFO_function;
}
function::function(unsigned ser, ex const & param1, ex const & param2, ex const & param3, ex const & param4, ex const & param5, ex const & param6)
    : exprseq(param1, param2, param3, param4, param5, param6), serial(ser)
{
    debugmsg("function constructor from unsigned,6*ex",LOGLEVEL_CONSTRUCT);
    tinfo_key = TINFO_function;
}
function::function(unsigned ser, ex const & param1, ex const & param2, ex const & param3, ex const & param4, ex const & param5, ex const & param6, ex const & param7)
    : exprseq(param1, param2, param3, param4, param5, param6, param7), serial(ser)
{
    debugmsg("function constructor from unsigned,7*ex",LOGLEVEL_CONSTRUCT);
    tinfo_key = TINFO_function;
}
function::function(unsigned ser, ex const & param1, ex const & param2, ex const & param3, ex const & param4, ex const & param5, ex const & param6, ex const & param7, ex const & param8)
    : exprseq(param1, param2, param3, param4, param5, param6, param7, param8), serial(ser)
{
    debugmsg("function constructor from unsigned,8*ex",LOGLEVEL_CONSTRUCT);
    tinfo_key = TINFO_function;
}
function::function(unsigned ser, ex const & param1, ex const & param2, ex const & param3, ex const & param4, ex const & param5, ex const & param6, ex const & param7, ex const & param8, ex const & param9)
    : exprseq(param1, param2, param3, param4, param5, param6, param7, param8, param9), serial(ser)
{
    debugmsg("function constructor from unsigned,9*ex",LOGLEVEL_CONSTRUCT);
    tinfo_key = TINFO_function;
}
function::function(unsigned ser, ex const & param1, ex const & param2, ex const & param3, ex const & param4, ex const & param5, ex const & param6, ex const & param7, ex const & param8, ex const & param9, ex const & param10)
    : exprseq(param1, param2, param3, param4, param5, param6, param7, param8, param9, param10), serial(ser)
{
    debugmsg("function constructor from unsigned,10*ex",LOGLEVEL_CONSTRUCT);
    tinfo_key = TINFO_function;
}

// end of generated lines

function::function(unsigned ser, exprseq const & es) : exprseq(es), serial(ser)
{
    debugmsg("function constructor from unsigned,exprseq",LOGLEVEL_CONSTRUCT);
    tinfo_key = TINFO_function;
}

function::function(unsigned ser, exvector const & v, bool discardable) 
    : exprseq(v,discardable), serial(ser)
{
    debugmsg("function constructor from string,exvector,bool",LOGLEVEL_CONSTRUCT);
    tinfo_key = TINFO_function;
}

function::function(unsigned ser, exvector * vp) 
    : exprseq(vp), serial(ser)
{
    debugmsg("function constructor from unsigned,exvector *",LOGLEVEL_CONSTRUCT);
    tinfo_key = TINFO_function;
}

//////////
// functions overriding virtual functions from bases classes
//////////

// public

basic * function::duplicate() const
{
    debugmsg("function duplicate",LOGLEVEL_DUPLICATE);
    return new function(*this);
}

void function::printraw(ostream & os) const
{
    debugmsg("function printraw",LOGLEVEL_PRINT);

    ASSERT(serial<registered_functions().size());

    os << "function(name=" << registered_functions()[serial].name;
    for (exvector::const_iterator it=seq.begin(); it!=seq.end(); ++it) {
        os << ",";
        (*it).bp->print(os);
    }
    os << ")";
}

void function::print(ostream & os, unsigned upper_precedence) const
{
    debugmsg("function print",LOGLEVEL_PRINT);

    ASSERT(serial<registered_functions().size());

    os << registered_functions()[serial].name;
    printseq(os,'(',',',')',exprseq::precedence,function::precedence);
}

void function::printtree(ostream & os, unsigned indent) const
{
    debugmsg("function printtree",LOGLEVEL_PRINT);

    ASSERT(serial<registered_functions().size());

    os << string(indent,' ') << "function "
       << registered_functions()[serial].name
       << ", hash=" << hashvalue << " (0x" << hex << hashvalue << dec << ")"
       << ", flags=" << flags
       << ", nops=" << nops() << endl;
    for (int i=0; i<nops(); ++i) {
        seq[i].printtree(os,indent+delta_indent);
    }
    os << string(indent+delta_indent,' ') << "=====" << endl;
}

void function::printcsrc(ostream & os, unsigned type, unsigned upper_precedence) const
{
    debugmsg("function print csrc",LOGLEVEL_PRINT);

    ASSERT(serial<registered_functions().size());

	// Print function name in lowercase
    string lname;
    lname=registered_functions()[serial].name;
    for (unsigned i=0; i<lname.size(); i++)
        lname[i] = tolower(lname[i]);
    os << lname << "(";

	// Print arguments, separated by commas
    exvector::const_iterator it = seq.begin();
    exvector::const_iterator itend = seq.end();
    while (it != itend) {
        it->bp->printcsrc(os, type, 0);
        it++;
        if (it != itend)
            os << ",";
    }
    os << ")";
}

ex function::expand(unsigned options) const
{
    return this->setflag(status_flags::expanded);
}

ex function::eval(int level) const
{
    ASSERT(serial<registered_functions().size());

    exvector eseq=evalchildren(level);    

    if (registered_functions()[serial].e==0) {
        return function(serial,eseq).hold();
    }
    switch (registered_functions()[serial].nparams) {
        // the following lines have been generated for max. 10 parameters
    case 1:
        return ((eval_funcp_1)(registered_functions()[serial].e))(eseq[1-1]);
        break;
    case 2:
        return ((eval_funcp_2)(registered_functions()[serial].e))(eseq[1-1], eseq[2-1]);
        break;
    case 3:
        return ((eval_funcp_3)(registered_functions()[serial].e))(eseq[1-1], eseq[2-1], eseq[3-1]);
        break;
    case 4:
        return ((eval_funcp_4)(registered_functions()[serial].e))(eseq[1-1], eseq[2-1], eseq[3-1], eseq[4-1]);
        break;
    case 5:
        return ((eval_funcp_5)(registered_functions()[serial].e))(eseq[1-1], eseq[2-1], eseq[3-1], eseq[4-1], eseq[5-1]);
        break;
    case 6:
        return ((eval_funcp_6)(registered_functions()[serial].e))(eseq[1-1], eseq[2-1], eseq[3-1], eseq[4-1], eseq[5-1], eseq[6-1]);
        break;
    case 7:
        return ((eval_funcp_7)(registered_functions()[serial].e))(eseq[1-1], eseq[2-1], eseq[3-1], eseq[4-1], eseq[5-1], eseq[6-1], eseq[7-1]);
        break;
    case 8:
        return ((eval_funcp_8)(registered_functions()[serial].e))(eseq[1-1], eseq[2-1], eseq[3-1], eseq[4-1], eseq[5-1], eseq[6-1], eseq[7-1], eseq[8-1]);
        break;
    case 9:
        return ((eval_funcp_9)(registered_functions()[serial].e))(eseq[1-1], eseq[2-1], eseq[3-1], eseq[4-1], eseq[5-1], eseq[6-1], eseq[7-1], eseq[8-1], eseq[9-1]);
        break;
    case 10:
        return ((eval_funcp_10)(registered_functions()[serial].e))(eseq[1-1], eseq[2-1], eseq[3-1], eseq[4-1], eseq[5-1], eseq[6-1], eseq[7-1], eseq[8-1], eseq[9-1], eseq[10-1]);
        break;

        // end of generated lines
    }
    throw(std::logic_error("function::eval(): invalid nparams"));
}

ex function::evalf(int level) const
{
    ASSERT(serial<registered_functions().size());

    exvector eseq=evalfchildren(level);
    
    if (registered_functions()[serial].ef==0) {
        return function(serial,eseq).hold();
    }
    switch (registered_functions()[serial].nparams) {
        // the following lines have been generated for max. 10 parameters
    case 1:
        return ((evalf_funcp_1)(registered_functions()[serial].ef))(eseq[1-1]);
        break;
    case 2:
        return ((evalf_funcp_2)(registered_functions()[serial].ef))(eseq[1-1], eseq[2-1]);
        break;
    case 3:
        return ((evalf_funcp_3)(registered_functions()[serial].ef))(eseq[1-1], eseq[2-1], eseq[3-1]);
        break;
    case 4:
        return ((evalf_funcp_4)(registered_functions()[serial].ef))(eseq[1-1], eseq[2-1], eseq[3-1], eseq[4-1]);
        break;
    case 5:
        return ((evalf_funcp_5)(registered_functions()[serial].ef))(eseq[1-1], eseq[2-1], eseq[3-1], eseq[4-1], eseq[5-1]);
        break;
    case 6:
        return ((evalf_funcp_6)(registered_functions()[serial].ef))(eseq[1-1], eseq[2-1], eseq[3-1], eseq[4-1], eseq[5-1], eseq[6-1]);
        break;
    case 7:
        return ((evalf_funcp_7)(registered_functions()[serial].ef))(eseq[1-1], eseq[2-1], eseq[3-1], eseq[4-1], eseq[5-1], eseq[6-1], eseq[7-1]);
        break;
    case 8:
        return ((evalf_funcp_8)(registered_functions()[serial].ef))(eseq[1-1], eseq[2-1], eseq[3-1], eseq[4-1], eseq[5-1], eseq[6-1], eseq[7-1], eseq[8-1]);
        break;
    case 9:
        return ((evalf_funcp_9)(registered_functions()[serial].ef))(eseq[1-1], eseq[2-1], eseq[3-1], eseq[4-1], eseq[5-1], eseq[6-1], eseq[7-1], eseq[8-1], eseq[9-1]);
        break;
    case 10:
        return ((evalf_funcp_10)(registered_functions()[serial].ef))(eseq[1-1], eseq[2-1], eseq[3-1], eseq[4-1], eseq[5-1], eseq[6-1], eseq[7-1], eseq[8-1], eseq[9-1], eseq[10-1]);
        break;

        // end of generated lines
    }
    throw(std::logic_error("function::evalf(): invalid nparams"));
}

ex function::thisexprseq(exvector const & v) const
{
    return function(serial,v);
}

ex function::thisexprseq(exvector * vp) const
{
    return function(serial,vp);
}

/** Implementation of ex::series for functions.
 *  @see ex::series */
ex function::series(symbol const & s, ex const & point, int order) const
{
    ASSERT(serial<registered_functions().size());

    if (registered_functions()[serial].s==0) {
        return basic::series(s, point, order);
    }
    switch (registered_functions()[serial].nparams) {
        // the following lines have been generated for max. 10 parameters
    case 1:
        return ((series_funcp_1)(registered_functions()[serial].s))(seq[1-1],s,point,order);
        break;
    case 2:
        return ((series_funcp_2)(registered_functions()[serial].s))(seq[1-1], seq[2-1],s,point,order);
        break;
    case 3:
        return ((series_funcp_3)(registered_functions()[serial].s))(seq[1-1], seq[2-1], seq[3-1],s,point,order);
        break;
    case 4:
        return ((series_funcp_4)(registered_functions()[serial].s))(seq[1-1], seq[2-1], seq[3-1], seq[4-1],s,point,order);
        break;
    case 5:
        return ((series_funcp_5)(registered_functions()[serial].s))(seq[1-1], seq[2-1], seq[3-1], seq[4-1], seq[5-1],s,point,order);
        break;
    case 6:
        return ((series_funcp_6)(registered_functions()[serial].s))(seq[1-1], seq[2-1], seq[3-1], seq[4-1], seq[5-1], seq[6-1],s,point,order);
        break;
    case 7:
        return ((series_funcp_7)(registered_functions()[serial].s))(seq[1-1], seq[2-1], seq[3-1], seq[4-1], seq[5-1], seq[6-1], seq[7-1],s,point,order);
        break;
    case 8:
        return ((series_funcp_8)(registered_functions()[serial].s))(seq[1-1], seq[2-1], seq[3-1], seq[4-1], seq[5-1], seq[6-1], seq[7-1], seq[8-1],s,point,order);
        break;
    case 9:
        return ((series_funcp_9)(registered_functions()[serial].s))(seq[1-1], seq[2-1], seq[3-1], seq[4-1], seq[5-1], seq[6-1], seq[7-1], seq[8-1], seq[9-1],s,point,order);
        break;
    case 10:
        return ((series_funcp_10)(registered_functions()[serial].s))(seq[1-1], seq[2-1], seq[3-1], seq[4-1], seq[5-1], seq[6-1], seq[7-1], seq[8-1], seq[9-1], seq[10-1],s,point,order);
        break;

        // end of generated lines
    }
    throw(std::logic_error("function::series(): invalid nparams"));
}

// protected

int function::compare_same_type(basic const & other) const
{
    ASSERT(is_of_type(other, function));
    function const & o=static_cast<function &>(const_cast<basic &>(other));

    if (serial!=o.serial) {
        return serial < o.serial ? -1 : 1;
    }
    return exprseq::compare_same_type(o);
}

bool function::is_equal_same_type(basic const & other) const
{
    ASSERT(is_of_type(other, function));
    function const & o=static_cast<function &>(const_cast<basic &>(other));

    if (serial!=o.serial) return false;
    return exprseq::is_equal_same_type(o);
}

unsigned function::return_type(void) const
{
    if (seq.size()==0) {
        return return_types::commutative;
    }
    return (*seq.begin()).return_type();
}
   
unsigned function::return_type_tinfo(void) const
{
    if (seq.size()==0) {
        return tinfo_key;
    }
    return (*seq.begin()).return_type_tinfo();
}

//////////
// new virtual functions which can be overridden by derived classes
//////////

// none

//////////
// non-virtual functions in this class
//////////

// protected

ex function::pdiff(unsigned diff_param) const // partial differentiation
{
    ASSERT(serial<registered_functions().size());
    
    if (registered_functions()[serial].d==0) {
        throw(std::logic_error(string("function::pdiff(") + registered_functions()[serial].name + "): no diff function defined"));
    }
    switch (registered_functions()[serial].nparams) {
        // the following lines have been generated for max. 10 parameters
    case 1:
        return ((diff_funcp_1)(registered_functions()[serial].d))(seq[1-1],diff_param);
        break;
    case 2:
        return ((diff_funcp_2)(registered_functions()[serial].d))(seq[1-1], seq[2-1],diff_param);
        break;
    case 3:
        return ((diff_funcp_3)(registered_functions()[serial].d))(seq[1-1], seq[2-1], seq[3-1],diff_param);
        break;
    case 4:
        return ((diff_funcp_4)(registered_functions()[serial].d))(seq[1-1], seq[2-1], seq[3-1], seq[4-1],diff_param);
        break;
    case 5:
        return ((diff_funcp_5)(registered_functions()[serial].d))(seq[1-1], seq[2-1], seq[3-1], seq[4-1], seq[5-1],diff_param);
        break;
    case 6:
        return ((diff_funcp_6)(registered_functions()[serial].d))(seq[1-1], seq[2-1], seq[3-1], seq[4-1], seq[5-1], seq[6-1],diff_param);
        break;
    case 7:
        return ((diff_funcp_7)(registered_functions()[serial].d))(seq[1-1], seq[2-1], seq[3-1], seq[4-1], seq[5-1], seq[6-1], seq[7-1],diff_param);
        break;
    case 8:
        return ((diff_funcp_8)(registered_functions()[serial].d))(seq[1-1], seq[2-1], seq[3-1], seq[4-1], seq[5-1], seq[6-1], seq[7-1], seq[8-1],diff_param);
        break;
    case 9:
        return ((diff_funcp_9)(registered_functions()[serial].d))(seq[1-1], seq[2-1], seq[3-1], seq[4-1], seq[5-1], seq[6-1], seq[7-1], seq[8-1], seq[9-1],diff_param);
        break;
    case 10:
        return ((diff_funcp_10)(registered_functions()[serial].d))(seq[1-1], seq[2-1], seq[3-1], seq[4-1], seq[5-1], seq[6-1], seq[7-1], seq[8-1], seq[9-1], seq[10-1],diff_param);
        break;

        // end of generated lines
    }        
    throw(std::logic_error("function::pdiff(): no diff function defined"));
}

vector<registered_function_info> & function::registered_functions(void)
{
    static vector<registered_function_info> * rf=new vector<registered_function_info>;
    return *rf;
}

// public

// the following lines have been generated for max. 10 parameters
unsigned function::register_new(char const * nm, eval_funcp_1 e,
                                 evalf_funcp_1 ef, diff_funcp_1 d, series_funcp_1 s)
{
    registered_function_info rfi={nm,1,0,eval_funcp(e),
                                  evalf_funcp(ef),diff_funcp(d),series_funcp(s)};
    registered_functions().push_back(rfi);
    return registered_functions().size()-1;
}
unsigned function::register_new(char const * nm, eval_funcp_2 e,
                                 evalf_funcp_2 ef, diff_funcp_2 d, series_funcp_2 s)
{
    registered_function_info rfi={nm,2,0,eval_funcp(e),
                                  evalf_funcp(ef),diff_funcp(d),series_funcp(s)};
    registered_functions().push_back(rfi);
    return registered_functions().size()-1;
}
unsigned function::register_new(char const * nm, eval_funcp_3 e,
                                 evalf_funcp_3 ef, diff_funcp_3 d, series_funcp_3 s)
{
    registered_function_info rfi={nm,3,0,eval_funcp(e),
                                  evalf_funcp(ef),diff_funcp(d),series_funcp(s)};
    registered_functions().push_back(rfi);
    return registered_functions().size()-1;
}
unsigned function::register_new(char const * nm, eval_funcp_4 e,
                                 evalf_funcp_4 ef, diff_funcp_4 d, series_funcp_4 s)
{
    registered_function_info rfi={nm,4,0,eval_funcp(e),
                                  evalf_funcp(ef),diff_funcp(d),series_funcp(s)};
    registered_functions().push_back(rfi);
    return registered_functions().size()-1;
}
unsigned function::register_new(char const * nm, eval_funcp_5 e,
                                 evalf_funcp_5 ef, diff_funcp_5 d, series_funcp_5 s)
{
    registered_function_info rfi={nm,5,0,eval_funcp(e),
                                  evalf_funcp(ef),diff_funcp(d),series_funcp(s)};
    registered_functions().push_back(rfi);
    return registered_functions().size()-1;
}
unsigned function::register_new(char const * nm, eval_funcp_6 e,
                                 evalf_funcp_6 ef, diff_funcp_6 d, series_funcp_6 s)
{
    registered_function_info rfi={nm,6,0,eval_funcp(e),
                                  evalf_funcp(ef),diff_funcp(d),series_funcp(s)};
    registered_functions().push_back(rfi);
    return registered_functions().size()-1;
}
unsigned function::register_new(char const * nm, eval_funcp_7 e,
                                 evalf_funcp_7 ef, diff_funcp_7 d, series_funcp_7 s)
{
    registered_function_info rfi={nm,7,0,eval_funcp(e),
                                  evalf_funcp(ef),diff_funcp(d),series_funcp(s)};
    registered_functions().push_back(rfi);
    return registered_functions().size()-1;
}
unsigned function::register_new(char const * nm, eval_funcp_8 e,
                                 evalf_funcp_8 ef, diff_funcp_8 d, series_funcp_8 s)
{
    registered_function_info rfi={nm,8,0,eval_funcp(e),
                                  evalf_funcp(ef),diff_funcp(d),series_funcp(s)};
    registered_functions().push_back(rfi);
    return registered_functions().size()-1;
}
unsigned function::register_new(char const * nm, eval_funcp_9 e,
                                 evalf_funcp_9 ef, diff_funcp_9 d, series_funcp_9 s)
{
    registered_function_info rfi={nm,9,0,eval_funcp(e),
                                  evalf_funcp(ef),diff_funcp(d),series_funcp(s)};
    registered_functions().push_back(rfi);
    return registered_functions().size()-1;
}
unsigned function::register_new(char const * nm, eval_funcp_10 e,
                                 evalf_funcp_10 ef, diff_funcp_10 d, series_funcp_10 s)
{
    registered_function_info rfi={nm,10,0,eval_funcp(e),
                                  evalf_funcp(ef),diff_funcp(d),series_funcp(s)};
    registered_functions().push_back(rfi);
    return registered_functions().size()-1;
}

// end of generated lines

//////////
// static member variables
//////////

// none

//////////
// global constants
//////////

const function some_function;
type_info const & typeid_function=typeid(some_function);

