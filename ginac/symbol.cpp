/** @file symbol.cpp
 *
 *  Implementation of GiNaC's symbolic objects. */

/*
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

#include "symbol.h"
#include "lst.h"
#include "print.h"
#include "archive.h"
#include "debugmsg.h"
#include "utils.h"

namespace GiNaC {

GINAC_IMPLEMENT_REGISTERED_CLASS_NO_CTORS(symbol, basic)

//////////
// default ctor, dtor, copy ctor assignment operator and helpers
//////////

symbol::symbol() : inherited(TINFO_symbol), serial(next_serial++)
{
	debugmsg("symbol default ctor", LOGLEVEL_CONSTRUCT);
	name = TeX_name = autoname_prefix()+ToString(serial);
	asexinfop = new assigned_ex_info;
	setflag(status_flags::evaluated | status_flags::expanded);
}

/** For use by copy ctor and assignment operator. */
void symbol::copy(const symbol & other)
{
	inherited::copy(other);
	name = other.name;
	TeX_name = other.TeX_name;
	serial = other.serial;
	asexinfop = other.asexinfop;
	++asexinfop->refcount;
}

void symbol::destroy(bool call_parent)
{
	if (--asexinfop->refcount == 0)
		delete asexinfop;
	if (call_parent)
		inherited::destroy(call_parent);
}

//////////
// other ctors
//////////

// public

symbol::symbol(const symbol & other)
{
	debugmsg("symbol copy ctor", LOGLEVEL_CONSTRUCT);
	copy(other);
}

symbol::symbol(const std::string & initname) : inherited(TINFO_symbol)
{
	debugmsg("symbol ctor from string", LOGLEVEL_CONSTRUCT);
	name = initname;
	TeX_name = default_TeX_name();
	serial = next_serial++;
	asexinfop = new assigned_ex_info;
	setflag(status_flags::evaluated | status_flags::expanded);
}

symbol::symbol(const std::string & initname, const std::string & texname) : inherited(TINFO_symbol)
{
	debugmsg("symbol ctor from string", LOGLEVEL_CONSTRUCT);
	name = initname;
	TeX_name = texname;
	serial = next_serial++;
	asexinfop = new assigned_ex_info;
	setflag(status_flags::evaluated | status_flags::expanded);
}

//////////
// archiving
//////////

/** Construct object from archive_node. */
symbol::symbol(const archive_node &n, const lst &sym_lst) : inherited(n, sym_lst)
{
	debugmsg("symbol ctor from archive_node", LOGLEVEL_CONSTRUCT);
	serial = next_serial++;
	if (!(n.find_string("name", name)))
		name = autoname_prefix() + ToString(serial);
	if (!(n.find_string("TeXname", TeX_name)))
		TeX_name = default_TeX_name();
	asexinfop = new assigned_ex_info;
	setflag(status_flags::evaluated);
}

/** Unarchive the object. */
ex symbol::unarchive(const archive_node &n, const lst &sym_lst)
{
	ex s = (new symbol(n, sym_lst))->setflag(status_flags::dynallocated);
	
	// If symbol is in sym_lst, return the existing symbol
	for (unsigned i=0; i<sym_lst.nops(); i++) {
		if (is_ex_of_type(sym_lst.op(i), symbol) && (ex_to_symbol(sym_lst.op(i)).name == ex_to_symbol(s).name))
			return sym_lst.op(i);
	}
	return s;
}

/** Archive the object. */
void symbol::archive(archive_node &n) const
{
	inherited::archive(n);
	n.add_string("name", name);
	if (TeX_name != default_TeX_name())
		n.add_string("TeX_name", TeX_name);
}

//////////
// functions overriding virtual functions from bases classes
//////////

// public

basic *symbol::duplicate() const
{
	debugmsg("symbol duplicate", LOGLEVEL_DUPLICATE);
	return new symbol(*this);
}

