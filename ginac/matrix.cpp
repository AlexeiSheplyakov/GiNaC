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
#include <map>
#include <stdexcept>

#include "matrix.h"
#include "archive.h"
#include "numeric.h"
#include "lst.h"
#include "utils.h"
#include "debugmsg.h"
#include "power.h"
#include "symbol.h"
#include "normal.h"

#ifndef NO_NAMESPACE_GINAC
namespace GiNaC {
#endif // ndef NO_NAMESPACE_GINAC

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
    row = other.row;
    col = other.col;
    m = other.m;  // STL's vector copying invoked here
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
ex matrix::op(int i) const
{
    return m[i];
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
    if ((level==1)&&(flags & status_flags::evaluated))
        return *this;
    
    // emergency break
    if (level == -max_recursion_level)
        throw (std::runtime_error("matrix::eval(): recursion limit exceeded"));
    
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
    if (level==1)
        return *this;
    
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
    const matrix & o = static_cast<matrix &>(const_cast<basic &>(other));
    
    // compare number of rows
    if (row != o.rows())
        return row < o.rows() ? -1 : 1;
    
    // compare number of columns
    if (col != o.cols())
        return col < o.cols() ? -1 : 1;
    
    // equal number of rows and columns, compare individual elements
    int cmpval;
    for (unsigned r=0; r<row; ++r) {
        for (unsigned c=0; c<col; ++c) {
            cmpval = ((*this)(r,c)).compare(o(r,c));
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
    if (col != other.col || row != other.row)
        throw (std::logic_error("matrix::add(): incompatible matrices"));
    
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
    if (col != other.col || row != other.row)
        throw (std::logic_error("matrix::sub(): incompatible matrices"));
    
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
    if (col != other.row)
        throw (std::logic_error("matrix::mul(): incompatible matrices"));
    
    exvector prod(row*other.col);
    
    for (unsigned r1=0; r1<row; ++r1) {
        for (unsigned c=0; c<col; ++c) {
            if (m[r1*col+c].is_zero())
                continue;
            for (unsigned r2=0; r2<other.col; ++r2)
                prod[r1*other.col+r2] += m[r1*col+c] * other.m[c*other.col+r2];
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
    if (ro<0 || ro>=row || co<0 || co>=col)
        throw (std::range_error("matrix::operator(): index out of range"));
    
    return m[ro*col+co];
}


/** Set individual elements manually.
 *
 *  @exception range_error (index out of range) */
matrix & matrix::set(unsigned ro, unsigned co, ex value)
{
    if (ro<0 || ro>=row || co<0 || co>=col)
        throw (std::range_error("matrix::set(): index out of range"));
    
    ensure_if_modifiable();
    m[ro*col+co] = value;
    return *this;
}


/** Transposed of an m x n matrix, producing a new n x m matrix object that
 *  represents the transposed. */
matrix matrix::transpose(void) const
{
    exvector trans(col*row);
    
    for (unsigned r=0; r<col; ++r)
        for (unsigned c=0; c<row; ++c)
            trans[r*row+c] = m[c*col+r];
    
    return matrix(col,row,trans);
}


/** Determinant of square matrix.  This routine doesn't actually calculate the
 *  determinant, it only implements some heuristics about which algorithm to
 *  call.  If all the elements of the matrix are elements of an integral domain
 *  the determinant is also in that integral domain and the result is expanded
 *  only.  If one or more elements are from a quotient field the determinant is
 *  usually also in that quotient field and the result is normalized before it
 *  is returned.  This implies that the determinant of the symbolic 2x2 matrix
 *  [[a/(a-b),1],[b/(a-b),1]] is returned as unity.  (In this respect, it
 *  behaves like MapleV and unlike Mathematica.)
 *
 *  @return    the determinant as a new expression
 *  @exception logic_error (matrix not square) */
ex matrix::determinant(void) const
{
    if (row!=col)
        throw (std::logic_error("matrix::determinant(): matrix not square"));
    GINAC_ASSERT(row*col==m.capacity());
    if (this->row==1)  // continuation would be pointless
        return m[0];
    
    bool numeric_flag = true;
    bool normal_flag = false;
    unsigned sparse_count = 0;  // count non-zero elements
    for (exvector::const_iterator r=m.begin(); r!=m.end(); ++r) {
        if (!(*r).is_zero())
            ++sparse_count;
        if (!(*r).info(info_flags::numeric))
            numeric_flag = false;
        if ((*r).info(info_flags::rational_function) &&
            !(*r).info(info_flags::crational_polynomial))
            normal_flag = true;
    }
    
    if (numeric_flag)  // purely numeric matrix
        return determinant_numeric();
    // Does anybody really know when a matrix is sparse?
    // Maybe <~row/2.2 nonzero elements average in a row?
    if (5*sparse_count<=row*col) {
        // copy *this:
        matrix tmp(*this);
        int sign;
        sign = tmp.fraction_free_elimination(true);
        if (normal_flag)
            return (sign*tmp.m[row*col-1]).normal();
        else
            return (sign*tmp.m[row*col-1]).expand();
    }
    
    // Now come the minor expansion schemes.  We always develop such that the
    // smallest minors (i.e, the trivial 1x1 ones) are on the rightmost column.
    // For this to be efficient it turns out that the emptiest columns (i.e.
    // the ones with most zeros) should be the ones on the right hand side.
    // Therefore we presort the columns of the matrix:
    typedef pair<unsigned,unsigned> uintpair;  // # of zeros, column
    vector<uintpair> c_zeros;  // number of zeros in column
    for (unsigned c=0; c<col; ++c) {
        unsigned acc = 0;
        for (unsigned r=0; r<row; ++r)
            if (m[r*col+c].is_zero())
                ++acc;
        c_zeros.push_back(uintpair(acc,c));
    }
    sort(c_zeros.begin(),c_zeros.end());
    vector<unsigned> pre_sort;  // unfortunately vector<uintpair> can't be used
                                // for permutation_sign.
    for (vector<uintpair>::iterator i=c_zeros.begin(); i!=c_zeros.end(); ++i)
        pre_sort.push_back(i->second);
    int sign = permutation_sign(pre_sort);
    exvector result(row*col);  // represents sorted matrix
    unsigned c = 0;
    for (vector<unsigned>::iterator i=pre_sort.begin();
         i!=pre_sort.end();
         ++i,++c) {
        for (unsigned r=0; r<row; ++r)
            result[r*col+c] = m[r*col+(*i)];
    }
    
    if (normal_flag)
        return sign*matrix(row,col,result).determinant_minor().normal();
    return sign*matrix(row,col,result).determinant_minor();
}


/** Trace of a matrix.  The result is normalized if it is in some quotient
 *  field and expanded only otherwise.  This implies that the trace of the
 *  symbolic 2x2 matrix [[a/(a-b),x],[y,b/(b-a)]] is recognized to be unity.
 *
 *  @return    the sum of diagonal elements
 *  @exception logic_error (matrix not square) */
ex matrix::trace(void) const
{
    if (row != col)
        throw (std::logic_error("matrix::trace(): matrix not square"));
    GINAC_ASSERT(row*col==m.capacity());
    
    ex tr;
    for (unsigned r=0; r<col; ++r)
        tr += m[r*col+r];
    
    if (tr.info(info_flags::rational_function) &&
        !tr.info(info_flags::crational_polynomial))
        return tr.normal();
    else
        return tr.expand();
}


/** Characteristic Polynomial.  Following mathematica notation the
 *  characteristic polynomial of a matrix M is defined as the determiant of
 *  (M - lambda * 1) where 1 stands for the unit matrix of the same dimension
 *  as M.  Note that some CASs define it with a sign inside the determinant
 *  which gives rise to an overall sign if the dimension is odd.  This method
 *  returns the characteristic polynomial collected in powers of lambda as a
 *  new expression.
 *
 *  @return    characteristic polynomial as new expression
 *  @exception logic_error (matrix not square)
 *  @see       matrix::determinant() */
ex matrix::charpoly(const symbol & lambda) const
{
    if (row != col)
        throw (std::logic_error("matrix::charpoly(): matrix not square"));
    
    bool numeric_flag = true;
    for (exvector::const_iterator r=m.begin(); r!=m.end(); ++r) {
        if (!(*r).info(info_flags::numeric)) {
            numeric_flag = false;
        }
    }
    
    // The pure numeric case is traditionally rather common.  Hence, it is
    // trapped and we use Leverrier's algorithm which goes as row^3 for
    // every coefficient.  The expensive part is the matrix multiplication.
    if (numeric_flag) {
        matrix B(*this);
        ex c = B.trace();
        ex poly = power(lambda,row)-c*power(lambda,row-1);
        for (unsigned i=1; i<row; ++i) {
            for (unsigned j=0; j<row; ++j)
                B.m[j*col+j] -= c;
            B = this->mul(B);
            c = B.trace()/ex(i+1);
            poly -= c*power(lambda,row-i-1);
        }
        if (row%2)
            return -poly;
        else
            return poly;
    }
    
    matrix M(*this);
    for (unsigned r=0; r<col; ++r)
        M.m[r*col+r] -= lambda;
    
    return M.determinant().collect(lambda);
}


/** Inverse of this matrix.
 *
 *  @return    the inverted matrix
 *  @exception logic_error (matrix not square)
 *  @exception runtime_error (singular matrix) */
matrix matrix::inverse(void) const
{
    if (row != col)
        throw (std::logic_error("matrix::inverse(): matrix not square"));
    
    matrix tmp(row,col);
    // set tmp to the unit matrix
    for (unsigned i=0; i<col; ++i)
        tmp.m[i*col+i] = _ex1();

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


// superfluous helper function
void matrix::ffe_swap(unsigned r1, unsigned c1, unsigned r2 ,unsigned c2)
{
    ensure_if_modifiable();
    
    ex tmp = ffe_get(r1,c1);
    ffe_set(r1,c1,ffe_get(r2,c2));
    ffe_set(r2,c2,tmp);
}

// superfluous helper function
void matrix::ffe_set(unsigned r, unsigned c, ex e)
{
    set(r-1,c-1,e);
}

// superfluous helper function
ex matrix::ffe_get(unsigned r, unsigned c) const
{
    return operator()(r-1,c-1);
}

/** Solve a set of equations for an m x n matrix by fraction-free Gaussian
 *  elimination.  Based on algorithm 9.1 from 'Algorithms for Computer Algebra'
 *  by Keith O. Geddes et al.
 *
 *  @param vars n x p matrix
 *  @param rhs m x p matrix
 *  @exception logic_error (incompatible matrices)
 *  @exception runtime_error (singular matrix) */
matrix matrix::fraction_free_elim(const matrix & vars,
                                  const matrix & rhs) const
{
    // FIXME: use implementation of matrix::fraction_free_elimination
    if ((row != rhs.row) || (col != vars.row) || (rhs.col != vars.col))
        throw (std::logic_error("matrix::fraction_free_elim(): incompatible matrices"));
    
    matrix a(*this);  // make a copy of the matrix
    matrix b(rhs);    // make a copy of the rhs vector
    
    // given an m x n matrix a, reduce it to upper echelon form
    unsigned m = a.row;
    unsigned n = a.col;
    int sign = 1;
    ex divisor = 1;
    unsigned r = 1;
    
    // eliminate below row r, with pivot in column k
    for (unsigned k=1; (k<=n)&&(r<=m); ++k) {
        // find a nonzero pivot
        unsigned p;
        for (p=r; (p<=m)&&(a.ffe_get(p,k).is_equal(_ex0())); ++p) {}
        // pivot is in row p
        if (p<=m) {
            if (p!=r) {
                // switch rows p and r
                for (unsigned j=k; j<=n; ++j)
                    a.ffe_swap(p,j,r,j);
                b.ffe_swap(p,1,r,1);
                // keep track of sign changes due to row exchange
                sign = -sign;
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
            divisor = a.ffe_get(r,k);
            r++;
        }
    }    
//     for (unsigned r=1; r<=m; ++r) {
//         for (unsigned c=1; c<=n; ++c) {
//             cout << a.ffe_get(r,c) << "\t";
//         }
//         cout << " | " <<  b.ffe_get(r,1) << endl;
//     }
    
#ifdef DO_GINAC_ASSERT
    // test if we really have an upper echelon matrix
    int zero_in_last_row = -1;
    for (unsigned r=1; r<=m; ++r) {
        int zero_in_this_row=0;
        for (unsigned c=1; c<=n; ++c) {
            if (a.ffe_get(r,c).is_zero())
               zero_in_this_row++;
            else
                break;
        }
        GINAC_ASSERT((zero_in_this_row>zero_in_last_row)||(zero_in_this_row=n));
        zero_in_last_row = zero_in_this_row;
    }
#endif // def DO_GINAC_ASSERT
    
    // assemble solution
    matrix sol(n,1);
    unsigned last_assigned_sol = n+1;
    for (unsigned r=m; r>0; --r) {
        unsigned first_non_zero = 1;
        while ((first_non_zero<=n)&&(a.ffe_get(r,first_non_zero).is_zero()))
            first_non_zero++;
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
            ex e = b.ffe_get(r,1);
            for (unsigned c=first_non_zero+1; c<=n; ++c) {
                e=e-a.ffe_get(r,c)*sol.ffe_get(c,1);
            }
            sol.ffe_set(first_non_zero,1,
                        (e/a.ffe_get(r,first_non_zero)).normal());
            last_assigned_sol = first_non_zero;
        }
    }
    // assign solutions for vars between 1 and
    // last_assigned_sol-1: free parameters
    for (unsigned c=1; c<=last_assigned_sol-1; ++c)
        sol.ffe_set(c,1,vars.ffe_get(c,1));
    
#ifdef DO_GINAC_ASSERT
    // test solution with echelon matrix
    for (unsigned r=1; r<=m; ++r) {
        ex e = 0;
        for (unsigned c=1; c<=n; ++c)
            e = e+a.ffe_get(r,c)*sol.ffe_get(c,1);
        if (!(e-b.ffe_get(r,1)).normal().is_zero()) {
            cout << "e=" << e;
            cout << "b.ffe_get(" << r<<",1)=" << b.ffe_get(r,1) << endl;
            cout << "diff=" << (e-b.ffe_get(r,1)).normal() << endl;
        }
        GINAC_ASSERT((e-b.ffe_get(r,1)).normal().is_zero());
    }
    
    // test solution with original matrix
    for (unsigned r=1; r<=m; ++r) {
        ex e = 0;
        for (unsigned c=1; c<=n; ++c)
            e = e+ffe_get(r,c)*sol.ffe_get(c,1);
        try {
            if (!(e-rhs.ffe_get(r,1)).normal().is_zero()) {
                cout << "e=" << e << endl;
                e.printtree(cout);
                ex en = e.normal();
                cout << "e.normal()=" << en << endl;
                en.printtree(cout);
                cout << "rhs.ffe_get(" << r<<",1)=" << rhs.ffe_get(r,1) << endl;
                cout << "diff=" << (e-rhs.ffe_get(r,1)).normal() << endl;
            }
        } catch (...) {
            ex xxx = e - rhs.ffe_get(r,1);
            cerr << "xxx=" << xxx << endl << endl;
        }
        GINAC_ASSERT((e-rhs.ffe_get(r,1)).normal().is_zero());
    }
#endif // def DO_GINAC_ASSERT
    
    return sol;
}

/** Solve a set of equations for an m x n matrix.
 *
 *  @param vars n x p matrix
 *  @param rhs m x p matrix
 *  @exception logic_error (incompatible matrices)
 *  @exception runtime_error (singular matrix) */
matrix matrix::solve(const matrix & vars,
                     const matrix & rhs) const
{
    if ((row != rhs.row) || (col != vars.row) || (rhs.col != vars.col))
        throw (std::logic_error("matrix::solve(): incompatible matrices"));
    
    throw (std::runtime_error("FIXME: need implementation."));
}

/** Old and obsolete interface: */
matrix matrix::old_solve(const matrix & v) const
{
    if ((v.row != col) || (col != v.row))
        throw (std::logic_error("matrix::solve(): incompatible matrices"));
    
    // build the augmented matrix of *this with v attached to the right
    matrix tmp(row,col+v.col);
    for (unsigned r=0; r<row; ++r) {
        for (unsigned c=0; c<col; ++c)
            tmp.m[r*tmp.col+c] = this->m[r*col+c];
        for (unsigned c=0; c<v.col; ++c)
            tmp.m[r*tmp.col+c+col] = v.m[r*v.col+c];
    }
    // cout << "augmented: " << tmp << endl;
    tmp.gauss_elimination();
    // cout << "degaussed: " << tmp << endl;
    // assemble the solution matrix
    exvector sol(v.row*v.col);
    for (unsigned c=0; c<v.col; ++c) {
        for (unsigned r=row; r>0; --r) {
            for (unsigned i=r; i<col; ++i)
                sol[(r-1)*v.col+c] -= tmp.m[(r-1)*tmp.col+i]*sol[i*v.col+c];
            sol[(r-1)*v.col+c] += tmp.m[(r-1)*tmp.col+col+c];
            sol[(r-1)*v.col+c] = (sol[(r-1)*v.col+c]/tmp.m[(r-1)*tmp.col+(r-1)]).normal();
        }
    }
    return matrix(v.row, v.col, sol);
}


// protected

/** Determinant of purely numeric matrix, using pivoting.
 *
 *  @see       matrix::determinant() */
ex matrix::determinant_numeric(void) const
{
    matrix tmp(*this);
    ex det = _ex1();
    ex piv;
    
    // standard Gauss method:
    for (unsigned r1=0; r1<row; ++r1) {
        int indx = tmp.pivot(r1);
        if (indx == -1)
            return _ex0();
        if (indx != 0)
            det *= _ex_1();
        det = det * tmp.m[r1*col+r1];
        for (unsigned r2=r1+1; r2<row; ++r2) {
            piv = tmp.m[r2*col+r1] / tmp.m[r1*col+r1];
            for (unsigned c=r1+1; c<col; c++) {
                tmp.m[r2*col+c] -= piv * tmp.m[r1*col+c];
            }
        }
    }
    
    return det;
}


/** Recursive determinant for small matrices having at least one symbolic
 *  entry.  The basic algorithm, known as Laplace-expansion, is enhanced by
 *  some bookkeeping to avoid calculation of the same submatrices ("minors")
 *  more than once.  According to W.M.Gentleman and S.C.Johnson this algorithm
 *  is better than elimination schemes for matrices of sparse multivariate
 *  polynomials and also for matrices of dense univariate polynomials if the
 *  matrix' dimesion is larger than 7.
 *
 *  @return the determinant as a new expression (in expanded form)
 *  @see matrix::determinant() */
ex matrix::determinant_minor(void) const
{
    // for small matrices the algorithm does not make any sense:
    if (this->row==1)
        return m[0];
    if (this->row==2)
        return (m[0]*m[3]-m[2]*m[1]).expand();
    if (this->row==3)
        return (m[0]*m[4]*m[8]-m[0]*m[5]*m[7]-
                m[1]*m[3]*m[8]+m[2]*m[3]*m[7]+
                m[1]*m[5]*m[6]-m[2]*m[4]*m[6]).expand();
    
    // This algorithm can best be understood by looking at a naive
    // implementation of Laplace-expansion, like this one:
    // ex det;
    // matrix minorM(this->row-1,this->col-1);
    // for (unsigned r1=0; r1<this->row; ++r1) {
    //     // shortcut if element(r1,0) vanishes
    //     if (m[r1*col].is_zero())
    //         continue;
    //     // assemble the minor matrix
    //     for (unsigned r=0; r<minorM.rows(); ++r) {
    //         for (unsigned c=0; c<minorM.cols(); ++c) {
    //             if (r<r1)
    //                 minorM.set(r,c,m[r*col+c+1]);
    //             else
    //                 minorM.set(r,c,m[(r+1)*col+c+1]);
    //         }
    //     }
    //     // recurse down and care for sign:
    //     if (r1%2)
    //         det -= m[r1*col] * minorM.determinant_minor();
    //     else
    //         det += m[r1*col] * minorM.determinant_minor();
    // }
    // return det.expand();
    // What happens is that while proceeding down many of the minors are
    // computed more than once.  In particular, there are binomial(n,k)
    // kxk minors and each one is computed factorial(n-k) times.  Therefore
    // it is reasonable to store the results of the minors.  We proceed from
    // right to left.  At each column c we only need to retrieve the minors
    // calculated in step c-1.  We therefore only have to store at most 
    // 2*binomial(n,n/2) minors.
    
    // Unique flipper counter for partitioning into minors
    vector<unsigned> Pkey;
    Pkey.reserve(this->col);
    // key for minor determinant (a subpartition of Pkey)
    vector<unsigned> Mkey;
    Mkey.reserve(this->col-1);
    // we store our subminors in maps, keys being the rows they arise from
    typedef map<vector<unsigned>,class ex> Rmap;
    typedef map<vector<unsigned>,class ex>::value_type Rmap_value;
    Rmap A;
    Rmap B;
    ex det;
    // initialize A with last column:
    for (unsigned r=0; r<this->col; ++r) {
        Pkey.erase(Pkey.begin(),Pkey.end());
        Pkey.push_back(r);
        A.insert(Rmap_value(Pkey,m[this->col*r+this->col-1]));
    }
    // proceed from right to left through matrix
    for (int c=this->col-2; c>=0; --c) {
        Pkey.erase(Pkey.begin(),Pkey.end());  // don't change capacity
        Mkey.erase(Mkey.begin(),Mkey.end());
        for (unsigned i=0; i<this->col-c; ++i)
            Pkey.push_back(i);
        unsigned fc = 0;  // controls logic for our strange flipper counter
        do {
            det = _ex0();
            for (unsigned r=0; r<this->col-c; ++r) {
                // maybe there is nothing to do?
                if (m[Pkey[r]*this->col+c].is_zero())
                    continue;
                // create the sorted key for all possible minors
                Mkey.erase(Mkey.begin(),Mkey.end());
                for (unsigned i=0; i<this->col-c; ++i)
                    if (i!=r)
                        Mkey.push_back(Pkey[i]);
                // Fetch the minors and compute the new determinant
                if (r%2)
                    det -= m[Pkey[r]*this->col+c]*A[Mkey];
                else
                    det += m[Pkey[r]*this->col+c]*A[Mkey];
            }
            // prevent build-up of deep nesting of expressions saves time:
            det = det.expand();
            // store the new determinant at its place in B:
            if (!det.is_zero())
                B.insert(Rmap_value(Pkey,det));
            // increment our strange flipper counter
            for (fc=this->col-c; fc>0; --fc) {
                ++Pkey[fc-1];
                if (Pkey[fc-1]<fc+c)
                    break;
            }
            if (fc<this->col-c)
                for (unsigned j=fc; j<this->col-c; ++j)
                    Pkey[j] = Pkey[j-1]+1;
        } while(fc);
        // next column, so change the role of A and B:
        A = B;
        B.clear();
    }
    
    return det;
}


/** Perform the steps of an ordinary Gaussian elimination to bring the matrix
 *  into an upper echelon form.
 *
 *  @return sign is 1 if an even number of rows was swapped, -1 if an odd
 *  number of rows was swapped and 0 if the matrix is singular. */
int matrix::gauss_elimination(void)
{
    int sign = 1;
    ensure_if_modifiable();
    for (unsigned r1=0; r1<row-1; ++r1) {
        int indx = pivot(r1);
        if (indx == -1)
            return 0;  // Note: leaves *this in a messy state.
        if (indx > 0)
            sign = -sign;
        for (unsigned r2=r1+1; r2<row; ++r2) {
            for (unsigned c=r1+1; c<col; ++c)
                this->m[r2*col+c] -= this->m[r2*col+r1]*this->m[r1*col+c]/this->m[r1*col+r1];
            for (unsigned c=0; c<=r1; ++c)
                this->m[r2*col+c] = _ex0();
        }
    }
    
    return sign;
}


/** Perform the steps of division free elimination to bring the matrix
 *  into an upper echelon form.
 *
 *  @return sign is 1 if an even number of rows was swapped, -1 if an odd
 *  number of rows was swapped and 0 if the matrix is singular. */
int matrix::division_free_elimination(void)
{
    int sign = 1;
    ensure_if_modifiable();
    for (unsigned r1=0; r1<row-1; ++r1) {
        int indx = pivot(r1);
        if (indx==-1)
            return 0;  // Note: leaves *this in a messy state.
        if (indx>0)
            sign = -sign;
        for (unsigned r2=r1+1; r2<row; ++r2) {
            for (unsigned c=r1+1; c<col; ++c)
                this->m[r2*col+c] = this->m[r1*col+r1]*this->m[r2*col+c] - this->m[r2*col+r1]*this->m[r1*col+c];
            for (unsigned c=0; c<=r1; ++c)
                this->m[r2*col+c] = _ex0();
        }
    }
    
    return sign;
}


/** Perform the steps of Bareiss' one-step fraction free elimination to bring
 *  the matrix into an upper echelon form.  Fraction free elimination means
 *  that divide is used straightforwardly, without computing GCDs first.  This
 *  is possible, since we know the divisor at each step.
 *  
 *  @param det may be set to true to save a lot of space if one is only
 *  interested in the last element (i.e. for calculating determinants), the
 *  others are set to zero in this case.
 *  @return sign is 1 if an even number of rows was swapped, -1 if an odd
 *  number of rows was swapped and 0 if the matrix is singular. */
int matrix::fraction_free_elimination(bool det)
{
    // Method:
    // (single-step fraction free elimination scheme, already known to Jordan)
    //
    // Usual division-free elimination sets m[0](r,c) = m(r,c) and then sets
    //     m[k+1](r,c) = m[k](k,k) * m[k](r,c) - m[k](r,k) * m[k](k,c).
    //
    // Bareiss (fraction-free) elimination in addition divides that element
    // by m[k-1](k-1,k-1) for k>1, where it can be shown by means of the
    // Sylvester determinant that this really divides m[k+1](r,c).
    //
    // We also allow rational functions where the original prove still holds.
    // However, we must care for numerator and denominator separately and
    // "manually" work in the integral domains because of subtle cancellations
    // (see below).  This blows up the bookkeeping a bit and the formula has
    // to be modified to expand like this (N{x} stands for numerator of x,
    // D{x} for denominator of x):
    //     N{m[k+1](r,c)} = N{m[k](k,k)}*N{m[k](r,c)}*D{m[k](r,k)}*D{m[k](k,c)}
    //                     -N{m[k](r,k)}*N{m[k](k,c)}*D{m[k](k,k)}*D{m[k](r,c)}
    //     D{m[k+1](r,c)} = D{m[k](k,k)}*D{m[k](r,c)}*D{m[k](r,k)}*D{m[k](k,c)}
    // where for k>1 we now divide N{m[k+1](r,c)} by
    //     N{m[k-1](k-1,k-1)}
    // and D{m[k+1](r,c)} by
    //     D{m[k-1](k-1,k-1)}.
    
    GINAC_ASSERT(det || row==col);
    ensure_if_modifiable();
    if (rows()==1)
        return 1;
    
    int sign = 1;
    ex divisor_n = 1;
    ex divisor_d = 1;
    ex dividend_n;
    ex dividend_d;
    
    // We populate temporary matrices to subsequently operate on.  There is
    // one holding numerators and another holding denominators of entries.
    // This is a must since the evaluator (or even earlier mul's constructor)
    // might cancel some trivial element which causes divide() to fail.  The
    // elements are normalized first (yes, even though this algorithm doesn't
    // need GCDs) since the elements of *this might be unnormalized, which
    // makes things more complicated than they need to be.
    matrix tmp_n(*this);
    matrix tmp_d(row,col);  // for denominators, if needed
    lst srl;  // symbol replacement list
    exvector::iterator it = m.begin();
    exvector::iterator tmp_n_it = tmp_n.m.begin();
    exvector::iterator tmp_d_it = tmp_d.m.begin();
    for (; it!= m.end(); ++it, ++tmp_n_it, ++tmp_d_it) {
        (*tmp_n_it) = (*it).normal().to_rational(srl);
        (*tmp_d_it) = (*tmp_n_it).denom();
        (*tmp_n_it) = (*tmp_n_it).numer();
    }
    
    for (unsigned r1=0; r1<row-1; ++r1) {
//         cout << "==<" << r1 << ">" << string(60,'=') << endl;
        int indx = tmp_n.pivot(r1);
        if (det && indx==-1)
            return 0;  // FIXME: what to do if det is false?
        if (indx>0) {
            sign = -sign;
            // rows r1 and indx were swapped, so pivot matrix tmp_d:
            for (unsigned c=0; c<col; ++c)
                tmp_d.m[row*indx+c].swap(tmp_d.m[row*r1+c]);
        }
//         cout << tmp_n << endl;
//         cout << tmp_d << endl;
        if (r1>0) {
            divisor_n = tmp_n.m[(r1-1)*col+(r1-1)].expand();
            divisor_d = tmp_d.m[(r1-1)*col+(r1-1)].expand();
            // save space by deleting no longer needed elements:
            if (det) {
                for (unsigned c=0; c<col; ++c) {
                    tmp_n.m[(r1-1)*col+c] = 0;
                    tmp_d.m[(r1-1)*col+c] = 1;
                }
            }
        }
        for (unsigned r2=r1+1; r2<row; ++r2) {
            for (unsigned c=r1+1; c<col; ++c) {
                dividend_n = (tmp_n.m[r1*col+r1]*tmp_n.m[r2*col+c]*
                              tmp_d.m[r2*col+r1]*tmp_d.m[r1*col+c]
                             -tmp_n.m[r2*col+r1]*tmp_n.m[r1*col+c]*
                              tmp_d.m[r1*col+r1]*tmp_d.m[r2*col+c]).expand();
                dividend_d = (tmp_d.m[r2*col+r1]*tmp_d.m[r1*col+c]*
                              tmp_d.m[r1*col+r1]*tmp_d.m[r2*col+c]).expand();
//                 cout << "Element " << r2 << ',' << c << endl;
//                 cout << "dividend_n==" << dividend_n << endl;
//                 cout << "dividend_d==" << dividend_d << endl;
//                 cout << " divisor_n==" << divisor_n << endl;
//                 cout << " divisor_d==" << divisor_d << endl;
//                 cout << string(20,'-') << endl;
                bool check = divide(dividend_n, divisor_n,
                                    tmp_n.m[r2*col+c],true);
                check &= divide(dividend_d, divisor_d,
                                tmp_d.m[r2*col+c],true);
                GINAC_ASSERT(check);
            }
            // fill up left hand side.
            for (unsigned c=0; c<=r1; ++c)
                tmp_n.m[r2*col+c] = _ex0();
        }
//         cout << tmp_n << endl;
//         cout << tmp_d << endl;        
    }
    // repopulate *this matrix:
    it = m.begin();
    tmp_n_it = tmp_n.m.begin();
    tmp_d_it = tmp_d.m.begin();
    for (; it!= m.end(); ++it, ++tmp_n_it, ++tmp_d_it)
        (*it) = ((*tmp_n_it)/(*tmp_d_it)).subs(srl);
    
    return sign;
}


/** Partial pivoting method.
 *  Usual pivoting (symbolic==false) returns the index to the element with the
 *  largest absolute value in column ro and swaps the current row with the one
 *  where the element was found.  With (symbolic==true) it does the same thing
 *  with the first non-zero element.
 *
 *  @param ro is the row to be inspected
 *  @param symbolic signal if we want the first non-zero element to be pivoted
 *  (true) or the one with the largest absolute value (false).
 *  @return 0 if no interchange occured, -1 if all are zero (usually signaling
 *  a degeneracy) and positive integer k means that rows ro and k were swapped.
 */
int matrix::pivot(unsigned ro, bool symbolic)
{
    unsigned k = ro;
    
    if (symbolic) {  // search first non-zero
        for (unsigned r=ro; r<row; ++r) {
            if (!m[r*col+ro].is_zero()) {
                k = r;
                break;
            }
        }
    } else {  // search largest
        numeric tmp(0);
        numeric maxn(-1);
        for (unsigned r=ro; r<row; ++r) {
            GINAC_ASSERT(is_ex_of_type(m[r*col+ro],numeric));
            if ((tmp = abs(ex_to_numeric(m[r*col+ro]))) > maxn &&
                !tmp.is_zero()) {
                maxn = tmp;
                k = r;
            }
        }
    }
    if (m[k*col+ro].is_zero())
        return -1;
    if (k!=ro) {  // swap rows
        ensure_if_modifiable();
        for (unsigned c=0; c<col; ++c) {
            m[k*col+c].swap(m[ro*col+c]);
        }
        return k;
    }
    return 0;
}

/** Convert list of lists to matrix. */
ex lst_to_matrix(const ex &l)
{
	if (!is_ex_of_type(l, lst))
		throw(std::invalid_argument("argument to lst_to_matrix() must be a lst"));

	// Find number of rows and columns
	unsigned rows = l.nops(), cols = 0, i, j;
	for (i=0; i<rows; i++)
		if (l.op(i).nops() > cols)
			cols = l.op(i).nops();

	// Allocate and fill matrix
	matrix &m = *new matrix(rows, cols);
	for (i=0; i<rows; i++)
		for (j=0; j<cols; j++)
			if (l.op(i).nops() > j)
				m.set(i, j, l.op(i).op(j));
			else
				m.set(i, j, ex(0));
	return m;
}

//////////
// global constants
//////////

const matrix some_matrix;
const type_info & typeid_matrix=typeid(some_matrix);

#ifndef NO_NAMESPACE_GINAC
} // namespace GiNaC
#endif // ndef NO_NAMESPACE_GINAC
