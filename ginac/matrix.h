/** @file matrix.h
 *
 *  Interface to symbolic matrices */

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

#ifndef __GINAC_MATRIX_H__
#define __GINAC_MATRIX_H__

#include <vector>
#include <ginac/basic.h>
#include <ginac/ex.h>

#ifndef NO_GINAC_NAMESPACE
namespace GiNaC {
#endif // ndef NO_GINAC_NAMESPACE

/** Symbolic matrices. */
class matrix : public basic
{
// friends
    friend ex determinant_numeric(const matrix & m);
    friend ex determinant_symbolic_perm(const matrix & m);
    friend ex determinant_symbolic_minor(const matrix & m);
// member functions

    // default constructor, destructor, copy constructor, assignment operator
    // and helpers:
public:
    matrix();
    ~matrix();
    matrix(matrix const & other);
    matrix const & operator=(matrix const & other);
protected:
    void copy(matrix const & other);
    void destroy(bool call_parent);

    // other constructors
public:
    matrix(int r, int c);
    matrix(int r, int c, exvector const & m2);
   
    // functions overriding virtual functions from bases classes
public:
    basic * duplicate() const;
    void print(ostream & os, unsigned upper_precedence=0) const;
    void printraw(ostream & os) const;
    int nops() const;
    ex & let_op(int const i);
    ex expand(unsigned options=0) const;
    bool has(ex const & other) const;
    ex eval(int level=0) const;
    ex evalf(int level=0) const;
    // ex subs(lst const & ls, lst const & lr) const;
protected:
    int compare_same_type(basic const & other) const;
    unsigned return_type(void) const { return return_types::noncommutative; };
    // new virtual functions which can be overridden by derived classes
    // (none)
    
    // non-virtual functions in this class
public:
    int rows() const            //! get number of rows.
        { return row; }
    int cols() const            //! get number of columns.
        { return col; }
    matrix add(matrix const & other) const;
    matrix sub(matrix const & other) const;
    matrix mul(matrix const & other) const;
    ex const & operator() (int ro, int co) const;
    matrix & set(int ro, int co, ex value);
    matrix transpose(void) const;
    ex determinant(bool normalized=true) const;
    ex trace(void) const;
    ex charpoly(ex const & lambda) const;
    matrix inverse(void) const;
    matrix fraction_free_elim(matrix const & vars, matrix const & v) const;
    matrix solve(matrix const & v) const;
protected:
    int pivot(int ro);
    void ffe_swap(int r1, int c1, int r2 ,int c2);
    void ffe_set(int r, int c, ex e);
    ex ffe_get(int r, int c) const;
    
// member variables
protected:
    int row;                    /**< number of rows      */
    int col;                    /**< number of columns   */
    exvector m;               /**< representation (cols indexed first) */
    static unsigned precedence;
};

// global constants
extern const matrix some_matrix;
extern type_info const & typeid_matrix;

// wrapper functions around member functions

inline int nops(matrix const & m)
{ return m.nops(); }

inline ex expand(matrix const & m, unsigned options=0)
{ return m.expand(options); }

inline bool has(matrix const & m, ex const & other)
{ return m.has(other); }

inline ex eval(matrix const & m, int level=0)
{ return m.eval(level); }

inline ex evalf(matrix const & m, int level=0)
{ return m.evalf(level); }

inline int rows(matrix const & m)
{ return m.rows(); }

inline int cols(matrix const & m)
{ return m.cols(); }

inline matrix transpose(matrix const & m)
{ return m.transpose(); }

inline ex determinant(matrix const & m, bool normalized=true)
{ return m.determinant(normalized); }

inline ex trace(matrix const & m)
{ return m.trace(); }

inline ex charpoly(matrix const & m, ex const & lambda)
{ return m.charpoly(lambda); }

inline matrix inverse(matrix const & m)
{ return m.inverse(); }

// utility functions
inline const matrix &ex_to_matrix(const ex &e)
{
	return static_cast<const matrix &>(*e.bp);
}

#ifndef NO_GINAC_NAMESPACE
} // namespace GiNaC
#endif // ndef NO_GINAC_NAMESPACE

#endif // ndef __GINAC_MATRIX_H__
