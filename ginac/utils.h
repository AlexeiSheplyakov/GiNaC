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

#include <strstream>
#include <string>
#include <stdexcept>
#include "config.h"
#include "assertion.h"

#ifndef NO_NAMESPACE_GINAC
namespace GiNaC {
#endif // ndef NO_NAMESPACE_GINAC

// This should be obsoleted once <sstream> is widely available.
template<class T>
std::string ToString(const T & t)
{
	char buf[256];
	std::ostrstream(buf,sizeof(buf)) << t << std::ends;
	return buf;
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

int compare_pointers(const void * a, const void * b);

/** Rotate lower 31 bits of unsigned value by one bit to the left
 *  (upper bits get cleared). */
inline unsigned rotate_left_31(unsigned n)
{
	// clear highest bit and shift 1 bit to the left
	n=(n & 0x7FFFFFFFU) << 1;

	// overflow? clear highest bit and set lowest bit
	if (n & 0x80000000U) {
		n=(n & 0x7FFFFFFFU) | 0x00000001U;
	}
	GINAC_ASSERT(n<0x80000000U);

	return n;
}

/** Golden ratio hash function. */
inline unsigned golden_ratio_hash(unsigned n)
{
	// This function requires arithmetic with at least 64 significant bits
#if SIZEOF_LONG_DOUBLE > 8
	// If "long double" is bigger than 64 bits, we assume that the mantissa
	// has at least 64 bits. This is not guaranteed but it's a good guess.
	const static long double golden_ratio = .618033988749894848204586834370;
	long double m = golden_ratio * n;
	return unsigned((m - int(m)) * 0x80000000);
#elif SIZEOF_LONG >= 8
	// "long" has 64 bits, so we prefer it because it might be more efficient
	// than "long long"
	unsigned long l = n * 0x4f1bbcddL;
	return (l & 0x7fffffffU) ^ (l >> 32);
#elif SIZEOF_LONG_LONG >= 8
	// This requires ´long long´ (or an equivalent 64 bit type)---which is,
	// unfortunately, not ANSI-compliant:
	unsigned long long l = n * 0x4f1bbcddLL;
	return (l & 0x7fffffffU) ^ (l >> 32);
#else
#error "No 64 bit data type. You lose."
#endif
}

// modified from stl_algo.h: always do com(*first1,*first2) instead of comp(*first2,*first1)
template <class InputIterator1, class InputIterator2, class OutputIterator, class Compare>
OutputIterator mymerge(InputIterator1 first1, InputIterator1 last1,
                       InputIterator2 first2, InputIterator2 last2,
                       OutputIterator result, Compare comp) {
	while (first1 != last1 && first2 != last2) {
		if (comp(*first1, *first2)) {
			*result = *first1;
			++first1;
		}
		else {
			*result = *first2;
			++first2;
		}
		++result;
	}
	return copy(first2, last2, copy(first1, last1, result));
}

// like merge(), but three lists with *last2<*first3
template <class InputIterator1, class InputIterator2, class InputIterator3,
          class OutputIterator, class Compare>
OutputIterator mymerge3(InputIterator1 first1, InputIterator1 last1,
                        InputIterator2 first2, InputIterator2 last2,
                        InputIterator3 first3, InputIterator3 last3,
                        OutputIterator result, Compare comp) {
	while (first1 != last1 && first2 != last2) {
		if (comp(*first1, *first2)) {
			*result = *first1;
			++first1;
		}
		else {
			*result = *first2;
			++first2;
		}
		++result;
	}
	
	if (first1==last1) {
		// list1 empty, copy rest of list2, then list3
		return copy(first3, last3, copy(first2, last2, result));
	} else {
		// list2 empty, merge rest of list1 with list3
		return mymerge(first1,last1,first3,last3,result,comp);
	}
}

// Compute the sign of a permutation of a vector of things.
template <typename T>
int permutation_sign(std::vector<T> s)
{
	if (s.size() < 2)
		return 0;
	int sigma = 1;
	for (typename std::vector<T>::iterator i=s.begin(); i!=s.end()-1; ++i) {
		for (typename std::vector<T>::iterator j=i+1; j!=s.end(); ++j) {
			if (*i == *j)
				return 0;
			if (*i > *j) {
				iter_swap(i,j);
				sigma = -sigma;
			}
		}
	}
	return sigma;
}

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

#ifndef NO_NAMESPACE_GINAC
} // namespace GiNaC
#endif // ndef NO_NAMESPACE_GINAC

#endif // ndef __GINAC_UTILS_H__
