/** @file symbol.cpp
 *
 *  Implementation of GiNaC's symbolic objects. */

/*
 *  GiNaC Copyright (C) 1999-2004 Johannes Gutenberg University Mainz, Germany
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

#include "symbol.h"
#include "lst.h"
#include "archive.h"
#include "tostring.h"
#include "utils.h"
#include "inifcns.h"

namespace GiNaC {

GINAC_IMPLEMENT_REGISTERED_CLASS_OPT(symbol, basic,
  print_func<print_context>(&symbol::do_print).
  print_func<print_latex>(&symbol::do_print_latex).
  print_func<print_tree>(&symbol::do_print_tree).
  print_func<print_python_repr>(&symbol::do_print_python_repr))

//////////
// default constructor
//////////

// symbol

symbol::symbol()
 : inherited(TINFO_symbol), asexinfop(new assigned_ex_info), serial(next_serial++), name(autoname_prefix() + ToString(serial)), TeX_name(name), ret_type(return_types::commutative), ret_type_tinfo(TINFO_symbol), domain(domain::complex)
{
	setflag(status_flags::evaluated | status_flags::expanded);
}

// realsymbol

realsymbol::realsymbol()
{
	domain = domain::real;
}

//////////
// other constructors
//////////

// public

// symbol

symbol::symbol(const std::string & initname, unsigned domain)
 : inherited(TINFO_symbol), asexinfop(new assigned_ex_info), serial(next_serial++), name(initname), TeX_name(default_TeX_name()), ret_type(return_types::commutative), ret_type_tinfo(TINFO_symbol), domain(domain)
{
	setflag(status_flags::evaluated | status_flags::expanded);
}

symbol::symbol(const std::string & initname, const std::string & texname, unsigned domain)
 : inherited(TINFO_symbol), asexinfop(new assigned_ex_info), serial(next_serial++), name(initname), TeX_name(texname), ret_type(return_types::commutative), ret_type_tinfo(TINFO_symbol), domain(domain)
{
	setflag(status_flags::evaluated | status_flags::expanded);
}

symbol::symbol(const std::string & initname, unsigned rt, unsigned rtt, unsigned domain)
 : inherited(TINFO_symbol), asexinfop(new assigned_ex_info), serial(next_serial++), name(initname), TeX_name(default_TeX_name()), ret_type(rt), ret_type_tinfo(rtt), domain(domain)
{
	setflag(status_flags::evaluated | status_flags::expanded);
}

symbol::symbol(const std::string & initname, const std::string & texname, unsigned rt, unsigned rtt, unsigned domain)
 : inherited(TINFO_symbol), asexinfop(new assigned_ex_info), serial(next_serial++), name(initname), TeX_name(texname), ret_type(rt), ret_type_tinfo(rtt), domain(domain)
{
	setflag(status_flags::evaluated | status_flags::expanded);
}

// realsymbol
	
realsymbol::realsymbol(const std::string & initname, unsigned domain)
 : symbol(initname, domain) { }

realsymbol::realsymbol(const std::string & initname, const std::string & texname, unsigned domain)
 : symbol(initname, texname, domain) { }

realsymbol::realsymbol(const std::string & initname, unsigned rt, unsigned rtt, unsigned domain)
 : symbol(initname, rt, rtt, domain) { }

realsymbol::realsymbol(const std::string & initname, const std::string & texname, unsigned rt, unsigned rtt, unsigned domain)
 : symbol(initname, texname, rt, rtt, domain) { }

//////////
// archiving
//////////

/** Construct object from archive_node. */
symbol::symbol(const archive_node &n, lst &sym_lst)
 : inherited(n, sym_lst), asexinfop(new assigned_ex_info), serial(next_serial++)
{
	if (!n.find_string("name", name))
		name = autoname_prefix() + ToString(serial);
	if (!n.find_string("TeXname", TeX_name))
		TeX_name = default_TeX_name();
	if (!n.find_unsigned("domain", domain))
		domain = domain::complex;
	if (!n.find_unsigned("return_type", ret_type))
		ret_type = return_types::commutative;
	if (!n.find_unsigned("return_type_tinfo", ret_type_tinfo))
		ret_type_tinfo = TINFO_symbol;
	setflag(status_flags::evaluated | status_flags::expanded);
}

/** Unarchive the object. */
ex symbol::unarchive(const archive_node &n, lst &sym_lst)
{
	ex s = (new symbol(n, sym_lst))->setflag(status_flags::dynallocated);

	// If symbol is in sym_lst, return the existing symbol
	for (lst::const_iterator it = sym_lst.begin(); it != sym_lst.end(); ++it) {
		if (is_a<symbol>(*it) && (ex_to<symbol>(*it).name == ex_to<symbol>(s).name))
			return *it;
	}

	// Otherwise add new symbol to list and return it
	sym_lst.append(s);
	return s;
}

/** Archive the object. */
void symbol::archive(archive_node &n) const
{
	inherited::archive(n);
	n.add_string("name", name);
	if (TeX_name != default_TeX_name())
		n.add_string("TeX_name", TeX_name);
	if (domain != domain::complex)
		n.add_unsigned("domain", domain);
	if (ret_type != return_types::commutative)
		n.add_unsigned("return_type", ret_type);
	if (ret_type_tinfo != TINFO_symbol)
		n.add_unsigned("return_type_tinfo", ret_type_tinfo);
}

