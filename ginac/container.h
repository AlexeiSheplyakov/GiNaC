/** @file container.h
 *
 *  Wrapper template for making GiNaC classes out of STL containers. */

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

#ifndef __GINAC_CONTAINER_H__
#define __GINAC_CONTAINER_H__

#include <iterator>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <list>

#include "ex.h"
#include "print.h"
#include "archive.h"
#include "assertion.h"

namespace GiNaC {


/** Helper template for encapsulating the reserve() mechanics of STL containers. */
template <template <class> class C>
class container_storage {
protected:
	typedef C<ex> STLT;

	container_storage() {}
	container_storage(size_t n, const ex & e) : seq(n, e) {}

	template <class In>
	container_storage(In b, In e) : seq(b, e) {}

	void reserve(size_t) {}
	static void reserve(STLT &, size_t) {}

	STLT seq;

	// disallow destruction of container through a container_storage*
protected:
	~container_storage() {}
};

template <>
inline void container_storage<std::vector>::reserve(size_t n) { seq.reserve(n); }

template <>
inline void container_storage<std::vector>::reserve(std::vector<ex> & v, size_t n) { v.reserve(n); }


/** Wrapper template for making GiNaC classes out of STL containers. */
template <template <class> class C>
class container : public basic, public container_storage<C> {
	GINAC_DECLARE_REGISTERED_CLASS(container, basic)

	typedef typename container_storage<C>::STLT STLT;

public:
	typedef typename STLT::const_iterator const_iterator;
	typedef typename STLT::const_reverse_iterator const_reverse_iterator;

protected:
	// helpers
	static unsigned get_tinfo() { return TINFO_fail; }
	static char get_open_delim() { return '('; }
	static char get_close_delim() { return ')'; }

	// constructors
public:
	container(STLT const & s, bool discardable = false) : inherited(get_tinfo())
	{
		if (discardable)
			seq.swap(const_cast<STLT &>(s));
		else
			seq = s;
	}

	explicit container(STLT * vp) : inherited(get_tinfo())
	{
		GINAC_ASSERT(vp);
		seq.swap(*vp);
		delete vp;
	}

	container(exvector::const_iterator b, exvector::const_iterator e)
	 : inherited(get_tinfo()), container_storage<C>(b, e) {}

	explicit container(const ex & p1)
	 : inherited(get_tinfo()), container_storage<C>(1, p1) {}

	container(const ex & p1, const ex & p2) : inherited(get_tinfo())
	{
		reserve(seq, 2);
		seq.push_back(p1); seq.push_back(p2);
	}

	container(const ex & p1, const ex & p2, const ex & p3) : inherited(get_tinfo())
	{
		reserve(seq, 3);
		seq.push_back(p1); seq.push_back(p2); seq.push_back(p3);
	}

	container(const ex & p1, const ex & p2, const ex & p3,
	                   const ex & p4) : inherited(get_tinfo())
	{
		reserve(seq, 4);
		seq.push_back(p1); seq.push_back(p2); seq.push_back(p3);
		seq.push_back(p4);
	}

	container(const ex & p1, const ex & p2, const ex & p3,
	          const ex & p4, const ex & p5) : inherited(get_tinfo())
	{
		reserve(seq, 5);
		seq.push_back(p1); seq.push_back(p2); seq.push_back(p3);
		seq.push_back(p4); seq.push_back(p5);
	}

	container(const ex & p1, const ex & p2, const ex & p3,
	          const ex & p4, const ex & p5, const ex & p6) : inherited(get_tinfo())
	{
		reserve(seq, 6);
		seq.push_back(p1); seq.push_back(p2); seq.push_back(p3);
		seq.push_back(p4); seq.push_back(p5); seq.push_back(p6);
	}

