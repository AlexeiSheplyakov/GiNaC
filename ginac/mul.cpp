/** @file mul.cpp
 *
 *  Implementation of GiNaC's products of expressions. */

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
#include <stdexcept>

#include "mul.h"
#include "add.h"
#include "power.h"

//////////
// default constructor, destructor, copy constructor assignment operator and helpers
//////////

// public

mul::mul()
{
    debugmsg("mul default constructor",LOGLEVEL_CONSTRUCT);
    tinfo_key = TINFO_mul;
}

mul::~mul()
{
    debugmsg("mul destructor",LOGLEVEL_DESTRUCT);
    destroy(0);
}

mul::mul(mul const & other)
{
    debugmsg("mul copy constructor",LOGLEVEL_CONSTRUCT);
    copy(other);
}

mul const & mul::operator=(mul const & other)
{
    debugmsg("mul operator=",LOGLEVEL_ASSIGNMENT);
    if (this != &other) {
        destroy(1);
        copy(other);
    }
    return *this;
}

// protected

void mul::copy(mul const & other)
{
    expairseq::copy(other);
}

void mul::destroy(bool call_parent)
{
    if (call_parent) expairseq::destroy(call_parent);
}

//////////
// other constructors
//////////

// public

mul::mul(ex const & lh, ex const & rh)
{
    debugmsg("mul constructor from ex,ex",LOGLEVEL_CONSTRUCT);
    tinfo_key = TINFO_mul;
    overall_coeff=exONE();
    construct_from_2_ex(lh,rh);
    ASSERT(is_canonical());
}

mul::mul(exvector const & v)
{
    debugmsg("mul constructor from exvector",LOGLEVEL_CONSTRUCT);
    tinfo_key = TINFO_mul;
    overall_coeff=exONE();
    construct_from_exvector(v);
    ASSERT(is_canonical());
}

/*
mul::mul(epvector const & v, bool do_not_canonicalize)
{
    debugmsg("mul constructor from epvector,bool",LOGLEVEL_CONSTRUCT);
    tinfo_key = TINFO_mul;
    if (do_not_canonicalize) {
        seq=v;
#ifdef EXPAIRSEQ_USE_HASHTAB
        combine_same_terms(); // to build hashtab
#endif // def EXPAIRSEQ_USE_HASHTAB
    } else {
        construct_from_epvector(v);
    }
    ASSERT(is_canonical());
}
*/

mul::mul(epvector const & v)
{
    debugmsg("mul constructor from epvector",LOGLEVEL_CONSTRUCT);
    tinfo_key = TINFO_mul;
    overall_coeff=exONE();
    construct_from_epvector(v);
    ASSERT(is_canonical());
}

mul::mul(epvector const & v, ex const & oc)
{
    debugmsg("mul constructor from epvector,ex",LOGLEVEL_CONSTRUCT);
    tinfo_key = TINFO_mul;
    overall_coeff=oc;
    construct_from_epvector(v);
    ASSERT(is_canonical());
}

mul::mul(epvector * vp, ex const & oc)
{
    debugmsg("mul constructor from epvector *,ex",LOGLEVEL_CONSTRUCT);
    tinfo_key = TINFO_mul;
    ASSERT(vp!=0);
    overall_coeff=oc;
    construct_from_epvector(*vp);
    delete vp;
    ASSERT(is_canonical());
}

mul::mul(ex const & lh, ex const & mh, ex const & rh)
{
    debugmsg("mul constructor from ex,ex,ex",LOGLEVEL_CONSTRUCT);
    tinfo_key = TINFO_mul;
    exvector factors;
    factors.reserve(3);
    factors.push_back(lh);
    factors.push_back(mh);
    factors.push_back(rh);
    overall_coeff=exONE();
    construct_from_exvector(factors);
    ASSERT(is_canonical());
}

//////////
// functions overriding virtual functions from bases classes
//////////

// public

basic * mul::duplicate() const
{
    debugmsg("mul duplicate",LOGLEVEL_ASSIGNMENT);
    return new mul(*this);
}

bool mul::info(unsigned inf) const
{
    // TODO: optimize
    if (inf==info_flags::polynomial || inf==info_flags::integer_polynomial || inf==info_flags::rational_polynomial || inf==info_flags::rational_function) {
        for (epvector::const_iterator it=seq.begin(); it!=seq.end(); ++it) {
            if (!(recombine_pair_to_ex(*it).info(inf)))
                return false;
        }
        return true;
    } else {
        return expairseq::info(inf);
    }
}

