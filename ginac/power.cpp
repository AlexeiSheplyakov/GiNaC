/** @file power.cpp
 *
 *  Implementation of GiNaC's symbolic exponentiation (basis^exponent). */

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

#include <vector>
#include <iostream>
#include <stdexcept>

#include "power.h"
#include "expairseq.h"
#include "add.h"
#include "mul.h"
#include "numeric.h"
#include "relational.h"
#include "symbol.h"
#include "debugmsg.h"

#ifndef NO_GINAC_NAMESPACE
namespace GiNaC {
#endif // ndef NO_GINAC_NAMESPACE

typedef vector<int> intvector;

//////////
// default constructor, destructor, copy constructor assignment operator and helpers
//////////

// public

power::power() : basic(TINFO_power)
{
    debugmsg("power default constructor",LOGLEVEL_CONSTRUCT);
}

power::~power()
{
    debugmsg("power destructor",LOGLEVEL_DESTRUCT);
    destroy(0);
}

power::power(power const & other)
{
    debugmsg("power copy constructor",LOGLEVEL_CONSTRUCT);
    copy(other);
}

power const & power::operator=(power const & other)
{
    debugmsg("power operator=",LOGLEVEL_ASSIGNMENT);
    if (this != &other) {
        destroy(1);
        copy(other);
    }
    return *this;
}

// protected

void power::copy(power const & other)
{
    basic::copy(other);
    basis=other.basis;
    exponent=other.exponent;
}

void power::destroy(bool call_parent)
{
    if (call_parent) basic::destroy(call_parent);
}

//////////
// other constructors
//////////

// public

power::power(ex const & lh, ex const & rh) : basic(TINFO_power), basis(lh), exponent(rh)
{
    debugmsg("power constructor from ex,ex",LOGLEVEL_CONSTRUCT);
    GINAC_ASSERT(basis.return_type()==return_types::commutative);
}

power::power(ex const & lh, numeric const & rh) : basic(TINFO_power), basis(lh), exponent(rh)
{
    debugmsg("power constructor from ex,numeric",LOGLEVEL_CONSTRUCT);
    GINAC_ASSERT(basis.return_type()==return_types::commutative);
}

//////////
// functions overriding virtual functions from bases classes
//////////

// public

basic * power::duplicate() const
{
    debugmsg("power duplicate",LOGLEVEL_DUPLICATE);
    return new power(*this);
}

void power::print(ostream & os, unsigned upper_precedence) const
{
    debugmsg("power print",LOGLEVEL_PRINT);
    if (precedence<=upper_precedence) os << "(";
    basis.print(os,precedence);
    os << "^";
    exponent.print(os,precedence);
    if (precedence<=upper_precedence) os << ")";
}

void power::printraw(ostream & os) const
{
    debugmsg("power printraw",LOGLEVEL_PRINT);

    os << "power(";
    basis.printraw(os);
    os << ",";
    exponent.printraw(os);
    os << ",hash=" << hashvalue << ",flags=" << flags << ")";
}

void power::printtree(ostream & os, unsigned indent) const
{
    debugmsg("power printtree",LOGLEVEL_PRINT);

    os << string(indent,' ') << "power: "
       << "hash=" << hashvalue << " (0x" << hex << hashvalue << dec << ")"
       << ", flags=" << flags << endl;
    basis.printtree(os,indent+delta_indent);
    exponent.printtree(os,indent+delta_indent);
}

static void print_sym_pow(ostream & os, unsigned type, const symbol &x, int exp)
{
    // Optimal output of integer powers of symbols to aid compiler CSE
    if (exp == 1) {
        x.printcsrc(os, type, 0);
    } else if (exp == 2) {
        x.printcsrc(os, type, 0);
        os << "*";
        x.printcsrc(os, type, 0);
    } else if (exp & 1) {
        x.printcsrc(os, 0);
        os << "*";
        print_sym_pow(os, type, x, exp-1);
    } else {
        os << "(";
        print_sym_pow(os, type, x, exp >> 1);
        os << ")*(";
        print_sym_pow(os, type, x, exp >> 1);
        os << ")";
    }
}

void power::printcsrc(ostream & os, unsigned type, unsigned upper_precedence) const
{
    debugmsg("power print csrc", LOGLEVEL_PRINT);
    
    // Integer powers of symbols are printed in a special, optimized way
    if (exponent.info(info_flags::integer) &&
        (is_ex_exactly_of_type(basis, symbol) ||
         is_ex_exactly_of_type(basis, constant))) {
        int exp = ex_to_numeric(exponent).to_int();
        if (exp > 0)
            os << "(";
        else {
            exp = -exp;
            if (type == csrc_types::ctype_cl_N)
                os << "recip(";
            else
                os << "1.0/(";
        }
        print_sym_pow(os, type, static_cast<const symbol &>(*basis.bp), exp);
        os << ")";

    // <expr>^-1 is printed as "1.0/<expr>" or with the recip() function of CLN
    } else if (exponent.compare(numMINUSONE()) == 0) {
        if (type == csrc_types::ctype_cl_N)
            os << "recip(";
        else
            os << "1.0/(";
        basis.bp->printcsrc(os, type, 0);
        os << ")";

    // Otherwise, use the pow() or expt() (CLN) functions
    } else {
        if (type == csrc_types::ctype_cl_N)
            os << "expt(";
        else
            os << "pow(";
        basis.bp->printcsrc(os, type, 0);
        os << ",";
        exponent.bp->printcsrc(os, type, 0);
        os << ")";
    }
}

bool power::info(unsigned inf) const
{
    if (inf==info_flags::polynomial ||
        inf==info_flags::integer_polynomial ||
        inf==info_flags::cinteger_polynomial ||
        inf==info_flags::rational_polynomial ||
        inf==info_flags::crational_polynomial) {
        return exponent.info(info_flags::nonnegint);
    } else if (inf==info_flags::rational_function) {
        return exponent.info(info_flags::integer);
    } else {
        return basic::info(inf);
    }
}

int power::nops() const
{
    return 2;
}

ex & power::let_op(int const i)
{
    GINAC_ASSERT(i>=0);
    GINAC_ASSERT(i<2);

    return i==0 ? basis : exponent;
}

int power::degree(symbol const & s) const
{
    if (is_exactly_of_type(*exponent.bp,numeric)) {
	if ((*basis.bp).compare(s)==0)
            return ex_to_numeric(exponent).to_int();
        else
            return basis.degree(s) * ex_to_numeric(exponent).to_int();
    }
    return 0;
}

int power::ldegree(symbol const & s) const 
{
    if (is_exactly_of_type(*exponent.bp,numeric)) {
	if ((*basis.bp).compare(s)==0)
            return ex_to_numeric(exponent).to_int();
        else
            return basis.ldegree(s) * ex_to_numeric(exponent).to_int();
    }
    return 0;
}

ex power::coeff(symbol const & s, int const n) const
{
    if ((*basis.bp).compare(s)!=0) {
        // basis not equal to s
        if (n==0) {
            return *this;
        } else {
            return exZERO();
        }
    } else if (is_exactly_of_type(*exponent.bp,numeric)&&
               (static_cast<numeric const &>(*exponent.bp).compare(numeric(n))==0)) {
        return exONE();
    }

    return exZERO();
}

ex power::eval(int level) const
{
    // simplifications: ^(x,0) -> 1 (0^0 handled here)
    //                  ^(x,1) -> x
    //                  ^(0,x) -> 0 (except if x is real and negative, in which case an exception is thrown)
    //                  ^(1,x) -> 1
    //                  ^(c1,c2) -> *(c1^n,c1^(c2-n)) (c1, c2 numeric(), 0<(c2-n)<1 except if c1,c2 are rational, but c1^c2 is not)
    //                  ^(^(x,c1),c2) -> ^(x,c1*c2) (c1, c2 numeric(), c2 integer or -1 < c1 <= 1, case c1=1 should not happen, see below!)
    //                  ^(*(x,y,z),c1) -> *(x^c1,y^c1,z^c1) (c1 integer)
    //                  ^(*(x,c1),c2) -> ^(x,c2)*c1^c2 (c1, c2 numeric(), c1>0)
    //                  ^(*(x,c1),c2) -> ^(-x,c2)*c1^c2 (c1, c2 numeric(), c1<0)
    
    debugmsg("power eval",LOGLEVEL_MEMBER_FUNCTION);

    if ((level==1)&&(flags & status_flags::evaluated)) {
        return *this;
    } else if (level == -max_recursion_level) {
        throw(std::runtime_error("max recursion level reached"));
    }
    
    ex const & ebasis    = level==1 ? basis    : basis.eval(level-1);
    ex const & eexponent = level==1 ? exponent : exponent.eval(level-1);

    bool basis_is_numerical=0;
    bool exponent_is_numerical=0;
    numeric * num_basis;
    numeric * num_exponent;

    if (is_exactly_of_type(*ebasis.bp,numeric)) {
        basis_is_numerical=1;
        num_basis=static_cast<numeric *>(ebasis.bp);
    }
    if (is_exactly_of_type(*eexponent.bp,numeric)) {
        exponent_is_numerical=1;
        num_exponent=static_cast<numeric *>(eexponent.bp);
    }

    // ^(x,0) -> 1 (0^0 also handled here)
    if (eexponent.is_zero())
        return exONE();

    // ^(x,1) -> x
    if (eexponent.is_equal(exONE()))
        return ebasis;

    // ^(0,x) -> 0 (except if x is real and negative)
    if (ebasis.is_zero()) {
        if (exponent_is_numerical && num_exponent->is_negative()) {
            throw(std::overflow_error("power::eval(): division by zero"));
        } else
            return exZERO();
    }

    // ^(1,x) -> 1
    if (ebasis.is_equal(exONE()))
        return exONE();

    if (basis_is_numerical && exponent_is_numerical) {
        // ^(c1,c2) -> c1^c2 (c1, c2 numeric(),
        // except if c1,c2 are rational, but c1^c2 is not)
        bool basis_is_crational = num_basis->is_crational();
        bool exponent_is_crational = num_exponent->is_crational();
        numeric res = (*num_basis).power(*num_exponent);
        
        if ((!basis_is_crational || !exponent_is_crational)
            || res.is_crational()) {
            return res;
        }
        GINAC_ASSERT(!num_exponent->is_integer());  // has been handled by now
        // ^(c1,n/m) -> *(c1^q,c1^(n/m-q)), 0<(n/m-h)<1, q integer
        if (basis_is_crational && exponent_is_crational
            && num_exponent->is_real()
            && !num_exponent->is_integer()) {
            numeric r, q, n, m;
            n = num_exponent->numer();
            m = num_exponent->denom();
            q = iquo(n, m, r);
            if (r.is_negative()) {
                r = r.add(m);
                q = q.sub(numONE());
            }
            if (q.is_zero())  // the exponent was in the allowed range 0<(n/m)<1
                return this->hold();
            else {
                epvector res(2);
                res.push_back(expair(ebasis,r.div(m)));
                res.push_back(expair(ex(num_basis->power(q)),exONE()));
                return (new mul(res))->setflag(status_flags::dynallocated | status_flags::evaluated);
                /*return mul(num_basis->power(q),
                           power(ex(*num_basis),ex(r.div(m)))).hold();
                */
                /* return (new mul(num_basis->power(q),
                   power(*num_basis,r.div(m)).hold()))->setflag(status_flags::dynallocated | status_flags::evaluated);
                */
            }
        }
    }

    // ^(^(x,c1),c2) -> ^(x,c1*c2)
    // (c1, c2 numeric(), c2 integer or -1 < c1 <= 1,
    // case c1=1 should not happen, see below!)
    if (exponent_is_numerical && is_ex_exactly_of_type(ebasis,power)) {
        power const & sub_power=ex_to_power(ebasis);
        ex const & sub_basis=sub_power.basis;
        ex const & sub_exponent=sub_power.exponent;
        if (is_ex_exactly_of_type(sub_exponent,numeric)) {
            numeric const & num_sub_exponent=ex_to_numeric(sub_exponent);
            GINAC_ASSERT(num_sub_exponent!=numeric(1));
            if (num_exponent->is_integer() || abs(num_sub_exponent)<1) {
                return power(sub_basis,num_sub_exponent.mul(*num_exponent));
            }
        }
    }
    
    // ^(*(x,y,z),c1) -> *(x^c1,y^c1,z^c1) (c1 integer)
    if (exponent_is_numerical && num_exponent->is_integer() &&
        is_ex_exactly_of_type(ebasis,mul)) {
        return expand_mul(ex_to_mul(ebasis), *num_exponent);
    }

    // ^(*(...,x;c1),c2) -> ^(*(...,x;1),c2)*c1^c2 (c1, c2 numeric(), c1>0)
    // ^(*(...,x,c1),c2) -> ^(*(...,x;-1),c2)*(-c1)^c2 (c1, c2 numeric(), c1<0)
    if (exponent_is_numerical && is_ex_exactly_of_type(ebasis,mul)) {
        GINAC_ASSERT(!num_exponent->is_integer()); // should have been handled above
        mul const & mulref=ex_to_mul(ebasis);
        if (!mulref.overall_coeff.is_equal(exONE())) {
            numeric const & num_coeff=ex_to_numeric(mulref.overall_coeff);
            if (num_coeff.is_real()) {
                if (num_coeff.is_positive()>0) {
                    mul * mulp=new mul(mulref);
                    mulp->overall_coeff=exONE();
                    mulp->clearflag(status_flags::evaluated);
                    mulp->clearflag(status_flags::hash_calculated);
                    return (new mul(power(*mulp,exponent),
                                    power(num_coeff,*num_exponent)))->
                        setflag(status_flags::dynallocated);
                } else {
                    GINAC_ASSERT(num_coeff.compare(numZERO())<0);
                    if (num_coeff.compare(numMINUSONE())!=0) {
                        mul * mulp=new mul(mulref);
                        mulp->overall_coeff=exMINUSONE();
                        mulp->clearflag(status_flags::evaluated);
                        mulp->clearflag(status_flags::hash_calculated);
                        return (new mul(power(*mulp,exponent),
                                        power(abs(num_coeff),*num_exponent)))->
                            setflag(status_flags::dynallocated);
                    }
                }
            }
        }
    }
        
    if (are_ex_trivially_equal(ebasis,basis) &&
        are_ex_trivially_equal(eexponent,exponent)) {
        return this->hold();
    }
    return (new power(ebasis, eexponent))->setflag(status_flags::dynallocated |
                                                   status_flags::evaluated);
}

ex power::evalf(int level) const
{
    debugmsg("power evalf",LOGLEVEL_MEMBER_FUNCTION);

    ex ebasis;
    ex eexponent;
    
    if (level==1) {
        ebasis=basis;
        eexponent=exponent;
    } else if (level == -max_recursion_level) {
        throw(std::runtime_error("max recursion level reached"));
    } else {
        ebasis=basis.evalf(level-1);
        eexponent=exponent.evalf(level-1);
    }

    return power(ebasis,eexponent);
}

ex power::subs(lst const & ls, lst const & lr) const
{
    ex const & subsed_basis=basis.subs(ls,lr);
    ex const & subsed_exponent=exponent.subs(ls,lr);

    if (are_ex_trivially_equal(basis,subsed_basis)&&
        are_ex_trivially_equal(exponent,subsed_exponent)) {
        return *this;
    }
    
    return power(subsed_basis, subsed_exponent);
}

ex power::simplify_ncmul(exvector const & v) const
{
    return basic::simplify_ncmul(v);
}

// protected

int power::compare_same_type(basic const & other) const
{
    GINAC_ASSERT(is_exactly_of_type(other, power));
    power const & o=static_cast<power const &>(const_cast<basic &>(other));

    int cmpval;
    cmpval=basis.compare(o.basis);
    if (cmpval==0) {
        return exponent.compare(o.exponent);
    }
    return cmpval;
}

unsigned power::return_type(void) const
{
    return basis.return_type();
}
   
unsigned power::return_type_tinfo(void) const
{
    return basis.return_type_tinfo();
}

ex power::expand(unsigned options) const
{
    ex expanded_basis=basis.expand(options);

    if (!is_ex_exactly_of_type(exponent,numeric)||
        !ex_to_numeric(exponent).is_integer()) {
        if (are_ex_trivially_equal(basis,expanded_basis)) {
            return this->hold();
        } else {
            return (new power(expanded_basis,exponent))->
                    setflag(status_flags::dynallocated);
        }
    }

    // integer numeric exponent
    numeric const & num_exponent=ex_to_numeric(exponent);
    int int_exponent = num_exponent.to_int();

    if (int_exponent > 0 && is_ex_exactly_of_type(expanded_basis,add)) {
        return expand_add(ex_to_add(expanded_basis), int_exponent);
    }

    if (is_ex_exactly_of_type(expanded_basis,mul)) {
        return expand_mul(ex_to_mul(expanded_basis), num_exponent);
    }

    // cannot expand further
    if (are_ex_trivially_equal(basis,expanded_basis)) {
        return this->hold();
    } else {
        return (new power(expanded_basis,exponent))->
               setflag(status_flags::dynallocated);
    }
}

//////////
// new virtual functions which can be overridden by derived classes
//////////

// none

//////////
// non-virtual functions in this class
//////////

ex power::expand_add(add const & a, int const n) const
{
    // expand a^n where a is an add and n is an integer

    if (n==2) {
        return expand_add_2(a);
    }
    
    int m=a.nops();
    exvector sum;
    sum.reserve((n+1)*(m-1));
    intvector k(m-1);
    intvector k_cum(m-1); // k_cum[l]:=sum(i=0,l,k[l]);
    intvector upper_limit(m-1);
    int l;
    
    for (int l=0; l<m-1; l++) {
        k[l]=0;
        k_cum[l]=0;
        upper_limit[l]=n;
    }

    while (1) {
        exvector term;
        term.reserve(m+1);
        for (l=0; l<m-1; l++) {
            ex const & b=a.op(l);
            GINAC_ASSERT(!is_ex_exactly_of_type(b,add));
            GINAC_ASSERT(!is_ex_exactly_of_type(b,power)||
                   !is_ex_exactly_of_type(ex_to_power(b).exponent,numeric)||
                   !ex_to_numeric(ex_to_power(b).exponent).is_pos_integer());
            if (is_ex_exactly_of_type(b,mul)) {
                term.push_back(expand_mul(ex_to_mul(b),numeric(k[l])));
            } else {
                term.push_back(power(b,k[l]));
            }
        }

        ex const & b=a.op(l);
        GINAC_ASSERT(!is_ex_exactly_of_type(b,add));
        GINAC_ASSERT(!is_ex_exactly_of_type(b,power)||
               !is_ex_exactly_of_type(ex_to_power(b).exponent,numeric)||
               !ex_to_numeric(ex_to_power(b).exponent).is_pos_integer());
        if (is_ex_exactly_of_type(b,mul)) {
            term.push_back(expand_mul(ex_to_mul(b),numeric(n-k_cum[m-2])));
        } else {
            term.push_back(power(b,n-k_cum[m-2]));
        }

        numeric f=binomial(numeric(n),numeric(k[0]));
        for (l=1; l<m-1; l++) {
            f=f*binomial(numeric(n-k_cum[l-1]),numeric(k[l]));
        }
        term.push_back(f);

        /*
        cout << "begin term" << endl;
        for (int i=0; i<m-1; i++) {
            cout << "k[" << i << "]=" << k[i] << endl;
            cout << "k_cum[" << i << "]=" << k_cum[i] << endl;
            cout << "upper_limit[" << i << "]=" << upper_limit[i] << endl;
        }
        for (exvector::const_iterator cit=term.begin(); cit!=term.end(); ++cit) {
            cout << *cit << endl;
        }
        cout << "end term" << endl;
        */

        // TODO: optimize this
        sum.push_back((new mul(term))->setflag(status_flags::dynallocated));
        
        // increment k[]
        l=m-2;
        while ((l>=0)&&((++k[l])>upper_limit[l])) {
            k[l]=0;    
            l--;
        }
        if (l<0) break;

        // recalc k_cum[] and upper_limit[]
        if (l==0) {
            k_cum[0]=k[0];
        } else {
            k_cum[l]=k_cum[l-1]+k[l];
        }
        for (int i=l+1; i<m-1; i++) {
            k_cum[i]=k_cum[i-1]+k[i];
        }

        for (int i=l+1; i<m-1; i++) {
            upper_limit[i]=n-k_cum[i-1];
        }   
    }
    return (new add(sum))->setflag(status_flags::dynallocated);
}

/*
ex power::expand_add_2(add const & a) const
{
    // special case: expand a^2 where a is an add

    epvector sum;
    sum.reserve((a.seq.size()*(a.seq.size()+1))/2);
    epvector::const_iterator last=a.seq.end();

    for (epvector::const_iterator cit0=a.seq.begin(); cit0!=last; ++cit0) {
        ex const & b=a.recombine_pair_to_ex(*cit0);
        GINAC_ASSERT(!is_ex_exactly_of_type(b,add));
        GINAC_ASSERT(!is_ex_exactly_of_type(b,power)||
               !is_ex_exactly_of_type(ex_to_power(b).exponent,numeric)||
               !ex_to_numeric(ex_to_power(b).exponent).is_pos_integer());
        if (is_ex_exactly_of_type(b,mul)) {
            sum.push_back(a.split_ex_to_pair(expand_mul(ex_to_mul(b),numTWO())));
        } else {
            sum.push_back(a.split_ex_to_pair((new power(b,exTWO()))->
                                              setflag(status_flags::dynallocated)));
        }
        for (epvector::const_iterator cit1=cit0+1; cit1!=last; ++cit1) {
            sum.push_back(a.split_ex_to_pair((new mul(a.recombine_pair_to_ex(*cit0),
                                                      a.recombine_pair_to_ex(*cit1)))->
                                              setflag(status_flags::dynallocated),
                                             exTWO()));
        }
    }

    GINAC_ASSERT(sum.size()==(a.seq.size()*(a.seq.size()+1))/2);

    return (new add(sum))->setflag(status_flags::dynallocated);
}
*/

ex power::expand_add_2(add const & a) const
{
    // special case: expand a^2 where a is an add

    epvector sum;
    unsigned a_nops=a.nops();
    sum.reserve((a_nops*(a_nops+1))/2);
    epvector::const_iterator last=a.seq.end();

    // power(+(x,...,z;c),2)=power(+(x,...,z;0),2)+2*c*+(x,...,z;0)+c*c
    // first part: ignore overall_coeff and expand other terms
    for (epvector::const_iterator cit0=a.seq.begin(); cit0!=last; ++cit0) {
        ex const & r=(*cit0).rest;
        ex const & c=(*cit0).coeff;
        
        GINAC_ASSERT(!is_ex_exactly_of_type(r,add));
        GINAC_ASSERT(!is_ex_exactly_of_type(r,power)||
               !is_ex_exactly_of_type(ex_to_power(r).exponent,numeric)||
               !ex_to_numeric(ex_to_power(r).exponent).is_pos_integer()||
               !is_ex_exactly_of_type(ex_to_power(r).basis,add)||
               !is_ex_exactly_of_type(ex_to_power(r).basis,mul)||
               !is_ex_exactly_of_type(ex_to_power(r).basis,power));

        if (are_ex_trivially_equal(c,exONE())) {
            if (is_ex_exactly_of_type(r,mul)) {
                sum.push_back(expair(expand_mul(ex_to_mul(r),numTWO()),exONE()));
            } else {
                sum.push_back(expair((new power(r,exTWO()))->setflag(status_flags::dynallocated),
                                     exONE()));
            }
        } else {
            if (is_ex_exactly_of_type(r,mul)) {
                sum.push_back(expair(expand_mul(ex_to_mul(r),numTWO()),
                                     ex_to_numeric(c).power_dyn(numTWO())));
            } else {
                sum.push_back(expair((new power(r,exTWO()))->setflag(status_flags::dynallocated),
                                     ex_to_numeric(c).power_dyn(numTWO())));
            }
        }
            
        for (epvector::const_iterator cit1=cit0+1; cit1!=last; ++cit1) {
            ex const & r1=(*cit1).rest;
            ex const & c1=(*cit1).coeff;
            sum.push_back(a.combine_ex_with_coeff_to_pair((new mul(r,r1))->setflag(status_flags::dynallocated),
                                                          numTWO().mul(ex_to_numeric(c)).mul_dyn(ex_to_numeric(c1))));
        }
    }

    GINAC_ASSERT(sum.size()==(a.seq.size()*(a.seq.size()+1))/2);

    // second part: add terms coming from overall_factor (if != 0)
    if (!a.overall_coeff.is_equal(exZERO())) {
        for (epvector::const_iterator cit=a.seq.begin(); cit!=a.seq.end(); ++cit) {
            sum.push_back(a.combine_pair_with_coeff_to_pair(*cit,ex_to_numeric(a.overall_coeff).mul_dyn(numTWO())));
        }
        sum.push_back(expair(ex_to_numeric(a.overall_coeff).power_dyn(numTWO()),exONE()));
    }
        
    GINAC_ASSERT(sum.size()==(a_nops*(a_nops+1))/2);
    
    return (new add(sum))->setflag(status_flags::dynallocated);
}

ex power::expand_mul(mul const & m, numeric const & n) const
{
    // expand m^n where m is a mul and n is and integer

    if (n.is_equal(numZERO())) {
        return exONE();
    }
    
    epvector distrseq;
    distrseq.reserve(m.seq.size());
    epvector::const_iterator last=m.seq.end();
    epvector::const_iterator cit=m.seq.begin();
    while (cit!=last) {
        if (is_ex_exactly_of_type((*cit).rest,numeric)) {
            distrseq.push_back(m.combine_pair_with_coeff_to_pair(*cit,n));
        } else {
            // it is safe not to call mul::combine_pair_with_coeff_to_pair()
            // since n is an integer
            distrseq.push_back(expair((*cit).rest,
                                      ex_to_numeric((*cit).coeff).mul(n)));
        }
        ++cit;
    }
    return (new mul(distrseq,ex_to_numeric(m.overall_coeff).power_dyn(n)))
                 ->setflag(status_flags::dynallocated);
}

/*
ex power::expand_commutative_3(ex const & basis, numeric const & exponent,
                             unsigned options) const
{
    // obsolete

    exvector distrseq;
    epvector splitseq;

    add const & addref=static_cast<add const &>(*basis.bp);

    splitseq=addref.seq;
    splitseq.pop_back();
    ex first_operands=add(splitseq);
    ex last_operand=addref.recombine_pair_to_ex(*(addref.seq.end()-1));
    
    int n=exponent.to_int();
    for (int k=0; k<=n; k++) {
        distrseq.push_back(binomial(n,k)*power(first_operands,numeric(k))*
                           power(last_operand,numeric(n-k)));
    }
    return ex((new add(distrseq))->setflag(status_flags::sub_expanded |
                                           status_flags::expanded |
                                           status_flags::dynallocated  )).
           expand(options);
}
*/

/*
ex power::expand_noncommutative(ex const & basis, numeric const & exponent,
                                unsigned options) const
{
    ex rest_power=ex(power(basis,exponent.add(numMINUSONE()))).
                  expand(options | expand_options::internal_do_not_expand_power_operands);

    return ex(mul(rest_power,basis),0).
           expand(options | expand_options::internal_do_not_expand_mul_operands);
}
*/

//////////
// static member variables
//////////

// protected

unsigned power::precedence=60;

//////////
// global constants
//////////

const power some_power;
type_info const & typeid_power=typeid(some_power);

#ifndef NO_GINAC_NAMESPACE
} // namespace GiNaC
#endif // ndef NO_GINAC_NAMESPACE