	container(const ex & p1, const ex & p2, const ex & p3,
	          const ex & p4, const ex & p5, const ex & p6,
	          const ex & p7) : inherited(get_tinfo())
	{
		reserve(seq, 7);
		seq.push_back(p1); seq.push_back(p2); seq.push_back(p3);
		seq.push_back(p4); seq.push_back(p5); seq.push_back(p6);
		seq.push_back(p7);
	}

	container(const ex & p1, const ex & p2, const ex & p3,
	          const ex & p4, const ex & p5, const ex & p6,
	          const ex & p7, const ex & p8) : inherited(get_tinfo())
	{
		reserve(seq, 8);
		seq.push_back(p1); seq.push_back(p2); seq.push_back(p3);
		seq.push_back(p4); seq.push_back(p5); seq.push_back(p6);
		seq.push_back(p7); seq.push_back(p8);
	}

	container(const ex & p1, const ex & p2, const ex & p3,
	          const ex & p4, const ex & p5, const ex & p6,
	          const ex & p7, const ex & p8, const ex & p9) : inherited(get_tinfo())
	{
		reserve(seq, 9);
		seq.push_back(p1); seq.push_back(p2); seq.push_back(p3);
		seq.push_back(p4); seq.push_back(p5); seq.push_back(p6);
		seq.push_back(p7); seq.push_back(p8); seq.push_back(p9);
	}

	container(const ex & p1, const ex & p2, const ex & p3,
	          const ex & p4, const ex & p5, const ex & p6,
	          const ex & p7, const ex & p8, const ex & p9,
	          const ex & p10) : inherited(get_tinfo())
	{
		reserve(seq, 10);
		seq.push_back(p1); seq.push_back(p2); seq.push_back(p3);
		seq.push_back(p4); seq.push_back(p5); seq.push_back(p6);
		seq.push_back(p7); seq.push_back(p8); seq.push_back(p9);
		seq.push_back(p10);
	}

	container(const ex & p1, const ex & p2, const ex & p3,
	          const ex & p4, const ex & p5, const ex & p6,
	          const ex & p7, const ex & p8, const ex & p9,
	          const ex & p10, const ex & p11) : inherited(get_tinfo())
	{
		reserve(seq, 11);
		seq.push_back(p1); seq.push_back(p2); seq.push_back(p3);
		seq.push_back(p4); seq.push_back(p5); seq.push_back(p6);
		seq.push_back(p7); seq.push_back(p8); seq.push_back(p9);
		seq.push_back(p10); seq.push_back(p11);
	}

	container(const ex & p1, const ex & p2, const ex & p3,
	          const ex & p4, const ex & p5, const ex & p6,
	          const ex & p7, const ex & p8, const ex & p9,
	          const ex & p10, const ex & p11, const ex & p12) : inherited(get_tinfo())
	{
		reserve(seq, 12);
		seq.push_back(p1); seq.push_back(p2); seq.push_back(p3);
		seq.push_back(p4); seq.push_back(p5); seq.push_back(p6);
		seq.push_back(p7); seq.push_back(p8); seq.push_back(p9);
		seq.push_back(p10); seq.push_back(p11); seq.push_back(p12);
	}

	container(const ex & p1, const ex & p2, const ex & p3,
	          const ex & p4, const ex & p5, const ex & p6,
	          const ex & p7, const ex & p8, const ex & p9,
	          const ex & p10, const ex & p11, const ex & p12,
	          const ex & p13) : inherited(get_tinfo())
	{
		reserve(seq, 13);
		seq.push_back(p1); seq.push_back(p2); seq.push_back(p3);
		seq.push_back(p4); seq.push_back(p5); seq.push_back(p6);
		seq.push_back(p7); seq.push_back(p8); seq.push_back(p9);
		seq.push_back(p10); seq.push_back(p11); seq.push_back(p12);
		seq.push_back(p13);
	}

