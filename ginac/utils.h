/** @file utils.h
 *
 *  Interface to several small and furry utilities needed within GiNaC but not
 *  of any interest to the user of the library. */

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

#ifndef __GINAC_UTILS_H__
#define __GINAC_UTILS_H__

#include "config.h"

#include <string>
#include <stdexcept>
#include <functional>
#if defined(HAVE_SSTREAM)
#include <sstream>
#elif defined(HAVE_STRSTREAM)
#include <strstream>
#else
#error Need either sstream or strstream
#endif
#include "assertion.h"

namespace GiNaC {

// This should be obsoleted once <sstream> is widely deployed.
template<class T>
std::string ToString(const T & t)
{
#if defined(HAVE_SSTREAM)
	std::ostringstream buf;
	buf << t << std::ends;
	return buf.str();
#else
	char buf[256];
	std::ostrstream(buf,sizeof(buf)) << t << std::ends;
	return buf;
#endif
}

/** Exception class thrown by classes which provide their own series expansion
 *  to signal that ordinary Taylor expansion is safe. */
class do_taylor {};

/** Exception class thrown when a singularity is encountered. */
class pole_error : public std::domain_error {
public:
	explicit pole_error(const std::string& what_arg, int degree);
	int degree(void) const;
private:
	int deg;
};

// some compilers (e.g. cygwin) define a macro log2, causing confusion
#ifndef log2
unsigned log2(unsigned n);
#endif

/** Compare two pointers (just to establish some sort of canonical order).
 *  @return -1, 0, or 1 */
inline int compare_pointers(const void * a, const void * b)
{
	if (a<b)
		return -1;
	else if (a>b)
		return 1;
	return 0;
}

/** Rotate lower 31 bits of unsigned value by one bit to the left
 *  (upper bit gets cleared). */
inline unsigned rotate_left_31(unsigned n)
{
	// clear highest bit and shift 1 bit to the left
	n = (n & 0x7FFFFFFFU) << 1;
	
	// overflow? clear highest bit and set lowest bit
	if (n & 0x80000000U)
		n = (n & 0x7FFFFFFFU) | 0x00000001U;
	
	GINAC_ASSERT(n<0x80000000U);
	
	return n;
}

/** Golden ratio hash function for the 31 least significant bits. */
inline unsigned golden_ratio_hash(unsigned n)
{
	// This function requires arithmetic with at least 64 significant bits
#if SIZEOF_LONG >= 8
	// So 'long' has 64 bits.  Excellent!  We prefer it because it might be
	// more efficient than 'long long'.
	unsigned long l = n * 0x4f1bbcddL;
	return (l & 0x7fffffffU) ^ (l >> 32);
#elif SIZEOF_LONG_LONG >= 8
	// This requires 'long long' (or an equivalent 64 bit type)---which is,
	// unfortunately, not ANSI-C++-compliant.
	// (Yet C99 demands it, which is reason for hope.)
	unsigned long long l = n * 0x4f1bbcddL;
	return (l & 0x7fffffffU) ^ (l >> 32);
#elif SIZEOF_LONG_DOUBLE > 8
	// If 'long double' is bigger than 64 bits, we assume that the mantissa
	// has at least 64 bits. This is not guaranteed but it's a good guess.
	// Unfortunately, it may lead to horribly slow code.
	const static long double golden_ratio = .618033988749894848204586834370;
	long double m = golden_ratio * n;
	return unsigned((m - int(m)) * 0x80000000);
#else
#error "No 64 bit data type. You lose."
#endif
}

/* Compute the sign of a permutation of a container, with and without an
   explicitly supplied comparison function. If the sign returned is 1 or -1,
   the container is sorted after the operation. */
template <class It>
int permutation_sign(It first, It last)
{
	if (first == last)
		return 0;
	--last;
	if (first == last)
		return 0;
	It flag = first;
	int sign = 1;

	do {
		It i = last, other = last;
		--other;
		bool swapped = false;
		while (i != first) {
			if (*i < *other) {
				std::iter_swap(other, i);
				flag = other;
				swapped = true;
				sign = -sign;
			} else if (!(*other < *i))
				return 0;
			--i; --other;
		}
		if (!swapped)
			return sign;
		++flag;
		if (flag == last)
			return sign;
		first = flag;
		i = first; other = first;
		++other;
		swapped = false;
		while (i != last) {
			if (*other < *i) {
				std::iter_swap(i, other);
				flag = other;
				swapped = true;
				sign = -sign;
			} else if (!(*i < *other))
				return 0;
			++i; ++other;
		}
		if (!swapped)
			return sign;
		last = flag;
		--last;
	} while (first != last);

	return sign;
}

template <class It, class Cmp>
int permutation_sign(It first, It last, Cmp comp)
{
	if (first == last)
		return 0;
	--last;
	if (first == last)
		return 0;
	It flag = first;
	int sign = 1;

	do {
		It i = last, other = last;
		--other;
		bool swapped = false;
		while (i != first) {
			if (comp(*i, *other)) {
				std::iter_swap(other, i);
				flag = other;
				swapped = true;
				sign = -sign;
			} else if (!comp(*other, *i))
				return 0;
			--i; --other;
		}
		if (!swapped)
			return sign;
		++flag;
		if (flag == last)
			return sign;
		first = flag;
		i = first; other = first;
		++other;
		swapped = false;
		while (i != last) {
			if (comp(*other, *i)) {
				std::iter_swap(i, other);
				flag = other;
				swapped = true;
				sign = -sign;
			} else if (!comp(*i, *other))
				return 0;
			++i; ++other;
		}
		if (!swapped)
			return sign;
		last = flag;
		--last;
	} while (first != last);

	return sign;
}

/* Implementation of shaker sort, only compares adjacent elements. */
template <class It, class Cmp>
void shaker_sort(It first, It last, Cmp comp)
{
	if (first == last)
		return;
	--last;
	if (first == last)
		return;
	It flag = first;

	do {
		It i = last, other = last;
		--other;
		bool swapped = false;
		while (i != first) {
			if (comp(*i, *other)) {
				std::iter_swap(other, i);
				flag = other;
				swapped = true;
			}
			--i; --other;
		}
		if (!swapped)
			return;
		++flag;
		if (flag == last)
			return;
		first = flag;
		i = first; other = first;
		++other;
		swapped = false;
		while (i != last) {
			if (comp(*other, *i)) {
				std::iter_swap(i, other);
				flag = other;
				swapped = true;
			}
			++i; ++other;
		}
		if (!swapped)
			return;
		last = flag;
		--last;
	} while (first != last);
}

/* In-place cyclic permutation of a container (no copying, only swapping). */
template <class It>
void cyclic_permutation(It first, It last, It new_first)
{
	unsigned num = last - first;
again:
	if (first == new_first || num < 2)
		return;

	unsigned num1 = new_first - first, num2 = last - new_first;
	if (num1 >= num2) {
		It a = first, b = new_first;
		while (b != last) {
			std::iter_swap(a, b);
			++a; ++b;
		}
		if (num1 > num2) {
			first += num2;
			num = num1;
			goto again;
		}
	} else {
		It a = new_first, b = last;
		do {
			--a; --b;
			std::iter_swap(a, b);
		} while (a != first);
		last -= num1;
		num = num2;
		goto again;
	}
}

/* Function objects for STL sort() etc. */
struct ex_is_less : public std::binary_function<ex, ex, bool> {
	bool operator() (const ex &lh, const ex &rh) const { return lh.compare(rh) < 0; }
};

struct ex_is_equal : public std::binary_function<ex, ex, bool> {
	bool operator() (const ex &lh, const ex &rh) const { return lh.is_equal(rh); }
};

// Collection of `construct on first use' wrappers for safely avoiding
// internal object replication without running into the `static
// initialization order fiasco'.  This chest of numbers helps speed up
// the library but should not be used outside it since it is
// potentially confusing.

class numeric;
class ex;

const numeric & _num_120(void);   // -120
const ex & _ex_120(void);
const numeric & _num_60(void);    // -60
const ex & _ex_60(void);
const numeric & _num_48(void);    // -48
const ex & _ex_48(void);
const numeric & _num_30(void);    // -30
const ex & _ex_30(void);
const numeric & _num_25(void);    // -25
const ex & _ex_25(void);
const numeric & _num_24(void);    // -24
const ex & _ex_24(void);
const numeric & _num_20(void);    // -20
const ex & _ex_20(void);
const numeric & _num_18(void);    // -18
const ex & _ex_18(void);
const numeric & _num_15(void);    // -15
const ex & _ex_15(void);
const numeric & _num_12(void);    // -12
const ex & _ex_12(void);
const numeric & _num_11(void);    // -11
const ex & _ex_11(void);
const numeric & _num_10(void);    // -10
const ex & _ex_10(void);
const numeric & _num_9(void);     // -9
const ex & _ex_9(void);
const numeric & _num_8(void);     // -8
const ex & _ex_8(void);
const numeric & _num_7(void);     // -7
const ex & _ex_7(void);
const numeric & _num_6(void);     // -6
const ex & _ex_6(void);
const numeric & _num_5(void);     // -5
const ex & _ex_5(void);
const numeric & _num_4(void);     // -4
const ex & _ex_4(void);
const numeric & _num_3(void);     // -3
const ex & _ex_3(void);
const numeric & _num_2(void);     // -2
const ex & _ex_2(void);
const numeric & _num_1(void);     // -1
const ex & _ex_1(void);
const numeric & _num_1_2(void);   // -1/2
const ex & _ex_1_2(void);
const numeric & _num_1_3(void);   // -1/3
const ex & _ex_1_3(void);
const numeric & _num_1_4(void);   // -1/4
const ex & _ex_1_4(void);
const numeric & _num0(void);      //  0
const ex & _ex0(void);
const numeric & _num1_4(void);    //  1/4
const ex & _ex1_4(void);
const numeric & _num1_3(void);    //  1/3
const ex & _ex1_3(void);
const numeric & _num1_2(void);    //  1/2
const ex & _ex1_2(void);
const numeric & _num1(void);      //  1
const ex & _ex1(void);
const numeric & _num2(void);      //  2
const ex & _ex2(void);
const numeric & _num3(void);      //  3
const ex & _ex3(void);
const numeric & _num4(void);      //  4
const ex & _ex4(void);
const numeric & _num5(void);      //  5
const ex & _ex5(void);
const numeric & _num6(void);      //  6
const ex & _ex6(void);
const numeric & _num7(void);      //  7
const ex & _ex7(void);
const numeric & _num8(void);      //  8
const ex & _ex8(void);
const numeric & _num9(void);      //  9
const ex & _ex9(void);
const numeric & _num10(void);     //  10
const ex & _ex10(void);
const numeric & _num11(void);     //  11
const ex & _ex11(void);
const numeric & _num12(void);     //  12
const ex & _ex12(void);
const numeric & _num15(void);     //  15
const ex & _ex15(void);
const numeric & _num18(void);     //  18
const ex & _ex18(void);
const numeric & _num20(void);     //  20
const ex & _ex20(void);
const numeric & _num24(void);     //  24
const ex & _ex24(void);
const numeric & _num25(void);     //  25
const ex & _ex25(void);
const numeric & _num30(void);     //  30
const ex & _ex30(void);
const numeric & _num48(void);     //  48
const ex & _ex48(void);
const numeric & _num60(void);     //  60
const ex & _ex60(void);
const numeric & _num120(void);    //  120
const ex & _ex120(void);


// Helper macros for class implementations (mostly useful for trivial classes)

#define DEFAULT_COPY(classname) \
void classname::copy(const classname & other) \
{ \
	inherited::copy(other); \
}

#define DEFAULT_DESTROY(classname) \
void classname::destroy(bool call_parent) \
{ \
	if (call_parent) \
		inherited::destroy(call_parent); \
}

#define DEFAULT_CTORS(classname) \
classname::classname() : inherited(TINFO_##classname) \
{ \
	debugmsg(#classname " default constructor", LOGLEVEL_CONSTRUCT); \
} \
DEFAULT_COPY(classname) \
DEFAULT_DESTROY(classname)

#define DEFAULT_UNARCHIVE(classname) \
ex classname::unarchive(const archive_node &n, const lst &sym_lst) \
{ \
	return (new classname(n, sym_lst))->setflag(status_flags::dynallocated); \
}

#define DEFAULT_ARCHIVING(classname) \
classname::classname(const archive_node &n, const lst &sym_lst) : inherited(n, sym_lst) \
{ \
	debugmsg(#classname " constructor from archive_node", LOGLEVEL_CONSTRUCT); \
} \
DEFAULT_UNARCHIVE(classname) \
void classname::archive(archive_node &n) const \
{ \
	inherited::archive(n); \
}

#define DEFAULT_COMPARE(classname) \
int classname::compare_same_type(const basic & other) const \
{ \
	/* by default, the objects are always identical */ \
	return 0; \
}

#define DEFAULT_PRINT(classname, text) \
void classname::print(const print_context & c, unsigned level) const \
{ \
	debugmsg(#classname " print", LOGLEVEL_PRINT); \
	if (is_of_type(c, print_tree)) \
		inherited::print(c, level); \
	else \
		c.s << text; \
}

#define DEFAULT_PRINT_LATEX(classname, text, latex) \
void classname::print(const print_context & c, unsigned level) const \
{ \
	debugmsg(#classname " print", LOGLEVEL_PRINT); \
	if (is_of_type(c, print_tree)) \
		inherited::print(c, level); \
	else if (is_of_type(c, print_latex)) \
		c.s << latex; \
	else \
		c.s << text; \
}

} // namespace GiNaC

#endif // ndef __GINAC_UTILS_H__
