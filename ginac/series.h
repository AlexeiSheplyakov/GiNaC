/** @file series.h
 *
 *  Interface to class for extended truncated power series. */

#ifndef _SERIES_H_
#define _SERIES_H_

#include "basic.h"
#include "ex.h"
#include "expairseq.h"
#include "symbol.h"


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

#endif