//////////
// functions overriding virtual functions from base classes
//////////

// public

void symbol::do_print(const print_context & c, unsigned level) const
{
	c.s << name;
}

void symbol::do_print_latex(const print_latex & c, unsigned level) const
{
	c.s << TeX_name;
}

void symbol::do_print_tree(const print_tree & c, unsigned level) const
{
	c.s << std::string(level, ' ') << name << " (" << class_name() << ")" << " @" << this
	    << ", serial=" << serial
	    << std::hex << ", hash=0x" << hashvalue << ", flags=0x" << flags << std::dec
	    << ", domain=" << domain
	    << std::endl;
}

void symbol::do_print_python_repr(const print_python_repr & c, unsigned level) const
{
	c.s << class_name() << "('" << name;
	if (TeX_name != default_TeX_name())
		c.s << "','" << TeX_name;
	c.s << "')";
}

bool symbol::info(unsigned inf) const
{
	if (inf == info_flags::symbol)
		return true;
	if (inf == info_flags::polynomial ||
	    inf == info_flags::integer_polynomial ||
	    inf == info_flags::cinteger_polynomial ||
	    inf == info_flags::rational_polynomial ||
	    inf == info_flags::crational_polynomial ||
	    inf == info_flags::rational_function)
		return true;
	if (inf == info_flags::real)
		return domain == domain::real;
	else
		return inherited::info(inf);
}

ex symbol::eval(int level) const
{
	if (level == -max_recursion_level)
		throw(std::runtime_error("max recursion level reached"));
	
	if (asexinfop->is_assigned) {
		setflag(status_flags::evaluated);
		if (level==1)
			return (asexinfop->assigned_expression);
		else
			return (asexinfop->assigned_expression).eval(level);
	} else {
		return this->hold();
	}
}

ex symbol::conjugate() const
{
	if (this->domain == domain::complex) {
		return GiNaC::conjugate_function(*this).hold();
	} else {
		return *this;
	}
}

// protected

/** Implementation of ex::diff() for single differentiation of a symbol.
 *  It returns 1 or 0.
 *
 *  @see ex::diff */
ex symbol::derivative(const symbol & s) const
{
	if (compare_same_type(s))
		return _ex0;
	else
		return _ex1;
}

int symbol::compare_same_type(const basic & other) const
{
	GINAC_ASSERT(is_a<symbol>(other));
	const symbol *o = static_cast<const symbol *>(&other);
	if (serial==o->serial) return 0;
	return serial < o->serial ? -1 : 1;
}

bool symbol::is_equal_same_type(const basic & other) const
{
	GINAC_ASSERT(is_a<symbol>(other));
	const symbol *o = static_cast<const symbol *>(&other);
	return serial==o->serial;
}

unsigned symbol::calchash() const
{
	hashvalue = golden_ratio_hash(tinfo() ^ serial);
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

// public

void symbol::assign(const ex & value)
{
	asexinfop->is_assigned = true;
	asexinfop->assigned_expression = value;
	clearflag(status_flags::evaluated | status_flags::expanded);
}

void symbol::unassign()
{
	if (asexinfop->is_assigned) {
		asexinfop->is_assigned = false;
		asexinfop->assigned_expression = _ex0;
	}
	setflag(status_flags::evaluated | status_flags::expanded);
}

// private

/** Symbols not constructed with a string get one assigned using this
 *  prefix and a number. */
std::string & symbol::autoname_prefix()
{
	static std::string *s = new std::string("symbol");
	return *s;
}

/** Return default TeX name for symbol. This recognizes some greek letters. */
std::string symbol::default_TeX_name() const
{
	if (name=="alpha"        || name=="beta"         || name=="gamma"
	 || name=="delta"        || name=="epsilon"      || name=="varepsilon"
	 || name=="zeta"         || name=="eta"          || name=="theta"
	 || name=="vartheta"     || name=="iota"         || name=="kappa"
	 || name=="lambda"       || name=="mu"           || name=="nu"
	 || name=="xi"           || name=="omicron"      || name=="pi"
	 || name=="varpi"        || name=="rho"          || name=="varrho"
	 || name=="sigma"        || name=="varsigma"     || name=="tau"
	 || name=="upsilon"      || name=="phi"          || name=="varphi"
	 || name=="chi"          || name=="psi"          || name=="omega"
	 || name=="Gamma"        || name=="Delta"        || name=="Theta"
	 || name=="Lambda"       || name=="Xi"           || name=="Pi"
	 || name=="Sigma"        || name=="Upsilon"      || name=="Phi"
	 || name=="Psi"          || name=="Omega")
		return "\\" + name;
	else
		return name;
}

//////////
// static member variables
//////////

// private

unsigned symbol::next_serial = 0;

//////////
// subclass assigned_ex_info
//////////

/** Default ctor.  Defaults to unassigned. */
symbol::assigned_ex_info::assigned_ex_info() throw() : is_assigned(false)
{
}

} // namespace GiNaC