	container(const ex & p1, const ex & p2, const ex & p3,
	          const ex & p4, const ex & p5, const ex & p6,
	          const ex & p7, const ex & p8, const ex & p9,
	          const ex & p10, const ex & p11, const ex & p12,
	          const ex & p13, const ex & p14) : inherited(get_tinfo())
	{
		reserve(seq, 14);
		seq.push_back(p1); seq.push_back(p2); seq.push_back(p3);
		seq.push_back(p4); seq.push_back(p5); seq.push_back(p6);
		seq.push_back(p7); seq.push_back(p8); seq.push_back(p9);
		seq.push_back(p10); seq.push_back(p11); seq.push_back(p12);
		seq.push_back(p13); seq.push_back(p14);
	}

	container(const ex & p1, const ex & p2, const ex & p3,
	          const ex & p4, const ex & p5, const ex & p6,
	          const ex & p7, const ex & p8, const ex & p9,
	          const ex & p10, const ex & p11, const ex & p12,
	          const ex & p13, const ex & p14, const ex & p15) : inherited(get_tinfo())
	{
		reserve(seq, 15);
		seq.push_back(p1); seq.push_back(p2); seq.push_back(p3);
		seq.push_back(p4); seq.push_back(p5); seq.push_back(p6);
		seq.push_back(p7); seq.push_back(p8); seq.push_back(p9);
		seq.push_back(p10); seq.push_back(p11); seq.push_back(p12);
		seq.push_back(p13); seq.push_back(p14); seq.push_back(p15);
	}

	container(const ex & p1, const ex & p2, const ex & p3,
	          const ex & p4, const ex & p5, const ex & p6,
	          const ex & p7, const ex & p8, const ex & p9,
	          const ex & p10, const ex & p11, const ex & p12,
	          const ex & p13, const ex & p14, const ex & p15,
	          const ex & p16) : inherited(get_tinfo())
	{
		reserve(seq, 16);
		seq.push_back(p1); seq.push_back(p2); seq.push_back(p3);
		seq.push_back(p4); seq.push_back(p5); seq.push_back(p6);
		seq.push_back(p7); seq.push_back(p8); seq.push_back(p9);
		seq.push_back(p10); seq.push_back(p11); seq.push_back(p12);
		seq.push_back(p13); seq.push_back(p14); seq.push_back(p15);
		seq.push_back(p16);
	}

	// functions overriding virtual functions from base classes
public:
	void print(const print_context & c, unsigned level = 0) const;
	bool info(unsigned inf) const { return inherited::info(inf); }
	unsigned precedence() const { return 10; }
	size_t nops() const { return seq.size(); }
	ex op(size_t i) const;
	ex & let_op(size_t i);
	ex eval(int level = 0) const;
	ex subs(const exmap & m, unsigned options = 0) const;

protected:
	bool is_equal_same_type(const basic & other) const;

	// new virtual functions which can be overridden by derived classes
protected:
	/** Similar to duplicate(), but with a preset sequence. Must be
	 *  overridden by derived classes. */
	virtual ex thiscontainer(const STLT & v) const { return container(v); }

	/** Similar to duplicate(), but with a preset sequence (which gets
	 *  deleted). Must be overridden by derived classes. */
	virtual ex thiscontainer(STLT * vp) const { return container(vp); }

	virtual void printseq(const print_context & c, char openbracket, char delim,
	                      char closebracket, unsigned this_precedence,
	                      unsigned upper_precedence = 0) const;

	// non-virtual functions in this class
private:
	void sort_(std::random_access_iterator_tag)
	{
		std::sort(seq.begin(), seq.end(), ex_is_less());
	}

	void sort_(std::input_iterator_tag)
	{
		seq.sort(ex_is_less());
	}

	void unique_()
	{
		typename STLT::iterator p = std::unique(seq.begin(), seq.end(), ex_is_equal());
		seq.erase(p, seq.end());
	}

public:
	container & prepend(const ex & b);
	container & append(const ex & b);
	container & remove_first();
	container & remove_last();
	container & remove_all();
	container & sort();
	container & unique();