typedef vector<int> intvector;

int mul::degree(symbol const & s) const
{
    int deg_sum=0;
    for (epvector::const_iterator cit=seq.begin(); cit!=seq.end(); ++cit) {
        deg_sum+=(*cit).rest.degree(s) * ex_to_numeric((*cit).coeff).to_int();
    }
    return deg_sum;
}

int mul::ldegree(symbol const & s) const
{
    int deg_sum=0;
    for (epvector::const_iterator cit=seq.begin(); cit!=seq.end(); ++cit) {
        deg_sum+=(*cit).rest.ldegree(s) * ex_to_numeric((*cit).coeff).to_int();
    }
    return deg_sum;
}

ex mul::coeff(symbol const & s, int const n) const
{
    exvector coeffseq;
    coeffseq.reserve(seq.size()+1);
    
    if (n==0) {
        // product of individual coeffs
        // if a non-zero power of s is found, the resulting product will be 0
        epvector::const_iterator it=seq.begin();
        while (it!=seq.end()) {
            coeffseq.push_back(recombine_pair_to_ex(*it).coeff(s,n));
            ++it;
        }
        coeffseq.push_back(overall_coeff);
        return (new mul(coeffseq))->setflag(status_flags::dynallocated);
    }
         
    epvector::const_iterator it=seq.begin();
    bool coeff_found=0;
    while (it!=seq.end()) {
        ex t=recombine_pair_to_ex(*it);
        ex c=t.coeff(s,n);
        if (!c.is_zero()) {
            coeffseq.push_back(c);
            coeff_found=1;
        } else {
            coeffseq.push_back(t);
        }
        ++it;
    }
    if (coeff_found) {
        coeffseq.push_back(overall_coeff);
        return (new mul(coeffseq))->setflag(status_flags::dynallocated);
    }
    
    return exZERO();
}

