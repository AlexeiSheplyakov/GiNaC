/** @file matrix.cpp
 *
 *  Implementation of symbolic matrices */

/*
 *  GiNaC Copyright (C) 1999-2000 Johannes Gutenberg University Mainz, Germany
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

#include <algorithm>
#include <stdexcept>

#include "matrix.h"
#include "archive.h"
#include "utils.h"
#include "debugmsg.h"

#ifndef NO_GINAC_NAMESPACE
namespace GiNaC {
#endif // ndef NO_GINAC_NAMESPACE

GINAC_IMPLEMENT_REGISTERED_CLASS(matrix, basic)

//////////
// default constructor, destructor, copy constructor, assignment operator
// and helpers:
//////////

// public

/** Default ctor.  Initializes to 1 x 1-dimensional zero-matrix. */
matrix::matrix()
    : inherited(TINFO_matrix), row(1), col(1)
{
    debugmsg("matrix default constructor",LOGLEVEL_CONSTRUCT);
    m.push_back(_ex0());
}

matrix::~matrix()
{
    debugmsg("matrix destructor",LOGLEVEL_DESTRUCT);
}

matrix::matrix(const matrix & other)
{
    debugmsg("matrix copy constructor",LOGLEVEL_CONSTRUCT);
    copy(other);
}

const matrix & matrix::operator=(const matrix & other)
{
    debugmsg("matrix operator=",LOGLEVEL_ASSIGNMENT);
    if (this != &other) {
        destroy(1);
        copy(other);
    }
    return *this;
}

// protected

void matrix::copy(const matrix & other)
{
    inherited::copy(other);
    row=other.row;
    col=other.col;
    m=other.m;  // use STL's vector copying
}

void matrix::destroy(bool call_parent)
{
    if (call_parent) inherited::destroy(call_parent);
}

//////////
// other constructors
//////////

// public

/** Very common ctor.  Initializes to r x c-dimensional zero-matrix.
 *
 *  @param r number of rows
 *  @param c number of cols */
matrix::matrix(unsigned r, unsigned c)
    : inherited(TINFO_matrix), row(r), col(c)
{
    debugmsg("matrix constructor from unsigned,unsigned",LOGLEVEL_CONSTRUCT);
    m.resize(r*c, _ex0());
}

// protected

/** Ctor from representation, for internal use only. */
matrix::matrix(unsigned r, unsigned c, const exvector & m2)
    : inherited(TINFO_matrix), row(r), col(c), m(m2)
{
    debugmsg("matrix constructor from unsigned,unsigned,exvector",LOGLEVEL_CONSTRUCT);
}

//////////
// archiving
//////////

/** Construct object from archive_node. */
matrix::matrix(const archive_node &n, const lst &sym_lst) : inherited(n, sym_lst)
{
    debugmsg("matrix constructor from archive_node", LOGLEVEL_CONSTRUCT);
    if (!(n.find_unsigned("row", row)) || !(n.find_unsigned("col", col)))
        throw (std::runtime_error("unknown matrix dimensions in archive"));
    m.reserve(row * col);
    for (unsigned int i=0; true; i++) {
        ex e;
        if (n.find_ex("m", e, sym_lst, i))
            m.push_back(e);
        else
            break;
    }
}

/** Unarchive the object. */
ex matrix::unarchive(const archive_node &n, const lst &sym_lst)
{
    return (new matrix(n, sym_lst))->setflag(status_flags::dynallocated);
}

/** Archive the object. */
void matrix::archive(archive_node &n) const
{
    inherited::archive(n);
    n.add_unsigned("row", row);
    n.add_unsigned("col", col);
    exvector::const_iterator i = m.begin(), iend = m.end();
    while (i != iend) {
        n.add_ex("m", *i);
        i++;
    }
}

//////////
// functions overriding virtual functions from bases classes
//////////

// public

basic * matrix::duplicate() const
{
    debugmsg("matrix duplicate",LOGLEVEL_DUPLICATE);
    return new matrix(*this);
}

void matrix::print(ostream & os, unsigned upper_precedence) const
{
    debugmsg("matrix print",LOGLEVEL_PRINT);
    os << "[[ ";
    for (unsigned r=0; r<row-1; ++r) {
        os << "[[";
        for (unsigned c=0; c<col-1; ++c) {
            os << m[r*col+c] << ",";
        }
        os << m[col*(r+1)-1] << "]], ";
    }
    os << "[[";
    for (unsigned c=0; c<col-1; ++c) {
        os << m[(row-1)*col+c] << ",";
    }
    os << m[row*col-1] << "]] ]]";
}