	const_iterator begin() const {return seq.begin();}
	const_iterator end() const {return seq.end();}
	const_reverse_iterator rbegin() const {return seq.rbegin();}
	const_reverse_iterator rend() const {return seq.rend();}

protected:
	STLT evalchildren(int level) const;
	STLT *subschildren(const exmap & m, unsigned options = 0) const;
};

/** Default constructor */
template <template <class> class C>
container<C>::container() : inherited(get_tinfo()) {}

/** Construct object from archive_node. */
template <template <class> class C>
container<C>::container(const archive_node &n, lst &sym_lst) : inherited(n, sym_lst)
{
	for (unsigned int i=0; true; i++) {
		ex e;
		if (n.find_ex("seq", e, sym_lst, i))
			seq.push_back(e);
		else
			break;
	}
}

/** Unarchive the object. */
template <template <class> class C>
ex container<C>::unarchive(const archive_node &n, lst &sym_lst)
{
	return (new container(n, sym_lst))->setflag(status_flags::dynallocated);
}

/** Archive the object. */
template <template <class> class C>
void container<C>::archive(archive_node &n) const
{
	inherited::archive(n);
	const_iterator i = seq.begin(), end = seq.end();
	while (i != end) {
		n.add_ex("seq", *i);
		++i;
	}
}

template <template <class> class C>
void container<C>::print(const print_context & c, unsigned level) const
{
	if (is_a<print_tree>(c)) {
		c.s << std::string(level, ' ') << class_name()
		    << std::hex << ", hash=0x" << hashvalue << ", flags=0x" << flags << std::dec
		    << ", nops=" << nops()
		    << std::endl;
		unsigned delta_indent = static_cast<const print_tree &>(c).delta_indent;
		const_iterator i = seq.begin(), end = seq.end();
		while (i != end) {
			i->print(c, level + delta_indent);
			++i;
		}
		c.s << std::string(level + delta_indent,' ') << "=====" << std::endl;
	} else if (is_a<print_python>(c)) {
		printseq(c, '[', ',', ']', precedence(), precedence()+1);
	} else if (is_a<print_python_repr>(c)) {
		c.s << class_name ();
		printseq(c, '(', ',', ')', precedence(), precedence()+1);
	} else {
		// always print brackets around seq, ignore upper_precedence
		printseq(c, get_open_delim(), ',', get_close_delim(), precedence(), precedence()+1);
	}
}

template <template <class> class C>
ex container<C>::op(size_t i) const
{
	GINAC_ASSERT(i < nops());

	const_iterator it = seq.begin();
	advance(it, i);
	return *it;
}

template <template <class> class C>
ex & container<C>::let_op(size_t i)
{
	GINAC_ASSERT(i < nops());

	ensure_if_modifiable();
	typename STLT::iterator it = seq.begin();
	advance(it, i);
	return *it;
}

template <template <class> class C>
ex container<C>::eval(int level) const
{
	if (level == 1)
		return hold();
	else
		return thiscontainer(evalchildren(level));
}

template <template <class> class C>
ex container<C>::subs(const exmap & m, unsigned options) const
{
	STLT *vp = subschildren(m, options);
	if (vp)
		return ex_to<basic>(thiscontainer(vp)).subs_one_level(m, options);
	else
		return subs_one_level(m, options);
}

/** Compare two containers of the same type. */
template <template <class> class C>
int container<C>::compare_same_type(const basic & other) const
{
	GINAC_ASSERT(is_a<container>(other));
	const container & o = static_cast<const container &>(other);

	const_iterator it1 = seq.begin(), it1end = seq.end(),
	               it2 = o.seq.begin(), it2end = o.seq.end();

	while (it1 != it1end && it2 != it2end) {
		int cmpval = it1->compare(*it2);
		if (cmpval)
			return cmpval;
		++it1; ++it2;
	}

	return (it1 == it1end) ? (it2 == it2end ? 0 : -1) : 1;
}

template <template <class> class C>
bool container<C>::is_equal_same_type(const basic & other) const
{
	GINAC_ASSERT(is_a<container>(other));
	const container & o = static_cast<const container &>(other);

	if (seq.size() != o.seq.size())
		return false;

	const_iterator it1 = seq.begin(), it1end = seq.end(), it2 = o.seq.begin();
	while (it1 != it1end) {
		if (!it1->is_equal(*it2))
			return false;
		++it1; ++it2;
	}

	return true;
}

/** Add element at front. */
template <template <class> class C>
container<C> & container<C>::prepend(const ex & b)
{
	ensure_if_modifiable();
	seq.push_front(b);
	return *this;
}

/** Add element at back. */
template <template <class> class C>
container<C> & container<C>::append(const ex & b)
{
	ensure_if_modifiable();
	seq.push_back(b);
	return *this;
}

/** Remove first element. */
template <template <class> class C>
container<C> & container<C>::remove_first()
{
	ensure_if_modifiable();
	seq.pop_front();
	return *this;
}

/** Remove last element. */
template <template <class> class C>
container<C> & container<C>::remove_last()
{
	ensure_if_modifiable();
	seq.pop_back();
	return *this;
}

/** Remove all elements. */
template <template <class> class C>
container<C> & container<C>::remove_all()
{
	ensure_if_modifiable();
	seq.clear();
	return *this;
}

/** Sort elements. */
template <template <class> class C>
container<C> & container<C>::sort()
{
	ensure_if_modifiable();
	sort_(std::iterator_traits<typename STLT::iterator>::iterator_category());
	return *this;
}

/** Specialization of container::unique_() for std::list. */
inline void container<std::list>::unique_()
{
	seq.unique(ex_is_equal());
}

/** Remove adjacent duplicate elements. */
template <template <class> class C>
container<C> & container<C>::unique()
{
	ensure_if_modifiable();
	unique_();
	return *this;
}

/** Print sequence of contained elements. */
template <template <class> class C>
void container<C>::printseq(const print_context & c, char openbracket, char delim,
                            char closebracket, unsigned this_precedence,
                            unsigned upper_precedence) const
{
	if (this_precedence <= upper_precedence)
		c.s << openbracket;

	if (!seq.empty()) {
		const_iterator it = seq.begin(), itend = seq.end();
		--itend;
		while (it != itend) {
			it->print(c, this_precedence);
			c.s << delim;
			++it;
		}
		it->print(c, this_precedence);
	}

	if (this_precedence <= upper_precedence)
		c.s << closebracket;
}

template <template <class> class C>
typename container<C>::STLT container<C>::evalchildren(int level) const
{
	if (level == 1)
		return seq;
	else if (level == -max_recursion_level)
		throw std::runtime_error("max recursion level reached");

	STLT s;
	reserve(s, seq.size());

	--level;
	const_iterator it = seq.begin(), itend = seq.end();
	while (it != itend) {
		s.push_back(it->eval(level));
		++it;
	}

	return s;
}

template <template <class> class C>
typename container<C>::STLT *container<C>::subschildren(const exmap & m, unsigned options) const
{
	// returns a NULL pointer if nothing had to be substituted
	// returns a pointer to a newly created epvector otherwise
	// (which has to be deleted somewhere else)

	const_iterator cit = seq.begin(), end = seq.end();
	while (cit != end) {
		const ex & subsed_ex = cit->subs(m, options);
		if (!are_ex_trivially_equal(*cit, subsed_ex)) {

			// copy first part of seq which hasn't changed
			STLT *s = new STLT(seq.begin(), cit);
			reserve(*s, seq.size());

			// insert changed element
			s->push_back(subsed_ex);
			++cit;

			// copy rest
			while (cit != end) {
				s->push_back(cit->subs(m, options));
				++cit;
			}

			return s;
		}

		++cit;
	}
	
	return 0; // nothing has changed
}

} // namespace GiNaC

#endif // ndef __GINAC_CONTAINER_H__
