#!/usr/bin/perl -w

$maxargs=10;

sub generate_seq {
    my ($seq_template,$n)=@_;
    my ($res,$N);
    
    $res='';
    for ($N=1; $N<=$n; $N++) {
        $res .= eval('"' . $seq_template . '"');
        if ($N!=$n) {
            $res .= ', ';
        }
    }
    return $res;
}

sub generate {
    my ($template,$seq_template1,$seq_template2)=@_;
    my ($res,$N,$SEQ);

    $res='';
    for ($N=1; $N<=$maxargs; $N++) {
        $SEQ1=generate_seq($seq_template1,$N);
        $SEQ2=generate_seq($seq_template2,$N);
        $res .= eval('"' . $template . '"');
        $SEQ1=''; # to avoid main::SEQ1 used only once warning
        $SEQ2=''; # same as above
    }
    return $res;
}

$declare_function_macro=generate(
    <<'END_OF_DECLARE_FUNCTION_MACRO','ex const & p${N}','p${N}');
#define DECLARE_FUNCTION_${N}P(NAME) \\
extern unsigned function_index_##NAME; \\
inline function NAME(${SEQ1}) { \\
    return function(function_index_##NAME, ${SEQ2}); \\
}

END_OF_DECLARE_FUNCTION_MACRO

$typedef_eval_funcp=generate(
'typedef ex (* eval_funcp_${N})(${SEQ1});'."\n",
'ex const &','');

$typedef_evalf_funcp=generate(
'typedef ex (* evalf_funcp_${N})(${SEQ1});'."\n",
'ex const &','');

$typedef_diff_funcp=generate(
'typedef ex (* diff_funcp_${N})(${SEQ1}, unsigned);'."\n",
'ex const &','');

$typedef_series_funcp=generate(
'typedef ex (* series_funcp_${N})(${SEQ1}, symbol const &, ex const &, int);'."\n",
'ex const &','');

$constructors_interface=generate(
'    function(unsigned ser, ${SEQ1});'."\n",
'ex const & param${N}','');

$register_new_interface=generate(
'    static unsigned register_new(char const * nm, eval_funcp_${N} e,'."\n".
'                                 evalf_funcp_${N} ef=0, diff_funcp_${N} d=0, series_funcp_${N} s=0);'.
"\n",'','');

$constructors_implementation=generate(
    <<'END_OF_CONSTRUCTORS_IMPLEMENTATION','ex const & param${N}','param${N}');
function::function(unsigned ser, ${SEQ1})
    : exprseq(${SEQ2}), serial(ser)
{
    debugmsg(\"function constructor from unsigned,${N}*ex\",LOGLEVEL_CONSTRUCT);
    tinfo_key = TINFO_FUNCTION;
}
END_OF_CONSTRUCTORS_IMPLEMENTATION

$eval_switch_statement=generate(
    <<'END_OF_EVAL_SWITCH_STATEMENT','eseq[${N}-1]','');
    case ${N}:
        return ((eval_funcp_${N})(registered_functions()[serial].e))(${SEQ1});
        break;
END_OF_EVAL_SWITCH_STATEMENT

$evalf_switch_statement=generate(
    <<'END_OF_EVALF_SWITCH_STATEMENT','eseq[${N}-1]','');
    case ${N}:
        return ((evalf_funcp_${N})(registered_functions()[serial].ef))(${SEQ1});
        break;
END_OF_EVALF_SWITCH_STATEMENT

$diff_switch_statement=generate(
    <<'END_OF_DIFF_SWITCH_STATEMENT','seq[${N}-1]','');
    case ${N}:
        return ((diff_funcp_${N})(registered_functions()[serial].d))(${SEQ1},diff_param);
        break;
END_OF_DIFF_SWITCH_STATEMENT

$series_switch_statement=generate(
    <<'END_OF_SERIES_SWITCH_STATEMENT','seq[${N}-1]','');
    case ${N}:
        return ((series_funcp_${N})(registered_functions()[serial].s))(${SEQ1},s,point,order);
        break;
END_OF_SERIES_SWITCH_STATEMENT

$register_new_implementation=generate(
    <<'END_OF_REGISTER_NEW_IMPLEMENTATION','','');
unsigned function::register_new(char const * nm, eval_funcp_${N} e,
                                 evalf_funcp_${N} ef, diff_funcp_${N} d, series_funcp_${N} s)
{
    registered_function_info rfi={nm,${N},0,eval_funcp(e),
                                  evalf_funcp(ef),diff_funcp(d),series_funcp(s)};
    registered_functions().push_back(rfi);
    return registered_functions().size()-1;
}
END_OF_REGISTER_NEW_IMPLEMENTATION

$interface=<<END_OF_INTERFACE;
/** \@file function.h
 *
 *  Interface to abstract class function (new function concept).
 *
 *  This file was generated automatically by function.pl.
 *  Please do not modify it directly, edit the perl script instead!
 *  function.pl options: \$maxargs=${maxargs} */

#ifndef _FUNCTION_H_
#define _FUNCTION_H_

#include <string>
#include <vector>
#include "config.h"

class function;

#include "exprseq.h"

// the following lines have been generated for max. ${maxargs} parameters
$declare_function_macro
// end of generated lines

#define REGISTER_FUNCTION(NAME,E,EF,D,S) \\
unsigned function_index_##NAME=function::register_new(#NAME,E,EF,D,S);

#define BEGIN_TYPECHECK \\
bool automatic_typecheck=true;

#define TYPECHECK(VAR,TYPE) \\
if (!is_ex_exactly_of_type(VAR,TYPE)) { \\
    automatic_typecheck=false; \\
} else

#define TYPECHECK_INTEGER(VAR) \\
if (!(VAR).info(info_flags::integer)) { \\
    automatic_typecheck=false; \\
} else

#define END_TYPECHECK(RV) \\
{} \\
if (!automatic_typecheck) { \\
    return RV.hold(); \\
}

typedef ex (* eval_funcp)();
typedef ex (* evalf_funcp)();
typedef ex (* diff_funcp)();
typedef ex (* series_funcp)();

// the following lines have been generated for max. ${maxargs} parameters
$typedef_eval_funcp
$typedef_evalf_funcp
$typedef_diff_funcp
$typedef_series_funcp
// end of generated lines

struct registered_function_info {
    char const * name;
    unsigned nparams;
    unsigned options;
    eval_funcp e;
    evalf_funcp ef;
    diff_funcp d;
    series_funcp s;
};

/** The class function is used to implement builtin functions like sin, cos...
    and user defined functions */
class function : public exprseq
{
    friend void ginsh_get_ginac_functions(void);

// member functions

    // default constructor, destructor, copy constructor assignment operator and helpers
public:
    function();
    ~function();
    function(function const & other);
    function const & operator=(function const & other);
protected:
    void copy(function const & other);
    void destroy(bool call_parent);

    // other constructors
public:
    function(unsigned ser);
    // the following lines have been generated for max. ${maxargs} parameters
$constructors_interface
    // end of generated lines
    function(unsigned ser, exprseq const & es);
    function(unsigned ser, exvector const & v, bool discardable=0);
    function(unsigned ser, exvector * vp); // vp will be deleted

    // functions overriding virtual functions from bases classes
public:
    basic * duplicate() const;
    void printraw(ostream & os) const; 
    void print(ostream & os, unsigned upper_precedence=0) const;
    void printtree(ostream & os, unsigned indent) const;
    void printcsrc(ostream & os, unsigned type, unsigned upper_precedence=0) const;
    ex expand(unsigned options=0) const;
    ex eval(int level=0) const;
    ex evalf(int level=0) const;
    ex diff(symbol const & s) const;
    ex series(symbol const & s, ex const & point, int order) const;
    ex thisexprseq(exvector const & v) const;
    ex thisexprseq(exvector * vp) const;
protected:
    int compare_same_type(basic const & other) const;
    bool is_equal_same_type(basic const & other) const;
    unsigned return_type(void) const;
    unsigned return_type_tinfo(void) const;
    
    // new virtual functions which can be overridden by derived classes
    // none
    
    // non-virtual functions in this class
protected:
    ex pdiff(unsigned diff_param) const; // partial differentiation
    static vector<registered_function_info> & registered_functions(void);
public:
    // the following lines have been generated for max. ${maxargs} parameters
$register_new_interface
    // end of generated lines
    unsigned getserial(void) const {return serial;}
    
// member variables

protected:
    unsigned serial;
};

// utility macros

#define is_ex_the_function(OBJ, FUNCNAME) \\
    (is_ex_exactly_of_type(OBJ, function) && static_cast<function *>(OBJ.bp)->getserial() == function_index_##FUNCNAME)

// global constants

extern const function some_function;
extern type_info const & typeid_function;

#endif // ndef _FUNCTION_H_

END_OF_INTERFACE

$implementation=<<END_OF_IMPLEMENTATION;
/** \@file function.cpp
 *
 *  Implementation of class function.
 *
 *  This file was generated automatically by function.pl.
 *  Please do not modify it directly, edit the perl script instead!
 *  function.pl options: \$maxargs=${maxargs} */

#include <string>
#include <stdexcept>

#include "ginac.h"

//////////
// default constructor, destructor, copy constructor assignment operator and helpers
//////////

// public

function::function() : serial(0)
{
    debugmsg("function default constructor",LOGLEVEL_CONSTRUCT);
    tinfo_key = TINFO_FUNCTION;
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
    tinfo_key = TINFO_FUNCTION;
}

// the following lines have been generated for max. ${maxargs} parameters
$constructors_implementation
// end of generated lines

function::function(unsigned ser, exprseq const & es) : exprseq(es), serial(ser)
{
    debugmsg("function constructor from unsigned,exprseq",LOGLEVEL_CONSTRUCT);
    tinfo_key = TINFO_FUNCTION;
}

function::function(unsigned ser, exvector const & v, bool discardable) 
    : exprseq(v,discardable), serial(ser)
{
    debugmsg("function constructor from string,exvector,bool",LOGLEVEL_CONSTRUCT);
    tinfo_key = TINFO_FUNCTION;
}

function::function(unsigned ser, exvector * vp) 
    : exprseq(vp), serial(ser)
{
    debugmsg("function constructor from unsigned,exvector *",LOGLEVEL_CONSTRUCT);
    tinfo_key = TINFO_FUNCTION;
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
        // the following lines have been generated for max. ${maxargs} parameters
${eval_switch_statement}
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
        // the following lines have been generated for max. ${maxargs} parameters
${evalf_switch_statement}
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
 *  \@see ex::series */
ex function::series(symbol const & s, ex const & point, int order) const
{
    ASSERT(serial<registered_functions().size());

    if (registered_functions()[serial].s==0) {
        return basic::series(s, point, order);
    }
    switch (registered_functions()[serial].nparams) {
        // the following lines have been generated for max. ${maxargs} parameters
${series_switch_statement}
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
        // the following lines have been generated for max. ${maxargs} parameters
${diff_switch_statement}
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

// the following lines have been generated for max. ${maxargs} parameters
$register_new_implementation
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

END_OF_IMPLEMENTATION

print "Creating interface file function.h...";
open OUT,">function.h" or die "cannot open function.h";
print OUT $interface;
close OUT;
print "ok.\n";

print "Creating implementation file function.cpp...";
open OUT,">function.cpp" or die "cannot open function.cpp";
print OUT $implementation;
close OUT;
print "ok.\n";

print "done.\n";
