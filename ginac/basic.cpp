/** @file basic.cpp
 *
 *  Implementation of GiNaC's ABC. */

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

#include <iostream>
#include <typeinfo>
#include <stdexcept>

#include "basic.h"
#include "ex.h"
#include "numeric.h"
#include "power.h"
#include "symbol.h"
#include "lst.h"
#include "ncmul.h"
#include "archive.h"
#include "utils.h"
#include "debugmsg.h"

namespace GiNaC {

GINAC_IMPLEMENT_REGISTERED_CLASS_NO_CTORS(basic, void)

//////////
// default ctor, dtor, copy ctor assignment operator and helpers
//////////

// public

basic::basic(const basic & other) : tinfo_key(TINFO_basic), flags(0), refcount(0)
{
	debugmsg("basic copy ctor", LOGLEVEL_CONSTRUCT);
	copy(other);
}

const basic & basic::operator=(const basic & other)
{
	debugmsg("basic operator=", LOGLEVEL_ASSIGNMENT);
	if (this != &other) {
		destroy(true);
		copy(other);
	}
	return *this;
}

// protected

// none (all conditionally inlined)

//////////
// other ctors
//////////

// none (all conditionally inlined)

//////////
// archiving
//////////

/** Construct object from archive_node. */
basic::basic(const archive_node &n, const lst &sym_lst) : flags(0), refcount(0)
{
	debugmsg("basic ctor from archive_node", LOGLEVEL_CONSTRUCT);

	// Reconstruct tinfo_key from class name
	std::string class_name;
	if (n.find_string("class", class_name))
		tinfo_key = find_tinfo_key(class_name);
	else
		throw (std::runtime_error("archive node contains no class name"));
}

/** Unarchive the object. */
ex basic::unarchive(const archive_node &n, const lst &sym_lst)
{
	return (new basic(n, sym_lst))->setflag(status_flags::dynallocated);
}

/** Archive the object. */
void basic::archive(archive_node &n) const
{
	n.add_string("class", class_name());
}

//////////
// functions overriding virtual functions from bases classes
//////////

// none

//////////
// new virtual functions which can be overridden by derived classes
//////////

// public

/** Output to ostream formatted as parsable (as in ginsh) input.
 *  Generally, superfluous parenthesis should be avoided as far as possible. */
void basic::print(std::ostream & os, unsigned upper_precedence) const
{
	debugmsg("basic print",LOGLEVEL_PRINT);
	os << "[" << class_name() << " object]";
}

/** Output to ostream in ugly raw format, so brave developers can have a look
 *  at the underlying structure. */
void basic::printraw(std::ostream & os) const
{
	debugmsg("basic printraw",LOGLEVEL_PRINT);
	os << "[" << class_name() << " object]";
}

/** Output to ostream formatted in tree- (indented-) form, so developers can
 *  have a look at the underlying structure. */
void basic::printtree(std::ostream & os, unsigned indent) const
{
	debugmsg("basic printtree",LOGLEVEL_PRINT);
	os << std::string(indent,' ') << "type=" << class_name()
	   << ", hash=" << hashvalue
	   << " (0x" << std::hex << hashvalue << std::dec << ")"
	   << ", flags=" << flags
	   << ", nops=" << nops() << std::endl;
	for (unsigned i=0; i<nops(); ++i) {
		op(i).printtree(os,indent+delta_indent);
	}
}

/** Output to ostream formatted as C-source.
 *
 *  @param os a stream for output
 *  @param type variable type (one of the csrc_types)
 *  @param upper_precedence operator precedence of caller
 *  @see ex::printcsrc */
void basic::printcsrc(std::ostream & os, unsigned type, unsigned upper_precedence) const
{
	debugmsg("basic print csrc", LOGLEVEL_PRINT);
}

/** Little wrapper arount print to be called within a debugger.
 *  This is needed because you cannot call foo.print(cout) from within the
 *  debugger because it might not know what cout is.  This method can be
 *  invoked with no argument and it will simply print to stdout.
 *
 *  @see basic::print*/
void basic::dbgprint(void) const
{
	this->print(std::cerr);
	std::cerr << std::endl;
}

/** Little wrapper arount printtree to be called within a debugger.
 *
 *  @see basic::dbgprint
 *  @see basic::printtree */
void basic::dbgprinttree(void) const
{
	this->printtree(std::cerr,0);
}

/** Create a new copy of this on the heap.  One can think of this as simulating
 *  a virtual copy constructor which is needed for instance by the refcounted
 *  construction of an ex from a basic. */
basic * basic::duplicate() const
{
	debugmsg("basic duplicate",LOGLEVEL_DUPLICATE);
	return new basic(*this);
}

/** Information about the object.
 *
 *  @see class info_flags */
bool basic::info(unsigned inf) const
{
	// all possible properties are false for basic objects
	return false;
}

/** Number of operands/members. */
unsigned basic::nops() const
{
	// iterating from 0 to nops() on atomic objects should be an empty loop,
	// and accessing their elements is a range error.  Container objects should
	// override this.
	return 0;
}

/** Return operand/member at position i. */
ex basic::op(int i) const
{
	return (const_cast<basic *>(this))->let_op(i);
}

/** Return modifyable operand/member at position i. */
ex & basic::let_op(int i)
{
	throw(std::out_of_range("op() out of range"));
}

ex basic::operator[](const ex & index) const
{
	if (is_exactly_of_type(*index.bp,numeric))
		return op(static_cast<const numeric &>(*index.bp).to_int());
	
	throw(std::invalid_argument("non-numeric indices not supported by this type"));
}

ex basic::operator[](int i) const
{
	return op(i);
}

/** Search ocurrences.  An object  'has' an expression if it is the expression
 *  itself or one of the children 'has' it.  As a consequence (according to
 *  the definition of children) given e=x+y+z, e.has(x) is true but e.has(x+y)
 *  is false. */
bool basic::has(const ex & other) const
{
	GINAC_ASSERT(other.bp!=0);
	if (is_equal(*other.bp)) return true;
	if (nops()>0) {
		for (unsigned i=0; i<nops(); i++)
			if (op(i).has(other))
				return true;
	}
	
	return false;
}

/** Return degree of highest power in symbol s. */
int basic::degree(const symbol & s) const
{
	return 0;
}

/** Return degree of lowest power in symbol s. */
int basic::ldegree(const symbol & s) const
{
	return 0;
}

/** Return coefficient of degree n in symbol s. */
ex basic::coeff(const symbol & s, int n) const
{
	return n==0 ? *this : _ex0();
}

/** Sort expression in terms of powers of some symbol.
 *  @param s symbol to sort in. */
ex basic::collect(const symbol & s) const
{
	ex x;
	for (int n=this->ldegree(s); n<=this->degree(s); n++)
		x += this->coeff(s,n)*power(s,n);
	
	return x;
}

/** Perform automatic non-interruptive symbolic evaluation on expression. */
ex basic::eval(int level) const
{
	// There is nothing to do for basic objects:
	return this->hold();
}

/** Evaluate object numerically. */
ex basic::evalf(int level) const
{
	// There is nothing to do for basic objects:
	return *this;
}

/** Perform automatic symbolic evaluations on indexed expression that
 *  contains this object as the base expression. */
ex basic::eval_indexed(const basic & i) const
 // this function can't take a "const ex & i" because that would result
 // in an infinite eval() loop
{
	// There is nothing to do for basic objects
	return i.hold();
}

/** Try to contract two indexed expressions. If a contraction exists, the
 *  function overwrites one or both arguments and returns true. Otherwise it
 *  returns false. It is guaranteed that both expressions are of class
 *  indexed (or a subclass) and that at least one dummy index has been
 *  found.
 *
 *  @param self The first indexed expression; it's base object is *this
 *  @param other The second indexed expression
 *  @return true if the contraction was successful, false otherwise */
bool basic::contract_with(ex & self, ex & other) const
{
	// Do nothing
	return false;
}

/** Substitute a set of symbols by arbitrary expressions. The ex returned
 *  will already be evaluated. */
ex basic::subs(const lst & ls, const lst & lr) const
{
	return *this;
}

/** Default interface of nth derivative ex::diff(s, n).  It should be called
 *  instead of ::derivative(s) for first derivatives and for nth derivatives it
 *  just recurses down.
 *
 *  @param s symbol to differentiate in
 *  @param nth order of differentiation
 *  @see ex::diff */
ex basic::diff(const symbol & s, unsigned nth) const
{
	// trivial: zeroth derivative
	if (nth==0)
		return ex(*this);
	
	// evaluate unevaluated *this before differentiating
	if (!(flags & status_flags::evaluated))
		return ex(*this).diff(s, nth);
	
	ex ndiff = this->derivative(s);
	while (!ndiff.is_zero() &&    // stop differentiating zeros
	       nth>1) {
		ndiff = ndiff.diff(s);
		--nth;
	}
	return ndiff;
}

/** Return a vector containing the free indices of an expression. */
exvector basic::get_free_indices(void) const
{
	return exvector(); // return an empty exvector
}

ex basic::simplify_ncmul(const exvector & v) const
{
	return simplified_ncmul(v);
}

// protected

/** Default implementation of ex::diff(). It simply throws an error message.
 *
 *  @exception logic_error (differentiation not supported by this type)
 *  @see ex::diff */
ex basic::derivative(const symbol & s) const
{
	throw(std::logic_error("differentiation not supported by this type"));
}

/** Returns order relation between two objects of same type.  This needs to be
 *  implemented by each class. It may never return anything else than 0,
 *  signalling equality, or +1 and -1 signalling inequality and determining
 *  the canonical ordering. */
int basic::compare_same_type(const basic & other) const
{
	return compare_pointers(this, &other);
}

/** Returns true if two objects of same type are equal.  Normally needs
 *  not be reimplemented as long as it wasn't overwritten by some parent
 *  class, since it just calls compare_same_type().  The reason why this
 *  function exists is that sometimes it is easier to determine equality
 *  than an order relation and then it can be overridden. */
bool basic::is_equal_same_type(const basic & other) const
{
	return this->compare_same_type(other)==0;
}

unsigned basic::return_type(void) const
{
	return return_types::commutative;
}

unsigned basic::return_type_tinfo(void) const
{
	return tinfo();
}

/** Compute the hash value of an object and if it makes sense to store it in
 *  the objects status_flags, do so.  The method inherited from class basic
 *  computes a hash value based on the type and hash values of possible
 *  members.  For this reason it is well suited for container classes but
 *  atomic classes should override this implementation because otherwise they
 *  would all end up with the same hashvalue. */
unsigned basic::calchash(void) const
{
	unsigned v = golden_ratio_hash(tinfo());
	for (unsigned i=0; i<nops(); i++) {
		v = rotate_left_31(v);
		v ^= (const_cast<basic *>(this))->op(i).gethash();
	}
	
	// mask out numeric hashes:
	v &= 0x7FFFFFFFU;
	
	// store calculated hash value only if object is already evaluated
	if (flags & status_flags::evaluated) {
		setflag(status_flags::hash_calculated);
		hashvalue = v;
	}

	return v;
}

/** Expand expression, i.e. multiply it out and return the result as a new
 *  expression. */
ex basic::expand(unsigned options) const
{
	return this->setflag(status_flags::expanded);
}


//////////
// non-virtual functions in this class
//////////

// public

/** Substitute symbols in expression and return the result as a new expression.
 *  There are two valid types of replacement arguments: 1) a relational like
 *  symbol==ex and 2) a list of relationals lst(symbol1==ex1,symbol2==ex2,...),
 *  which is converted to subs(lst(symbol1,symbol2,...),lst(ex1,ex2,...)).
 *  In addition, an object of class idx can be used instead of a symbol. */
ex basic::subs(const ex & e) const
{
	if (e.info(info_flags::relation_equal)) {
		return subs(lst(e));
	}
	if (!e.info(info_flags::list)) {
		throw(std::invalid_argument("basic::subs(ex): argument must be a list"));
	}
	lst ls;
	lst lr;
	for (unsigned i=0; i<e.nops(); i++) {
		if (!e.op(i).info(info_flags::relation_equal)) {
			throw(std::invalid_argument("basic::subs(ex): argument must be a list or equations"));
		}
		if (!e.op(i).op(0).info(info_flags::symbol)) {
			if (!e.op(i).op(0).info(info_flags::idx)) {
				throw(std::invalid_argument("basic::subs(ex): lhs must be a symbol or an idx"));
			}
		}
		ls.append(e.op(i).op(0));
		lr.append(e.op(i).op(1));
	}
	return subs(ls,lr);
}

/** Compare objects to establish canonical ordering.
 *  All compare functions return: -1 for *this less than other, 0 equal,
 *  1 greater. */
int basic::compare(const basic & other) const
{
	unsigned hash_this = gethash();
	unsigned hash_other = other.gethash();
	
	if (hash_this<hash_other) return -1;
	if (hash_this>hash_other) return 1;
	
	unsigned typeid_this = tinfo();
	unsigned typeid_other = other.tinfo();
	
	if (typeid_this<typeid_other) {
// 		std::cout << "hash collision, different types: " 
// 		          << *this << " and " << other << std::endl;
// 		this->printraw(std::cout);
// 		std::cout << " and ";
// 		other.printraw(std::cout);
// 		std::cout << std::endl;
		return -1;
	}
	if (typeid_this>typeid_other) {
// 		std::cout << "hash collision, different types: " 
// 		          << *this << " and " << other << std::endl;
// 		this->printraw(std::cout);
// 		std::cout << " and ";
// 		other.printraw(std::cout);
// 		std::cout << std::endl;
		return 1;
	}
	
	GINAC_ASSERT(typeid(*this)==typeid(other));
	
	int cmpval = compare_same_type(other);
	if ((cmpval!=0) && (hash_this<0x80000000U)) {
// 		std::cout << "hash collision, same type: " 
// 		          << *this << " and " << other << std::endl;
// 		this->printraw(std::cout);
// 		std::cout << " and ";
// 		other.printraw(std::cout);
// 		std::cout << std::endl;
	}
	return cmpval;
}

/** Test for equality.
 *  This is only a quick test, meaning objects should be in the same domain.
 *  You might have to .expand(), .normal() objects first, depending on the
 *  domain of your computation, to get a more reliable answer.
 *
 *  @see is_equal_same_type */
bool basic::is_equal(const basic & other) const
{
	if (this->gethash()!=other.gethash())
		return false;
	if (this->tinfo()!=other.tinfo())
		return false;
	
	GINAC_ASSERT(typeid(*this)==typeid(other));
	
	return this->is_equal_same_type(other);
}

// protected

/** Stop further evaluation.
 *
 *  @see basic::eval */
const basic & basic::hold(void) const
{
	return this->setflag(status_flags::evaluated);
}

/** Ensure the object may be modified without hurting others, throws if this
 *  is not the case. */
void basic::ensure_if_modifiable(void) const
{
	if (this->refcount>1)
		throw(std::runtime_error("cannot modify multiply referenced object"));
}

//////////
// static member variables
//////////

// protected

unsigned basic::precedence = 70;
unsigned basic::delta_indent = 4;

//////////
// global variables
//////////

int max_recursion_level = 1024;

} // namespace GiNaC