void symbol::print(const print_context & c, unsigned level) const
{
	debugmsg("symbol print", LOGLEVEL_PRINT);

	if (is_of_type(c, print_tree)) {

		c.s << std::string(level, ' ') << name << " (" << class_name() << ")"
		    << ", serial=" << serial
		    << std::hex << ", hash=0x" << hashvalue << ", flags=0x" << flags << std::dec
		    << std::endl;

	} else if (is_of_type(c, print_latex))
		c.s << TeX_name;
	else
		c.s << name;
}

bool symbol::info(unsigned inf) const
{
	if (inf==info_flags::symbol) return true;
	if (inf==info_flags::polynomial ||
	    inf==info_flags::integer_polynomial ||
	    inf==info_flags::cinteger_polynomial ||
	    inf==info_flags::rational_polynomial ||
	    inf==info_flags::crational_polynomial ||
	    inf==info_flags::rational_function)
		return true;
	else
		return inherited::info(inf);
}

ex symbol::expand(unsigned options) const
{
	return this->hold();
}

bool symbol::has(const ex & other) const
{
	if (this->is_equal(*other.bp))
		return true;
	else
		return false;
}

int symbol::degree(const ex & s) const
{
	return is_equal(*s.bp) ? 1 : 0;
}

int symbol::ldegree(const ex & s) const
{
	return is_equal(*s.bp) ? 1 : 0;
}

ex symbol::coeff(const ex & s, int n) const
{
	if (is_equal(*s.bp))
		return n==1 ? _ex1() : _ex0();
	else
		return n==0 ? *this : _ex0();
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

// protected

/** Implementation of ex::diff() for single differentiation of a symbol.
 *  It returns 1 or 0.
 *
 *  @see ex::diff */
ex symbol::derivative(const symbol & s) const
{
	if (compare_same_type(s))
		return _ex0();
	else
		return _ex1();
}

int symbol::compare_same_type(const basic & other) const
{
	GINAC_ASSERT(is_of_type(other,symbol));
	const symbol *o = static_cast<const symbol *>(&other);
	if (serial==o->serial) return 0;
	return serial < o->serial ? -1 : 1;
}

bool symbol::is_equal_same_type(const basic & other) const
{
	GINAC_ASSERT(is_of_type(other,symbol));
	const symbol *o = static_cast<const symbol *>(&other);
	return serial==o->serial;
}

unsigned symbol::return_type(void) const
{
	return return_types::commutative;
}
   
unsigned symbol::return_type_tinfo(void) const
{
	return tinfo_key;
}

unsigned symbol::calchash(void) const
{
	// this is where the schoolbook method
	// (golden_ratio_hash(tinfo()) ^ serial)
	// is not good enough yet...
	hashvalue = golden_ratio_hash(golden_ratio_hash(tinfo()) ^ serial);
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
	asexinfop->is_assigned = 1;
	asexinfop->assigned_expression = value;
	clearflag(status_flags::evaluated | status_flags::expanded);
}

void symbol::unassign(void)
{
	if (asexinfop->is_assigned) {
		asexinfop->is_assigned = 0;
		asexinfop->assigned_expression = _ex0();
	}
	setflag(status_flags::evaluated | status_flags::expanded);
}

// private

/** Symbols not constructed with a string get one assigned using this
 *  prefix and a number. */
std::string & symbol::autoname_prefix(void)
{
	static std::string *s = new std::string("symbol");
	return *s;
}

/** Return default TeX name for symbol. This recognizes some greek letters. */
std::string symbol::default_TeX_name(void) const
{
	if (name=="alpha"        || name=="beta"         || name=="gamma"
	 || name=="delta"        || name=="epsilon"      || name=="varepsilon"
	 || name=="zeta"         || name=="eta"          || name=="theta"
	 || name=="vartheta"     || name=="iota"         || name=="kappa"
	 || name=="lambda"       || name=="mu"           || name=="nu"
	 || name=="xi"           || name=="omicron"      || name=="pi"
	 || name=="varpi"        || name=="rho"          || name=="varrho"
	 || name=="sigma"        || name=="varsigma"     || name=="tau"
	 || name=="upsilon"      || name=="phi"          || name=="varphix"
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
symbol::assigned_ex_info::assigned_ex_info(void) : is_assigned(0), refcount(1)
{
}

} // namespace GiNaC
