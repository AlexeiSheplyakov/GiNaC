/** @file matrix.h
 *
 *  Interface to symbolic matrices */

#ifndef _MATRIX_H_
#define _MATRIX_H_

#include "basic.h"
#include "ex.h"
#include <vector>

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
    matrix(int r, int c, vector<ex> const & m2);
   
    // functions overriding virtual functions from bases classes
public:
    basic * duplicate() const;
    void printraw(ostream & os) const;
    void print(ostream & os, unsigned upper_precedence=0) const;
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
    vector<ex> m;               /**< representation (cols indexed first) */
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

// macros

#define ex_to_matrix(X) (static_cast<matrix const &>(*(X).bp))

#endif // ndef _MATRIX_H_