/*
ex mul::eval(int level) const
{
    // simplifications: *(...,x,(c1,1),(c2,1)) -> *(...,x,(c1*c2,1)) (c1, c2 numeric(), move pairs to end first)
    //                  *(...,x,1) -> *(...,x)
    //                  *(...,x,0) -> 0
    //                  *(+(x,y,...),(c,1)) -> *(+(*(x,c),*(y,c),...)) (c numeric())
    //                  *(x) -> x
    //                  *() -> 1

    debugmsg("mul eval",LOGLEVEL_MEMBER_FUNCTION);

    if ((level==1)&&(flags & status_flags::evaluated)) {
#ifdef DOASSERT
        for (epvector::const_iterator cit=seq.begin(); cit!=seq.end(); ++cit) {
            ASSERT((!is_ex_exactly_of_type((*cit).rest,mul))||
                   (!(ex_to_numeric((*cit).coeff).is_integer())));
        }

        // test if all numerics were moved to the end and
        // all numerics with coeff 1 to the very end
        if (seq.size()!=0) {
            epvector::const_iterator cit=seq.end();
            bool all_coeff_1=true;
            bool all_numeric=true;
            do {
                cit--;
                if (is_ex_exactly_of_type((*cit).rest,numeric)) {
                    ASSERT(all_numeric);
                    if ((*cit).coeff.is_equal(exONE())) {
                        ASSERT(all_coeff_1);
                    } else {
                        all_coeff_1=false;
                    }
                } else {
                    all_numeric=false;
                }
            } while (cit!=seq.begin());
        }
#endif // def DOASSERT    
        return *this;
    }

    epvector newseq;
    epvector::iterator it1,it2;
    bool seq_copied=false;

    epvector * evaled_seqp=evalchildren(level);
    if (evaled_seqp!=0) {
        // do more evaluation later
        return (new mul(evaled_seqp))->setflag(status_flags::dynallocated);
    }

    // combine pairs with coeff 1 (all numerics should be at end, assert below)
    if (seq.size()>1) {
        // count number of pairs with coeff 1
        unsigned num_coeff_1=0;
        bool still_numeric=true;
        epvector::const_iterator cit=seq.end();
        unsigned first_pos;
        unsigned second_pos;
        do {
            cit--;
            if (is_ex_exactly_of_type((*cit).rest,numeric)) {
                if ((*cit).coeff.is_equal(exONE())) {
                    num_coeff_1++;
                }
            } else {
                still_numeric=false;
            }
        } while ((cit!=seq.begin())&&still_numeric);
        if (num_coeff_1>1) {
            newseq=seq;
            
    }
    
    
#ifdef DOASSERT
    for (epvector::const_iterator cit=seq.begin(); cit!=seq.end(); ++cit) {
        ASSERT((!is_ex_exactly_of_type((*cit).rest,mul))||
               (!(ex_to_numeric((*cit).coeff).is_integer())));
    }

    // test if all numerics were moved to the end and
    // all numerics with coeff 1 to the very end
    if (seq.size()!=0) {
        epvector::const_iterator cit=seq.end();
        bool all_coeff_1=true;
        bool all_numeric=true;
        do {
            cit--;
            if (is_ex_exactly_of_type((*cit).rest,numeric)) {
                ASSERT(all_numeric);
                if ((*cit).coeff.is_equal(exONE())) {
                    ASSERT(all_coeff_1);
                } else {
                    all_coeff_1=false;
                }
            } else {
                all_numeric=false;
            }
        } while (cit!=seq.begin());
    }
#endif // def DOASSERT
    
    if (flags & status_flags::evaluated) {
        return *this;
    }
    
    expair const & last_expair=*(seq.end()-1);
    expair const & next_to_last_expair=*(seq.end()-2);
    int seq_size = seq.size();

    // *(...,x,(c1,1),(c2,1)) -> *(...,x,(c1*c2,1)) (c1, c2 numeric())
    if ((!seq_copied) && (seq_size>=2) &&
        is_ex_exactly_of_type(last_expair.rest,numeric) &&
        ex_to_numeric(last_expair.coeff).is_equal(numONE()) &&
        is_ex_exactly_of_type(next_to_last_expair.rest,numeric) &&
        ex_to_numeric(next_to_last_expair.coeff).is_equal(numONE()) ) {
        newseq=seq;
        seq_copied=true;
        it2=newseq.end()-1;
        it1=it2-1;
    }
    while (seq_copied && (newseq.size()>=2) &&
           is_ex_exactly_of_type((*it1).rest,numeric) &&
           ex_to_numeric((*it1).coeff).is_equal(numONE()) &&
           is_ex_exactly_of_type((*it2).rest,numeric) &&
           ex_to_numeric((*it2).coeff).is_equal(numONE()) ) {
        *it1=expair(ex_to_numeric((*it1).rest).mul_dyn(ex_to_numeric((*it2).rest)),exONE());
        newseq.pop_back();
        it2=newseq.end()-1;
        it1=it2-1;
    }

    // *(...,x,1) -> *(...,x)
    if ((!seq_copied) && (seq_size>=1) &&
        (is_ex_exactly_of_type(last_expair.rest,numeric)) &&
        (ex_to_numeric(last_expair.rest).compare(numONE())==0)) {
        newseq=seq;
        seq_copied=true;
        it2=newseq.end()-1;
    }
    if (seq_copied && (newseq.size()>=1) &&
        (is_ex_exactly_of_type((*it2).rest,numeric)) &&
        (ex_to_numeric((*it2).rest).compare(numONE())==0)) {
        newseq.pop_back();
        it2=newseq.end()-1;
    }

    // *(...,x,0) -> 0
    if ((!seq_copied) && (seq_size>=1) &&
        (is_ex_exactly_of_type(last_expair.rest,numeric)) &&
        (ex_to_numeric(last_expair.rest).is_zero())) {
        return exZERO();
    }
    if (seq_copied && (newseq.size()>=1) &&
        (is_ex_exactly_of_type((*it2).rest,numeric)) &&
        (ex_to_numeric((*it2).rest).is_zero())) {
        return exZERO();
    }

    // *(+(x,y,...),c) -> +(*(x,c),*(y,c),...) (c numeric(), no powers of +())
    if ((!seq_copied) && (seq_size==2) &&
        is_ex_exactly_of_type(next_to_last_expair.rest,add) &&
        is_ex_exactly_of_type(last_expair.rest,numeric) &&
        ex_to_numeric(last_expair.coeff).is_equal(numONE()) &&
        (ex_to_numeric(next_to_last_expair.coeff).compare(numONE())==0)) {
        add const & addref=ex_to_add(next_to_last_expair.rest);
        epvector distrseq;
        distrseq.reserve(addref.seq.size());
        for (epvector::const_iterator cit=addref.seq.begin(); cit!=addref.seq.end(); ++cit) {
            distrseq.push_back(addref.combine_pair_with_coeff_to_pair(*cit,
                                   last_expair.rest));
        }
        // special treatment for the last element if it is numeric (to
        // avoid terms like (2/3)*(3/2)) is no longer necessary, this
        // is handled in add::combine_pair_with_coeff_to_pair()
        return (new add(distrseq,1))->setflag(status_flags::dynallocated  |
                                              status_flags::evaluated );
    }
    if (seq_copied && (newseq.size()==2) &&
        is_ex_exactly_of_type(newseq[0].rest,add) &&
        is_ex_exactly_of_type(newseq[1].rest,numeric) &&
        ex_to_numeric(newseq[1].coeff).is_equal(numONE()) &&
        (ex_to_numeric(newseq[0].coeff).compare(numONE())==0)) {
        add const & addref=ex_to_add(newseq[0].rest);
        epvector distrseq;
        distrseq.reserve(addref.seq.size());
        for (epvector::const_iterator cit=addref.seq.begin(); cit!=addref.seq.end(); ++cit) {
            distrseq.push_back(addref.combine_pair_with_coeff_to_pair(*cit,
                                   newseq[1].rest));
        }
        // special treatment for the last element if it is numeric (to
        // avoid terms like (2/3)*(3/2)) is no longer necessary, this
        // is handled in add::combine_pair_with_coeff_to_pair()
        return (new add(distrseq,1))->setflag(status_flags::dynallocated  |
                                              status_flags::evaluated );
    }
    
    // *() -> 1
    if ((!seq_copied) && (seq_size==0)) {
        return exONE();
    } else if (seq_copied && (newseq.size()==0)) {
        return exONE();
    }

    // *(x) -> x
    if ((!seq_copied) && (seq_size==1)) {
        return recombine_pair_to_ex(*(seq.begin()));
    } else if (seq_copied && (newseq.size()==1)) {
        return recombine_pair_to_ex(*(newseq.begin()));
    }

    if (!seq_copied) return this->hold();

    return (new mul(newseq,1))->setflag(status_flags::dynallocated  |
                                        status_flags::evaluated );
}
*/