void matrix::printraw(ostream & os) const
{
    debugmsg("matrix printraw",LOGLEVEL_PRINT);
    os << "matrix(" << row << "," << col <<",";
    for (unsigned r=0; r<row-1; ++r) {
        os << "(";
        for (unsigned c=0; c<col-1; ++c) {
            os << m[r*col+c] << ",";
        }
        os << m[col*(r-1)-1] << "),";
    }
    os << "(";
    for (unsigned c=0; c<col-1; ++c) {
        os << m[(row-1)*col+c] << ",";
    }
    os << m[row*col-1] << "))";
}

/** nops is defined to be rows x columns. */
unsigned matrix::nops() const
{
    return row*col;
}

/** returns matrix entry at position (i/col, i%col). */
ex & matrix::let_op(int i)
{
    return m[i];
}

/** expands the elements of a matrix entry by entry. */
ex matrix::expand(unsigned options) const
{
    exvector tmp(row*col);
    for (unsigned i=0; i<row*col; ++i) {
        tmp[i]=m[i].expand(options);
    }
    return matrix(row, col, tmp);
}

/** Search ocurrences.  A matrix 'has' an expression if it is the expression
 *  itself or one of the elements 'has' it. */
bool matrix::has(const ex & other) const
{
    GINAC_ASSERT(other.bp!=0);
    
    // tautology: it is the expression itself
    if (is_equal(*other.bp)) return true;
    
    // search all the elements
    for (exvector::const_iterator r=m.begin(); r!=m.end(); ++r) {
        if ((*r).has(other)) return true;
    }
    return false;
}

/** evaluate matrix entry by entry. */
ex matrix::eval(int level) const
{
    debugmsg("matrix eval",LOGLEVEL_MEMBER_FUNCTION);
    
    // check if we have to do anything at all
    if ((level==1)&&(flags & status_flags::evaluated)) {
        return *this;
    }
    
    // emergency break
    if (level == -max_recursion_level) {
        throw (std::runtime_error("matrix::eval(): recursion limit exceeded"));
    }
    
    // eval() entry by entry
    exvector m2(row*col);
    --level;    
    for (unsigned r=0; r<row; ++r) {
        for (unsigned c=0; c<col; ++c) {
            m2[r*col+c] = m[r*col+c].eval(level);
        }
    }
    
    return (new matrix(row, col, m2))->setflag(status_flags::dynallocated |
                                               status_flags::evaluated );
}

/** evaluate matrix numerically entry by entry. */
ex matrix::evalf(int level) const
{
    debugmsg("matrix evalf",LOGLEVEL_MEMBER_FUNCTION);
        
    // check if we have to do anything at all
    if (level==1) {
        return *this;
    }
    
    // emergency break
    if (level == -max_recursion_level) {
        throw (std::runtime_error("matrix::evalf(): recursion limit exceeded"));
    }
    
    // evalf() entry by entry
    exvector m2(row*col);
    --level;
    for (unsigned r=0; r<row; ++r) {
        for (unsigned c=0; c<col; ++c) {
            m2[r*col+c] = m[r*col+c].evalf(level);
        }
    }
    return matrix(row, col, m2);
}

// protected

int matrix::compare_same_type(const basic & other) const
{
    GINAC_ASSERT(is_exactly_of_type(other, matrix));
    const matrix & o=static_cast<matrix &>(const_cast<basic &>(other));
    
    // compare number of rows
    if (row != o.rows()) {
        return row < o.rows() ? -1 : 1;
    }
    
    // compare number of columns
    if (col != o.cols()) {
        return col < o.cols() ? -1 : 1;
    }
    
    // equal number of rows and columns, compare individual elements
    int cmpval;
    for (unsigned r=0; r<row; ++r) {
        for (unsigned c=0; c<col; ++c) {
            cmpval=((*this)(r,c)).compare(o(r,c));
            if (cmpval!=0) return cmpval;
        }
    }
    // all elements are equal => matrices are equal;
    return 0;
}

//////////
// non-virtual functions in this class
//////////

// public

/** Sum of matrices.
 *
 *  @exception logic_error (incompatible matrices) */
