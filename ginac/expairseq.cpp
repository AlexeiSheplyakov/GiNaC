/** @file expairseq.cpp
 *
 *  Implementation of sequences of expression pairs. */

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

#include <algorithm>
#include <string>
#include <stdexcept>

#include "expairseq.h"
#include "lst.h"
#include "debugmsg.h"
#include "utils.h"

#ifndef NO_GINAC_NAMESPACE
namespace GiNaC {
#endif // ndef NO_GINAC_NAMESPACE

#ifdef EXPAIRSEQ_USE_HASHTAB
#error "FIXME: expair_needs_further_processing not yet implemented for hashtabs, sorry. A.F."
#endif // def EXPAIRSEQ_USE_HASHTAB

//////////
// helper classes
//////////

class epp_is_less
{
public:
    bool operator()(epp const & lh, epp const & rh) const
    {
        return (*lh).is_less(*rh);
    }
};

//////////
// default constructor, destructor, copy constructor assignment operator and helpers
//////////

// public

expairseq::expairseq(expairseq const & other)
{
    debugmsg("expairseq copy constructor",LOGLEVEL_CONSTRUCT);
    copy(other);
}

expairseq const & expairseq::operator=(expairseq const & other)
{
    debugmsg("expairseq operator=",LOGLEVEL_ASSIGNMENT);
    if (this != &other) {
        destroy(1);
        copy(other);
    }
    return *this;
}

// protected

void expairseq::copy(expairseq const & other)
{
    basic::copy(other);
    seq=other.seq;
    overall_coeff=other.overall_coeff;
#ifdef EXPAIRSEQ_USE_HASHTAB
    // copy hashtab
    hashtabsize=other.hashtabsize;
    if (hashtabsize!=0) {
    hashmask=other.hashmask;
        hashtab.resize(hashtabsize);
        epvector::const_iterator osb=other.seq.begin();
        for (unsigned i=0; i<hashtabsize; ++i) {
            hashtab[i].clear();
            for (epplist::const_iterator cit=other.hashtab[i].begin();
                 cit!=other.hashtab[i].end(); ++cit) {
                hashtab[i].push_back(seq.begin()+((*cit)-osb));
            }
        }
    } else {
        hashtab.clear();
    }
#endif // def EXPAIRSEQ_USE_HASHTAB
}

//////////
// other constructors
//////////

expairseq::expairseq(ex const & lh, ex const & rh) : basic(TINFO_expairseq)
{
    debugmsg("expairseq constructor from ex,ex",LOGLEVEL_CONSTRUCT);
    construct_from_2_ex(lh,rh);
    GINAC_ASSERT(is_canonical());
}

expairseq::expairseq(exvector const & v) : basic(TINFO_expairseq)
{
    debugmsg("expairseq constructor from exvector",LOGLEVEL_CONSTRUCT);
    construct_from_exvector(v);
    GINAC_ASSERT(is_canonical());
}

/*
expairseq::expairseq(epvector const & v, bool do_not_canonicalize) :
    basic(TINFO_expairseq)
{
    debugmsg("expairseq constructor from epvector",LOGLEVEL_CONSTRUCT);
    if (do_not_canonicalize) {
        seq=v;
#ifdef EXPAIRSEQ_USE_HASHTAB
        combine_same_terms(); // to build hashtab
#endif // def EXPAIRSEQ_USE_HASHTAB
    } else {
        construct_from_epvector(v);
    }
    GINAC_ASSERT(is_canonical());
}
*/

expairseq::expairseq(epvector const & v, ex const & oc) :
    basic(TINFO_expairseq), overall_coeff(oc)
{
    debugmsg("expairseq constructor from epvector,ex",LOGLEVEL_CONSTRUCT);
    construct_from_epvector(v);
    GINAC_ASSERT(is_canonical());
}

expairseq::expairseq(epvector * vp, ex const & oc) :
    basic(TINFO_expairseq), overall_coeff(oc)
{
    debugmsg("expairseq constructor from epvector *,ex",LOGLEVEL_CONSTRUCT);
    GINAC_ASSERT(vp!=0);
    construct_from_epvector(*vp);
    delete vp;
    GINAC_ASSERT(is_canonical());
}

//////////
// functions overriding virtual functions from bases classes
//////////

// public

basic * expairseq::duplicate() const
{
    debugmsg("expairseq duplicate",LOGLEVEL_DUPLICATE);
    return new expairseq(*this);
}

bool expairseq::info(unsigned inf) const
{
    return basic::info(inf);
}

int expairseq::nops() const
{
    if (overall_coeff.is_equal(default_overall_coeff())) {
        return seq.size();
    }
    return seq.size()+1;
}

ex expairseq::op(int const i) const
{
    if (unsigned(i)<seq.size()) {
        return recombine_pair_to_ex(seq[i]);
    }
    GINAC_ASSERT(!overall_coeff.is_equal(default_overall_coeff()));
    return overall_coeff;
}

ex & expairseq::let_op(int const i)
{
    throw(std::logic_error("let_op not defined for expairseq and derived classes (add,mul,...)"));
}

ex expairseq::eval(int level) const
{
    if ((level==1)&&(flags & status_flags::evaluated)) {
        return *this;
    }

    epvector * vp=evalchildren(level);
    if (vp==0) {
        return this->hold();
    }

    return (new expairseq(vp,overall_coeff))
               ->setflag(status_flags::dynallocated |
                         status_flags::evaluated );
}

ex expairseq::evalf(int level) const
{
    return thisexpairseq(evalfchildren(level),overall_coeff);
}

ex expairseq::normal(lst &sym_lst, lst &repl_lst, int level) const
{
    ex n=thisexpairseq(normalchildren(level),overall_coeff);
    return n.bp->basic::normal(sym_lst,repl_lst,level);
}

ex expairseq::subs(lst const & ls, lst const & lr) const
{
    epvector * vp=subschildren(ls,lr);
    if (vp==0) {
        return *this;
    }
    return thisexpairseq(vp,overall_coeff);
}

// protected

int expairseq::compare_same_type(basic const & other) const
{
    GINAC_ASSERT(is_of_type(other, expairseq));
    expairseq const & o=static_cast<expairseq const &>(const_cast<basic &>(other));

    int cmpval;
    
    // compare number of elements
    if (seq.size() != o.seq.size()) {
        return (seq.size()<o.seq.size()) ? -1 : 1;
    }

    // compare overall_coeff
    cmpval=overall_coeff.compare(o.overall_coeff);
    if (cmpval!=0) return cmpval;

    //if (seq.size()==0) return 0; // empty expairseq's are equal

#ifdef EXPAIRSEQ_USE_HASHTAB
    GINAC_ASSERT(hashtabsize==o.hashtabsize);
    if (hashtabsize==0) {
#endif // def EXPAIRSEQ_USE_HASHTAB
        epvector::const_iterator cit1=seq.begin();
        epvector::const_iterator cit2=o.seq.begin();
        epvector::const_iterator last1=seq.end();
        epvector::const_iterator last2=o.seq.end();
        
        for (; (cit1!=last1)&&(cit2!=last2); ++cit1, ++cit2) {
            cmpval=(*cit1).compare(*cit2);
            if (cmpval!=0) return cmpval;
        }

        GINAC_ASSERT(cit1==last1);
        GINAC_ASSERT(cit2==last2);
        
        return 0;
#ifdef EXPAIRSEQ_USE_HASHTAB
    }

    // compare number of elements in each hashtab entry
    for (unsigned i=0; i<hashtabsize; ++i) {
        unsigned cursize=hashtab[i].size();
        if (cursize != o.hashtab[i].size()) {
            return (cursize < o.hashtab[i].size()) ? -1 : 1;
        }
    }
    
    // compare individual (sorted) hashtab entries
    for (unsigned i=0; i<hashtabsize; ++i) {
        unsigned sz=hashtab[i].size();
        if (sz>0) {
            epplist const & eppl1=hashtab[i];
            epplist const & eppl2=o.hashtab[i];
            epplist::const_iterator it1=eppl1.begin();
            epplist::const_iterator it2=eppl2.begin();
            while (it1!=eppl1.end()) {
                cmpval=(*(*it1)).compare(*(*it2));
                if (cmpval!=0) return cmpval;
                ++it1;
                ++it2;
            }
        }
    }
    
    return 0; // equal
#endif // def EXPAIRSEQ_USE_HASHTAB
}

bool expairseq::is_equal_same_type(basic const & other) const
{
    expairseq const & o=dynamic_cast<expairseq const &>(const_cast<basic &>(other));

    // compare number of elements
    if (seq.size() != o.seq.size()) return false;

    // compare overall_coeff
    if (!overall_coeff.is_equal(o.overall_coeff)) return false;

#ifdef EXPAIRSEQ_USE_HASHTAB
    // compare number of elements in each hashtab entry
    if (hashtabsize!=o.hashtabsize) {
        cout << "this:" << endl;
        printtree(cout,0);
        cout << "other:" << endl;
        other.printtree(cout,0);
    }
        
    GINAC_ASSERT(hashtabsize==o.hashtabsize);
    
    if (hashtabsize==0) {
#endif // def EXPAIRSEQ_USE_HASHTAB
        epvector::const_iterator cit1=seq.begin();
        epvector::const_iterator cit2=o.seq.begin();
        epvector::const_iterator last1=seq.end();
        
        while (cit1!=last1) {
            if (!(*cit1).is_equal(*cit2)) return false;
            ++cit1;
            ++cit2;
        }
        
        return true;
#ifdef EXPAIRSEQ_USE_HASHTAB
    }

    for (unsigned i=0; i<hashtabsize; ++i) {
        if (hashtab[i].size() != o.hashtab[i].size()) return false;
    }

    // compare individual sorted hashtab entries
    for (unsigned i=0; i<hashtabsize; ++i) {
        unsigned sz=hashtab[i].size();
        if (sz>0) {
            epplist const & eppl1=hashtab[i];
            epplist const & eppl2=o.hashtab[i];
            epplist::const_iterator it1=eppl1.begin();
            epplist::const_iterator it2=eppl2.begin();
            while (it1!=eppl1.end()) {
                if (!(*(*it1)).is_equal(*(*it2))) return false;
                ++it1;
                ++it2;
            }
        }
    }

    return true;
#endif // def EXPAIRSEQ_USE_HASHTAB
}

unsigned expairseq::return_type(void) const
{
    return return_types::noncommutative_composite;
}

unsigned expairseq::calchash(void) const
{
    unsigned v=golden_ratio_hash(tinfo());
    epvector::const_iterator last=seq.end();
    for (epvector::const_iterator cit=seq.begin(); cit!=last; ++cit) {
#ifndef EXPAIRSEQ_USE_HASHTAB
        v=rotate_left_31(v); // rotation would spoil commutativity
#endif // ndef EXPAIRSEQ_USE_HASHTAB
        v ^= (*cit).rest.gethash();
    }

    v ^= overall_coeff.gethash();
    v=v & 0x7FFFFFFFU;
    
    // store calculated hash value only if object is already evaluated
    if (flags & status_flags::evaluated) {
        setflag(status_flags::hash_calculated);
        hashvalue=v;
    }

    return v;
}

ex expairseq::expand(unsigned options) const
{
    epvector * vp=expandchildren(options);
    if (vp==0) {
        return *this;
    }
    return thisexpairseq(vp,overall_coeff);
}

//////////
// new virtual functions which can be overridden by derived classes
//////////

// protected

ex expairseq::thisexpairseq(epvector const & v,ex const & oc) const
{
    return expairseq(v,oc);
}

ex expairseq::thisexpairseq(epvector * vp, ex const & oc) const
{
    return expairseq(vp,oc);
}

expair expairseq::split_ex_to_pair(ex const & e) const
{
    return expair(e,exONE());
}

expair expairseq::combine_ex_with_coeff_to_pair(ex const & e,
                                                ex const & c) const
{
    GINAC_ASSERT(is_ex_exactly_of_type(c,numeric));

    return expair(e,c);
}

expair expairseq::combine_pair_with_coeff_to_pair(expair const & p,
                                                  ex const & c) const
{
    GINAC_ASSERT(is_ex_exactly_of_type(p.coeff,numeric));
    GINAC_ASSERT(is_ex_exactly_of_type(c,numeric));
    
    return expair(p.rest,ex_to_numeric(p.coeff).mul_dyn(ex_to_numeric(c)));
}

ex expairseq::recombine_pair_to_ex(expair const & p) const
{
    return lst(p.rest,p.coeff);
}

bool expairseq::expair_needs_further_processing(epp it)
{
    return false;
}

ex expairseq::default_overall_coeff(void) const
{
    return exZERO();
}

void expairseq::combine_overall_coeff(ex const & c)
{
    GINAC_ASSERT(is_ex_exactly_of_type(overall_coeff,numeric));
    GINAC_ASSERT(is_ex_exactly_of_type(c,numeric));
    overall_coeff = ex_to_numeric(overall_coeff).add_dyn(ex_to_numeric(c));
}

void expairseq::combine_overall_coeff(ex const & c1, ex const & c2)
{
    GINAC_ASSERT(is_ex_exactly_of_type(overall_coeff,numeric));
    GINAC_ASSERT(is_ex_exactly_of_type(c1,numeric));
    GINAC_ASSERT(is_ex_exactly_of_type(c2,numeric));
    overall_coeff = ex_to_numeric(overall_coeff).
                        add_dyn(ex_to_numeric(c1).mul(ex_to_numeric(c2)));
}

bool expairseq::can_make_flat(expair const & p) const
{
    return true;
}

    
//////////
// non-virtual functions in this class
//////////

void expairseq::construct_from_2_ex_via_exvector(ex const & lh, ex const & rh)
{
    exvector v;
    v.reserve(2);
    v.push_back(lh);
    v.push_back(rh);
    construct_from_exvector(v);
#ifdef EXPAIRSEQ_USE_HASHTAB
    GINAC_ASSERT((hashtabsize==0)||(hashtabsize>=minhashtabsize));
    GINAC_ASSERT(hashtabsize==calc_hashtabsize(seq.size()));
#endif // def EXPAIRSEQ_USE_HASHTAB
}

void expairseq::construct_from_2_ex(ex const & lh, ex const & rh)
{
    if (lh.bp->tinfo()==tinfo()) {
       if (rh.bp->tinfo()==tinfo()) {
#ifdef EXPAIRSEQ_USE_HASHTAB
           unsigned totalsize=ex_to_expairseq(lh).seq.size()+
                              ex_to_expairseq(rh).seq.size();
           if (calc_hashtabsize(totalsize)!=0) {
               construct_from_2_ex_via_exvector(lh,rh);
           } else {
#endif // def EXPAIRSEQ_USE_HASHTAB
               construct_from_2_expairseq(ex_to_expairseq(lh),
                                          ex_to_expairseq(rh));
#ifdef EXPAIRSEQ_USE_HASHTAB
           }
#endif // def EXPAIRSEQ_USE_HASHTAB
           return;
       } else {
#ifdef EXPAIRSEQ_USE_HASHTAB
           unsigned totalsize=ex_to_expairseq(lh).seq.size()+1;
           if (calc_hashtabsize(totalsize)!=0) {
               construct_from_2_ex_via_exvector(lh,rh);
           } else {
#endif // def EXPAIRSEQ_USE_HASHTAB
               construct_from_expairseq_ex(ex_to_expairseq(lh),rh);
#ifdef EXPAIRSEQ_USE_HASHTAB
           }
#endif // def EXPAIRSEQ_USE_HASHTAB
           return;
       }
    } else if (rh.bp->tinfo()==tinfo()) {
#ifdef EXPAIRSEQ_USE_HASHTAB
        unsigned totalsize=ex_to_expairseq(rh).seq.size()+1;
        if (calc_hashtabsize(totalsize)!=0) {
            construct_from_2_ex_via_exvector(lh,rh);
        } else {
#endif // def EXPAIRSEQ_USE_HASHTAB
            construct_from_expairseq_ex(ex_to_expairseq(rh),lh);
#ifdef EXPAIRSEQ_USE_HASHTAB
        }
#endif // def EXPAIRSEQ_USE_HASHTAB
        return;
    }

#ifdef EXPAIRSEQ_USE_HASHTAB
    if (calc_hashtabsize(2)!=0) {
        construct_from_2_ex_via_exvector(lh,rh);
        return;
    }
    hashtabsize=0;
#endif // def EXPAIRSEQ_USE_HASHTAB
    
    if (is_ex_exactly_of_type(lh,numeric)) {
        if (is_ex_exactly_of_type(rh,numeric)) {
            combine_overall_coeff(lh);
            combine_overall_coeff(rh);
        } else {
            combine_overall_coeff(lh);
            seq.push_back(split_ex_to_pair(rh));
        }
    } else {
        if (is_ex_exactly_of_type(rh,numeric)) {
            combine_overall_coeff(rh);
            seq.push_back(split_ex_to_pair(lh));
        } else {
            expair p1=split_ex_to_pair(lh);
            expair p2=split_ex_to_pair(rh);

            int cmpval=p1.rest.compare(p2.rest);
            if (cmpval==0) {
                p1.coeff=ex_to_numeric(p1.coeff).add_dyn(ex_to_numeric(p2.coeff));
                if (!ex_to_numeric(p1.coeff).is_zero()) {
                    // no further processing is necessary, since this
                    // one element will usually be recombined in eval()
                    seq.push_back(p1);
                }
            } else {
                seq.reserve(2);
                if (cmpval<0) {
                    seq.push_back(p1);
                    seq.push_back(p2);
                } else {
                    seq.push_back(p2);
                    seq.push_back(p1);
                }
            }
        }
    }
}

void expairseq::construct_from_2_expairseq(expairseq const & s1,
                                           expairseq const & s2)
{
    combine_overall_coeff(s1.overall_coeff);
    combine_overall_coeff(s2.overall_coeff);

    epvector::const_iterator first1=s1.seq.begin();
    epvector::const_iterator last1=s1.seq.end();
    epvector::const_iterator first2=s2.seq.begin();
    epvector::const_iterator last2=s2.seq.end();

    seq.reserve(s1.seq.size()+s2.seq.size());

    bool needs_further_processing=false;
    
    while (first1!=last1 && first2!=last2) {
        int cmpval=(*first1).rest.compare((*first2).rest);
        if (cmpval==0) {
            // combine terms
            numeric const & newcoeff=ex_to_numeric((*first1).coeff).
                                     add(ex_to_numeric((*first2).coeff));
            if (!newcoeff.is_zero()) {
                seq.push_back(expair((*first1).rest,newcoeff));
                if (expair_needs_further_processing(seq.end()-1)) {
                    needs_further_processing = true;
                }
            }
            ++first1;
            ++first2;
        } else if (cmpval<0) {
            seq.push_back(*first1);
            ++first1;
        } else {
            seq.push_back(*first2);
            ++first2;
        }
    }
    
    while (first1!=last1) {
        seq.push_back(*first1);
        ++first1;
    }
    while (first2!=last2) {
        seq.push_back(*first2);
        ++first2;
    }

    if (needs_further_processing) {
        epvector v=seq;
        seq.clear();
        construct_from_epvector(v);
    }
}

void expairseq::construct_from_expairseq_ex(expairseq const & s,
                                            ex const & e)
{
    combine_overall_coeff(s.overall_coeff);
    if (is_ex_exactly_of_type(e,numeric)) {
        combine_overall_coeff(e);
        seq=s.seq;
        return;
    }

    epvector::const_iterator first=s.seq.begin();
    epvector::const_iterator last=s.seq.end();
    expair p=split_ex_to_pair(e);

    seq.reserve(s.seq.size()+1);
    bool p_pushed=0;

    bool needs_further_processing=false;

    // merge p into s.seq
    while (first!=last) {
        int cmpval=(*first).rest.compare(p.rest);
        if (cmpval==0) {
            // combine terms
            numeric const & newcoeff=ex_to_numeric((*first).coeff).
                                     add(ex_to_numeric(p.coeff));
            if (!newcoeff.is_zero()) {
                seq.push_back(expair((*first).rest,newcoeff));
                if (expair_needs_further_processing(seq.end()-1)) {
                    needs_further_processing = true;
                }
            }
            ++first;
            p_pushed=1;
            break;
        } else if (cmpval<0) {
            seq.push_back(*first);
            ++first;
        } else {
            seq.push_back(p);
            p_pushed=1;
            break;
        }
    }

    if (p_pushed) {
        // while loop exited because p was pushed, now push rest of s.seq
        while (first!=last) {
            seq.push_back(*first);
            ++first;
        }
    } else {
        // while loop exited because s.seq was pushed, now push p
        seq.push_back(p);
    }

    if (needs_further_processing) {
        epvector v=seq;
        seq.clear();
        construct_from_epvector(v);
    }
}

void expairseq::construct_from_exvector(exvector const & v)
{
    // simplifications: +(a,+(b,c),d) -> +(a,b,c,d) (associativity)
    //                  +(d,b,c,a) -> +(a,b,c,d) (canonicalization)
    //                  +(...,x,*(x,c1),*(x,c2)) -> +(...,*(x,1+c1+c2)) (c1, c2 numeric())
    //                  (same for (+,*) -> (*,^)

    make_flat(v);
#ifdef EXPAIRSEQ_USE_HASHTAB
    combine_same_terms();
#else
    canonicalize();
    combine_same_terms_sorted_seq();
#endif // def EXPAIRSEQ_USE_HASHTAB
}

void expairseq::construct_from_epvector(epvector const & v)
{
    // simplifications: +(a,+(b,c),d) -> +(a,b,c,d) (associativity)
    //                  +(d,b,c,a) -> +(a,b,c,d) (canonicalization)
    //                  +(...,x,*(x,c1),*(x,c2)) -> +(...,*(x,1+c1+c2)) (c1, c2 numeric())
    //                  (same for (+,*) -> (*,^)

    make_flat(v);
#ifdef EXPAIRSEQ_USE_HASHTAB
    combine_same_terms();
#else
    canonicalize();
    combine_same_terms_sorted_seq();
#endif // def EXPAIRSEQ_USE_HASHTAB
}

#include <iostream>

void expairseq::make_flat(exvector const & v)
{
    exvector::const_iterator cit, citend = v.end();

    // count number of operands which are of same expairseq derived type
    // and their cumulative number of operands
    int nexpairseqs=0;
    int noperands=0;
    cit=v.begin();
    while (cit!=citend) {
        if (cit->bp->tinfo()==tinfo()) {
            nexpairseqs++;
            noperands+=ex_to_expairseq(*cit).seq.size();
        }
        ++cit;
    }

    // reserve seq and coeffseq which will hold all operands
    seq.reserve(v.size()+noperands-nexpairseqs);

    // copy elements and split off numerical part
    cit=v.begin();
    while (cit!=citend) {
        if (cit->bp->tinfo()==tinfo()) {
            expairseq const & subseqref=ex_to_expairseq(*cit);
            combine_overall_coeff(subseqref.overall_coeff);
            epvector::const_iterator cit_s=subseqref.seq.begin();
            while (cit_s!=subseqref.seq.end()) {
                seq.push_back(*cit_s);
                ++cit_s;
            }
        } else {
            if (is_ex_exactly_of_type(*cit,numeric)) {
                combine_overall_coeff(*cit);
            } else {
                seq.push_back(split_ex_to_pair(*cit));
            }
        }
        ++cit;
    }

    /*
    cout << "after make flat" << endl;
    for (epvector::const_iterator cit=seq.begin(); cit!=seq.end(); ++cit) {
        (*cit).printraw(cout);
    }
    cout << endl;
    cout.flush();
    */
}

void expairseq::make_flat(epvector const & v)
{
    epvector::const_iterator cit, citend = v.end();

    // count number of operands which are of same expairseq derived type
    // and their cumulative number of operands
    int nexpairseqs=0;
    int noperands=0;
    cit=v.begin();
    while (cit!=citend) {
        if (cit->rest.bp->tinfo()==tinfo()) {
            nexpairseqs++;
            noperands+=ex_to_expairseq((*cit).rest).seq.size();
        }
        ++cit;
    }

    // reserve seq and coeffseq which will hold all operands
    seq.reserve(v.size()+noperands-nexpairseqs);

    // copy elements and split off numerical part
    cit=v.begin();
    while (cit!=citend) {
        if ((cit->rest.bp->tinfo()==tinfo())&&can_make_flat(*cit)) {
            expairseq const & subseqref=ex_to_expairseq((*cit).rest);
            combine_overall_coeff(ex_to_numeric(subseqref.overall_coeff),
                                  ex_to_numeric((*cit).coeff));
            epvector::const_iterator cit_s=subseqref.seq.begin();
            while (cit_s!=subseqref.seq.end()) {
                seq.push_back(expair((*cit_s).rest,
                              ex_to_numeric((*cit_s).coeff).mul_dyn(ex_to_numeric((*cit).coeff))));
                //seq.push_back(combine_pair_with_coeff_to_pair(*cit_s,
                //                                              (*cit).coeff));
                ++cit_s;
            }
        } else {
            if ((*cit).is_numeric_with_coeff_1()) {
                combine_overall_coeff((*cit).rest);
            //if (is_ex_exactly_of_type((*cit).rest,numeric)) {
            //    combine_overall_coeff(recombine_pair_to_ex(*cit));
            } else {
                seq.push_back(*cit);
            }
        }
        ++cit;
    }
}

epvector * expairseq::bubblesort(epvector::iterator itbegin, epvector::iterator itend)
{
    unsigned n=itend-itbegin;

    epvector * sp=new epvector;
    sp->reserve(n);

    epvector::iterator last=itend-1;
    for (epvector::iterator it1=itbegin; it1!=last; ++it1) {
        for (epvector::iterator it2=it1+1; it2!=itend; ++it2) {
            if ((*it2).rest.compare((*it1).rest)<0) {
                iter_swap(it1,it2);
            }
        }
        sp->push_back(*it1);
    }
    sp->push_back(*last);
    return sp;
}

epvector * expairseq::mergesort(epvector::iterator itbegin, epvector::iterator itend)
{
    unsigned n=itend-itbegin;
    /*
    if (n==1) {
        epvector * sp=new epvector;
        sp->push_back(*itbegin);
        return sp;
    }
    */
    if (n<16) return bubblesort(itbegin, itend);
    unsigned m=n/2;
    
    epvector * s1p=mergesort(itbegin, itbegin+m);
    epvector * s2p=mergesort(itbegin+m, itend);

    epvector * sp=new epvector;
    sp->reserve(s1p->size()+s2p->size());

    epvector::iterator first1=s1p->begin();
    epvector::iterator last1=s1p->end();

    epvector::iterator first2=s2p->begin();
    epvector::iterator last2=s2p->end();
    
    while (first1 != last1 && first2 != last2) {
        if ((*first1).rest.compare((*first2).rest)<0) {
            sp->push_back(*first1);
            ++first1;
        } else {
            sp->push_back(*first2);
            ++first2;
        }
    }

    if (first1 != last1) {
        while (first1 != last1) {
            sp->push_back(*first1);
            ++first1;
        }
    } else {
        while (first2 != last2) {
            sp->push_back(*first2);
            ++first2;
        }
    }

    delete s1p;
    delete s2p;
    
    return sp;
}
            

void expairseq::canonicalize(void)
{
    // canonicalize
    sort(seq.begin(),seq.end(),expair_is_less());
    /*
    sort(seq.begin(),seq.end(),expair_is_less_old());
    if (seq.size()>1) {
        if (is_ex_exactly_of_type((*(seq.begin())).rest,numeric)) {
            sort(seq.begin(),seq.end(),expair_is_less());
        } else {
            epvector::iterator last_numeric=seq.end();
            do {
                last_numeric--;
            } while (is_ex_exactly_of_type((*last_numeric).rest,numeric));
            last_numeric++;
            sort(last_numeric,seq.end(),expair_is_less());
        }
    }
    */
    
    /*
    epvector * sorted_seqp=mergesort(seq.begin(),seq.end());
    epvector::iterator last=sorted_seqp->end();
    epvector::iterator it2=seq.begin();
    for (epvector::iterator it1=sorted_seqp->begin(); it1!=last; ++it1, ++it2) {
        iter_swap(it1,it2);
    }
    delete sorted_seqp;
    */

    /*
    cout << "after canonicalize" << endl;
    for (epvector::const_iterator cit=seq.begin(); cit!=seq.end(); ++cit) {
        (*cit).printraw(cout);
    }
    cout << endl;
    cout.flush();
    */
}

void expairseq::combine_same_terms_sorted_seq(void)
{
    bool needs_further_processing=false;
    
    // combine same terms, drop term with coeff 0
    if (seq.size()>1) {
        epvector::iterator itin1=seq.begin();
        epvector::iterator itin2=itin1+1;
        epvector::iterator itout=itin1;
        epvector::iterator last=seq.end();
        // must_copy will be set to true the first time some combination is possible
        // from then on the sequence has changed and must be compacted
        bool must_copy=false;
        while (itin2!=last) {
            if ((*itin1).rest.compare((*itin2).rest)==0) {
                (*itin1).coeff=ex_to_numeric((*itin1).coeff).
                               add_dyn(ex_to_numeric((*itin2).coeff));
                if (expair_needs_further_processing(itin1)) {
                    needs_further_processing = true;
                }
                must_copy=true;
            } else {
                if (!ex_to_numeric((*itin1).coeff).is_zero()) {
                    if (must_copy) {
                        *itout=*itin1;
                    }
                    ++itout;
                }
                itin1=itin2;
            }
            ++itin2;
        }
        if (!ex_to_numeric((*itin1).coeff).is_zero()) {
            if (must_copy) {
                *itout=*itin1;
            }
            ++itout;
        }
        if (itout!=last) {
            seq.erase(itout,last);
        }
    }

    /*
    cout << "after combine" << endl;
    for (epvector::const_iterator cit=seq.begin(); cit!=seq.end(); ++cit) {
        (*cit).printraw(cout);
    }
    cout << endl;
    cout.flush();
    */
    
    if (needs_further_processing) {
        epvector v=seq;
        seq.clear();
        construct_from_epvector(v);
    }
}

#ifdef EXPAIRSEQ_USE_HASHTAB

unsigned expairseq::calc_hashtabsize(unsigned sz) const
{
    unsigned size;
    unsigned nearest_power_of_2 = 1 << log2(sz);
    //    if (nearest_power_of_2 < maxhashtabsize/hashtabfactor) {
    //  size=nearest_power_of_2*hashtabfactor;
    size=nearest_power_of_2/hashtabfactor;
    if (size<minhashtabsize) return 0;
    GINAC_ASSERT(hashtabsize<=0x8000000U); // really max size due to 31 bit hashing
    // hashtabsize must be a power of 2
    GINAC_ASSERT((1U << log2(size))==size);
    return size;
}

unsigned expairseq::calc_hashindex(ex const & e) const
{
    // calculate hashindex
    unsigned hash=e.gethash();
    unsigned hashindex;
    if (is_a_numeric_hash(hash)) {
        hashindex=hashmask;
    } else {
        hashindex=hash & hashmask;
        // last hashtab entry is reserved for numerics
        if (hashindex==hashmask) hashindex=0;
    }
    GINAC_ASSERT(hashindex>=0);
    GINAC_ASSERT((hashindex<hashtabsize)||(hashtabsize==0));
    return hashindex;
}

void expairseq::shrink_hashtab(void)
{
    unsigned new_hashtabsize;
    while (hashtabsize!=(new_hashtabsize=calc_hashtabsize(seq.size()))) {
        GINAC_ASSERT(new_hashtabsize<hashtabsize);
        if (new_hashtabsize==0) {
            hashtab.clear();
            hashtabsize=0;
            canonicalize();
            return;
        }
        
        // shrink by a factor of 2
        unsigned half_hashtabsize=hashtabsize/2;
        for (unsigned i=0; i<half_hashtabsize-1; ++i) {
            hashtab[i].merge(hashtab[i+half_hashtabsize],epp_is_less());
        }
        // special treatment for numeric hashes
        hashtab[0].merge(hashtab[half_hashtabsize-1],epp_is_less());
        hashtab[half_hashtabsize-1]=hashtab[hashtabsize-1];
        hashtab.resize(half_hashtabsize);
        hashtabsize=half_hashtabsize;
        hashmask=hashtabsize-1;
    }
}

void expairseq::remove_hashtab_entry(epvector::const_iterator element)
{
    if (hashtabsize==0) return; // nothing to do
    
    // calculate hashindex of element to be deleted
    unsigned hashindex=calc_hashindex((*element).rest);

    // find it in hashtab and remove it
    epplist & eppl=hashtab[hashindex];
    epplist::iterator epplit=eppl.begin();
    bool erased=false;
    while (epplit!=eppl.end()) {
        if (*epplit == element) {
            eppl.erase(epplit);
            erased=true;
            break;
        }
        ++epplit;
    }
    if (!erased) {
        printtree(cout,0);
        cout << "tried to erase " << element-seq.begin() << endl;
        cout << "size " << seq.end()-seq.begin() << endl;

        unsigned hashindex=calc_hashindex((*element).rest);
        epplist & eppl=hashtab[hashindex];
        epplist::iterator epplit=eppl.begin();
        bool erased=false;
        while (epplit!=eppl.end()) {
            if (*epplit == element) {
                eppl.erase(epplit);
                erased=true;
                break;
            }
            ++epplit;
        }
        GINAC_ASSERT(erased);
    }
    GINAC_ASSERT(erased);
}

void expairseq::move_hashtab_entry(epvector::const_iterator oldpos,
                                   epvector::iterator newpos)
{
    GINAC_ASSERT(hashtabsize!=0);
    
    // calculate hashindex of element which was moved
    unsigned hashindex=calc_hashindex((*newpos).rest);

    // find it in hashtab and modify it
    epplist & eppl=hashtab[hashindex];
    epplist::iterator epplit=eppl.begin();
    while (epplit!=eppl.end()) {
        if (*epplit == oldpos) {
            *epplit=newpos;
            break;
        }
        ++epplit;
    }
    GINAC_ASSERT(epplit!=eppl.end());
}

void expairseq::sorted_insert(epplist & eppl, epp elem)
{
    epplist::iterator current=eppl.begin();
    while ((current!=eppl.end())&&((*(*current)).is_less(*elem))) {
        ++current;
    }
    eppl.insert(current,elem);
}    

void expairseq::build_hashtab_and_combine(epvector::iterator & first_numeric,
                                          epvector::iterator & last_non_zero,
                                          vector<bool> & touched,
                                          unsigned & number_of_zeroes)
{
    epp current=seq.begin();

    while (current!=first_numeric) {
        if (is_ex_exactly_of_type((*current).rest,numeric)) {
            --first_numeric;
            iter_swap(current,first_numeric);
        } else {
            // calculate hashindex
            unsigned currenthashindex=calc_hashindex((*current).rest);

            // test if there is already a matching expair in the hashtab-list
            epplist & eppl=hashtab[currenthashindex];
            epplist::iterator epplit=eppl.begin();
            while (epplit!=eppl.end()) {
                if ((*current).rest.is_equal((*(*epplit)).rest)) break;
                ++epplit;
            }
            if (epplit==eppl.end()) {
                // no matching expair found, append this to end of list
                sorted_insert(eppl,current);
                ++current;
            } else {
                // epplit points to a matching expair, combine it with current
                (*(*epplit)).coeff=ex_to_numeric((*(*epplit)).coeff).
                                   add_dyn(ex_to_numeric((*current).coeff));
                
                // move obsolete current expair to end by swapping with last_non_zero element
                // if this was a numeric, it is swapped with the expair before first_numeric 
                iter_swap(current,last_non_zero);
                --first_numeric;
                if (first_numeric!=last_non_zero) iter_swap(first_numeric,current);
                --last_non_zero;
                ++number_of_zeroes;
                // test if combined term has coeff 0 and can be removed is done later
                touched[(*epplit)-seq.begin()]=true;
            }
        }
    }
}    

void expairseq::drop_coeff_0_terms(epvector::iterator & first_numeric,
                                   epvector::iterator & last_non_zero,
                                   vector<bool> & touched,
                                   unsigned & number_of_zeroes)
{
    // move terms with coeff 0 to end and remove them from hashtab
    // check only those elements which have been touched
    epp current=seq.begin();
    unsigned i=0;
    while (current!=first_numeric) {
        if (!touched[i]) {
            ++current;
            ++i;
        } else if (!ex_to_numeric((*current).coeff).is_equal(numZERO())) {
            ++current;
            ++i;
        } else {
            remove_hashtab_entry(current);

            // move element to the end, unless it is already at the end
            if (current!=last_non_zero) {
                iter_swap(current,last_non_zero);
                --first_numeric;
                bool numeric_swapped=first_numeric!=last_non_zero;
                if (numeric_swapped) iter_swap(first_numeric,current);
                epvector::iterator changed_entry;

                if (numeric_swapped) {
                    changed_entry=first_numeric;
                } else {
                    changed_entry=last_non_zero;
                }

                --last_non_zero;
                ++number_of_zeroes;

                if (first_numeric!=current) {
                
                    // change entry in hashtab which referred to first_numeric or last_non_zero to current
                    move_hashtab_entry(changed_entry,current);
                    touched[current-seq.begin()]=touched[changed_entry-seq.begin()];
                }
            } else {
                --first_numeric;
                --last_non_zero;
                ++number_of_zeroes;
            }
        }
    }
    GINAC_ASSERT(i==current-seq.begin());
}

bool expairseq::has_coeff_0(void) const
{
    for (epvector::const_iterator cit=seq.begin(); cit!=seq.end(); ++cit) {
        if ((*cit).coeff.is_equal(exZERO())) {
            return true;
        }
    }
    return false;
}

void expairseq::add_numerics_to_hashtab(epvector::iterator first_numeric,
                                        epvector::const_iterator last_non_zero)
{
    if (first_numeric==seq.end()) return; // no numerics    

    epvector::iterator current=first_numeric;
    epvector::const_iterator last=last_non_zero+1;
    while (current!=last) {
        sorted_insert(hashtab[hashmask],current);
        ++current;
    }
}

void expairseq::combine_same_terms(void)
{
    // combine same terms, drop term with coeff 0, move numerics to end
    
    // calculate size of hashtab
    hashtabsize=calc_hashtabsize(seq.size());

    // hashtabsize is a power of 2
    hashmask=hashtabsize-1;

    // allocate hashtab
    hashtab.clear();
    hashtab.resize(hashtabsize);

    if (hashtabsize==0) {
        canonicalize();
        combine_same_terms_sorted_seq();
        GINAC_ASSERT(!has_coeff_0());
        return;
    }

    // iterate through seq, move numerics to end,
    // fill hashtab and combine same terms
    epvector::iterator first_numeric=seq.end();
    epvector::iterator last_non_zero=seq.end()-1;

    vector<bool> touched;
    touched.reserve(seq.size());
    for (unsigned i=0; i<seq.size(); ++i) touched[i]=false;

    unsigned number_of_zeroes=0;

    GINAC_ASSERT(!has_coeff_0());
    build_hashtab_and_combine(first_numeric,last_non_zero,touched,number_of_zeroes);
    /*
    cout << "in combine:" << endl;
    printtree(cout,0);
    cout << "size=" << seq.end() - seq.begin() << endl;
    cout << "first_numeric=" << first_numeric - seq.begin() << endl;
    cout << "last_non_zero=" << last_non_zero - seq.begin() << endl;
    for (unsigned i=0; i<seq.size(); ++i) {
        if (touched[i]) cout << i << " is touched" << endl;
    }
    cout << "end in combine" << endl;
    */
    
    // there should not be any terms with coeff 0 from the beginning,
    // so it should be safe to skip this step
    if (number_of_zeroes!=0) {
        drop_coeff_0_terms(first_numeric,last_non_zero,touched,number_of_zeroes);
        /*
        cout << "in combine after drop:" << endl;
        printtree(cout,0);
        cout << "size=" << seq.end() - seq.begin() << endl;
        cout << "first_numeric=" << first_numeric - seq.begin() << endl;
        cout << "last_non_zero=" << last_non_zero - seq.begin() << endl;
        for (unsigned i=0; i<seq.size(); ++i) {
            if (touched[i]) cout << i << " is touched" << endl;
        }
        cout << "end in combine after drop" << endl;
        */
    }

    add_numerics_to_hashtab(first_numeric,last_non_zero);

    // pop zero elements
    for (unsigned i=0; i<number_of_zeroes; ++i) {
        seq.pop_back();
    }

    // shrink hashtabsize to calculated value
    GINAC_ASSERT(!has_coeff_0());

    shrink_hashtab();

    GINAC_ASSERT(!has_coeff_0());
}

#endif // def EXPAIRSEQ_USE_HASHTAB

bool expairseq::is_canonical() const
{
    if (seq.size()<=1) return 1;

#ifdef EXPAIRSEQ_USE_HASHTAB
    if (hashtabsize>0) return 1; // not canoncalized
#endif // def EXPAIRSEQ_USE_HASHTAB
    
    epvector::const_iterator it=seq.begin();
    epvector::const_iterator it_last=it;
    for (++it; it!=seq.end(); it_last=it, ++it) {
        if (!((*it_last).is_less(*it)||(*it_last).is_equal(*it))) {
            if (!is_ex_exactly_of_type((*it_last).rest,numeric)||
                !is_ex_exactly_of_type((*it).rest,numeric)) {
                // double test makes it easier to set a breakpoint...
                if (!is_ex_exactly_of_type((*it_last).rest,numeric)||
                    !is_ex_exactly_of_type((*it).rest,numeric)) {
                    printpair(cout,*it_last,0);
                    cout << ">";
                    printpair(cout,*it,0);
                    cout << "\n";
                    cout << "pair1:" << endl;
                    (*it_last).rest.printtree(cout);
                    (*it_last).coeff.printtree(cout);
                    cout << "pair2:" << endl;
                    (*it).rest.printtree(cout);
                    (*it).coeff.printtree(cout);
                    return 0;
                }
            }
        }
    }
    return 1;
}

epvector * expairseq::expandchildren(unsigned options) const
{
    epvector::const_iterator last=seq.end();
    epvector::const_iterator cit=seq.begin();
    while (cit!=last) {
        ex const & expanded_ex=(*cit).rest.expand(options);
        if (!are_ex_trivially_equal((*cit).rest,expanded_ex)) {

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
            s->push_back(combine_ex_with_coeff_to_pair(expanded_ex,
                                                       (*cit2).coeff));
            ++cit2;
            // copy rest
            while (cit2!=last) {
                s->push_back(combine_ex_with_coeff_to_pair((*cit2).rest.expand(options),
                                                           (*cit2).coeff));
                ++cit2;
            }
            return s;
        }
        ++cit;
    }
    
    return 0; // nothing has changed
}
   
epvector * expairseq::evalchildren(int level) const
{
    // returns a NULL pointer if nothing had to be evaluated
    // returns a pointer to a newly created epvector otherwise
    // (which has to be deleted somewhere else)

    if (level==1) {
        return 0;
    }
    if (level == -max_recursion_level) {
        throw(std::runtime_error("max recursion level reached"));
    }

    --level;
    epvector::const_iterator last=seq.end();
    epvector::const_iterator cit=seq.begin();
    while (cit!=last) {
        ex const & evaled_ex=(*cit).rest.eval(level);
        if (!are_ex_trivially_equal((*cit).rest,evaled_ex)) {

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
            s->push_back(combine_ex_with_coeff_to_pair(evaled_ex,
                                                       (*cit2).coeff));
            ++cit2;
            // copy rest
            while (cit2!=last) {
                s->push_back(combine_ex_with_coeff_to_pair((*cit2).rest.eval(level),
                                                           (*cit2).coeff));
                ++cit2;
            }
            return s;
        }
        ++cit;
    }
    
    return 0; // nothing has changed
}

epvector expairseq::evalfchildren(int level) const
{
    epvector s;
    s.reserve(seq.size());

    if (level==1) {
        return seq;
    }
    if (level == -max_recursion_level) {
        throw(std::runtime_error("max recursion level reached"));
    }
    --level;
    for (epvector::const_iterator it=seq.begin(); it!=seq.end(); ++it) {
        s.push_back(combine_ex_with_coeff_to_pair((*it).rest.evalf(level),
                                                  (*it).coeff));
    }
    return s;
}

epvector expairseq::normalchildren(int level) const
{
    epvector s;
    s.reserve(seq.size());

    if (level==1) {
        return seq;
    }
    if (level == -max_recursion_level) {
        throw(std::runtime_error("max recursion level reached"));
    }
    --level;
    for (epvector::const_iterator it=seq.begin(); it!=seq.end(); ++it) {
        s.push_back(combine_ex_with_coeff_to_pair((*it).rest.normal(level),
                                                  (*it).coeff));
    }
    return s;
}

epvector expairseq::diffchildren(symbol const & y) const
{
    epvector s;
    s.reserve(seq.size());

    for (epvector::const_iterator it=seq.begin(); it!=seq.end(); ++it) {
        s.push_back(combine_ex_with_coeff_to_pair((*it).rest.diff(y),
                                                  (*it).coeff));
    }
    return s;
}

epvector * expairseq::subschildren(lst const & ls, lst const & lr) const
{
    // returns a NULL pointer if nothing had to be substituted
    // returns a pointer to a newly created epvector otherwise
    // (which has to be deleted somewhere else)

    epvector::const_iterator last=seq.end();
    epvector::const_iterator cit=seq.begin();
    while (cit!=last) {
        ex const & subsed_ex=(*cit).rest.subs(ls,lr);
        if (!are_ex_trivially_equal((*cit).rest,subsed_ex)) {

            // something changed, copy seq, subs and return it
            epvector *s=new epvector;
            s->reserve(seq.size());

            // copy parts of seq which are known not to have changed
            epvector::const_iterator cit2=seq.begin();
            while (cit2!=cit) {
                s->push_back(*cit2);
                ++cit2;
            }
            // copy first changed element
            s->push_back(combine_ex_with_coeff_to_pair(subsed_ex,
                                                       (*cit2).coeff));
            ++cit2;
            // copy rest
            while (cit2!=last) {
                s->push_back(combine_ex_with_coeff_to_pair((*cit2).rest.subs(ls,lr),
                                                           (*cit2).coeff));
                ++cit2;
	    }
            return s;
        }
        ++cit;
    }
    
    return 0; // nothing has changed
}

/*
epvector expairseq::subschildren(lst const & ls, lst const & lr) const
{
    epvector s;
    s.reserve(seq.size());

    for (epvector::const_iterator it=seq.begin(); it!=seq.end(); ++it) {
        s.push_back(split_ex_to_pair((*it).rest.subs(ls,lr),(*it).coeff));
    }
    return s;
}
*/

/*
void expairseq::sort(epviter first, epviter last, expair_is_less comp)
{
    if (first != last) {
        introsort_loop(first, last, lg(last - first) * 2, comp);
        __final_insertion_sort(first, last, comp);
    }
}

ptrdiff_t expairseq::lg(ptrdiff_t n)
{
    ptrdiff_t k;
    for (k = 0; n > 1; n >>= 1) ++k;
    return k;
}

void expairseq::introsort_loop(epviter first, epviter last,
                               ptrdiff_t depth_limit, expair_is_less comp)
{
    while (last - first > stl_threshold) {
        if (depth_limit == 0) {
            partial_sort(first, last, last, comp);
            return;
        }
        --depth_limit;
        epviter cut = unguarded_partition(first, last,
                      expair(__median(*first, *(first + (last - first)/2),
                      *(last - 1), comp)), comp);
        introsort_loop(cut, last, depth_limit, comp);
        last = cut;
    }
}

epviter expairseq::unguarded_partition(epviter first, epviter last, 
                                       expair pivot, expair_is_less comp)
{
    while (1) {
        while (comp(*first, pivot)) ++first;
        --last;
        while (comp(pivot, *last)) --last;
        if (!(first < last)) return first;
        iter_swap(first, last);
        ++first;
    }
}

void expairseq::partial_sort(epviter first, epviter middle, epviter last,
                             expair_is_less comp) {
  make_heap(first, middle, comp);
  for (RandomAccessIterator i = middle; i < last; ++i)
    if (comp(*i, *first))
      __pop_heap(first, middle, i, T(*i), comp, distance_type(first));
  sort_heap(first, middle, comp);
}
*/

//////////
// static member variables
//////////

// protected

unsigned expairseq::precedence=10;

#ifdef EXPAIRSEQ_USE_HASHTAB
unsigned expairseq::maxhashtabsize=0x4000000U;
unsigned expairseq::minhashtabsize=0x1000U;
unsigned expairseq::hashtabfactor=1;
#endif // def EXPAIRSEQ_USE_HASHTAB

//////////
// global constants
//////////

const expairseq some_expairseq;
type_info const & typeid_expairseq=typeid(some_expairseq);

#ifndef NO_GINAC_NAMESPACE
} // namespace GiNaC
#endif // ndef NO_GINAC_NAMESPACE
