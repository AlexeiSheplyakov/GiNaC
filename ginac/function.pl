$maxargs=13;

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

sub generate_from_to {
    my ($template,$seq_template1,$seq_template2,$from,$to)=@_;
    my ($res,$N,$SEQ);

    $res='';
    for ($N=$from; $N<=$to; $N++) {
        $SEQ1=generate_seq($seq_template1,$N);
        $SEQ2=generate_seq($seq_template2,$N);
        $res .= eval('"' . $template . '"');
        $SEQ1=''; # to avoid main::SEQ1 used only once warning
        $SEQ2=''; # same as above
    }
    return $res;
}

sub generate {
    my ($template,$seq_template1,$seq_template2)=@_;
    return generate_from_to($template,$seq_template1,$seq_template2,1,$maxargs);
}

$declare_function_macro_namespace = <<'END_OF_DECLARE_FUNCTION_1_AND_2P_MACRO_NAMESPACE';
#define DECLARE_FUNCTION_1P(NAME) \
extern const unsigned function_index_##NAME; \
inline GiNaC::function NAME(const GiNaC::ex & p1) { \
    return GiNaC::function(function_index_##NAME, p1); \
}
#define DECLARE_FUNCTION_2P(NAME) \
extern const unsigned function_index_##NAME; \
inline GiNaC::function NAME(const GiNaC::ex & p1, const GiNaC::ex & p2) { \
    return GiNaC::function(function_index_##NAME, p1, p2); \
}

END_OF_DECLARE_FUNCTION_1_AND_2P_MACRO_NAMESPACE

$declare_function_macro_namespace .= generate_from_to(
    <<'END_OF_DECLARE_FUNCTION_MACRO_NAMESPACE','const GiNaC::ex & p${N}','p${N}',3,$maxargs);
#define DECLARE_FUNCTION_${N}P(NAME) \\
extern const unsigned function_index_##NAME; \\
inline GiNaC::function NAME(${SEQ1}) { \\
    return GiNaC::function(function_index_##NAME, ${SEQ2}); \\
}

END_OF_DECLARE_FUNCTION_MACRO_NAMESPACE

$declare_function_macro_no_namespace = <<'END_OF_DECLARE_FUNCTION_1_AND_2P_MACRO_NO_NAMESPACE';
#define DECLARE_FUNCTION_1P(NAME) \
extern const unsigned function_index_##NAME; \
inline function NAME(const ex & p1) { \
    return function(function_index_##NAME, p1); \
}
#define DECLARE_FUNCTION_2P(NAME) \
extern const unsigned function_index_##NAME; \
inline function NAME(const ex & p1, const ex & p2) { \
    return function(function_index_##NAME, p1, p2); \
}

END_OF_DECLARE_FUNCTION_1_AND_2P_MACRO_NO_NAMESPACE

$declare_function_macro_no_namespace .= generate_from_to(
    <<'END_OF_DECLARE_FUNCTION_MACRO_NO_NAMESPACE','const ex & p${N}','p${N}',3,$maxargs);
#define DECLARE_FUNCTION_${N}P(NAME) \\
extern const unsigned function_index_##NAME; \\
inline function NAME(${SEQ1}) { \\
    return function(function_index_##NAME, ${SEQ2}); \\
}

END_OF_DECLARE_FUNCTION_MACRO_NO_NAMESPACE

$typedef_eval_funcp=generate(
'typedef ex (* eval_funcp_${N})(${SEQ1});'."\n",
'const ex &','');

$typedef_evalf_funcp=generate(
'typedef ex (* evalf_funcp_${N})(${SEQ1});'."\n",
'const ex &','');

$typedef_derivative_funcp=generate(
'typedef ex (* derivative_funcp_${N})(${SEQ1}, unsigned);'."\n",
'const ex &','');

$typedef_series_funcp=generate(
'typedef ex (* series_funcp_${N})(${SEQ1}, const relational &, int, unsigned);'."\n",
'const ex &','');

$eval_func_interface=generate('    function_options & eval_func(eval_funcp_${N} e);'."\n",'','');

$evalf_func_interface=generate('    function_options & evalf_func(evalf_funcp_${N} ef);'."\n",'','');

$derivative_func_interface=generate('    function_options & derivative_func(derivative_funcp_${N} d);'."\n",'','');

$series_func_interface=generate('    function_options & series_func(series_funcp_${N} s);'."\n",'','');

$constructors_interface=generate(
'    function(unsigned ser, ${SEQ1});'."\n",
'const ex & param${N}','');

$constructors_implementation=generate(
    <<'END_OF_CONSTRUCTORS_IMPLEMENTATION','const ex & param${N}','param${N}');
function::function(unsigned ser, ${SEQ1})
    : exprseq(${SEQ2}), serial(ser)
{
    debugmsg(\"function constructor from unsigned,${N}*ex\",LOGLEVEL_CONSTRUCT);
    tinfo_key = TINFO_function;
}
END_OF_CONSTRUCTORS_IMPLEMENTATION

$eval_switch_statement=generate(
    <<'END_OF_EVAL_SWITCH_STATEMENT','seq[${N}-1]','');
    case ${N}:
        eval_result=((eval_funcp_${N})(registered_functions()[serial].eval_f))(${SEQ1});
        break;
END_OF_EVAL_SWITCH_STATEMENT

$evalf_switch_statement=generate(
    <<'END_OF_EVALF_SWITCH_STATEMENT','eseq[${N}-1]','');
    case ${N}:
        return ((evalf_funcp_${N})(registered_functions()[serial].evalf_f))(${SEQ1});
        break;
END_OF_EVALF_SWITCH_STATEMENT

$diff_switch_statement=generate(
    <<'END_OF_DIFF_SWITCH_STATEMENT','seq[${N}-1]','');
    case ${N}:
        return ((derivative_funcp_${N})(registered_functions()[serial].derivative_f))(${SEQ1},diff_param);
        break;
END_OF_DIFF_SWITCH_STATEMENT

$series_switch_statement=generate(
    <<'END_OF_SERIES_SWITCH_STATEMENT','seq[${N}-1]','');
    case ${N}:
        try {
            res = ((series_funcp_${N})(registered_functions()[serial].series_f))(${SEQ1},r,order,options);
        } catch (do_taylor) {
            res = basic::series(r, order, options);
        }
        return res;
        break;
END_OF_SERIES_SWITCH_STATEMENT

$eval_func_implementation=generate(
    <<'END_OF_EVAL_FUNC_IMPLEMENTATION','','');
function_options & function_options::eval_func(eval_funcp_${N} e)
{
    test_and_set_nparams(${N});
    eval_f=eval_funcp(e);
    return *this;
}        
END_OF_EVAL_FUNC_IMPLEMENTATION

$evalf_func_implementation=generate(
    <<'END_OF_EVALF_FUNC_IMPLEMENTATION','','');
function_options & function_options::evalf_func(evalf_funcp_${N} ef)
{
    test_and_set_nparams(${N});
    evalf_f=evalf_funcp(ef);
    return *this;
}        
END_OF_EVALF_FUNC_IMPLEMENTATION

$derivative_func_implementation=generate(
    <<'END_OF_DERIVATIVE_FUNC_IMPLEMENTATION','','');
function_options & function_options::derivative_func(derivative_funcp_${N} d)
{
    test_and_set_nparams(${N});
    derivative_f=derivative_funcp(d);
    return *this;
}        
END_OF_DERIVATIVE_FUNC_IMPLEMENTATION

$series_func_implementation=generate(
    <<'END_OF_SERIES_FUNC_IMPLEMENTATION','','');
function_options & function_options::series_func(series_funcp_${N} s)
{
    test_and_set_nparams(${N});
    series_f=series_funcp(s);
    return *this;
}        
END_OF_SERIES_FUNC_IMPLEMENTATION

$interface=<<END_OF_INTERFACE;
/** \@file function.h
 *
 *  Interface to abstract class function (new function concept). */

/*
 *  This file was generated automatically by function.pl.
 *  Please do not modify it directly, edit the perl script instead!
 *  function.pl options: \$maxargs=${maxargs}
 *
 *  GiNaC Copyright (C) 1999-2000 Johannes Gutenberg University Mainz, Germany
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

#ifndef __GINAC_FUNCTION_H__
#define __GINAC_FUNCTION_H__

#include <string>
#include <vector>

// CINT needs <algorithm> to work properly with <vector> 
#include <algorithm>

#include "exprseq.h"

#ifndef NO_NAMESPACE_GINAC

// the following lines have been generated for max. ${maxargs} parameters
$declare_function_macro_namespace
// end of generated lines

#else // ndef NO_NAMESPACE_GINAC

// the following lines have been generated for max. ${maxargs} parameters
$declare_function_macro_no_namespace
// end of generated lines

#endif // ndef NO_NAMESPACE_GINAC

#ifndef NO_NAMESPACE_GINAC

#define REGISTER_FUNCTION(NAME,OPT) \\
const unsigned function_index_##NAME= \\
    GiNaC::function::register_new(GiNaC::function_options(#NAME).OPT);

#define REGISTER_FUNCTION_OLD(NAME,E,EF,D,S) \\
const unsigned function_index_##NAME= \\
    GiNaC::function::register_new(GiNaC::function_options(#NAME). \\
                                  eval_func(E). \\
                                  evalf_func(EF). \\
                                  derivative_func(D). \\
                                  series_func(S));

#else // ndef NO_NAMESPACE_GINAC

#define REGISTER_FUNCTION(NAME,OPT) \\
const unsigned function_index_##NAME= \\
    function::register_new(function_options(#NAME).OPT);

#define REGISTER_FUNCTION_OLD(NAME,E,EF,D,S) \\
const unsigned function_index_##NAME= \\
    function::register_new(function_options(#NAME). \\
                           eval_func(E). \\
                           evalf_func(EF). \\
                           derivative_func(D). \\
                           series_func(S));

#endif // ndef NO_NAMESPACE_GINAC

#define BEGIN_TYPECHECK \\
bool automatic_typecheck=true;

#define TYPECHECK(VAR,TYPE) \\
if (!is_ex_exactly_of_type(VAR,TYPE)) { \\
    automatic_typecheck=false; \\
} else

#ifndef NO_NAMESPACE_GINAC

#define TYPECHECK_INTEGER(VAR) \\
if (!(VAR).info(GiNaC::info_flags::integer)) { \\
    automatic_typecheck=false; \\
} else

#else // ndef NO_NAMESPACE_GINAC

#define TYPECHECK_INTEGER(VAR) \\
if (!(VAR).info(info_flags::integer)) { \\
    automatic_typecheck=false; \\
} else

#endif // ndef NO_NAMESPACE_GINAC

#define END_TYPECHECK(RV) \\
{} \\
if (!automatic_typecheck) { \\
    return RV.hold(); \\
}

#ifndef NO_NAMESPACE_GINAC
namespace GiNaC {
#endif // ndef NO_NAMESPACE_GINAC

class function;

typedef ex (* eval_funcp)();
typedef ex (* evalf_funcp)();
typedef ex (* derivative_funcp)();
typedef ex (* series_funcp)();

// the following lines have been generated for max. ${maxargs} parameters
$typedef_eval_funcp
$typedef_evalf_funcp
$typedef_derivative_funcp
$typedef_series_funcp
// end of generated lines

class function_options
{
    friend class function;
public:
    function_options();
    function_options(std::string const & n, std::string const & tn=std::string());
    ~function_options();
    void initialize(void);
    function_options & set_name(std::string const & n, std::string const & tn=std::string());
// the following lines have been generated for max. ${maxargs} parameters
$eval_func_interface
$evalf_func_interface
$derivative_func_interface
$series_func_interface
// end of generated lines
    function_options & set_return_type(unsigned rt, unsigned rtt=0);
    function_options & do_not_evalf_params(void);
    function_options & remember(unsigned size, unsigned assoc_size=0,
                                unsigned strategy=remember_strategies::delete_never);
    function_options & overloaded(unsigned o);
    void test_and_set_nparams(unsigned n);
    std::string get_name(void) const { return name; }
    unsigned get_nparams(void) const { return nparams; }

protected:
    std::string name;
    std::string TeX_name;

    unsigned nparams;

    eval_funcp eval_f;
    evalf_funcp evalf_f;
    derivative_funcp derivative_f;
    series_funcp series_f;

    bool evalf_params_first;

    bool use_return_type;
    unsigned return_type;
    unsigned return_type_tinfo;

    bool use_remember;
    unsigned remember_size;
    unsigned remember_assoc_size;
    unsigned remember_strategy;

    unsigned functions_with_same_name;
};

/** The class function is used to implement builtin functions like sin, cos...
    and user defined functions */
class function : public exprseq
{
    GINAC_DECLARE_REGISTERED_CLASS(function, exprseq)

    // CINT has a linking problem
#ifndef __MAKECINT__
    friend void ginsh_get_ginac_functions(void);
#endif // def __MAKECINT__

    friend class remember_table_entry;
    // friend class remember_table_list;
    // friend class remember_table;

// member functions

    // default constructor, destructor, copy constructor assignment operator and helpers
public:
    function();
    ~function();
    function(const function & other);
    const function & operator=(const function & other);
protected:
    void copy(const function & other);
    void destroy(bool call_parent);

    // other constructors
public:
    function(unsigned ser);
    // the following lines have been generated for max. ${maxargs} parameters
$constructors_interface
    // end of generated lines
    function(unsigned ser, const exprseq & es);
    function(unsigned ser, const exvector & v, bool discardable=0);
    function(unsigned ser, exvector * vp); // vp will be deleted

    // functions overriding virtual functions from bases classes
public:
    basic * duplicate() const;
    void printraw(std::ostream & os) const; 
    void print(std::ostream & os, unsigned upper_precedence=0) const;
    void printtree(std::ostream & os, unsigned indent) const;
    void printcsrc(std::ostream & os, unsigned type, unsigned upper_precedence=0) const;
    ex expand(unsigned options=0) const;
    ex eval(int level=0) const;
    ex evalf(int level=0) const;
    ex series(const relational & r, int order, unsigned options = 0) const;
    ex thisexprseq(const exvector & v) const;
    ex thisexprseq(exvector * vp) const;
protected:
    ex derivative(const symbol & s) const;
    int compare_same_type(const basic & other) const;
    bool is_equal_same_type(const basic & other) const;
    unsigned return_type(void) const;
    unsigned return_type_tinfo(void) const;
    
    // new virtual functions which can be overridden by derived classes
    // none
    
    // non-virtual functions in this class
protected:
    ex pderivative(unsigned diff_param) const; // partial differentiation
    static std::vector<function_options> & registered_functions(void);
    bool lookup_remember_table(ex & result) const;
    void store_remember_table(ex const & result) const;
public:
    static unsigned register_new(function_options const & opt);
    static unsigned find_function(const std::string &name, unsigned nparams);
    unsigned getserial(void) const {return serial;}
    
// member variables

protected:
    unsigned serial;
};

// utility functions/macros
inline const function &ex_to_function(const ex &e)
{
    return static_cast<const function &>(*e.bp);
}

#ifndef NO_NAMESPACE_GINAC

#define is_ex_the_function(OBJ, FUNCNAME) \\
    (is_ex_exactly_of_type(OBJ, function) && static_cast<GiNaC::function *>(OBJ.bp)->getserial() == function_index_##FUNCNAME)

#else // ndef NO_NAMESPACE_GINAC

#define is_ex_the_function(OBJ, FUNCNAME) \\
    (is_ex_exactly_of_type(OBJ, function) && static_cast<function *>(OBJ.bp)->getserial() == function_index_##FUNCNAME)

#endif // ndef NO_NAMESPACE_GINAC

// global constants

extern const function some_function;
extern const type_info & typeid_function;

#ifndef NO_NAMESPACE_GINAC
} // namespace GiNaC
#endif // ndef NO_NAMESPACE_GINAC

#endif // ndef __GINAC_FUNCTION_H__

END_OF_INTERFACE

$implementation=<<END_OF_IMPLEMENTATION;
/** \@file function.cpp
 *
 *  Implementation of class function. */

/*
 *  This file was generated automatically by function.pl.
 *  Please do not modify it directly, edit the perl script instead!
 *  function.pl options: \$maxargs=${maxargs}
 *
 *  GiNaC Copyright (C) 1999-2000 Johannes Gutenberg University Mainz, Germany
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
#include <list>

#include "function.h"
#include "ex.h"
#include "lst.h"
#include "archive.h"
#include "inifcns.h"
#include "utils.h"
#include "debugmsg.h"
#include "remember.h"

#ifndef NO_NAMESPACE_GINAC
namespace GiNaC {
#endif // ndef NO_NAMESPACE_GINAC

//////////
// helper class function_options
//////////

function_options::function_options()
{
    initialize();
}

function_options::function_options(std::string const & n, std::string const & tn)
{
    initialize();
    set_name(n,tn);
}

function_options::~function_options()
{
    // nothing to clean up at the moment
}

void function_options::initialize(void)
{
    set_name("unnamed_function","\\\\operatorname{unnamed}");
    nparams=0;
    eval_f=evalf_f=derivative_f=series_f=0;
    evalf_params_first=true;
    use_return_type=false;
    use_remember=false;
    functions_with_same_name=1;
}

function_options & function_options::set_name(std::string const & n,
                                              std::string const & tn)
{
    name=n;
    if (tn==std::string()) {
        TeX_name="\\\\operatorname{"+name+"}";
    } else {
        TeX_name=tn;
    }
    return *this;
}

// the following lines have been generated for max. ${maxargs} parameters
$eval_func_implementation
$evalf_func_implementation
$derivative_func_implementation
$series_func_implementation
// end of generated lines

function_options & function_options::set_return_type(unsigned rt, unsigned rtt)
{
    use_return_type=true;
    return_type=rt;
    return_type_tinfo=rtt;
    return *this;
}

function_options & function_options::do_not_evalf_params(void)
{
    evalf_params_first=false;
    return *this;
}

function_options & function_options::remember(unsigned size,
                                              unsigned assoc_size,
                                              unsigned strategy)
{
    use_remember=true;
    remember_size=size;
    remember_assoc_size=assoc_size;
    remember_strategy=strategy;
    return *this;
}

function_options & function_options::overloaded(unsigned o)
{
    functions_with_same_name=o;
    return *this;
}
    
void function_options::test_and_set_nparams(unsigned n)
{
    if (nparams==0) {
        nparams=n;
    } else if (nparams!=n) {
        // we do not throw an exception here because this code is
        // usually executed before main(), so the exception could not
        // caught anyhow
        std::cerr << "WARNING: number of parameters ("
                  << n << ") differs from number set before (" 
                  << nparams << ")" << std::endl;
    }
}

GINAC_IMPLEMENT_REGISTERED_CLASS(function, exprseq)

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

function::function(const function & other)
{
    debugmsg("function copy constructor",LOGLEVEL_CONSTRUCT);
    copy(other);
}

const function & function::operator=(const function & other)
{
    debugmsg("function operator=",LOGLEVEL_ASSIGNMENT);
    if (this != &other) {
        destroy(1);
        copy(other);
    }
    return *this;
}

// protected

void function::copy(const function & other)
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

// the following lines have been generated for max. ${maxargs} parameters
$constructors_implementation
// end of generated lines

function::function(unsigned ser, const exprseq & es) : exprseq(es), serial(ser)
{
    debugmsg("function constructor from unsigned,exprseq",LOGLEVEL_CONSTRUCT);
    tinfo_key = TINFO_function;
}

function::function(unsigned ser, const exvector & v, bool discardable) 
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
// archiving
//////////

/** Construct object from archive_node. */
function::function(const archive_node &n, const lst &sym_lst) : inherited(n, sym_lst)
{
    debugmsg("function constructor from archive_node", LOGLEVEL_CONSTRUCT);

    // Find serial number by function name
    std::string s;
    if (n.find_string("name", s)) {
        unsigned int ser = 0;
        std::vector<function_options>::const_iterator i = registered_functions().begin(), iend = registered_functions().end();
        while (i != iend) {
            if (s == i->name) {
                serial = ser;
                return;
            }
            i++; ser++;
        }
        throw (std::runtime_error("unknown function '" + s + "' in archive"));
    } else
        throw (std::runtime_error("unnamed function in archive"));
}

/** Unarchive the object. */
ex function::unarchive(const archive_node &n, const lst &sym_lst)
{
    return (new function(n, sym_lst))->setflag(status_flags::dynallocated);
}

/** Archive the object. */
void function::archive(archive_node &n) const
{
    inherited::archive(n);
    GINAC_ASSERT(serial < registered_functions().size());
    n.add_string("name", registered_functions()[serial].name);
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

void function::printraw(std::ostream & os) const
{
    debugmsg("function printraw",LOGLEVEL_PRINT);

    GINAC_ASSERT(serial<registered_functions().size());

    os << "function(name=" << registered_functions()[serial].name;
    for (exvector::const_iterator it=seq.begin(); it!=seq.end(); ++it) {
        os << ",";
        (*it).bp->print(os);
    }
    os << ")";
}

void function::print(std::ostream & os, unsigned upper_precedence) const
{
    debugmsg("function print",LOGLEVEL_PRINT);

    GINAC_ASSERT(serial<registered_functions().size());

    os << registered_functions()[serial].name;
    printseq(os,'(',',',')',exprseq::precedence,function::precedence);
}

void function::printtree(std::ostream & os, unsigned indent) const
{
    debugmsg("function printtree",LOGLEVEL_PRINT);

    GINAC_ASSERT(serial<registered_functions().size());

    os << std::string(indent,' ') << "function "
       << registered_functions()[serial].name
       << ", hash=" << hashvalue 
       << " (0x" << std::hex << hashvalue << std::dec << ")"
       << ", flags=" << flags
       << ", nops=" << nops() << std::endl;
    for (unsigned i=0; i<nops(); ++i) {
        seq[i].printtree(os,indent+delta_indent);
    }
    os << std::string(indent+delta_indent,' ') << "=====" << std::endl;
}

void function::printcsrc(std::ostream & os, unsigned type, unsigned upper_precedence) const
{
    debugmsg("function print csrc",LOGLEVEL_PRINT);

    GINAC_ASSERT(serial<registered_functions().size());

	// Print function name in lowercase
    std::string lname;
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
    GINAC_ASSERT(serial<registered_functions().size());

    if (level>1) {
        // first evaluate children, then we will end up here again
        return function(serial,evalchildren(level));
    }

    if (registered_functions()[serial].eval_f==0) {
        return this->hold();
    }

    bool use_remember=registered_functions()[serial].use_remember;
    ex eval_result;
    if (use_remember && lookup_remember_table(eval_result)) {
        return eval_result;
    }

    switch (registered_functions()[serial].nparams) {
        // the following lines have been generated for max. ${maxargs} parameters
${eval_switch_statement}
        // end of generated lines
    default:
        throw(std::logic_error("function::eval(): invalid nparams"));
    }
    if (use_remember) {
        store_remember_table(eval_result);
    }
    return eval_result;
}

ex function::evalf(int level) const
{
    GINAC_ASSERT(serial<registered_functions().size());

    exvector eseq=evalfchildren(level);
    
    if (registered_functions()[serial].evalf_f==0) {
        return function(serial,eseq).hold();
    }
    switch (registered_functions()[serial].nparams) {
        // the following lines have been generated for max. ${maxargs} parameters
${evalf_switch_statement}
        // end of generated lines
    }
    throw(std::logic_error("function::evalf(): invalid nparams"));
}

ex function::thisexprseq(const exvector & v) const
{
    return function(serial,v);
}

ex function::thisexprseq(exvector * vp) const
{
    return function(serial,vp);
}

/** Implementation of ex::series for functions.
 *  \@see ex::series */
ex function::series(const relational & r, int order, unsigned options = 0) const
{
    GINAC_ASSERT(serial<registered_functions().size());

    if (registered_functions()[serial].series_f==0) {
        return basic::series(r, order);
    }
    ex res;
    switch (registered_functions()[serial].nparams) {
        // the following lines have been generated for max. ${maxargs} parameters
${series_switch_statement}
        // end of generated lines
    }
    throw(std::logic_error("function::series(): invalid nparams"));
}

// protected


/** Implementation of ex::diff() for functions. It applies the chain rule,
 *  except for the Order term function.
 *  \@see ex::diff */
ex function::derivative(const symbol & s) const
{
    ex result;
    
    if (serial == function_index_Order) {
        // Order Term function only differentiates the argument
        return Order(seq[0].diff(s));
    } else if (serial == function_index_Derivative) {
        // Inert derivative performs chain rule on the first argument only, and
        // adds differentiation parameter to list (second argument)
        GINAC_ASSERT(is_ex_exactly_of_type(seq[0], function));
        GINAC_ASSERT(is_ex_exactly_of_type(seq[1], function));
        ex fcn = seq[0];
        ex arg_diff;
        for (unsigned i=0; i!=fcn.nops(); i++) {
            arg_diff = fcn.op(i).diff(s);
            if (!arg_diff.is_zero()) {
                lst new_lst = ex_to_lst(seq[1]);
                new_lst.append(i);
                result += arg_diff * Derivative(fcn, new_lst);
            }
        }
    } else {
        // Chain rule
        ex arg_diff;
        for (unsigned i=0; i!=seq.size(); i++) {
            arg_diff = seq[i].diff(s);
            // We apply the chain rule only when it makes sense.  This is not
            // just for performance reasons but also to allow functions to
            // throw when differentiated with respect to one of its arguments
            // without running into trouble with our automatic full
            // differentiation:
            if (!arg_diff.is_zero())
                result += pderivative(i)*arg_diff;
        }
    }
    return result;
}

int function::compare_same_type(const basic & other) const
{
    GINAC_ASSERT(is_of_type(other, function));
    const function & o=static_cast<function &>(const_cast<basic &>(other));

    if (serial!=o.serial) {
        return serial < o.serial ? -1 : 1;
    }
    return exprseq::compare_same_type(o);
}

bool function::is_equal_same_type(const basic & other) const
{
    GINAC_ASSERT(is_of_type(other, function));
    const function & o=static_cast<function &>(const_cast<basic &>(other));

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

ex function::pderivative(unsigned diff_param) const // partial differentiation
{
    GINAC_ASSERT(serial<registered_functions().size());
    
    if (registered_functions()[serial].derivative_f==0) {
        return Derivative(*this, lst(diff_param));
    }
    switch (registered_functions()[serial].nparams) {
        // the following lines have been generated for max. ${maxargs} parameters
${diff_switch_statement}
        // end of generated lines
    }        
    throw(std::logic_error("function::pderivative(): no diff function defined"));
}

std::vector<function_options> & function::registered_functions(void)
{
    static std::vector<function_options> * rf = new std::vector<function_options>;
    return *rf;
}

bool function::lookup_remember_table(ex & result) const
{
    return remember_table::remember_tables()[serial].lookup_entry(*this,result);
}

void function::store_remember_table(ex const & result) const
{
    remember_table::remember_tables()[serial].add_entry(*this,result);
}

// public

unsigned function::register_new(function_options const & opt)
{
    unsigned same_name=0;
    for (unsigned i=0; i<registered_functions().size(); ++i) {
        if (registered_functions()[i].name==opt.name) {
            same_name++;
        }
    }
    if (same_name>=opt.functions_with_same_name) {
        // we do not throw an exception here because this code is
        // usually executed before main(), so the exception could not
        // caught anyhow
        std::cerr << "WARNING: function name " << opt.name
                  << " already in use!" << std::endl;
    }
    registered_functions().push_back(opt);
    if (opt.use_remember) {
        remember_table::remember_tables().
            push_back(remember_table(opt.remember_size,
                                     opt.remember_assoc_size,
                                     opt.remember_strategy));
    } else {
        remember_table::remember_tables().push_back(remember_table());
    }
    return registered_functions().size()-1;
}

/** Find serial number of function by name and number of parameters.
 *  Throws exception if function was not found. */
unsigned function::find_function(const std::string &name, unsigned nparams)
{
    std::vector<function_options>::const_iterator i = function::registered_functions().begin(), end = function::registered_functions().end();
    unsigned serial = 0;
    while (i != end) {
        if (i->get_name() == name && i->get_nparams() == nparams)
            return serial;
        i++;
        serial++;
    }
    throw (std::runtime_error("no function '" + name + "' with " + ToString(nparams) + " parameters defined"));
}

//////////
// static member variables
//////////

// none

//////////
// global constants
//////////

const function some_function;
const type_info & typeid_function=typeid(some_function);

#ifndef NO_NAMESPACE_GINAC
} // namespace GiNaC
#endif // ndef NO_NAMESPACE_GINAC

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
