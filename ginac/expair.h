/** @file expair.h
 *
 *  Definition of expression pairs (building blocks of expairseq). */

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

#ifndef __GINAC_EXPAIR_H__
#define __GINAC_EXPAIR_H__

#include <ginac/ex.h>
#include <ginac/numeric.h>

#ifndef NO_GINAC_NAMESPACE
namespace GiNaC {
#endif // ndef NO_GINAC_NAMESPACE

/** A pair of expressions.
 *  This similar to, but slightly extended STL's pair<> but we need to account
 *  for methods like .compare() */
class expair
{
public:
    expair() {}
    ~expair() {}
    expair(const expair & other) : rest(other.rest), coeff(other.coeff)
    {
        GINAC_ASSERT(is_ex_exactly_of_type(coeff,numeric));
    }
    const expair & operator=(const expair & other)
    {
        if (this != &other) {
            rest=other.rest;
            coeff=other.coeff;
        }
        return *this;
    }
    expair(const ex & r, const ex & c) : rest(r), coeff(c)
    {
        GINAC_ASSERT(is_ex_exactly_of_type(coeff,numeric));
    }
    
    bool is_numeric_with_coeff_1(void) const
    {
        GINAC_ASSERT(is_ex_exactly_of_type(coeff,numeric));
        return is_ex_exactly_of_type(rest,numeric) &&
               (coeff.is_equal(ex(1)));
    }

    bool is_equal(const expair & other) const
    {
        return (rest.is_equal(other.rest) && coeff.is_equal(other.coeff));
    }
    bool is_less(const expair & other) const 
    {
        return (rest.compare(other.rest)<0) ||
               (!(other.rest.compare(rest)<0) && (coeff.compare(other.coeff)<0));
    }
    int compare(const expair & other) const
    {
        int cmpval=rest.compare(other.rest);
        if (cmpval!=0) return cmpval;
        cmpval=coeff.compare(other.coeff);
        return cmpval;
    }

    bool is_less_old2(const expair & other) const 
    {
        /*
        bool this_numeric_with_coeff_1=is_numeric_with_coeff_1();
        bool other_numeric_with_coeff_1=other.is_numeric_with_coeff_1();
        if (this_numeric_with_coeff_1) {
            if (other_numeric_with_coeff_1) {
                // both have coeff 1: compare rests
                return rest.compare(other.rest)<0;
            }
            // only this has coeff 1: >
            return false;
        } else if (other_numeric_with_coeff_1) {
            // only other has coeff 1: <
            return true;
        }
        return (rest.compare(other.rest)<0) ||
               (!(other.rest.compare(rest)<0) &&
                 (coeff.compare(other.coeff)<0));
        */
        if (is_ex_exactly_of_type(rest,numeric) &&
            is_ex_exactly_of_type(other.rest,numeric)) {
            if (coeff.is_equal(ex(1))) {
                if ((other.coeff).is_equal(ex(1))) {
                    // both have coeff 1: compare rests
                    return rest.compare(other.rest)<0;
                }
                // only this has coeff 1: >
                return false;
            } else if ((other.coeff).is_equal(ex(1))) {
                // only other has coeff 1: <
                return true;
            }
            // neither has coeff 1: usual compare        
        }
        return (rest.compare(other.rest)<0) ||
               (!(other.rest.compare(rest)<0) &&
                 (coeff.compare(other.coeff)<0));
    }
    int compare_old2(const expair & other) const
    {
        if (is_ex_exactly_of_type(rest,numeric) &&
            is_ex_exactly_of_type(other.rest,numeric)) {
            if ((coeff).is_equal(ex(1))) {
                if ((other.coeff).is_equal(ex(1))) {
                    // both have coeff 1: compare rests
                    return rest.compare(other.rest);
                }
                // only this has coeff 1: >
                return 1;
            } else if ((other.coeff).is_equal(ex(1))) {
                // only other has coeff 1: <
                return -1;
            }
            // neither has coeff 1: usual compare        
        }
        /*
        bool this_numeric_with_coeff_1=is_numeric_with_coeff_1();
        bool other_numeric_with_coeff_1=other.is_numeric_with_coeff_1();
        if (this_numeric_with_coeff_1) {
            if (other_numeric_with_coeff_1) {
                // both have coeff 1: compare rests
                return rest.compare(other.rest);
            }
            // only this has coeff 1: >
            return 1;
        } else if (other_numeric_with_coeff_1) {
            // only other has coeff 1: <
            return -1;
            // neither has coeff 1: usual compare        
        }
        */
        int cmpval=rest.compare(other.rest);
        if (cmpval!=0) return cmpval;
        return coeff.compare(other.coeff);
    }
    bool is_less_old(const expair & other) const 
    {
        return (rest.compare(other.rest)<0) ||
               (!(other.rest.compare(rest)<0) && (coeff.compare(other.coeff)<0));
    }
    int compare_old(const expair & other) const
    {
        int cmpval=rest.compare(other.rest);
        if (cmpval!=0) return cmpval;
        cmpval=coeff.compare(other.coeff);
        return cmpval;
    }

    void printraw(ostream & os) const
    {
        os << "expair(";
        rest.printraw(os);
        os << ",";
        coeff.printraw(os);
        os << ")";
    }

    ex rest;
    ex coeff;
};

class expair_is_less
{
public:
    bool operator()(const expair & lh, const expair & rh) const
    {
        return lh.is_less(rh);
    }
};

class expair_is_less_old
{
public:
    bool operator()(const expair & lh, const expair & rh) const
    {
        return lh.is_less_old(rh);
    }
};

#ifndef NO_GINAC_NAMESPACE
} // namespace GiNaC
#endif // ndef NO_GINAC_NAMESPACE

#endif // ndef __GINAC_EXPAIR_H__