ex mul::eval(int level) const
{
    // simplifications  *(...,x;0) -> 0
    //                  *(+(x,y,...);c) -> *(+(*(x,c),*(y,c),...)) (c numeric())
    //                  *(x;1) -> x
    //                  *(;c) -> c

    debugmsg("mul eval",LOGLEVEL_MEMBER_FUNCTION);

    epvector * evaled_seqp=evalchildren(level);
    if (evaled_seqp!=0) {
        // do more evaluation later
        return (new mul(evaled_seqp,overall_coeff))->
                   setflag(status_flags::dynallocated);
    }

#ifdef DOASSERT
    for (epvector::const_iterator cit=seq.begin(); cit!=seq.end(); ++cit) {
        ASSERT((!is_ex_exactly_of_type((*cit).rest,mul))||
               (!(ex_to_numeric((*cit).coeff).is_integer())));
        ASSERT(!((*cit).is_numeric_with_coeff_1()));
        if (is_ex_exactly_of_type(recombine_pair_to_ex(*cit),numeric)) {
            printtree(cerr,0);
        }
        ASSERT(!is_ex_exactly_of_type(recombine_pair_to_ex(*cit),numeric));
        /* for paranoia */
        expair p=split_ex_to_pair(recombine_pair_to_ex(*cit));
        ASSERT(p.rest.is_equal((*cit).rest));
        ASSERT(p.coeff.is_equal((*cit).coeff));
        /* end paranoia */
    }
#endif // def DOASSERT

    if (flags & status_flags::evaluated) {
        ASSERT(seq.size()>0);
        ASSERT((seq.size()>1)||!overall_coeff.is_equal(exONE()));
        return *this;
    }

    int seq_size=seq.size();
    if (overall_coeff.is_equal(exZERO())) {
        // *(...,x;0) -> 0
        return exZERO();
    } else if (seq_size==0) {
        // *(;c) -> c
        return overall_coeff;
    } else if ((seq_size==1)&&overall_coeff.is_equal(exONE())) {
        // *(x;1) -> x
        return recombine_pair_to_ex(*(seq.begin()));
    } else if ((seq_size==1) &&
               is_ex_exactly_of_type((*seq.begin()).rest,add) &&
               ex_to_numeric((*seq.begin()).coeff).is_equal(numONE())) {
        // *(+(x,y,...);c) -> +(*(x,c),*(y,c),...) (c numeric(), no powers of +())
        add const & addref=ex_to_add((*seq.begin()).rest);
        epvector distrseq;
        distrseq.reserve(addref.seq.size());
        for (epvector::const_iterator cit=addref.seq.begin(); cit!=addref.seq.end(); ++cit) {
            distrseq.push_back(addref.combine_pair_with_coeff_to_pair(*cit,
                                   overall_coeff));
        }
        return (new add(distrseq,
                        ex_to_numeric(addref.overall_coeff).
                        mul_dyn(ex_to_numeric(overall_coeff))))
            ->setflag(status_flags::dynallocated  |
                      status_flags::evaluated );
    }
    return this->hold();
}

