/** @file exam_structure.cpp
 *
 *  Small test for the structure<> template. */

/*
 *  GiNaC Copyright (C) 1999-2005 Johannes Gutenberg University Mainz, Germany
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

#include "exams.h"


struct sprod_s {
	ex left, right;

	sprod_s() {}
	sprod_s(const ex & l, const ex & r) : left(l), right(r) {}
};

inline bool operator==(const sprod_s & lhs, const sprod_s & rhs)
{
	return lhs.left.is_equal(rhs.left) && lhs.right.is_equal(rhs.right);
}
     
inline bool operator<(const sprod_s & lhs, const sprod_s & rhs)
{
	return lhs.left.compare(rhs.left) < 0 ? true : lhs.right.compare(rhs.right) < 0;
}

typedef structure<sprod_s, compare_std_less> sprod;

inline ex make_sprod(const ex & l, const ex & r)
{
	return sprod(sprod_s(l, r));
}

void sprod::print(const print_context & c, unsigned level) const
{
	const sprod_s & sp = get_struct();
	c.s << "<" << sp.left << "|" << sp.right << ">";
}

ex sprod::eval(int level) const
{
	// symmetric scalar product
	const sprod_s & sp = get_struct();
	if (sp.left.compare(sp.right) <= 0)
		return hold();
	else
		return make_sprod(sp.right, sp.left);
}

unsigned exam_structure()
{
	unsigned result = 0;

	cout << "examining structure template" << flush;
	clog << "----------structure template:" << endl;

	symbol x("x"), y("y");
	ex e;

	e = make_sprod(x, y) - make_sprod(y, x);
	if (!e.is_zero()) {
		clog << "<x|y>-<y|x> erroneously returned " << e << " instead of 0" << endl;
		++result;
	}

	cout << '.' << flush;

	e = make_sprod(x, x) - make_sprod(y, y);
	if (e.is_zero()) {
		clog << "<x|x>-<y|y> erroneously returned 0" << endl;
		++result;
	}

	cout << '.' << flush;

	if (!result) {
		cout << " passed " << endl;
		clog << "(no output)" << endl;
	} else {
		cout << " failed " << endl;
	}

	return result;
}
