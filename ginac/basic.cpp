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
#include <stdexcept>
#ifdef DO_GINAC_ASSERT
#  include <typeinfo>
#endif

#include "basic.h"
#include "ex.h"
#include "numeric.h"
#include "power.h"
#include "symbol.h"
#include "lst.h"
#include "ncmul.h"
#include "relational.h"
#include "print.h"
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
DEFAULT_UNARCHIVE(basic)

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

/** Output to stream.
 *  @param c print context object that describes the output formatting
 *  @param level value that is used to identify the precedence or indentation
 *               level for placing parentheses and formatting */
void basic::print(const print_context & c, unsigned level) const
{
	debugmsg("basic print", LOGLEVEL_PRINT);

	if (is_of_type(c, print_tree)) {

		c.s << std::string(level, ' ') << class_name()
		    << std::hex << ", hash=0x" << hashvalue << ", flags=0x" << flags << std::dec
		    << ", nops=" << nops()
		    << std::endl;
		for (unsigned i=0; i<nops(); ++i)
			op(i).print(c, level + static_cast<const print_tree &>(c).delta_indent);

	} else
		c.s << "[" << class_name() << " object]";
}

/** Little wrapper around print to be called within a debugger.
 *  This is needed because you cannot call foo.print(cout) from within the
 *  debugger because it might not know what cout is.  This method can be
 *  invoked with no argument and it will simply print to stdout.
 *
 *  @see basic::print */
void basic::dbgprint(void) const
{
	this->print(std::cerr);
	std::cerr << std::endl;
}

/** Little wrapper around printtree to be called within a debugger.
 *
 *  @see basic::dbgprint
 *  @see basic::printtree */
void basic::dbgprinttree(void) const
{
	this->print(print_tree(std::cerr));
}

