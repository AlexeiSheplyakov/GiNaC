/** @file utils.h
 *
 *  Interface to several small and furry utilities. */

#ifndef _UTILS_H_
#define _UTILS_H_

#include <strstream>
#include <string>
#include "config.h"

template<class T>
string ToString(T const & t)
{
    char buf[256];
    ostrstream(buf,sizeof(buf)) << t << ends;
    return buf;
}

unsigned log2(unsigned n);

int compare_pointers(void const * a, void const * b);

#define DYNCONSTCAST(FINALTYPE,BASICTYPE,EXPRESSION) \
    dynamic_cast<FINALTYPE>(const_cast<BASICTYPE>(EXPRESSION))

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

#endif // ndef _UTILS_H_