/*
ex mul::eval(int level) const
{
    // simplifications: *(...,x,c1,c2) -> *(...,x,c1*c2) (c1, c2 numeric())
    //                  *(...,(c1,c2)) -> (...,(c1^c2,1)) (normalize)
    //                  *(...,x,1) -> +(...,x)
    //                  *(...,x,0) -> 0
    //                  *(+(x,y,...),c) -> *(+(*(x,c),*(y,c),...)) (c numeric())
    //                  *(x) -> x
    //                  *() -> 1

    debugmsg("mul eval",LOGLEVEL_MEMBER_FUNCTION);

    epvector newseq=seq;
    epvector::iterator it1,it2;
    
    // *(...,x,c1,c2) -> *(...,x,c1*c2) (c1, c2 numeric())
    it2=newseq.end()-1;
    it1=it2-1;
    while ((newseq.size()>=2)&&is_exactly_of_type(*(*it1).rest.bp,numeric)&&
                               is_exactly_of_type(*(*it2).rest.bp,numeric)) {
        *it1=expair(ex_to_numeric((*it1).rest).power(ex_to_numeric((*it1).coeff))
                    .mul(ex_to_numeric((*it2).rest).power(ex_to_numeric((*it2).coeff))),exONE());
        newseq.pop_back();
        it2=newseq.end()-1;
        it1=it2-1;
    }

    if ((newseq.size()>=1)&&is_exactly_of_type(*(*it2).rest.bp,numeric)) {
        // *(...,(c1,c2)) -> (...,(c1^c2,1)) (normalize)
        *it2=expair(ex_to_numeric((*it2).rest).power(ex_to_numeric((*it2).coeff)),exONE());
        // *(...,x,1) -> *(...,x)
        if (static_cast<numeric &>(*(*it2).rest.bp).compare(numONE())==0) {
            newseq.pop_back();
            it2=newseq.end()-1;
        }
    }

    // *(...,x,0) -> 0
    if ((newseq.size()>=1)&&is_exactly_of_type(*(*it2).rest.bp,numeric)) {
        if (static_cast<numeric &>(*(*it2).rest.bp).is_zero()==0) {
            return exZERO();
        }
    }

    // *(+(x,y,...),c) -> +(*(x,c),*(y,c),...) (c numeric(), no powers of +())
    if ((newseq.size()==2)&&is_ex_exactly_of_type(newseq[0].rest,add)&&
        is_ex_exactly_of_type(newseq[1].rest,numeric)&&
        (ex_to_numeric(newseq[0].coeff).compare(numONE())==0)) {
        add const & addref=ex_to_add(newseq[0].rest);
        numeric const & numref=ex_to_numeric(newseq[1].rest);
        epvector distrseq;
        distrseq.reserve(addref.seq.size());
        for (epvector::const_iterator cit=addref.seq.begin(); cit!=addref.seq.end(); ++cit) {
            distrseq.push_back(expair((*cit).rest,ex_to_numeric((*cit).coeff).mul(numref)));
        }
        return (new add(distrseq,1))->setflag(status_flags::dynallocated  |
                                              status_flags::evaluated );
    }
    
    if (newseq.size()==0) {
        // *() -> 1
        return exONE();
    } else if (newseq.size()==1) {
        // *(x) -> x
        return recombine_pair_to_ex(*newseq.begin());
    }

    return (new mul(newseq,1))->setflag(status_flags::dynallocated  |
                                        status_flags::evaluated );
}
*/