/** Return relative operator precedence (for parenthizing output). */
unsigned basic::precedence(void) const
{
	return 70;
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

/** Search ocurrences.  An object 'has' an expression if it is the expression
 *  itself or one of the children 'has' it.  As a consequence (according to
 *  the definition of children) given e=x+y+z, e.has(x) is true but e.has(x+y)
 *  is false.  The expression can also contain wildcards. */
bool basic::has(const ex & other) const
{
	GINAC_ASSERT(other.bp!=0);
	lst repl_lst;
	if (match(*other.bp, repl_lst))
		return true;
	for (unsigned i=0; i<nops(); i++)
		if (op(i).has(other))
			return true;
	
	return false;
}

/** Construct new expression by applying the specified function to all
 *  sub-expressions (one level only, not recursively). */
ex basic::map(map_func f) const
{
	unsigned num = nops();
	if (num == 0)
		return *this;

	basic *copy = duplicate();
	copy->setflag(status_flags::dynallocated);
	copy->clearflag(status_flags::hash_calculated);
	ex e(*copy);
	for (unsigned i=0; i<num; i++)
		e.let_op(i) = f(e.op(i));
	return e.eval();
}

/** Return degree of highest power in object s. */
int basic::degree(const ex & s) const
{
	return 0;
}

/** Return degree of lowest power in object s. */
int basic::ldegree(const ex & s) const
{
	return 0;
}

/** Return coefficient of degree n in object s. */
ex basic::coeff(const ex & s, int n) const
{
	return n==0 ? *this : _ex0();
}

/** Sort expanded expression in terms of powers of some object(s).
 *  @param s object(s) to sort in
 *  @param distributed recursive or distributed form (only used when s is a list) */
ex basic::collect(const ex & s, bool distributed) const
{
	ex x;
	if (is_ex_of_type(s, lst)) {

		// List of objects specified
		if (s.nops() == 1)
			return collect(s.op(0));

		else if (distributed) {

			// Get lower/upper degree of all symbols in list
			int num = s.nops();
			struct sym_info {
				ex sym;
				int ldeg, deg;
				int cnt;  // current degree, 'counter'
				ex coeff; // coefficient for degree 'cnt'
			};
			sym_info *si = new sym_info[num];
			ex c = *this;
			for (int i=0; i<num; i++) {
				si[i].sym = s.op(i);
				si[i].ldeg = si[i].cnt = this->ldegree(si[i].sym);
				si[i].deg = this->degree(si[i].sym);
				c = si[i].coeff = c.coeff(si[i].sym, si[i].cnt);
			}

			while (true) {

				// Calculate coeff*x1^c1*...*xn^cn
				ex y = _ex1();
				for (int i=0; i<num; i++) {
					int cnt = si[i].cnt;
					y *= power(si[i].sym, cnt);
				}
				x += y * si[num - 1].coeff;

				// Increment counters
				int n = num - 1;
				while (true) {
					si[n].cnt++;
					if (si[n].cnt <= si[n].deg) {
						// Update coefficients
						ex c;
						if (n == 0)
							c = *this;
						else
							c = si[n - 1].coeff;
						for (int i=n; i<num; i++)
							c = si[i].coeff = c.coeff(si[i].sym, si[i].cnt);
						break;
					}
					if (n == 0)
						goto done;
					si[n].cnt = si[n].ldeg;
					n--;
				}
			}

done:		delete[] si;

		} else {

			// Recursive form
			x = *this;
			for (int n=s.nops()-1; n>=0; n--)
				x = x.collect(s[n]);
		}

	} else {

		// Only one object specified
		for (int n=this->ldegree(s); n<=this->degree(s); ++n)
			x += this->coeff(s,n)*power(s,n);
	}
	
	// correct for lost fractional arguments and return
	return x + (*this - x).expand();
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

/** Evaluate sums and products of matrices. */
ex basic::evalm(void) const
{
	if (nops() == 0)
		return *this;
	else
		return map(GiNaC::evalm);
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

/** Add two indexed expressions. They are guaranteed to be of class indexed
 *  (or a subclass) and their indices are compatible. This function is used
 *  internally by simplify_indexed().
 *
 *  @param self First indexed expression; it's base object is *this
 *  @param other Second indexed expression
 *  @return sum of self and other 
 *  @see ex::simplify_indexed() */
ex basic::add_indexed(const ex & self, const ex & other) const
{
	return self + other;
}

/** Multiply an indexed expression with a scalar. This function is used
 *  internally by simplify_indexed().
 *
 *  @param self Indexed expression; it's base object is *this
 *  @param other Numeric value
 *  @return product of self and other
 *  @see ex::simplify_indexed() */
ex basic::scalar_mul_indexed(const ex & self, const numeric & other) const
{
	return self * other;
}

/** Try to contract two indexed expressions that appear in the same product. 
 *  If a contraction exists, the function overwrites one or both of the
 *  expressions and returns true. Otherwise it returns false. It is
 *  guaranteed that both expressions are of class indexed (or a subclass)
 *  and that at least one dummy index has been found. This functions is
 *  used internally by simplify_indexed().
 *
 *  @param self Pointer to first indexed expression; it's base object is *this
 *  @param other Pointer to second indexed expression
 *  @param v The complete vector of factors
 *  @return true if the contraction was successful, false otherwise
 *  @see ex::simplify_indexed() */
bool basic::contract_with(exvector::iterator self, exvector::iterator other, exvector & v) const
{
	// Do nothing
	return false;
}

/** Check whether the expression matches a given pattern. For every wildcard
 *  object in the pattern, an expression of the form "wildcard == matching_expression"
 *  is added to repl_lst. */
bool basic::match(const ex & pattern, lst & repl_lst) const
{
/*
	Sweet sweet shapes, sweet sweet shapes,
	That's the key thing, right right.
	Feed feed face, feed feed shapes,
	But who is the king tonight?
	Who is the king tonight?
	Pattern is the thing, the key thing-a-ling,
	But who is the king of pattern?
	But who is the king, the king thing-a-ling,
	Who is the king of Pattern?
	Bog is the king, the king thing-a-ling,
	Bog is the king of Pattern.
	Ba bu-bu-bu-bu bu-bu-bu-bu-bu-bu bu-bu
	Bog is the king of Pattern.
*/

	if (is_ex_exactly_of_type(pattern, wildcard)) {

		// Wildcard matches anything, but check whether we already have found
		// a match for that wildcard first (if so, it the earlier match must
		// be the same expression)
		for (unsigned i=0; i<repl_lst.nops(); i++) {
			if (repl_lst.op(i).op(0).is_equal(pattern))
				return is_equal(*repl_lst.op(i).op(1).bp);
		}
		repl_lst.append(pattern == *this);
		return true;

	} else {

		// Expression must be of the same type as the pattern
		if (tinfo() != pattern.bp->tinfo())
			return false;

		// Number of subexpressions must match
		if (nops() != pattern.nops())
			return false;

		// No subexpressions? Then just compare the objects (there can't be
		// wildcards in the pattern)
		if (nops() == 0)
			return is_equal(*pattern.bp);

		// Otherwise the subexpressions must match one-to-one
		for (unsigned i=0; i<nops(); i++)
			if (!op(i).match(pattern.op(i), repl_lst))
				return false;

		// Looks similar enough, match found
		return true;
	}
}

/** Substitute a set of objects by arbitrary expressions. The ex returned
 *  will already be evaluated. */
ex basic::subs(const lst & ls, const lst & lr, bool no_pattern) const
{
	GINAC_ASSERT(ls.nops() == lr.nops());

	if (no_pattern) {
		for (unsigned i=0; i<ls.nops(); i++) {
			if (is_equal(*ls.op(i).bp))
				return lr.op(i);
		}
	} else {
		for (unsigned i=0; i<ls.nops(); i++) {
			lst repl_lst;
			if (match(*ls.op(i).bp, repl_lst))
				return lr.op(i).bp->subs(repl_lst, true); // avoid infinite recursion when re-substituting the wildcards
		}
	}

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
 *  the canonical ordering.  (Perl hackers will wonder why C++ doesn't feature
 *  the spaceship operator <=> for denoting just this.) */
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

/** Substitute objects in an expression (syntactic substitution) and return
 *  the result as a new expression.  There are two valid types of
 *  replacement arguments: 1) a relational like object==ex and 2) a list of
 *  relationals lst(object1==ex1,object2==ex2,...), which is converted to
 *  subs(lst(object1,object2,...),lst(ex1,ex2,...)). */
ex basic::subs(const ex & e, bool no_pattern) const
{
	if (e.info(info_flags::relation_equal)) {
		return subs(lst(e), no_pattern);
	}
	if (!e.info(info_flags::list)) {
		throw(std::invalid_argument("basic::subs(ex): argument must be a list"));
	}
	lst ls;
	lst lr;
	for (unsigned i=0; i<e.nops(); i++) {
		ex r = e.op(i);
		if (!r.info(info_flags::relation_equal)) {
			throw(std::invalid_argument("basic::subs(ex): argument must be a list of equations"));
		}
		ls.append(r.op(0));
		lr.append(r.op(1));
	}
	return subs(ls, lr, no_pattern);
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
// 		this->print(print_tree(std::cout));
// 		std::cout << " and ";
// 		other.print(print_tree(std::cout));
// 		std::cout << std::endl;
		return -1;
	}
	if (typeid_this>typeid_other) {
// 		std::cout << "hash collision, different types: " 
// 		          << *this << " and " << other << std::endl;
// 		this->print(print_tree(std::cout));
// 		std::cout << " and ";
// 		other.print(print_tree(std::cout));
// 		std::cout << std::endl;
		return 1;
	}
	
	GINAC_ASSERT(typeid(*this)==typeid(other));
	
// 	int cmpval = compare_same_type(other);
// 	if ((cmpval!=0) && (hash_this<0x80000000U)) {
// 		std::cout << "hash collision, same type: " 
// 		          << *this << " and " << other << std::endl;
// 		this->print(print_tree(std::cout));
// 		std::cout << " and ";
// 		other.print(print_tree(std::cout));
// 		std::cout << std::endl;
// 	}
// 	return cmpval;
	
	return compare_same_type(other);
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
// global variables
//////////

int max_recursion_level = 1024;

} // namespace GiNaC
