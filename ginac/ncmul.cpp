/** @file ncmul.cpp
 *
 *  Implementation of GiNaC's non-commutative products of expressions. */

#include <algorithm>
#include <iostream>
#include <stdexcept>

#include "ginac.h"

//////////
// default constructor, destructor, copy constructor assignment operator and helpers
//////////

// public

ncmul::ncmul()
{
    debugmsg("ncmul default constructor",LOGLEVEL_CONSTRUCT);
    tinfo_key = TINFO_NCMUL;
}

ncmul::~ncmul()
{
    debugmsg("ncmul destructor",LOGLEVEL_DESTRUCT);
    destroy(0);
}

ncmul::ncmul(ncmul const & other)
{
    debugmsg("ncmul copy constructor",LOGLEVEL_CONSTRUCT);
    copy(other);
}

ncmul const & ncmul::operator=(ncmul const & other)
{
    debugmsg("ncmul operator=",LOGLEVEL_ASSIGNMENT);
    if (this != &other) {
        destroy(1);
        copy(other);
    }
    return *this;
}

// protected

void ncmul::copy(ncmul const & other)
{
    exprseq::copy(other);
}

void ncmul::destroy(bool call_parent)
{
    if (call_parent) exprseq::destroy(call_parent);
}

//////////
// other constructors
//////////

// public

ncmul::ncmul(ex const & lh, ex const & rh) :
    exprseq(lh,rh)
{
    debugmsg("ncmul constructor from ex,ex",LOGLEVEL_CONSTRUCT);
    tinfo_key = TINFO_NCMUL;
}

ncmul::ncmul(ex const & f1, ex const & f2, ex const & f3) :
    exprseq(f1,f2,f3)
{
    debugmsg("ncmul constructor from 3 ex",LOGLEVEL_CONSTRUCT);
    tinfo_key = TINFO_NCMUL;
}

ncmul::ncmul(ex const & f1, ex const & f2, ex const & f3,
      ex const & f4) : exprseq(f1,f2,f3,f4)
{
    debugmsg("ncmul constructor from 4 ex",LOGLEVEL_CONSTRUCT);
    tinfo_key = TINFO_NCMUL;
}

ncmul::ncmul(ex const & f1, ex const & f2, ex const & f3,
      ex const & f4, ex const & f5) : exprseq(f1,f2,f3,f4,f5)
{
    debugmsg("ncmul constructor from 5 ex",LOGLEVEL_CONSTRUCT);
    tinfo_key = TINFO_NCMUL;
}

ncmul::ncmul(ex const & f1, ex const & f2, ex const & f3,
      ex const & f4, ex const & f5, ex const & f6) :
    exprseq(f1,f2,f3,f4,f5,f6)
{
    debugmsg("ncmul constructor from 6 ex",LOGLEVEL_CONSTRUCT);
    tinfo_key = TINFO_NCMUL;
}

ncmul::ncmul(exvector const & v, bool discardable) : exprseq(v,discardable)
{
    debugmsg("ncmul constructor from exvector,bool",LOGLEVEL_CONSTRUCT);
    tinfo_key = TINFO_NCMUL;
}

ncmul::ncmul(exvector * vp) : exprseq(vp)
{
    debugmsg("ncmul constructor from exvector *",LOGLEVEL_CONSTRUCT);
    tinfo_key = TINFO_NCMUL;
}
    
//////////
// functions overriding virtual functions from bases classes
//////////

// public

basic * ncmul::duplicate() const
{
    debugmsg("ncmul duplicate",LOGLEVEL_ASSIGNMENT);
    return new ncmul(*this);
}

bool ncmul::info(unsigned inf) const
{
    throw(std::logic_error("which flags have to be implemented in ncmul::info()?"));
}

typedef vector<int> intvector;