exvector mul::get_indices(void) const
{
    // return union of indices of factors
    exvector iv;
    for (epvector::const_iterator cit=seq.begin(); cit!=seq.end(); ++cit) {
        exvector subiv=(*cit).rest.get_indices();
        iv.reserve(iv.size()+subiv.size());
        for (exvector::const_iterator cit2=subiv.begin(); cit2!=subiv.end(); ++cit2) {
            iv.push_back(*cit2);
        }
    }
    return iv;
}

ex mul::simplify_ncmul(exvector const & v) const
{
    throw(std::logic_error("mul::simplify_ncmul() should never have been called!"));
}

// protected

int mul::compare_same_type(basic const & other) const
{
    return expairseq::compare_same_type(other);
}

bool mul::is_equal_same_type(basic const & other) const
{
    return expairseq::is_equal_same_type(other);
}

unsigned mul::return_type(void) const
{
    if (seq.size()==0) {
        // mul without factors: should not happen, but commutes
        return return_types::commutative;
    }

    bool all_commutative=1;
    unsigned rt;
    epvector::const_iterator cit_noncommutative_element; // point to first found nc element

    for (epvector::const_iterator cit=seq.begin(); cit!=seq.end(); ++cit) {
        rt=(*cit).rest.return_type();
        if (rt==return_types::noncommutative_composite) return rt; // one ncc -> mul also ncc
        if ((rt==return_types::noncommutative)&&(all_commutative)) {
            // first nc element found, remember position
            cit_noncommutative_element=cit;
            all_commutative=0;
        }
        if ((rt==return_types::noncommutative)&&(!all_commutative)) {
	        // another nc element found, compare type_infos
            if ((*cit_noncommutative_element).rest.return_type_tinfo()!=(*cit).rest.return_type_tinfo()) {
	        // diffent types -> mul is ncc
	        return return_types::noncommutative_composite;
            }
        }
    }
    // all factors checked
    return all_commutative ? return_types::commutative : return_types::noncommutative;
}
   
unsigned mul::return_type_tinfo(void) const
{
    if (seq.size()==0) {
        // mul without factors: should not happen
        return tinfo_key;
    }
    // return type_info of first noncommutative element
    for (epvector::const_iterator cit=seq.begin(); cit!=seq.end(); ++cit) {
        if ((*cit).rest.return_type()==return_types::noncommutative) {
            return (*cit).rest.return_type_tinfo();
        }
    }
    // no noncommutative element found, should not happen
    return tinfo_key;
}

ex mul::thisexpairseq(epvector const & v, ex const & oc) const
{
    return (new mul(v,oc))->setflag(status_flags::dynallocated);
}

ex mul::thisexpairseq(epvector * vp, ex const & oc) const
{
    return (new mul(vp,oc))->setflag(status_flags::dynallocated);
}

expair mul::split_ex_to_pair(ex const & e) const
{
    if (is_ex_exactly_of_type(e,power)) {
        power const & powerref=ex_to_power(e);
        if (is_ex_exactly_of_type(powerref.exponent,numeric)) {
            return expair(powerref.basis,powerref.exponent);
        }
    }
    return expair(e,exONE());
}
    
expair mul::combine_ex_with_coeff_to_pair(ex const & e,
                                          ex const & c) const
{
    // to avoid duplication of power simplification rules,
    // we create a temporary power object
    // otherwise it would be hard to correctly simplify
    // expression like (4^(1/3))^(3/2)
    if (are_ex_trivially_equal(c,exONE())) {
        return split_ex_to_pair(e);
    }
    return split_ex_to_pair(power(e,c));
}
    
expair mul::combine_pair_with_coeff_to_pair(expair const & p,
                                            ex const & c) const
{
    // to avoid duplication of power simplification rules,
    // we create a temporary power object
    // otherwise it would be hard to correctly simplify
    // expression like (4^(1/3))^(3/2)
    if (are_ex_trivially_equal(c,exONE())) {
        return p;
    }
    return split_ex_to_pair(power(recombine_pair_to_ex(p),c));
}
    
ex mul::recombine_pair_to_ex(expair const & p) const
{
    // if (p.coeff.compare(exONE())==0) {
    // if (are_ex_trivially_equal(p.coeff,exONE())) {
    if (ex_to_numeric(p.coeff).is_equal(numONE())) {
        return p.rest;
    } else {
        return power(p.rest,p.coeff);
    }
}