matrix matrix::add(const matrix & other) const
{
    if (col != other.col || row != other.row) {
        throw (std::logic_error("matrix::add(): incompatible matrices"));
    }
    
    exvector sum(this->m);
    exvector::iterator i;
    exvector::const_iterator ci;
    for (i=sum.begin(), ci=other.m.begin();
         i!=sum.end();
         ++i, ++ci) {
        (*i) += (*ci);
    }
    return matrix(row,col,sum);
}

/** Difference of matrices.
 *
 *  @exception logic_error (incompatible matrices) */
matrix matrix::sub(const matrix & other) const
{
    if (col != other.col || row != other.row) {
        throw (std::logic_error("matrix::sub(): incompatible matrices"));
    }
    
    exvector dif(this->m);
    exvector::iterator i;
    exvector::const_iterator ci;
    for (i=dif.begin(), ci=other.m.begin();
         i!=dif.end();
         ++i, ++ci) {
        (*i) -= (*ci);
    }
    return matrix(row,col,dif);
}

/** Product of matrices.
 *
 *  @exception logic_error (incompatible matrices) */
matrix matrix::mul(const matrix & other) const
{
    if (col != other.row) {
        throw (std::logic_error("matrix::mul(): incompatible matrices"));
    }
    
    exvector prod(row*other.col);
    for (unsigned i=0; i<row; ++i) {
        for (unsigned j=0; j<other.col; ++j) {
            for (unsigned l=0; l<col; ++l) {
                prod[i*other.col+j] += m[i*col+l] * other.m[l*other.col+j];
            }
        }
    }
    return matrix(row, other.col, prod);
}

/** operator() to access elements.
 *
 *  @param ro row of element
 *  @param co column of element 
 *  @exception range_error (index out of range) */
const ex & matrix::operator() (unsigned ro, unsigned co) const
{
    if (ro<0 || ro>=row || co<0 || co>=col) {
        throw (std::range_error("matrix::operator(): index out of range"));
    }
    
    return m[ro*col+co];
}

/** Set individual elements manually.
 *
 *  @exception range_error (index out of range) */
matrix & matrix::set(unsigned ro, unsigned co, ex value)
{
    if (ro<0 || ro>=row || co<0 || co>=col) {
        throw (std::range_error("matrix::set(): index out of range"));
    }
    
    ensure_if_modifiable();
    m[ro*col+co]=value;
    return *this;
}

/** Transposed of an m x n matrix, producing a new n x m matrix object that
 *  represents the transposed. */
matrix matrix::transpose(void) const
{
    exvector trans(col*row);
    
    for (unsigned r=0; r<col; ++r) {
        for (unsigned c=0; c<row; ++c) {
            trans[r*row+c] = m[c*col+r];
        }
    }
    return matrix(col,row,trans);
}

/* Determiant of purely numeric matrix, using pivoting. This routine is only
 * called internally by matrix::determinant(). */
ex determinant_numeric(const matrix & M)
{
    GINAC_ASSERT(M.rows()==M.cols());  // cannot happen, just in case...
    matrix tmp(M);
    ex det=_ex1();
    ex piv;
    
    for (unsigned r1=0; r1<M.rows(); ++r1) {
        int indx = tmp.pivot(r1);
        if (indx == -1) {
            return _ex0();
        }
        if (indx != 0) {
            det *= _ex_1();
        }
        det = det * tmp.m[r1*M.cols()+r1];
        for (unsigned r2=r1+1; r2<M.rows(); ++r2) {
            piv = tmp.m[r2*M.cols()+r1] / tmp.m[r1*M.cols()+r1];
            for (unsigned c=r1+1; c<M.cols(); c++) {
                tmp.m[r2*M.cols()+c] -= piv * tmp.m[r1*M.cols()+c];
            }
        }
    }
    return det;
}