ex ncmul::expand(unsigned options) const
{
    exvector sub_expanded_seq;
    intvector positions_of_adds;
    intvector number_of_add_operands;

    exvector expanded_seq=expandchildren(options);

    positions_of_adds.resize(expanded_seq.size());
    number_of_add_operands.resize(expanded_seq.size());

    int number_of_adds=0;
    int number_of_expanded_terms=1;

    unsigned current_position=0;
    exvector::const_iterator last=expanded_seq.end();
    for (exvector::const_iterator cit=expanded_seq.begin(); cit!=last; ++cit) {
        if (is_ex_exactly_of_type((*cit),add)) {
            positions_of_adds[number_of_adds]=current_position;
            add const & expanded_addref=ex_to_add(*cit);
            number_of_add_operands[number_of_adds]=expanded_addref.seq.size();
            number_of_expanded_terms *= expanded_addref.seq.size();
            number_of_adds++;
        }
        current_position++;
    }

    if (number_of_adds==0) {
        return (new ncmul(expanded_seq,1))->setflag(status_flags::dynallocated ||
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
        exvector term;
        term=expanded_seq;
        for (l=0; l<number_of_adds; l++) {
            ASSERT(is_ex_exactly_of_type(expanded_seq[positions_of_adds[l]],add));
            add const & addref=ex_to_add(expanded_seq[positions_of_adds[l]]);
            term[positions_of_adds[l]]=addref.recombine_pair_to_ex(addref.seq[k[l]]);
        }
        distrseq.push_back((new ncmul(term,1))->setflag(status_flags::dynallocated |
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

int ncmul::degree(symbol const & s) const
{
    int deg_sum=0;
    for (exvector::const_iterator cit=seq.begin(); cit!=seq.end(); ++cit) {
        deg_sum+=(*cit).degree(s);
    }
    return deg_sum;
}

int ncmul::ldegree(symbol const & s) const
{
    int deg_sum=0;
    for (exvector::const_iterator cit=seq.begin(); cit!=seq.end(); ++cit) {
        deg_sum+=(*cit).ldegree(s);
    }
    return deg_sum;
}

ex ncmul::coeff(symbol const & s, int const n) const
{
    exvector coeffseq;
    coeffseq.reserve(seq.size());

    if (n==0) {
        // product of individual coeffs
        // if a non-zero power of s is found, the resulting product will be 0
        exvector::const_iterator it=seq.begin();
        while (it!=seq.end()) {
            coeffseq.push_back((*it).coeff(s,n));
            ++it;
        }
        return (new ncmul(coeffseq,1))->setflag(status_flags::dynallocated);
    }
         
    exvector::const_iterator it=seq.begin();
    bool coeff_found=0;
    while (it!=seq.end()) {
        ex c=(*it).coeff(s,n);
        if (!c.is_zero()) {
            coeffseq.push_back(c);
            coeff_found=1;
        } else {
            coeffseq.push_back(*it);
        }
        ++it;
    }

    if (coeff_found) return (new ncmul(coeffseq,1))->setflag(status_flags::dynallocated);
    
    return exZERO();
}

unsigned ncmul::count_factors(ex const & e) const
{
    if ((is_ex_exactly_of_type(e,mul)&&(e.return_type()!=return_types::commutative))||
        (is_ex_exactly_of_type(e,ncmul))) {
        unsigned factors=0;
        for (int i=0; i<e.nops(); i++) {
            factors += count_factors(e.op(i));
        }
        return factors;
    }
    return 1;
}
        
void ncmul::append_factors(exvector & v, ex const & e) const
{
    if ((is_ex_exactly_of_type(e,mul)&&(e.return_type()!=return_types::commutative))||
        (is_ex_exactly_of_type(e,ncmul))) {
        for (int i=0; i<e.nops(); i++) {
            append_factors(v,e.op(i));
        }
        return;
    }
    v.push_back(e);
}

typedef vector<unsigned> unsignedvector;
typedef vector<exvector> exvectorvector;

ex ncmul::eval(int level) const
{
    // simplifications: ncmul(...,*(x1,x2),...,ncmul(x3,x4),...) ->
    //                      ncmul(...,x1,x2,...,x3,x4,...) (associativity)
    //                  ncmul(x) -> x
    //                  ncmul() -> 1
    //                  ncmul(...,c1,...,c2,...) ->
    //                      *(c1,c2,ncmul(...)) (pull out commutative elements)
    //                  ncmul(x1,y1,x2,y2) -> *(ncmul(x1,x2),ncmul(y1,y2))
    //                      (collect elements of same type)
    //                  ncmul(x1,x2,x3,...) -> x::eval_ncmul(x1,x2,x3,...)
    // the following rule would be nice, but produces a recursion,
    // which must be trapped by introducing a flag that the sub-ncmuls()
    // are already evaluated (maybe later...)
    //                  ncmul(x1,x2,...,X,y1,y2,...) ->
    //                      ncmul(ncmul(x1,x2,...),X,ncmul(y1,y2,...)
    //                      (X noncommutative_composite)

    if ((level==1)&&(flags & status_flags::evaluated)) {
        return *this;
    }

    exvector evaledseq=evalchildren(level);

    // ncmul(...,*(x1,x2),...,ncmul(x3,x4),...) ->
    //     ncmul(...,x1,x2,...,x3,x4,...) (associativity)
    unsigned factors=0;
    for (exvector::const_iterator cit=evaledseq.begin(); cit!=evaledseq.end(); ++cit) {
        factors += count_factors(*cit);
    }

    exvector assocseq;
    assocseq.reserve(factors);
    for (exvector::const_iterator cit=evaledseq.begin(); cit!=evaledseq.end(); ++cit) {
        append_factors(assocseq,*cit);
    }

    // ncmul(x) -> x
    if (assocseq.size()==1) return *(seq.begin());

    // ncmul() -> 1
    if (assocseq.size()==0) return exONE();

    // determine return types
    unsignedvector rettypes;
    rettypes.reserve(assocseq.size());
    unsigned i=0;
    unsigned count_commutative=0;
    unsigned count_noncommutative=0;
    unsigned count_noncommutative_composite=0;
    for (exvector::const_iterator cit=assocseq.begin(); cit!=assocseq.end(); ++cit) {
        switch (rettypes[i]=(*cit).return_type()) {
        case return_types::commutative:
            count_commutative++;
            break;
        case return_types::noncommutative:
            count_noncommutative++;
            break;
        case return_types::noncommutative_composite:
            count_noncommutative_composite++;
            break;
        default:
            throw(std::logic_error("ncmul::eval(): invalid return type"));
        }
        ++i;
    }
    ASSERT(count_commutative+count_noncommutative+count_noncommutative_composite==assocseq.size());

    // ncmul(...,c1,...,c2,...) ->
    //     *(c1,c2,ncmul(...)) (pull out commutative elements)
    if (count_commutative!=0) {
        exvector commutativeseq;
        commutativeseq.reserve(count_commutative+1);
        exvector noncommutativeseq;
        noncommutativeseq.reserve(assocseq.size()-count_commutative);
        for (i=0; i<assocseq.size(); ++i) {
            if (rettypes[i]==return_types::commutative) {
                commutativeseq.push_back(assocseq[i]);
            } else {
                noncommutativeseq.push_back(assocseq[i]);
            }
        }
        commutativeseq.push_back((new ncmul(noncommutativeseq,1))->
                                  setflag(status_flags::dynallocated));
        return (new mul(commutativeseq))->setflag(status_flags::dynallocated);
    }
        
    // ncmul(x1,y1,x2,y2) -> *(ncmul(x1,x2),ncmul(y1,y2))
    //     (collect elements of same type)

    if (count_noncommutative_composite==0) {
        // there are neither commutative nor noncommutative_composite
        // elements in assocseq
        ASSERT(count_commutative==0);

        exvectorvector evv;
        unsignedvector rttinfos;
        evv.reserve(assocseq.size());
        rttinfos.reserve(assocseq.size());

        for (exvector::const_iterator cit=assocseq.begin(); cit!=assocseq.end(); ++cit) {
            unsigned ti=(*cit).return_type_tinfo();
            // search type in vector of known types
            for (i=0; i<rttinfos.size(); ++i) {
                if (ti==rttinfos[i]) {
                    evv[i].push_back(*cit);
                    break;
                }
            }
            if (i>=rttinfos.size()) {
                // new type
                rttinfos.push_back(ti);
                evv.push_back(exvector());
                (*(evv.end()-1)).reserve(assocseq.size());
                (*(evv.end()-1)).push_back(*cit);
            }
        }

#ifdef DOASSERT
        ASSERT(evv.size()==rttinfos.size());
        ASSERT(evv.size()>0);
        unsigned s=0;
        for (i=0; i<evv.size(); ++i) {
            s += evv[i].size();
        }
        ASSERT(s==assocseq.size());
#endif // def DOASSERT
        
        // if all elements are of same type, simplify the string
        if (evv.size()==1) {
            return evv[0][0].simplify_ncmul(evv[0]);
        }
        
        exvector splitseq;
        splitseq.reserve(evv.size());
        for (i=0; i<evv.size(); ++i) {
            splitseq.push_back((new ncmul(evv[i]))->
                               setflag(status_flags::dynallocated));
        }

        return (new mul(splitseq))->setflag(status_flags::dynallocated);
    }
    
    return (new ncmul(assocseq))->setflag(status_flags::dynallocated |
                                          status_flags::evaluated);
}

exvector ncmul::get_indices(void) const
{
    // return union of indices of factors
    exvector iv;
    for (exvector::const_iterator cit=seq.begin(); cit!=seq.end(); ++cit) {
        exvector subiv=(*cit).get_indices();
        iv.reserve(iv.size()+subiv.size());
        for (exvector::const_iterator cit2=subiv.begin(); cit2!=subiv.end(); ++cit2) {
            iv.push_back(*cit2);
        }
    }
    return iv;
}

ex ncmul::subs(lst const & ls, lst const & lr) const
{
    return ncmul(subschildren(ls, lr));
}

ex ncmul::thisexprseq(exvector const & v) const
{
    return (new ncmul(v))->setflag(status_flags::dynallocated);
}

ex ncmul::thisexprseq(exvector * vp) const
{
    return (new ncmul(vp))->setflag(status_flags::dynallocated);
}

// protected

int ncmul::compare_same_type(basic const & other) const
{
    return exprseq::compare_same_type(other);
}

unsigned ncmul::return_type(void) const
{
    if (seq.size()==0) {
        // ncmul without factors: should not happen, but commutes
        return return_types::commutative;
    }

    bool all_commutative=1;
    unsigned rt;
    exvector::const_iterator cit_noncommutative_element; // point to first found nc element

    for (exvector::const_iterator cit=seq.begin(); cit!=seq.end(); ++cit) {
        rt=(*cit).return_type();
        if (rt==return_types::noncommutative_composite) return rt; // one ncc -> mul also ncc
        if ((rt==return_types::noncommutative)&&(all_commutative)) {
            // first nc element found, remember position
            cit_noncommutative_element=cit;
            all_commutative=0;
        }
        if ((rt==return_types::noncommutative)&&(!all_commutative)) {
	    // another nc element found, compare type_infos
            if ((*cit_noncommutative_element).return_type_tinfo()!=(*cit).return_type_tinfo()) {
	        // diffent types -> mul is ncc
	        return return_types::noncommutative_composite;
            }
        }
    }
    // all factors checked
    ASSERT(!all_commutative); // not all factors should commute, because this is a ncmul();
    return all_commutative ? return_types::commutative : return_types::noncommutative;
}
   
unsigned ncmul::return_type_tinfo(void) const
{
    if (seq.size()==0) {
        // mul without factors: should not happen
        return tinfo_key;
    }
    // return type_info of first noncommutative element
    for (exvector::const_iterator cit=seq.begin(); cit!=seq.end(); ++cit) {
        if ((*cit).return_type()==return_types::noncommutative) {
            return (*cit).return_type_tinfo();
        }
    }
    // no noncommutative element found, should not happen
    return tinfo_key;
}

//////////
// new virtual functions which can be overridden by derived classes
//////////

// none

//////////
// non-virtual functions in this class
//////////

exvector ncmul::expandchildren(unsigned options) const
{
    exvector s;
    s.reserve(seq.size());

    for (exvector::const_iterator it=seq.begin(); it!=seq.end(); ++it) {
        s.push_back((*it).expand(options));
    }
    return s;
}

exvector const & ncmul::get_factors(void) const
{
    return seq;
}

//////////
// static member variables
//////////

// protected

unsigned ncmul::precedence=50;


//////////
// global constants
//////////

const ncmul some_ncmul;
type_info const & typeid_ncmul=typeid(some_ncmul);

//////////
// friend functions
//////////

ex nonsimplified_ncmul(exvector const & v)
{
    return (new ncmul(v))->setflag(status_flags::dynallocated);
}

ex simplified_ncmul(exvector const & v)
{
    if (v.size()==0) {
        return exONE();
    } else if (v.size()==1) {
        return v[0];
    }
    return (new ncmul(v))->setflag(status_flags::dynallocated |
                                   status_flags::evaluated);
}