bool mul::expair_needs_further_processing(epp it)
{
    if (is_ex_exactly_of_type((*it).rest,mul) &&
        ex_to_numeric((*it).coeff).is_integer()) {
        // combined pair is product with integer power -> expand it
        *it=split_ex_to_pair(recombine_pair_to_ex(*it));
        return true;
    }
    if (is_ex_exactly_of_type((*it).rest,numeric)) {
        expair ep=split_ex_to_pair(recombine_pair_to_ex(*it));
        if (!ep.is_equal(*it)) {
            // combined pair is a numeric power which can be simplified
            *it=ep;
            return true;
        }
        if (ex_to_numeric((*it).coeff).is_equal(numONE())) {
            // combined pair has coeff 1 and must be moved to the end
            return true;
        }
    }
    return false;
}       

ex mul::default_overall_coeff(void) const
{
    return exONE();
}

void mul::combine_overall_coeff(ex const & c)
{
    ASSERT(is_ex_exactly_of_type(overall_coeff,numeric));
    ASSERT(is_ex_exactly_of_type(c,numeric));
    overall_coeff = ex_to_numeric(overall_coeff).mul_dyn(ex_to_numeric(c));
}

void mul::combine_overall_coeff(ex const & c1, ex const & c2)
{
    ASSERT(is_ex_exactly_of_type(overall_coeff,numeric));
    ASSERT(is_ex_exactly_of_type(c1,numeric));
    ASSERT(is_ex_exactly_of_type(c2,numeric));
    overall_coeff = ex_to_numeric(overall_coeff).
                        mul_dyn(ex_to_numeric(c1).power(ex_to_numeric(c2)));
}

bool mul::can_make_flat(expair const & p) const
{
    ASSERT(is_ex_exactly_of_type(p.coeff,numeric));
    // this assertion will probably fail somewhere
    // it would require a more careful make_flat, obeying the power laws
    // probably should return true only if p.coeff is integer
    return ex_to_numeric(p.coeff).is_equal(numONE());
}

ex mul::expand(unsigned options) const
{
    exvector sub_expanded_seq;
    intvector positions_of_adds;
    intvector number_of_add_operands;

    epvector * expanded_seqp=expandchildren(options);

    epvector const & expanded_seq = expanded_seqp==0 ? seq : *expanded_seqp;

    positions_of_adds.resize(expanded_seq.size());
    number_of_add_operands.resize(expanded_seq.size());

    int number_of_adds=0;
    int number_of_expanded_terms=1;

    unsigned current_position=0;
    epvector::const_iterator last=expanded_seq.end();
    for (epvector::const_iterator cit=expanded_seq.begin(); cit!=last; ++cit) {
        if (is_ex_exactly_of_type((*cit).rest,add)&&
            (ex_to_numeric((*cit).coeff).is_equal(numONE()))) {
            positions_of_adds[number_of_adds]=current_position;
            add const & expanded_addref=ex_to_add((*cit).rest);
            int addref_nops=expanded_addref.nops();
            number_of_add_operands[number_of_adds]=addref_nops;
            number_of_expanded_terms *= addref_nops;
            number_of_adds++;
        }
        current_position++;
    }

    if (number_of_adds==0) {
        if (expanded_seqp==0) {
            return this->setflag(status_flags::expanded);
        }
        return (new mul(expanded_seqp,overall_coeff))->
                     setflag(status_flags::dynallocated ||
                             status_flags::expanded);
    }

    exvector distrseq;
    distrseq.reserve(number_of_expanded_terms);

    intvector k;
    k.resize(number_of_adds);
    
    int l;
    for (l=0; l<number_of_adds; l++) {
        k[l]=0;
    }

    while (1) {
        epvector term;
        term=expanded_seq;
        for (l=0; l<number_of_adds; l++) {
            add const & addref=ex_to_add(expanded_seq[positions_of_adds[l]].rest);
            ASSERT(term[positions_of_adds[l]].coeff.compare(exONE())==0);
            term[positions_of_adds[l]]=split_ex_to_pair(addref.op(k[l]));
        }
        /*
        cout << "mul::expand() term begin" << endl;
        for (epvector::const_iterator cit=term.begin(); cit!=term.end(); ++cit) {
            cout << "rest" << endl;
            (*cit).rest.printtree(cout);
            cout << "coeff" << endl;
            (*cit).coeff.printtree(cout);
        }
        cout << "mul::expand() term end" << endl;
        */
        distrseq.push_back((new mul(term,overall_coeff))->
                                setflag(status_flags::dynallocated |
                                        status_flags::expanded));

        // increment k[]
        l=number_of_adds-1;
        while ((l>=0)&&((++k[l])>=number_of_add_operands[l])) {
            k[l]=0;    
            l--;
        }
        if (l<0) break;
    }

    if (expanded_seqp!=0) {
        delete expanded_seqp;
    }
    /*
    cout << "mul::expand() distrseq begin" << endl;
    for (exvector::const_iterator cit=distrseq.begin(); cit!=distrseq.end(); ++cit) {
        (*cit).printtree(cout);
    }
    cout << "mul::expand() distrseq end" << endl;
    */

    return (new add(distrseq))->setflag(status_flags::dynallocated |
                                        status_flags::expanded);
}