// Compute the sign of a permutation of a vector of things, used internally
// by determinant_symbolic_perm() where it is instantiated for int.
template <class T>
int permutation_sign(vector<T> s)
{
    if (s.size() < 2)
        return 0;
    int sigma=1;
    for (typename vector<T>::iterator i=s.begin(); i!=s.end()-1; ++i) {
        for (typename vector<T>::iterator j=i+1; j!=s.end(); ++j) {
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

/** Determinant built by application of the full permutation group. This
 *  routine is only called internally by matrix::determinant(). */
ex determinant_symbolic_perm(const matrix & M)
{
    GINAC_ASSERT(M.rows()==M.cols());  // cannot happen, just in case...
    
    if (M.rows()==1) {  // speed things up
        return M(0,0);
    }
    
    ex det;
    ex term;
    vector<unsigned> sigma(M.cols());
    for (unsigned i=0; i<M.cols(); ++i) sigma[i]=i;
    
    do {
        term = M(sigma[0],0);
        for (unsigned i=1; i<M.cols(); ++i) term *= M(sigma[i],i);
        det += permutation_sign(sigma)*term;
    } while (next_permutation(sigma.begin(), sigma.end()));
    
    return det;
}

/** Recursive determiant for small matrices having at least one symbolic entry.
 *  This algorithm is also known as Laplace-expansion. This routine is only
 *  called internally by matrix::determinant(). */
ex determinant_symbolic_minor(const matrix & M)
{
    GINAC_ASSERT(M.rows()==M.cols());  // cannot happen, just in case...
    
    if (M.rows()==1) {  // end of recursion
        return M(0,0);
    }
    if (M.rows()==2) {  // speed things up
        return (M(0,0)*M(1,1)-
                M(1,0)*M(0,1));
    }
    if (M.rows()==3) {  // speed things up even a little more
        return ((M(2,1)*M(0,2)-M(2,2)*M(0,1))*M(1,0)+
                (M(1,2)*M(0,1)-M(1,1)*M(0,2))*M(2,0)+
                (M(2,2)*M(1,1)-M(2,1)*M(1,2))*M(0,0));
    }
    
    ex det;
    matrix minorM(M.rows()-1,M.cols()-1);
    for (unsigned r1=0; r1<M.rows(); ++r1) {
        // assemble the minor matrix
        for (unsigned r=0; r<minorM.rows(); ++r) {
            for (unsigned c=0; c<minorM.cols(); ++c) {
                if (r<r1) {
                    minorM.set(r,c,M(r,c+1));
                } else {
                    minorM.set(r,c,M(r+1,c+1));
                }
            }
        }
        // recurse down
        if (r1%2) {
            det -= M(r1,0) * determinant_symbolic_minor(minorM);
        } else {
            det += M(r1,0) * determinant_symbolic_minor(minorM);
        }
    }
    return det;
}

/*  Leverrier algorithm for large matrices having at least one symbolic entry.
 *  This routine is only called internally by matrix::determinant(). The
 *  algorithm is deemed bad for symbolic matrices since it returns expressions
 *  that are very hard to canonicalize. */
/*ex determinant_symbolic_leverrier(const matrix & M)
 *{
 *    GINAC_ASSERT(M.rows()==M.cols());  // cannot happen, just in case...
 *    
 *    matrix B(M);
 *    matrix I(M.row, M.col);
 *    ex c=B.trace();
 *    for (unsigned i=1; i<M.row; ++i) {
 *        for (unsigned j=0; j<M.row; ++j)
 *            I.m[j*M.col+j] = c;
 *        B = M.mul(B.sub(I));
 *        c = B.trace()/ex(i+1);
 *    }
 *    if (M.row%2) {
 *        return c;
 *    } else {
 *        return -c;
 *    }
 *}*/

/** Determinant of square matrix.  This routine doesn't actually calculate the
 *  determinant, it only implements some heuristics about which algorithm to
 *  call.  When the parameter for normalization is explicitly turned off this
 *  method does not normalize its result at the end, which might imply that
 *  the symbolic 2x2 matrix [[a/(a-b),1],[b/(a-b),1]] is not immediatly
 *  recognized to be unity.  (This is Mathematica's default behaviour, it
 *  should be used with care.)
 *
 *  @param     normalized may be set to false if no normalization of the
 *             result is desired (i.e. to force Mathematica behavior, Maple
 *             does normalize the result).
 *  @return    the determinant as a new expression
 *  @exception logic_error (matrix not square) */
ex matrix::determinant(bool normalized) const
{
    if (row != col) {
        throw (std::logic_error("matrix::determinant(): matrix not square"));
    }

    // check, if there are non-numeric entries in the matrix:
    for (exvector::const_iterator r=m.begin(); r!=m.end(); ++r) {
        if (!(*r).info(info_flags::numeric)) {
            if (normalized) {
                return determinant_symbolic_minor(*this).normal();
            } else {
                return determinant_symbolic_perm(*this);
            }
        }
    }
    // if it turns out that all elements are numeric
    return determinant_numeric(*this);
}

/** Trace of a matrix.
 *
 *  @return    the sum of diagonal elements
 *  @exception logic_error (matrix not square) */
ex matrix::trace(void) const
{
    if (row != col) {
        throw (std::logic_error("matrix::trace(): matrix not square"));
    }
    
    ex tr;
    for (unsigned r=0; r<col; ++r) {
        tr += m[r*col+r];
    }
    return tr;
}

/** Characteristic Polynomial.  The characteristic polynomial of a matrix M is
 *  defined as the determiant of (M - lambda * 1) where 1 stands for the unit
 *  matrix of the same dimension as M.  This method returns the characteristic
 *  polynomial as a new expression.
 *
 *  @return    characteristic polynomial as new expression
 *  @exception logic_error (matrix not square)
 *  @see       matrix::determinant() */
ex matrix::charpoly(const ex & lambda) const
{
    if (row != col) {
        throw (std::logic_error("matrix::charpoly(): matrix not square"));
    }
    
    matrix M(*this);
    for (unsigned r=0; r<col; ++r) {
        M.m[r*col+r] -= lambda;
    }
    return (M.determinant());
}

/** Inverse of this matrix.
 *
 *  @return    the inverted matrix
 *  @exception logic_error (matrix not square)
 *  @exception runtime_error (singular matrix) */
matrix matrix::inverse(void) const
{
    if (row != col) {
        throw (std::logic_error("matrix::inverse(): matrix not square"));
    }
    
    matrix tmp(row,col);
    // set tmp to the unit matrix
    for (unsigned i=0; i<col; ++i) {
        tmp.m[i*col+i] = _ex1();
    }
    // create a copy of this matrix
    matrix cpy(*this);
    for (unsigned r1=0; r1<row; ++r1) {
        int indx = cpy.pivot(r1);
        if (indx == -1) {
            throw (std::runtime_error("matrix::inverse(): singular matrix"));
        }
        if (indx != 0) {  // swap rows r and indx of matrix tmp
            for (unsigned i=0; i<col; ++i) {
                tmp.m[r1*col+i].swap(tmp.m[indx*col+i]);
            }
        }
        ex a1 = cpy.m[r1*col+r1];
        for (unsigned c=0; c<col; ++c) {
            cpy.m[r1*col+c] /= a1;
            tmp.m[r1*col+c] /= a1;
        }
        for (unsigned r2=0; r2<row; ++r2) {
            if (r2 != r1) {
                ex a2 = cpy.m[r2*col+r1];
                for (unsigned c=0; c<col; ++c) {
                    cpy.m[r2*col+c] -= a2 * cpy.m[r1*col+c];
                    tmp.m[r2*col+c] -= a2 * tmp.m[r1*col+c];
                }
            }
        }
    }
    return tmp;
}

void matrix::ffe_swap(unsigned r1, unsigned c1, unsigned r2 ,unsigned c2)
{
    ensure_if_modifiable();
    
    ex tmp=ffe_get(r1,c1);
    ffe_set(r1,c1,ffe_get(r2,c2));
    ffe_set(r2,c2,tmp);
}

void matrix::ffe_set(unsigned r, unsigned c, ex e)
{
    set(r-1,c-1,e);
}

ex matrix::ffe_get(unsigned r, unsigned c) const
{
    return operator()(r-1,c-1);
}

/** Solve a set of equations for an m x n matrix by fraction-free Gaussian
 *  elimination. Based on algorithm 9.1 from 'Algorithms for Computer Algebra'
 *  by Keith O. Geddes et al.
 *
 *  @param vars n x p matrix
 *  @param rhs m x p matrix
 *  @exception logic_error (incompatible matrices)
 *  @exception runtime_error (singular matrix) */
matrix matrix::fraction_free_elim(const matrix & vars,
                                  const matrix & rhs) const
{
    if ((row != rhs.row) || (col != vars.row) || (rhs.col != vars.col)) {
        throw (std::logic_error("matrix::solve(): incompatible matrices"));
    }
    
    matrix a(*this); // make a copy of the matrix
    matrix b(rhs);     // make a copy of the rhs vector
    
    // given an m x n matrix a, reduce it to upper echelon form
    unsigned m=a.row;
    unsigned n=a.col;
    int sign=1;
    ex divisor=1;
    unsigned r=1;
    
    // eliminate below row r, with pivot in column k
    for (unsigned k=1; (k<=n)&&(r<=m); ++k) {
        // find a nonzero pivot
        unsigned p;
        for (p=r; (p<=m)&&(a.ffe_get(p,k).is_equal(_ex0())); ++p) {}
        // pivot is in row p
        if (p<=m) {
            if (p!=r) {
                // switch rows p and r
                for (unsigned j=k; j<=n; ++j) {
                    a.ffe_swap(p,j,r,j);
                }
                b.ffe_swap(p,1,r,1);
                // keep track of sign changes due to row exchange
                sign=-sign;
            }
            for (unsigned i=r+1; i<=m; ++i) {
                for (unsigned j=k+1; j<=n; ++j) {
                    a.ffe_set(i,j,(a.ffe_get(r,k)*a.ffe_get(i,j)
                                  -a.ffe_get(r,j)*a.ffe_get(i,k))/divisor);
                    a.ffe_set(i,j,a.ffe_get(i,j).normal() /*.normal() */ );
                }
                b.ffe_set(i,1,(a.ffe_get(r,k)*b.ffe_get(i,1)
                              -b.ffe_get(r,1)*a.ffe_get(i,k))/divisor);
                b.ffe_set(i,1,b.ffe_get(i,1).normal() /*.normal() */ );
                a.ffe_set(i,k,0);
            }
            divisor=a.ffe_get(r,k);
            r++;
        }
    }
    // optionally compute the determinant for square or augmented matrices
    // if (r==m+1) { det=sign*divisor; } else { det=0; }
    
    /*
    for (unsigned r=1; r<=m; ++r) {
        for (unsigned c=1; c<=n; ++c) {
            cout << a.ffe_get(r,c) << "\t";
        }
        cout << " | " <<  b.ffe_get(r,1) << endl;
    }
    */
    
#ifdef DO_GINAC_ASSERT
    // test if we really have an upper echelon matrix
    int zero_in_last_row=-1;
    for (unsigned r=1; r<=m; ++r) {
        int zero_in_this_row=0;
        for (unsigned c=1; c<=n; ++c) {
            if (a.ffe_get(r,c).is_equal(_ex0())) {
               zero_in_this_row++;
            } else {
                break;
            }
        }
        GINAC_ASSERT((zero_in_this_row>zero_in_last_row)||(zero_in_this_row=n));
        zero_in_last_row=zero_in_this_row;
    }
#endif // def DO_GINAC_ASSERT
    
    // assemble solution
    matrix sol(n,1);
    unsigned last_assigned_sol=n+1;
    for (unsigned r=m; r>0; --r) {
        unsigned first_non_zero=1;
        while ((first_non_zero<=n)&&(a.ffe_get(r,first_non_zero).is_zero())) {
            first_non_zero++;
        }
        if (first_non_zero>n) {
            // row consists only of zeroes, corresponding rhs must be 0 as well
            if (!b.ffe_get(r,1).is_zero()) {
                throw (std::runtime_error("matrix::fraction_free_elim(): singular matrix"));
            }
        } else {
            // assign solutions for vars between first_non_zero+1 and
            // last_assigned_sol-1: free parameters
            for (unsigned c=first_non_zero+1; c<=last_assigned_sol-1; ++c) {
                sol.ffe_set(c,1,vars.ffe_get(c,1));
            }
            ex e=b.ffe_get(r,1);
            for (unsigned c=first_non_zero+1; c<=n; ++c) {
                e=e-a.ffe_get(r,c)*sol.ffe_get(c,1);
            }
            sol.ffe_set(first_non_zero,1,
                        (e/a.ffe_get(r,first_non_zero)).normal());
            last_assigned_sol=first_non_zero;
        }
    }
    // assign solutions for vars between 1 and
    // last_assigned_sol-1: free parameters
    for (unsigned c=1; c<=last_assigned_sol-1; ++c) {
        sol.ffe_set(c,1,vars.ffe_get(c,1));
    }

    /*
    for (unsigned c=1; c<=n; ++c) {
        cout << vars.ffe_get(c,1) << "->" << sol.ffe_get(c,1) << endl;
    }
    */
    
#ifdef DO_GINAC_ASSERT
    // test solution with echelon matrix
    for (unsigned r=1; r<=m; ++r) {
        ex e=0;
        for (unsigned c=1; c<=n; ++c) {
            e=e+a.ffe_get(r,c)*sol.ffe_get(c,1);
        }
        if (!(e-b.ffe_get(r,1)).normal().is_zero()) {
            cout << "e=" << e;
            cout << "b.ffe_get(" << r<<",1)=" << b.ffe_get(r,1) << endl;
            cout << "diff=" << (e-b.ffe_get(r,1)).normal() << endl;
        }
        GINAC_ASSERT((e-b.ffe_get(r,1)).normal().is_zero());
    }

    // test solution with original matrix
    for (unsigned r=1; r<=m; ++r) {
        ex e=0;
        for (unsigned c=1; c<=n; ++c) {
            e=e+ffe_get(r,c)*sol.ffe_get(c,1);
        }
        try {
        if (!(e-rhs.ffe_get(r,1)).normal().is_zero()) {
            cout << "e=" << e << endl;
            e.printtree(cout);
            ex en=e.normal();
            cout << "e.normal()=" << en << endl;
            en.printtree(cout);
            cout << "rhs.ffe_get(" << r<<",1)=" << rhs.ffe_get(r,1) << endl;
            cout << "diff=" << (e-rhs.ffe_get(r,1)).normal() << endl;
        }
        } catch (...) {
            ex xxx=e-rhs.ffe_get(r,1);
            cerr << "xxx=" << xxx << endl << endl;
        }
        GINAC_ASSERT((e-rhs.ffe_get(r,1)).normal().is_zero());
    }
#endif // def DO_GINAC_ASSERT
    
    return sol;
}   
    
/** Solve simultaneous set of equations. */
matrix matrix::solve(const matrix & v) const
{
    if (!(row == col && col == v.row)) {
        throw (std::logic_error("matrix::solve(): incompatible matrices"));
    }
    
    // build the extended matrix of *this with v attached to the right
    matrix tmp(row,col+v.col);
    for (unsigned r=0; r<row; ++r) {
        for (unsigned c=0; c<col; ++c) {
            tmp.m[r*tmp.col+c] = m[r*col+c];
        }
        for (unsigned c=0; c<v.col; ++c) {
            tmp.m[r*tmp.col+c+col] = v.m[r*v.col+c];
        }
    }
    for (unsigned r1=0; r1<row; ++r1) {
        int indx = tmp.pivot(r1);
        if (indx == -1) {
            throw (std::runtime_error("matrix::solve(): singular matrix"));
        }
        for (unsigned c=r1; c<tmp.col; ++c) {
            tmp.m[r1*tmp.col+c] /= tmp.m[r1*tmp.col+r1];
        }
        for (unsigned r2=r1+1; r2<row; ++r2) {
            for (unsigned c=r1; c<tmp.col; ++c) {
                tmp.m[r2*tmp.col+c]
                    -= tmp.m[r2*tmp.col+r1] * tmp.m[r1*tmp.col+c];
            }
        }
    }
    
    // assemble the solution matrix
    exvector sol(v.row*v.col);
    for (unsigned c=0; c<v.col; ++c) {
        for (unsigned r=col-1; r>=0; --r) {
            sol[r*v.col+c] = tmp[r*tmp.col+c];
            for (unsigned i=r+1; i<col; ++i) {
                sol[r*v.col+c]
                    -= tmp[r*tmp.col+i] * sol[i*v.col+c];
            }
        }
    }
    return matrix(v.row, v.col, sol);
}

// protected

/** Partial pivoting method.
 *  Usual pivoting returns the index to the element with the largest absolute
 *  value and swaps the current row with the one where the element was found.
 *  Here it does the same with the first non-zero element. (This works fine,
 *  but may be far from optimal for numerics.) */
int matrix::pivot(unsigned ro)
{
    unsigned k=ro;
    
    for (unsigned r=ro; r<row; ++r) {
        if (!m[r*col+ro].is_zero()) {
            k = r;
            break;
        }
    }
    if (m[k*col+ro].is_zero()) {
        return -1;
    }
    if (k!=ro) {  // swap rows
        for (unsigned c=0; c<col; ++c) {
            m[k*col+c].swap(m[ro*col+c]);
        }
        return k;
    }
    return 0;
}

//////////
// global constants
//////////

const matrix some_matrix;
const type_info & typeid_matrix=typeid(some_matrix);

#ifndef NO_GINAC_NAMESPACE
} // namespace GiNaC
#endif // ndef NO_GINAC_NAMESPACE
