/** @file utils.h
 *
 *  Interface to several small and furry utilities needed within GiNaC but not
 *  of any interest to the user of the library. */

/*
 *  GiNaC Copyright (C) 1999-2002 Johannes Gutenberg University Mainz, Germany
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
#include "assertion.h"

namespace GiNaC {

/** Exception class thrown by classes which provide their own series expansion
 *  to signal that ordinary Taylor expansion is safe. */
class do_taylor {};

/** Exception class thrown by functions to signal unimplemented functionality
 *  so the expression may just be .hold() */
class dunno {};

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

template <class It, class Cmp, class Swap>
int permutation_sign(It first, It last, Cmp comp, Swap swapit)
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
				swapit(*other, *i);
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
				swapit(*i, *other);
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
template <class It, class Cmp, class Swap>
void shaker_sort(It first, It last, Cmp comp, Swap swapit)
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
				swapit(*other, *i);
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
				swapit(*i, *other);
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
template <class It, class Swap>
void cyclic_permutation(It first, It last, It new_first, Swap swapit)
{
	unsigned num = last - first;
again:
	if (first == new_first || num < 2)
		return;

	unsigned num1 = new_first - first, num2 = last - new_first;
	if (num1 >= num2) {
		It a = first, b = new_first;
		while (b != last) {
			swapit(*a, *b);
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
			swapit(*a, *b);
		} while (a != first);
		last -= num1;
		num = num2;
		goto again;
	}
}


// Collection of `construct on first use' wrappers for safely avoiding
// internal object replication without running into the `static
// initialization order fiasco'.  This chest of numbers helps speed up
// the library but should not be used outside it since it is
// potentially confusing.

class numeric;
class ex;