/*
ex mul::expand(unsigned options) const
{
    exvector sub_expanded_seq;
    intvector positions_of_adds;
    intvector number_of_add_operands;

    sub_expanded_seq.resize(seq.size());
    positions_of_adds.resize(seq.size());
    number_of_add_operands.reserve(seq.size());

    int number_of_adds=0;
    int number_of_expanded_terms=1;
    for (unsigned current_position=0; current_position<seq.size(); current_position++) {
        ex const & expanded_ex=recombine_pair_to_ex(seq[current_position]).expand(options);
        if (is_ex_exactly_of_type(expanded_ex,add)) {
            positions_of_adds[number_of_adds]=current_position;
            add const & expanded_addref=ex_to_add(expanded_ex);
            number_of_add_operands[number_of_adds]=expanded_addref.seq.size();
            number_of_expanded_terms *= expanded_addref.seq.size();
            number_of_adds++;
        }
        sub_expanded_seq.push_back(expanded_ex);
    }

    exvector distrseq;
    distrseq.reserve(number_of_expanded_terms);

    intvector k;
    k.resize(number_of_adds);
    
    int l;
    for (l=0; l<number_of_adds; l++) {
        k[l]=0;
    }

    while (1) {
        exvector term;
        term=sub_expanded_seq;
        for (l=0; l<number_of_adds; l++) {
            add const & addref=ex_to_add(sub_expanded_seq[positions_of_adds[l]]);
            term[positions_of_adds[l]]=addref.recombine_pair_to_ex(addref.seq[k[l]]);
        }
        distrseq.push_back((new mul(term))->setflag(status_flags::dynallocated |
                                                    status_flags::expanded));

        // increment k[]
        l=number_of_adds-1;
        while ((l>=0)&&((++k[l])>=number_of_add_operands[l])) {
            k[l]=0;    
            l--;
        }
        if (l<0) break;
    }

    return (new add(distrseq))->setflag(status_flags::dynallocated |
                                        status_flags::expanded);
}
*/

//////////
// new virtual functions which can be overridden by derived classes
//////////

// none

//////////
// non-virtual functions in this class
//////////

epvector * mul::expandchildren(unsigned options) const
{
    epvector::const_iterator last=seq.end();
    epvector::const_iterator cit=seq.begin();
    while (cit!=last) {
        ex const & factor=recombine_pair_to_ex(*cit);
        ex const & expanded_factor=factor.expand(options);
        if (!are_ex_trivially_equal(factor,expanded_factor)) {

            // something changed, copy seq, eval and return it
            epvector *s=new epvector;
            s->reserve(seq.size());

            // copy parts of seq which are known not to have changed
            epvector::const_iterator cit2=seq.begin();
            while (cit2!=cit) {
                s->push_back(*cit2);
                ++cit2;
            }
            // copy first changed element
            s->push_back(split_ex_to_pair(expanded_factor));
            ++cit2;
            // copy rest
            while (cit2!=last) {
                s->push_back(split_ex_to_pair(recombine_pair_to_ex(*cit2).expand(options)));
                ++cit2;
            }
            return s;
        }
        ++cit;
    }
    
    return 0; // nothing has changed
}
   
//////////
// static member variables
//////////

// protected

unsigned mul::precedence=50;


//////////
// global constants
//////////

const mul some_mul;
type_info const & typeid_mul=typeid(some_mul);


