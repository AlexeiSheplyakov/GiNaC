/** @file series.h
 *
 *  Interface to class for extended truncated power series.
 *
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

#ifndef __GINAC_SERIES_H__
#define __GINAC_SERIES_H__

/** This class holds a extended truncated power series (positive and negative
 *  integer powers). It consists of expression coefficients (only non-zero
 *  coefficients are stored), an expansion variable and an expansion point.
 *  Other classes must provide members to convert into this type. */
class series : public basic
{
    typedef basic inherited;

    // default constructor, destructor, copy constructor, assignment operator and helpers
public:
    series();
    ~series();
    series(series const &other);
    series const &operator=(series const &other);
protected:
    void copy(series const &other);
    void destroy(bool call_parent);

    // other constructors
public:
    series(ex const &var_, ex const &point_, epvector const &ops_);

    // functions overriding virtual functions from base classes
public:
    basic *duplicate() const;
    void printraw(ostream &os) const;
    void print(ostream &os, unsigned upper_precedence=0) const;
    int degree(symbol const &s) const;
    int ldegree(symbol const &s) const;
    ex coeff(symbol const &s, int const n=1) const;
    ex eval(int level=0) const;
    ex evalf(int level=0) const;
    ex diff(symbol const & s) const;
    ex normal(lst &sym_lst, lst &repl_lst, int level=0) const;

    // non-virtual functions in this class
public:
    ex convert_to_poly(bool no_order = false) const;
    bool is_compatible_to(const series &other) const {return var.compare(other.var) == 0 && point.compare(other.point) == 0;}
    bool is_zero(void) const {return seq.size() == 0;}
    ex add_series(const series &other) const;
    ex mul_const(const numeric &other) const;
    ex mul_series(const series &other) const;
    ex power_const(const numeric &p, int deg) const;

protected:
    /** Vector of {coefficient, power} pairs */
    epvector seq;

    /** Series variable (holds a symbol) */
    ex var;

    /** Expansion point */
    ex point;
};

// global constants
extern const series some_series;
extern type_info const & typeid_series;

#define ex_to_series(X) (static_cast<class series const &>(*(X).bp))
#define series_to_poly(X) (static_cast<series const &>(*(X).bp).convert_to_poly(true))

#endif // ndef __GINAC_SERIES_H__