extern const numeric *_num_120_p;
extern const numeric &_num_120;
extern const ex _ex_120;
extern const numeric *_num_60_p;
extern const numeric &_num_60;
extern const ex _ex_60;
extern const numeric *_num_48_p;
extern const numeric &_num_48;
extern const ex _ex_48;
extern const numeric *_num_30_p;
extern const numeric &_num_30;
extern const ex _ex_30;
extern const numeric *_num_25_p;
extern const numeric &_num_25;
extern const ex _ex_25;
extern const numeric *_num_24_p;
extern const numeric &_num_24;
extern const ex _ex_24;
extern const numeric *_num_20_p;
extern const numeric &_num_20;
extern const ex _ex_20;
extern const numeric *_num_18_p;
extern const numeric &_num_18;
extern const ex _ex_18;
extern const numeric *_num_15_p;
extern const numeric &_num_15;
extern const ex _ex_15;
extern const numeric *_num_12_p;
extern const numeric &_num_12;
extern const ex _ex_12;
extern const numeric *_num_11_p;
extern const numeric &_num_11;
extern const ex _ex_11;
extern const numeric *_num_10_p;
extern const numeric &_num_10;
extern const ex _ex_10;
extern const numeric *_num_9_p;
extern const numeric &_num_9;
extern const ex _ex_9;
extern const numeric *_num_8_p;
extern const numeric &_num_8;
extern const ex _ex_8;
extern const numeric *_num_7_p;
extern const numeric &_num_7;
extern const ex _ex_7;
extern const numeric *_num_6_p;
extern const numeric &_num_6;
extern const ex _ex_6;
extern const numeric *_num_5_p;
extern const numeric &_num_5;
extern const ex _ex_5;
extern const numeric *_num_4_p;
extern const numeric &_num_4;
extern const ex _ex_4;
extern const numeric *_num_3_p;
extern const numeric &_num_3;
extern const ex _ex_3;
extern const numeric *_num_2_p;
extern const numeric &_num_2;
extern const ex _ex_2;
extern const numeric *_num_1_p;
extern const numeric &_num_1;
extern const ex _ex_1;
extern const numeric *_num_1_2_p;
extern const numeric &_num_1_2;
extern const ex _ex_1_2;
extern const numeric *_num_1_3_p;
extern const numeric &_num_1_3;
extern const ex _ex_1_3;
extern const numeric *_num_1_4_p;
extern const numeric &_num_1_4;
extern const ex _ex_1_4;
extern const numeric *_num0_p;
extern const numeric &_num0;
extern const ex _ex0;
extern const numeric *_num1_4_p;
extern const numeric &_num1_4;
extern const ex _ex1_4;
extern const numeric *_num1_3_p;
extern const numeric &_num1_3;
extern const ex _ex1_3;
extern const numeric *_num1_2_p;
extern const numeric &_num1_2;
extern const ex _ex1_2;
extern const numeric *_num1_p;
extern const numeric &_num1;
extern const ex _ex1;
extern const numeric *_num2_p;
extern const numeric &_num2;
extern const ex _ex2;
extern const numeric *_num3_p;
extern const numeric &_num3;
extern const ex _ex3;
extern const numeric *_num4_p;
extern const numeric &_num4;
extern const ex _ex4;
extern const numeric *_num5_p;
extern const numeric &_num5;
extern const ex _ex5;
extern const numeric *_num6_p;
extern const numeric &_num6;
extern const ex _ex6;
extern const numeric *_num7_p;
extern const numeric &_num7;
extern const ex _ex7;
extern const numeric *_num8_p;
extern const numeric &_num8;
extern const ex _ex8;
extern const numeric *_num9_p;
extern const numeric &_num9;
extern const ex _ex9;
extern const numeric *_num10_p;
extern const numeric &_num10;
extern const ex _ex10;
extern const numeric *_num11_p;
extern const numeric &_num11;
extern const ex _ex11;
extern const numeric *_num12_p;
extern const numeric &_num12;
extern const ex _ex12;
extern const numeric *_num15_p;
extern const numeric &_num15;
extern const ex _ex15;
extern const numeric *_num18_p;
extern const numeric &_num18;
extern const ex _ex18;
extern const numeric *_num20_p;
extern const numeric &_num20;
extern const ex _ex20;
extern const numeric *_num24_p;
extern const numeric &_num24;
extern const ex _ex24;
extern const numeric *_num25_p;
extern const numeric &_num25;
extern const ex _ex25;
extern const numeric *_num30_p;
extern const numeric &_num30;
extern const ex _ex30;
extern const numeric *_num48_p;
extern const numeric &_num48;
extern const ex _ex48;
extern const numeric *_num60_p;
extern const numeric &_num60;
extern const ex _ex60;
extern const numeric *_num120_p;
extern const numeric &_num120;
extern const ex _ex120;


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
classname::classname() : inherited(TINFO_##classname) {} \
DEFAULT_COPY(classname) \
DEFAULT_DESTROY(classname)

#define DEFAULT_UNARCHIVE(classname) \
ex classname::unarchive(const archive_node &n, const lst &sym_lst) \
{ \
	return (new classname(n, sym_lst))->setflag(status_flags::dynallocated); \
}

#define DEFAULT_ARCHIVING(classname) \
classname::classname(const archive_node &n, const lst &sym_lst) : inherited(n, sym_lst) {} \
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
	if (is_a<print_tree>(c)) \
		inherited::print(c, level); \
	else \
		c.s << text; \
}

#define DEFAULT_PRINT_LATEX(classname, text, latex) \
void classname::print(const print_context & c, unsigned level) const \
{ \
	if (is_a<print_tree>(c)) \
		inherited::print(c, level); \
	else if (is_a<print_latex>(c)) \
		c.s << latex; \
	else \
		c.s << text; \
}

// Obsolete convenience macros.  TO BE PHASED OUT SOON!
// Use the inlined template functions in basic.h instead.  (FIXME: remove them)

#define is_of_type(OBJ,TYPE) \
	(dynamic_cast<const TYPE *>(&OBJ)!=0)

#define is_exactly_of_type(OBJ,TYPE) \
	((OBJ).tinfo()==GiNaC::TINFO_##TYPE)

#define is_ex_of_type(OBJ,TYPE) \
	(dynamic_cast<const TYPE *>((OBJ).bp)!=0)

#define is_ex_exactly_of_type(OBJ,TYPE) \
	((*(OBJ).bp).tinfo()==GiNaC::TINFO_##TYPE)

} // namespace GiNaC


#endif // ndef __GINAC_UTILS_H__
