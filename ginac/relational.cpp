/** @file relational.cpp
 *
 *  Implementation of relations between expressions */

/*
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

#include <stdexcept>

#include "relational.h"
#include "numeric.h"
#include "archive.h"
#include "utils.h"
#include "debugmsg.h"

#ifndef NO_NAMESPACE_GINAC
namespace GiNaC {
#endif // ndef NO_NAMESPACE_GINAC

GINAC_IMPLEMENT_REGISTERED_CLASS(relational, basic)

//////////
// default constructor, destructor, copy constructor assignment operator and helpers
//////////

// public

relational::relational() : basic(TINFO_relational)
{
	debugmsg("relational default constructor",LOGLEVEL_CONSTRUCT);
}

relational::~relational()
{
	debugmsg("relational destructor",LOGLEVEL_DESTRUCT);
	destroy(0);
}

relational::relational(const relational & other)
{
	debugmsg("relational copy constructor",LOGLEVEL_CONSTRUCT);
	copy(other);
}

const relational & relational::operator=(const relational & other)
{
	debugmsg("relational operator=",LOGLEVEL_ASSIGNMENT);
	if (this != &other) {
		destroy(1);
		copy(other);
	}
	return *this;
}

// protected

void relational::copy(const relational & other)
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

relational::relational(const ex & lhs, const ex & rhs, operators oper) : basic(TINFO_relational)
{
	debugmsg("relational constructor ex,ex,operator",LOGLEVEL_CONSTRUCT);
	lh=lhs;
	rh=rhs;
	o=oper;
}

//////////
// archiving
//////////

/** Construct object from archive_node. */
relational::relational(const archive_node &n, const lst &sym_lst) : inherited(n, sym_lst)
{
	debugmsg("relational constructor from archive_node", LOGLEVEL_CONSTRUCT);
	unsigned int opi;
	if (!(n.find_unsigned("op", opi)))
		throw (std::runtime_error("unknown relational operator in archive"));
	o = (operators)opi;
	n.find_ex("lh", lh, sym_lst);
	n.find_ex("rh", rh, sym_lst);
}

/** Unarchive the object. */
ex relational::unarchive(const archive_node &n, const lst &sym_lst)
{
	return (new relational(n, sym_lst))->setflag(status_flags::dynallocated);
}

/** Archive the object. */
void relational::archive(archive_node &n) const
{
	inherited::archive(n);
	n.add_ex("lh", lh);
	n.add_ex("rh", rh);
	n.add_unsigned("op", o);
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

void relational::print(std::ostream & os, unsigned upper_precedence) const
{
	debugmsg("relational print",LOGLEVEL_PRINT);
	if (precedence<=upper_precedence) os << "(";
	lh.print(os,precedence);
	switch (o) {
	case equal:
		os << "==";
		break;
	case not_equal:
		os << "!=";
		break;
	case less:
		os << "<";
		break;
	case less_or_equal:
		os << "<=";
		break;
	case greater:
		os << ">";
		break;
	case greater_or_equal:
		os << ">=";
		break;
	default:
		os << "(INVALID RELATIONAL OPERATOR)";
	}
	rh.print(os,precedence);
	if (precedence<=upper_precedence) os << ")";
}

void relational::printraw(std::ostream & os) const
{
	debugmsg("relational printraw",LOGLEVEL_PRINT);
	os << "RELATIONAL(";
	lh.printraw(os);
	os << ",";
	rh.printraw(os);
	os << ",";
	switch (o) {
	case equal:
		os << "==";
		break;
	case not_equal:
		os << "!=";
		break;
	case less:
		os << "<";
		break;
	case less_or_equal:
		os << "<=";
		break;
	case greater:
		os << ">";
		break;
	case greater_or_equal:
		os << ">=";
		break;
	default:
		os << "(INVALID RELATIONAL OPERATOR)";
	}
	os << ")";
}

void relational::printcsrc(std::ostream & os, unsigned type, unsigned upper_precedence) const
{
	debugmsg("relational print csrc", LOGLEVEL_PRINT);
	if (precedence<=upper_precedence)
		os << "(";

	// Print left-hand expression
	lh.bp->printcsrc(os, type, precedence);

	// Print relational operator
	switch (o) {
		case equal:
			os << "==";
			break;
		case not_equal:
			os << "!=";
			break;
		case less:
			os << "<";
			break;
		case less_or_equal:
			os << "<=";
			break;
		case greater:
			os << ">";
			break;
		case greater_or_equal:
			os << ">=";
			break;
		default:
			os << "(INVALID RELATIONAL OPERATOR)";
			break;
	}

	// Print right-hand operator
	rh.bp->printcsrc(os, type, precedence);

	if (precedence <= upper_precedence)
		os << ")";
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

unsigned relational::nops() const
{
	return 2;
}

ex & relational::let_op(int i)
{
	GINAC_ASSERT(i>=0);
	GINAC_ASSERT(i<2);

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

ex relational::simplify_ncmul(const exvector & v) const
{
	return lh.simplify_ncmul(v);
}

// protected

int relational::compare_same_type(const basic & other) const
{
	GINAC_ASSERT(is_exactly_of_type(other, relational));
	const relational & oth=static_cast<const relational &>(const_cast<basic &>(other));
	
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
	GINAC_ASSERT(lh.return_type()==rh.return_type());
	return lh.return_type();
}
   
unsigned relational::return_type_tinfo(void) const
{
	GINAC_ASSERT(lh.return_type_tinfo()==rh.return_type_tinfo());
	return lh.return_type_tinfo();
}

//////////
// new virtual functions which can be overridden by derived classes
//////////

/** Left hand side of relational. */
ex relational::lhs(void) const
{
	return lh;
}

/** Right hand side of relational. */
ex relational::rhs(void) const
{
	return rh;    
}

//////////
// non-virtual functions in this class
//////////

relational::operator bool() const
{
	// please note that (a<b) == false does not imply (a>=b) == true
	// a false result means the comparison is either false or undecidable
	// (except for !=, where true means unequal or undecidable)
	ex df=lh-rh;
	if (!is_ex_exactly_of_type(df,numeric)) {
		return o==not_equal ? true : false; // cannot decide on non-numerical results
	}
	int cmpval=ex_to_numeric(df).compare(_num0());
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
const type_info & typeid_relational=typeid(some_relational);

#ifndef NO_NAMESPACE_GINAC
} // namespace GiNaC
#endif // ndef NO_NAMESPACE_GINAC
