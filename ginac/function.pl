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
	my ($template,$seq_template1,$seq_template2,$seq_template3,$from,$to)=@_;
	my ($res,$N,$SEQ);

	$res='';
	for ($N=$from; $N<=$to; $N++) {
		$SEQ1=generate_seq($seq_template1,$N);
		$SEQ2=generate_seq($seq_template2,$N);
		$SEQ3=generate_seq($seq_template3,$N);
		$res .= eval('"' . $template . '"');
		$SEQ1=''; # to avoid main::SEQ1 used only once warning
		$SEQ2=''; # same as above
		$SEQ3=''; # same as above
	}
	return $res;
}

sub generate {
	my ($template,$seq_template1,$seq_template2,$seq_template3)=@_;
	return generate_from_to($template,$seq_template1,$seq_template2,$seq_template3,1,$maxargs);
}

$declare_function_macro = generate(
	<<'END_OF_DECLARE_FUNCTION_MACRO','typename T${N}','const T${N} & p${N}','GiNaC::ex(p${N})');
#define DECLARE_FUNCTION_${N}P(NAME) \\
extern const unsigned function_index_##NAME; \\
template<${SEQ1}> \\
inline const GiNaC::function NAME(${SEQ2}) { \\
	return GiNaC::function(function_index_##NAME, ${SEQ3}); \\
}

END_OF_DECLARE_FUNCTION_MACRO

$typedef_eval_funcp=generate(
'typedef ex (* eval_funcp_${N})(${SEQ1});'."\n",
'const ex &','','');

$typedef_evalf_funcp=generate(
'typedef ex (* evalf_funcp_${N})(${SEQ1});'."\n",
'const ex &','','');

$typedef_derivative_funcp=generate(
'typedef ex (* derivative_funcp_${N})(${SEQ1}, unsigned);'."\n",
'const ex &','','');

$typedef_series_funcp=generate(
'typedef ex (* series_funcp_${N})(${SEQ1}, const relational &, int, unsigned);'."\n",
'const ex &','','');

$eval_func_interface=generate('    function_options & eval_func(eval_funcp_${N} e);'."\n",'','','');

$evalf_func_interface=generate('    function_options & evalf_func(evalf_funcp_${N} ef);'."\n",'','','');

$derivative_func_interface=generate('    function_options & derivative_func(derivative_funcp_${N} d);'."\n",'','','');

$series_func_interface=generate('    function_options & series_func(series_funcp_${N} s);'."\n",'','','');

$constructors_interface=generate(
'    function(unsigned ser, ${SEQ1});'."\n",
'const ex & param${N}','','');

$constructors_implementation=generate(
	<<'END_OF_CONSTRUCTORS_IMPLEMENTATION','const ex & param${N}','param${N}','');
function::function(unsigned ser, ${SEQ1})
	: exprseq(${SEQ2}), serial(ser)
{
	debugmsg(\"function ctor from unsigned,${N}*ex\",LOGLEVEL_CONSTRUCT);
	tinfo_key = TINFO_function;
}
END_OF_CONSTRUCTORS_IMPLEMENTATION

$eval_switch_statement=generate(
	<<'END_OF_EVAL_SWITCH_STATEMENT','seq[${N}-1]','','');
	case ${N}:
		eval_result = ((eval_funcp_${N})(registered_functions()[serial].eval_f))(${SEQ1});
		break;
END_OF_EVAL_SWITCH_STATEMENT

$evalf_switch_statement=generate(
	<<'END_OF_EVALF_SWITCH_STATEMENT','eseq[${N}-1]','','');
	case ${N}:
		return ((evalf_funcp_${N})(registered_functions()[serial].evalf_f))(${SEQ1});
END_OF_EVALF_SWITCH_STATEMENT

$diff_switch_statement=generate(
	<<'END_OF_DIFF_SWITCH_STATEMENT','seq[${N}-1]','','');
	case ${N}:
		return ((derivative_funcp_${N})(registered_functions()[serial].derivative_f))(${SEQ1},diff_param);
END_OF_DIFF_SWITCH_STATEMENT

$series_switch_statement=generate(
	<<'END_OF_SERIES_SWITCH_STATEMENT','seq[${N}-1]','','');
	case ${N}:
		try {
			res = ((series_funcp_${N})(registered_functions()[serial].series_f))(${SEQ1},r,order,options);
		} catch (do_taylor) {
			res = basic::series(r, order, options);
		}
		return res;
END_OF_SERIES_SWITCH_STATEMENT

$eval_func_implementation=generate(
	<<'END_OF_EVAL_FUNC_IMPLEMENTATION','','','');
function_options & function_options::eval_func(eval_funcp_${N} e)
{
	test_and_set_nparams(${N});
	eval_f = eval_funcp(e);
	return *this;
}
END_OF_EVAL_FUNC_IMPLEMENTATION

$evalf_func_implementation=generate(
	<<'END_OF_EVALF_FUNC_IMPLEMENTATION','','','');
function_options & function_options::evalf_func(evalf_funcp_${N} ef)
{
	test_and_set_nparams(${N});
	evalf_f = evalf_funcp(ef);
	return *this;
}
END_OF_EVALF_FUNC_IMPLEMENTATION

$derivative_func_implementation=generate(
	<<'END_OF_DERIVATIVE_FUNC_IMPLEMENTATION','','','');
function_options & function_options::derivative_func(derivative_funcp_${N} d)
{
	test_and_set_nparams(${N});
	derivative_f = derivative_funcp(d);
	return *this;
}
END_OF_DERIVATIVE_FUNC_IMPLEMENTATION

$series_func_implementation=generate(
	<<'END_OF_SERIES_FUNC_IMPLEMENTATION','','','');
function_options & function_options::series_func(series_funcp_${N} s)
{
	test_and_set_nparams(${N});
	series_f = series_funcp(s);
	return *this;
}
END_OF_SERIES_FUNC_IMPLEMENTATION

$interface=<<END_OF_INTERFACE;
/** \@file function.h
 *
 *  Interface to class of symbolic functions. */

/*
 *  This file was generated automatically by function.pl.
 *  Please do not modify it directly, edit the perl script instead!
 *  function.pl options: \$maxargs=${maxargs}
 *
 *  GiNaC Copyright (C) 1999-2001 Johannes Gutenberg University Mainz, Germany
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

// the following lines have been generated for max. ${maxargs} parameters
$declare_function_macro
// end of generated lines

#define REGISTER_FUNCTION(NAME,OPT) \\
const unsigned function_index_##NAME= \\
	GiNaC::function::register_new(GiNaC::function_options(#NAME).OPT);

// The TYPECHECK-macros were used inside the _evalf() functions.  They are
// considered obsolete now:  (FIXME: remove them)

#define BEGIN_TYPECHECK \\
bool automatic_typecheck=true;

#define TYPECHECK(VAR,TYPE) \\
if (!is_exactly_a<TYPE>(VAR)) { \\
	automatic_typecheck=false; \\
} else

#define TYPECHECK_INTEGER(VAR) \\
if (!(VAR).info(GiNaC::info_flags::integer)) { \\
	automatic_typecheck=false; \\
} else

#define END_TYPECHECK(RV) \\
{} \\
if (!automatic_typecheck) { \\
	return RV.hold(); \\
}

namespace GiNaC {

class function;
class symmetry;

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
	friend class fderivative;
public:
	function_options();
	function_options(std::string const & n, std::string const & tn=std::string());
	~function_options();
	void initialize(void);
	function_options & set_name(std::string const & n, std::string const & tn=std::string());
	function_options & latex_name(std::string const & tn);
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
	function_options & set_symmetry(const symmetry & s);
	void test_and_set_nparams(unsigned n);
	std::string get_name(void) const { return name; }
	unsigned get_nparams(void) const { return nparams; }
	bool has_derivative(void) const { return derivative_f != NULL; }

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

	ex symtree;
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

	// other ctors
public:
	function(unsigned ser);
	// the following lines have been generated for max. ${maxargs} parameters
$constructors_interface
	// end of generated lines
	function(unsigned ser, const exprseq & es);
	function(unsigned ser, const exvector & v, bool discardable = false);
	function(unsigned ser, exvector * vp); // vp will be deleted

	// functions overriding virtual functions from base classes
public:
	void print(const print_context & c, unsigned level = 0) const;
	unsigned precedence(void) const {return 70;}
	int degree(const ex & s) const;
	int ldegree(const ex & s) const;
	ex coeff(const ex & s, int n = 1) const;
	ex expand(unsigned options=0) const;
	ex eval(int level=0) const;
	ex evalf(int level=0) const;
	unsigned calchash(void) const;
	ex series(const relational & r, int order, unsigned options = 0) const;
	ex thisexprseq(const exvector & v) const;
	ex thisexprseq(exvector * vp) const;
protected:
	ex derivative(const symbol & s) const;
	bool is_equal_same_type(const basic & other) const;
	bool match_same_type(const basic & other) const;
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
	unsigned get_serial(void) const {return serial;}
	std::string get_name(void) const;
	
// member variables

protected:
	unsigned serial;
};

// utility functions/macros

/** Specialization of is_exactly_a<function>(obj) for objects of type function. */
template<> inline bool is_exactly_a<function>(const basic & obj)
{
	return obj.tinfo()==TINFO_function;
}

#define is_ex_the_function(OBJ, FUNCNAME) \\
	(is_exactly_a<GiNaC::function>(OBJ) && ex_to<GiNaC::function>(OBJ).get_serial() == function_index_##FUNCNAME)

} // namespace GiNaC

#endif // ndef __GINAC_FUNCTION_H__

END_OF_INTERFACE

$implementation=<<END_OF_IMPLEMENTATION;
/** \@file function.cpp
 *
 *  Implementation of class of symbolic functions. */

/*
 *  This file was generated automatically by function.pl.
 *  Please do not modify it directly, edit the perl script instead!
 *  function.pl options: \$maxargs=${maxargs}
 *
 *  GiNaC Copyright (C) 1999-2001 Johannes Gutenberg University Mainz, Germany
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
#include "fderivative.h"
#include "ex.h"
#include "lst.h"
#include "symmetry.h"
#include "print.h"
#include "archive.h"
#include "inifcns.h"
#include "tostring.h"
#include "utils.h"
#include "debugmsg.h"
#include "remember.h"

namespace GiNaC {

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
	set_name("unnamed_function","\\\\mbox{unnamed}");
	nparams = 0;
	eval_f = evalf_f = derivative_f = series_f = 0;
	evalf_params_first = true;
	use_return_type = false;
	use_remember = false;
	functions_with_same_name = 1;
	symtree = 0;
}

function_options & function_options::set_name(std::string const & n,
                                              std::string const & tn)
{
	name=n;
	if (tn==std::string())
		TeX_name = "\\\\mbox{"+name+"}";
	else
		TeX_name = tn;
	return *this;
}

function_options & function_options::latex_name(std::string const & tn)
{
	TeX_name=tn;
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
	use_return_type = true;
	return_type = rt;
	return_type_tinfo = rtt;
	return *this;
}

function_options & function_options::do_not_evalf_params(void)
{
	evalf_params_first = false;
	return *this;
}

function_options & function_options::remember(unsigned size,
                                              unsigned assoc_size,
                                              unsigned strategy)
{
	use_remember = true;
	remember_size = size;
	remember_assoc_size = assoc_size;
	remember_strategy = strategy;
	return *this;
}

function_options & function_options::overloaded(unsigned o)
{
	functions_with_same_name = o;
	return *this;
}

function_options & function_options::set_symmetry(const symmetry & s)
{
	symtree = s;
	return *this;
}
	
void function_options::test_and_set_nparams(unsigned n)
{
	if (nparams==0) {
		nparams = n;
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
// default ctor, dtor, copy ctor assignment operator and helpers
//////////

// public

function::function() : serial(0)
{
	debugmsg("function default ctor",LOGLEVEL_CONSTRUCT);
	tinfo_key = TINFO_function;
}

// protected

void function::copy(const function & other)
{
	inherited::copy(other);
	serial = other.serial;
}

void function::destroy(bool call_parent)
{
	if (call_parent)
		inherited::destroy(call_parent);
}

//////////
// other ctors
//////////

// public

function::function(unsigned ser) : serial(ser)
{
	debugmsg("function ctor from unsigned",LOGLEVEL_CONSTRUCT);
	tinfo_key = TINFO_function;
}

// the following lines have been generated for max. ${maxargs} parameters
$constructors_implementation
// end of generated lines

function::function(unsigned ser, const exprseq & es) : exprseq(es), serial(ser)
{
	debugmsg("function ctor from unsigned,exprseq",LOGLEVEL_CONSTRUCT);
	tinfo_key = TINFO_function;
}

function::function(unsigned ser, const exvector & v, bool discardable) 
  : exprseq(v,discardable), serial(ser)
{
	debugmsg("function ctor from string,exvector,bool",LOGLEVEL_CONSTRUCT);
	tinfo_key = TINFO_function;
}

function::function(unsigned ser, exvector * vp) 
  : exprseq(vp), serial(ser)
{
	debugmsg("function ctor from unsigned,exvector *",LOGLEVEL_CONSTRUCT);
	tinfo_key = TINFO_function;
}

//////////
// archiving
//////////

/** Construct object from archive_node. */
function::function(const archive_node &n, const lst &sym_lst) : inherited(n, sym_lst)
{
	debugmsg("function ctor from archive_node", LOGLEVEL_CONSTRUCT);

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
			++i; ++ser;
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
// functions overriding virtual functions from base classes
//////////

// public

void function::print(const print_context & c, unsigned level) const
{
	debugmsg("function print", LOGLEVEL_PRINT);

	GINAC_ASSERT(serial<registered_functions().size());

	if (is_of_type(c, print_tree)) {

		c.s << std::string(level, ' ') << class_name() << " "
		    << registered_functions()[serial].name
		    << std::hex << ", hash=0x" << hashvalue << ", flags=0x" << flags << std::dec
		    << ", nops=" << nops()
		    << std::endl;
		unsigned delta_indent = static_cast<const print_tree &>(c).delta_indent;
		for (unsigned i=0; i<seq.size(); ++i)
			seq[i].print(c, level + delta_indent);
		c.s << std::string(level + delta_indent, ' ') << "=====" << std::endl;

	} else if (is_of_type(c, print_csrc)) {

		// Print function name in lowercase
		std::string lname = registered_functions()[serial].name;
		unsigned num = lname.size();
		for (unsigned i=0; i<num; i++)
			lname[i] = tolower(lname[i]);
		c.s << lname << "(";

		// Print arguments, separated by commas
		exvector::const_iterator it = seq.begin(), itend = seq.end();
		while (it != itend) {
			it->print(c);
			++it;
			if (it != itend)
				c.s << ",";
		}
		c.s << ")";

	} else if (is_of_type(c, print_latex)) {
		c.s << registered_functions()[serial].TeX_name;
		printseq(c, '(', ',', ')', exprseq::precedence(), function::precedence());
	} else {
		c.s << registered_functions()[serial].name;
		printseq(c, '(', ',', ')', exprseq::precedence(), function::precedence());
	}
}

ex function::expand(unsigned options) const
{
	// Only expand arguments when asked to do so
	if (options & expand_options::expand_function_args)
		return inherited::expand(options);
	else
		return (options == 0) ? setflag(status_flags::expanded) : *this;
}

int function::degree(const ex & s) const
{
	return is_equal(ex_to<basic>(s)) ? 1 : 0;
}

int function::ldegree(const ex & s) const
{
	return is_equal(ex_to<basic>(s)) ? 1 : 0;
}

ex function::coeff(const ex & s, int n) const
{
	if (is_equal(ex_to<basic>(s)))
		return n==1 ? _ex1() : _ex0();
	else
		return n==0 ? ex(*this) : _ex0();
}

ex function::eval(int level) const
{
	GINAC_ASSERT(serial<registered_functions().size());

	if (level>1) {
		// first evaluate children, then we will end up here again
		return function(serial,evalchildren(level));
	}

	const function_options &opt = registered_functions()[serial];

	// Canonicalize argument order according to the symmetry properties
	if (seq.size() > 1 && !(opt.symtree.is_zero())) {
		exvector v = seq;
		GINAC_ASSERT(is_a<symmetry>(opt.symtree));
		int sig = canonicalize(v.begin(), ex_to<symmetry>(opt.symtree));
		if (sig != INT_MAX) {
			// Something has changed while sorting arguments, more evaluations later
			if (sig == 0)
				return _ex0();
			return ex(sig) * thisexprseq(v);
		}
	}

	if (opt.eval_f==0) {
		return this->hold();
	}

	bool use_remember = opt.use_remember;
	ex eval_result;
	if (use_remember && lookup_remember_table(eval_result)) {
		return eval_result;
	}

	switch (opt.nparams) {
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

	// Evaluate children first
	exvector eseq;
	if (level == 1)
		eseq = seq;
	else if (level == -max_recursion_level)
		throw(std::runtime_error("max recursion level reached"));
	else
		eseq.reserve(seq.size());
	--level;
	exvector::const_iterator it = seq.begin(), itend = seq.end();
	while (it != itend) {
		eseq.push_back(it->evalf(level));
		++it;
	}
	
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

unsigned function::calchash(void) const
{
	unsigned v = golden_ratio_hash(golden_ratio_hash(tinfo()) ^ serial);
	for (unsigned i=0; i<nops(); i++) {
		v = rotate_left_31(v);
		v ^= this->op(i).gethash();
	}
	v &= 0x7FFFFFFFU;
	if (flags & status_flags::evaluated) {
		setflag(status_flags::hash_calculated);
		hashvalue = v;
	}
	return v;
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
ex function::series(const relational & r, int order, unsigned options) const
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
	} else {
		// Chain rule
		ex arg_diff;
		unsigned num = seq.size();
		for (unsigned i=0; i<num; i++) {
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
	const function & o = static_cast<const function &>(other);

	if (serial != o.serial)
		return serial < o.serial ? -1 : 1;
	else
		return exprseq::compare_same_type(o);
}

bool function::is_equal_same_type(const basic & other) const
{
	GINAC_ASSERT(is_of_type(other, function));
	const function & o = static_cast<const function &>(other);

	if (serial != o.serial)
		return false;
	else
		return exprseq::is_equal_same_type(o);
}

bool function::match_same_type(const basic & other) const
{
	GINAC_ASSERT(is_of_type(other, function));
	const function & o = static_cast<const function &>(other);

	return serial == o.serial;
}

unsigned function::return_type(void) const
{
	if (seq.empty())
		return return_types::commutative;
	else
		return seq.begin()->return_type();
}

unsigned function::return_type_tinfo(void) const
{
	if (seq.empty())
		return tinfo_key;
	else
		return seq.begin()->return_type_tinfo();
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
	
	// No derivative defined? Then return abstract derivative object
	if (registered_functions()[serial].derivative_f == NULL)
		return fderivative(serial, diff_param, seq);

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
	unsigned same_name = 0;
	for (unsigned i=0; i<registered_functions().size(); ++i) {
		if (registered_functions()[i].name==opt.name) {
			++same_name;
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
		++i;
		++serial;
	}
	throw (std::runtime_error("no function '" + name + "' with " + ToString(nparams) + " parameters defined"));
}

/** Return the print name of the function. */
std::string function::get_name(void) const
{
	GINAC_ASSERT(serial<registered_functions().size());
	return registered_functions()[serial].name;
}

} // namespace GiNaC

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
