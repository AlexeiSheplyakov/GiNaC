/** @file ptr.h
 *
 *  Reference-counted pointer template. */

/*
 *  GiNaC Copyright (C) 1999-2003 Johannes Gutenberg University Mainz, Germany
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

#ifndef __GINAC_PTR_H__
#define __GINAC_PTR_H__

#include <functional>
#include <iosfwd>

#include "assertion.h"

namespace GiNaC {

/** Class of (intrusively) reference-counted pointers that support
 *  copy-on-write semantics.
 *
 *  Requirements for T:
 *    T::refcount member that supports ++refcount, --refcount, refcount = 1,
 *      refcount == 0 and refcount > 1
 *    T* T::duplicate() member function (only if makewriteable() is used) */
template <class T> class ptr {
	friend class std::less< ptr<T> >;

	// NB: This implementation of reference counting is not thread-safe.
	// The reference counter needs to be incremented/decremented atomically,
	// and makewritable() requires locking.

public:
    // no default ctor: a ptr is never unbound

	/** Bind ptr to newly created object, start reference counting. */
	ptr(T *t) : p(t) { GINAC_ASSERT(p); p->refcount = 1; }

	/** Bind ptr to existing reference-counted object. */
	explicit ptr(T &t) : p(&t) { ++p->refcount; }

	ptr(const ptr & other) : p(other.p) { ++p->refcount; }

	~ptr()
	{
		if (--p->refcount == 0)
			delete p;
	}

	ptr &operator=(const ptr & other)
	{
		// NB: must first increment other.p->refcount, since other might be *this.
		++other.p->refcount;
		if (--p->refcount == 0)
			delete p;
		p = other.p;
		return *this;
	}

	T &operator*() const { return *p; }
	T *operator->() const { return p; }

	friend inline T *get_pointer(const ptr & x) { return x.p; }

	/** Announce your intention to modify the object bound to this ptr.
	 *  This ensures that the object is not shared by any other ptrs. */
	void makewritable()
	{
		if (p->refcount > 1) {
			T *p2 = p->duplicate();
			p2->refcount = 1;
			--p->refcount;
			p = p2;
		}
	}

	/** Swap the bound object of this ptr with another ptr. */
	void swap(ptr & other)
	{
		T *t = p;
		p = other.p;
		other.p = t;
	}

	// ptr<>s are always supposed to be bound to a valid object, so we don't
	// provide support for "if (p)", "if (!p)", "if (p==0)" and "if (p!=0)".
	// We do, however, provide support for comparing ptr<>s with other ptr<>s
	// to different (probably derived) types and raw pointers.

	template <class U>
	bool operator==(const ptr<U> & rhs) const { return p == get_pointer(rhs); }

	template <class U>
	bool operator!=(const ptr<U> & rhs) const { return p != get_pointer(rhs); }

	template <class U>
	inline friend bool operator==(const ptr & lhs, const U * rhs) { return lhs.p == rhs; }

	template <class U>
	inline friend bool operator!=(const ptr & lhs, const U * rhs) { return lhs.p != rhs; }

	template <class U>
	inline friend bool operator==(const U * lhs, const ptr & rhs) { return lhs == rhs.p; }

	template <class U>
	inline friend bool operator!=(const U * lhs, const ptr & rhs) { return lhs != rhs.p; }

	inline friend std::ostream & operator<<(std::ostream & os, const ptr<T> & rhs)
	{
		os << rhs.p;
	}

private:
	T *p;
};

} // namespace GiNaC

namespace std {

/** Specialization of std::less for ptr<T> to enable ordering of ptr<T>
 *  objects (e.g. for the use as std::map keys). */
template <class T> struct less< GiNaC::ptr<T> >
 : public binary_function<GiNaC::ptr<T>, GiNaC::ptr<T>, bool> {
	bool operator()(const GiNaC::ptr<T> &lhs, const GiNaC::ptr<T> &rhs) const
	{
		return less<T*>()(lhs.p, rhs.p);
	}
};

} // namespace std

#endif // ndef __GINAC_UTILS_H__
