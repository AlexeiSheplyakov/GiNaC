/** @file utils.h
 *
 *  Interface to several small and furry utilities. */

/*
 *  GiNaC Copyright (C) 1999 Johannes Gutenberg University Mainz, Germany
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
#include "config.h"

namespace GiNaC {

template<class T>
string ToString(T const & t)
{
    char buf[256];
    ostrstream(buf,sizeof(buf)) << t << ends;
    return buf;
}

unsigned log2(unsigned n);

int compare_pointers(void const * a, void const * b);

// modified from stl_algo.h: always do com(*first1,*first2) instead of comp(*first2,*first1)
template <class InputIterator1, class InputIterator2, class OutputIterator,
          class Compare>
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

} // namespace GiNaC

#endif // ndef __GINAC_UTILS_H__
