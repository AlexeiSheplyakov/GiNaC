/** @file matrix.h
 *
 *  Interface to symbolic matrices */

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

#ifndef __GINAC_MATRIX_H__
#define __GINAC_MATRIX_H__

#include <vector>
#include "basic.h"
#include "ex.h"

namespace GiNaC {

/** Symbolic matrices. */
class matrix : public basic
{
	GINAC_DECLARE_REGISTERED_CLASS(matrix, basic)
	
	// other ctors
public:
	matrix(unsigned r, unsigned c);
	matrix(unsigned r, unsigned c, const exvector & m2);
	matrix(unsigned r, unsigned c, const lst & l);
	
	// functions overriding virtual functions from base classes
public:
	void print(const print_context & c, unsigned level = 0) const;
	unsigned nops() const;
	ex op(int i) const;
	ex & let_op(int i);
	ex eval(int level=0) const;
	ex evalm(void) const {return *this;}
	ex subs(const lst & ls, const lst & lr, bool no_pattern = false) const;
	ex eval_indexed(const basic & i) const;
	ex add_indexed(const ex & self, const ex & other) const;
	ex scalar_mul_indexed(const ex & self, const numeric & other) const;
	bool contract_with(exvector::iterator self, exvector::iterator other, exvector & v) const;

protected:
	bool match_same_type(const basic & other) const;
	unsigned return_type(void) const { return return_types::noncommutative; };
	
	// non-virtual functions in this class
public:
	unsigned rows(void) const        /// Get number of rows.
		{ return row; }
	unsigned cols(void) const        /// Get number of columns.
		{ return col; }
	matrix add(const matrix & other) const;
	matrix sub(const matrix & other) const;
	matrix mul(const matrix & other) const;
	matrix mul(const numeric & other) const;
	matrix mul_scalar(const ex & other) const;
	matrix pow(const ex & expn) const;
	const ex & operator() (unsigned ro, unsigned co) const;
	ex & operator() (unsigned ro, unsigned co);
	matrix & set(unsigned ro, unsigned co, const ex & value) { (*this)(ro, co) = value; return *this; }
	matrix transpose(void) const;
	ex determinant(unsigned algo = determinant_algo::automatic) const;
	ex trace(void) const;
	ex charpoly(const symbol & lambda) const;
	matrix inverse(void) const;
	matrix solve(const matrix & vars, const matrix & rhs,
	             unsigned algo = solve_algo::automatic) const;
protected:
	ex determinant_minor(void) const;
	int gauss_elimination(const bool det = false);
	int division_free_elimination(const bool det = false);
	int fraction_free_elimination(const bool det = false);
	int pivot(unsigned ro, unsigned co, bool symbolic = true);
	
// member variables
protected:
	unsigned row;             ///< number of rows
	unsigned col;             ///< number of columns
	exvector m;               ///< representation (cols indexed first)
};


// wrapper functions around member functions

inline unsigned nops(const matrix & m)
{ return m.nops(); }

inline ex expand(const matrix & m, unsigned options = 0)
{ return m.expand(options); }

inline ex eval(const matrix & m, int level = 0)
{ return m.eval(level); }

inline ex evalf(const matrix & m, int level = 0)
{ return m.evalf(level); }

inline unsigned rows(const matrix & m)
{ return m.rows(); }

inline unsigned cols(const matrix & m)
{ return m.cols(); }

inline matrix transpose(const matrix & m)
{ return m.transpose(); }

inline ex determinant(const matrix & m, unsigned options = determinant_algo::automatic)
{ return m.determinant(options); }

inline ex trace(const matrix & m)
{ return m.trace(); }

inline ex charpoly(const matrix & m, const symbol & lambda)
{ return m.charpoly(lambda); }

inline matrix inverse(const matrix & m)
{ return m.inverse(); }

// utility functions

/** Specialization of is_exactly_a<matrix>(obj) for matrix objects. */
template<> inline bool is_exactly_a<matrix>(const basic & obj)
{
	return obj.tinfo()==TINFO_matrix;
}

/** Convert list of lists to matrix. */
extern ex lst_to_matrix(const lst & l);

/** Convert list of diagonal elements to matrix. */
extern ex diag_matrix(const lst & l);

/** Create a r times c unit matrix. */
extern ex unit_matrix(unsigned r, unsigned c);

/** Create a x times x unit matrix. */
inline ex unit_matrix(unsigned x)
{ return unit_matrix(x, x); }

} // namespace GiNaC

#endif // ndef __GINAC_MATRIX_H__
