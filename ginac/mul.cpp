/** @file mul.cpp
 *
 *  Implementation of GiNaC's products of expressions. */

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

#include <vector>
#include <stdexcept>

#include "mul.h"
#include "add.h"
#include "power.h"
#include "archive.h"
#include "debugmsg.h"
#include "utils.h"

#ifndef NO_NAMESPACE_GINAC
namespace GiNaC {
#endif // ndef NO_NAMESPACE_GINAC

GINAC_IMPLEMENT_REGISTERED_CLASS(mul, expairseq)

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

mul::mul(const mul & other)
{
    debugmsg("mul copy constructor",LOGLEVEL_CONSTRUCT);
    copy(other);
}

const mul & mul::operator=(const mul & other)
{
    debugmsg("mul operator=",LOGLEVEL_ASSIGNMENT);
    if (this != &other) {
        destroy(1);
        copy(other);
    }
    return *this;
}

// protected

void mul::copy(const mul & other)
{
    inherited::copy(other);
}

void mul::destroy(bool call_parent)
{
    if (call_parent) inherited::destroy(call_parent);
}

//////////
// other constructors
//////////

// public

mul::mul(const ex & lh, const ex & rh)
{
    debugmsg("mul constructor from ex,ex",LOGLEVEL_CONSTRUCT);
    tinfo_key = TINFO_mul;
    overall_coeff = _ex1();
    construct_from_2_ex(lh,rh);
    GINAC_ASSERT(is_canonical());
}

mul::mul(const exvector & v)
{
    debugmsg("mul constructor from exvector",LOGLEVEL_CONSTRUCT);
    tinfo_key = TINFO_mul;
    overall_coeff = _ex1();
    construct_from_exvector(v);
    GINAC_ASSERT(is_canonical());
}

mul::mul(const epvector & v)
{
    debugmsg("mul constructor from epvector",LOGLEVEL_CONSTRUCT);
    tinfo_key = TINFO_mul;
    overall_coeff = _ex1();
    construct_from_epvector(v);
    GINAC_ASSERT(is_canonical());
}

mul::mul(const epvector & v, const ex & oc)
{
    debugmsg("mul constructor from epvector,ex",LOGLEVEL_CONSTRUCT);
    tinfo_key = TINFO_mul;
    overall_coeff = oc;
    construct_from_epvector(v);
    GINAC_ASSERT(is_canonical());
}

mul::mul(epvector * vp, const ex & oc)
{
    debugmsg("mul constructor from epvector *,ex",LOGLEVEL_CONSTRUCT);
    tinfo_key = TINFO_mul;
    GINAC_ASSERT(vp!=0);
    overall_coeff = oc;
    construct_from_epvector(*vp);
    delete vp;
    GINAC_ASSERT(is_canonical());
}

mul::mul(const ex & lh, const ex & mh, const ex & rh)
{
    debugmsg("mul constructor from ex,ex,ex",LOGLEVEL_CONSTRUCT);
    tinfo_key = TINFO_mul;
    exvector factors;
    factors.reserve(3);
    factors.push_back(lh);
    factors.push_back(mh);
    factors.push_back(rh);
    overall_coeff = _ex1();
    construct_from_exvector(factors);
    GINAC_ASSERT(is_canonical());
}

//////////
// archiving
//////////

/** Construct object from archive_node. */
mul::mul(const archive_node &n, const lst &sym_lst) : inherited(n, sym_lst)
{
    debugmsg("mul constructor from archive_node", LOGLEVEL_CONSTRUCT);
}

/** Unarchive the object. */
ex mul::unarchive(const archive_node &n, const lst &sym_lst)
{
    return (new mul(n, sym_lst))->setflag(status_flags::dynallocated);
}

/** Archive the object. */
void mul::archive(archive_node &n) const
{
    inherited::archive(n);
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

void mul::print(ostream & os, unsigned upper_precedence) const
{
    debugmsg("mul print",LOGLEVEL_PRINT);
    if (precedence<=upper_precedence) os << "(";
    bool first=true;
    // first print the overall numeric coefficient:
    numeric coeff = ex_to_numeric(overall_coeff);
    if (coeff.csgn()==-1) os << '-';
    if (!coeff.is_equal(_num1()) &&
        !coeff.is_equal(_num_1())) {
        if (coeff.is_rational()) {
            if (coeff.is_negative())
                os << -coeff;
            else
                os << coeff;
        } else {
            if (coeff.csgn()==-1)
                (-coeff).print(os, precedence);
            else
                coeff.print(os, precedence);
        }
        os << '*';
    }
    // then proceed with the remaining factors:
    for (epvector::const_iterator cit=seq.begin(); cit!=seq.end(); ++cit) {
        if (!first) {
            os << '*';
        } else {
            first=false;
        }
        recombine_pair_to_ex(*cit).print(os,precedence);
    }
    if (precedence<=upper_precedence) os << ")";
}

void mul::printraw(ostream & os) const
{
    debugmsg("mul printraw",LOGLEVEL_PRINT);

    os << "*(";
    for (epvector::const_iterator it=seq.begin(); it!=seq.end(); ++it) {
        os << "(";
        (*it).rest.bp->printraw(os);
        os << ",";
        (*it).coeff.bp->printraw(os);
        os << "),";
    }
    os << ",hash=" << hashvalue << ",flags=" << flags;
    os << ")";
}

void mul::printcsrc(ostream & os, unsigned type, unsigned upper_precedence) const
{
    debugmsg("mul print csrc", LOGLEVEL_PRINT);
    if (precedence <= upper_precedence)
        os << "(";

    if (!overall_coeff.is_equal(_ex1())) {
        overall_coeff.bp->printcsrc(os,type,precedence);
        os << "*";
    }
    
    // Print arguments, separated by "*" or "/"
    epvector::const_iterator it = seq.begin();
    epvector::const_iterator itend = seq.end();
    while (it != itend) {

        // If the first argument is a negative integer power, it gets printed as "1.0/<expr>"
        if (it == seq.begin() && ex_to_numeric(it->coeff).is_integer() && it->coeff.compare(_num0()) < 0) {
            if (type == csrc_types::ctype_cl_N)
                os << "recip(";
            else
                os << "1.0/";
        }

        // If the exponent is 1 or -1, it is left out
        if (it->coeff.compare(_ex1()) == 0 || it->coeff.compare(_num_1()) == 0)
            it->rest.bp->printcsrc(os, type, precedence);
        else
            // outer parens around ex needed for broken gcc-2.95 parser:
            (ex(power(it->rest, abs(ex_to_numeric(it->coeff))))).bp->printcsrc(os, type, upper_precedence);

        // Separator is "/" for negative integer powers, "*" otherwise
        it++;
        if (it != itend) {
            if (ex_to_numeric(it->coeff).is_integer() && it->coeff.compare(_num0()) < 0)
                os << "/";
            else
                os << "*";
        }
    }
    if (precedence <= upper_precedence)
        os << ")";
}

bool mul::info(unsigned inf) const
{
    // TODO: optimize
    if (inf==info_flags::polynomial ||
        inf==info_flags::integer_polynomial ||
        inf==info_flags::cinteger_polynomial ||
        inf==info_flags::rational_polynomial ||
        inf==info_flags::crational_polynomial ||
        inf==info_flags::rational_function) {
        for (epvector::const_iterator it=seq.begin(); it!=seq.end(); ++it) {
            if (!(recombine_pair_to_ex(*it).info(inf)))
                return false;
        }
        return overall_coeff.info(inf);
    } else {
        return inherited::info(inf);
    }
}

typedef vector<int> intvector;

int mul::degree(const symbol & s) const
{
    int deg_sum=0;
    for (epvector::const_iterator cit=seq.begin(); cit!=seq.end(); ++cit) {
        deg_sum+=(*cit).rest.degree(s) * ex_to_numeric((*cit).coeff).to_int();
    }
    return deg_sum;
}

int mul::ldegree(const symbol & s) const
{
    int deg_sum=0;
    for (epvector::const_iterator cit=seq.begin(); cit!=seq.end(); ++cit) {
        deg_sum+=(*cit).rest.ldegree(s) * ex_to_numeric((*cit).coeff).to_int();
    }
    return deg_sum;
}

ex mul::coeff(const symbol & s, int n) const
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
    
    return _ex0();
}

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

#ifdef DO_GINAC_ASSERT
    for (epvector::const_iterator cit=seq.begin(); cit!=seq.end(); ++cit) {
        GINAC_ASSERT((!is_ex_exactly_of_type((*cit).rest,mul))||
               (!(ex_to_numeric((*cit).coeff).is_integer())));
        GINAC_ASSERT(!((*cit).is_numeric_with_coeff_1()));
        if (is_ex_exactly_of_type(recombine_pair_to_ex(*cit),numeric)) {
            printtree(cerr,0);
        }
        GINAC_ASSERT(!is_ex_exactly_of_type(recombine_pair_to_ex(*cit),numeric));
        /* for paranoia */
        expair p=split_ex_to_pair(recombine_pair_to_ex(*cit));
        GINAC_ASSERT(p.rest.is_equal((*cit).rest));
        GINAC_ASSERT(p.coeff.is_equal((*cit).coeff));
        /* end paranoia */
    }
#endif // def DO_GINAC_ASSERT

    if (flags & status_flags::evaluated) {
        GINAC_ASSERT(seq.size()>0);
        GINAC_ASSERT((seq.size()>1)||!overall_coeff.is_equal(_ex1()));
        return *this;
    }

    int seq_size=seq.size();
    if (overall_coeff.is_equal(_ex0())) {
        // *(...,x;0) -> 0
        return _ex0();
    } else if (seq_size==0) {
        // *(;c) -> c
        return overall_coeff;
    } else if ((seq_size==1)&&overall_coeff.is_equal(_ex1())) {
        // *(x;1) -> x
        return recombine_pair_to_ex(*(seq.begin()));
    } else if ((seq_size==1) &&
               is_ex_exactly_of_type((*seq.begin()).rest,add) &&
               ex_to_numeric((*seq.begin()).coeff).is_equal(_num1())) {
        // *(+(x,y,...);c) -> +(*(x,c),*(y,c),...) (c numeric(), no powers of +())
        const add & addref=ex_to_add((*seq.begin()).rest);
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

ex mul::simplify_ncmul(const exvector & v) const
{
    throw(std::logic_error("mul::simplify_ncmul() should never have been called!"));
}

// protected

/** Implementation of ex::diff() for a product. It applies the product rule.
 *  @see ex::diff */
ex mul::derivative(const symbol & s) const
{
    exvector new_seq;
    new_seq.reserve(seq.size());

    // D(a*b*c)=D(a)*b*c+a*D(b)*c+a*b*D(c)
    for (unsigned i=0; i!=seq.size(); i++) {
        epvector sub_seq=seq;
        sub_seq[i] = split_ex_to_pair(sub_seq[i].coeff*
                                      power(sub_seq[i].rest,sub_seq[i].coeff-1)*
                                      sub_seq[i].rest.diff(s));
        new_seq.push_back((new mul(sub_seq,overall_coeff))->setflag(status_flags::dynallocated));
    }
    return (new add(new_seq))->setflag(status_flags::dynallocated);
}

int mul::compare_same_type(const basic & other) const
{
    return inherited::compare_same_type(other);
}

bool mul::is_equal_same_type(const basic & other) const
{
    return inherited::is_equal_same_type(other);
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

ex mul::thisexpairseq(const epvector & v, const ex & oc) const
{
    return (new mul(v,oc))->setflag(status_flags::dynallocated);
}

ex mul::thisexpairseq(epvector * vp, const ex & oc) const
{
    return (new mul(vp,oc))->setflag(status_flags::dynallocated);
}

expair mul::split_ex_to_pair(const ex & e) const
{
    if (is_ex_exactly_of_type(e,power)) {
        const power & powerref=ex_to_power(e);
        if (is_ex_exactly_of_type(powerref.exponent,numeric)) {
            return expair(powerref.basis,powerref.exponent);
        }
    }
    return expair(e,_ex1());
}
    
expair mul::combine_ex_with_coeff_to_pair(const ex & e,
                                          const ex & c) const
{
    // to avoid duplication of power simplification rules,
    // we create a temporary power object
    // otherwise it would be hard to correctly simplify
    // expression like (4^(1/3))^(3/2)
    if (are_ex_trivially_equal(c,_ex1())) {
        return split_ex_to_pair(e);
    }
    return split_ex_to_pair(power(e,c));
}
    
expair mul::combine_pair_with_coeff_to_pair(const expair & p,
                                            const ex & c) const
{
    // to avoid duplication of power simplification rules,
    // we create a temporary power object
    // otherwise it would be hard to correctly simplify
    // expression like (4^(1/3))^(3/2)
    if (are_ex_trivially_equal(c,_ex1())) {
        return p;
    }
    return split_ex_to_pair(power(recombine_pair_to_ex(p),c));
}
    
ex mul::recombine_pair_to_ex(const expair & p) const
{
    // if (p.coeff.compare(_ex1())==0) {
    // if (are_ex_trivially_equal(p.coeff,_ex1())) {
    if (ex_to_numeric(p.coeff).is_equal(_num1())) {
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
        if (ex_to_numeric((*it).coeff).is_equal(_num1())) {
            // combined pair has coeff 1 and must be moved to the end
            return true;
        }
    }
    return false;
}       

ex mul::default_overall_coeff(void) const
{
    return _ex1();
}

void mul::combine_overall_coeff(const ex & c)
{
    GINAC_ASSERT(is_ex_exactly_of_type(overall_coeff,numeric));
    GINAC_ASSERT(is_ex_exactly_of_type(c,numeric));
    overall_coeff = ex_to_numeric(overall_coeff).mul_dyn(ex_to_numeric(c));
}

void mul::combine_overall_coeff(const ex & c1, const ex & c2)
{
    GINAC_ASSERT(is_ex_exactly_of_type(overall_coeff,numeric));
    GINAC_ASSERT(is_ex_exactly_of_type(c1,numeric));
    GINAC_ASSERT(is_ex_exactly_of_type(c2,numeric));
    overall_coeff = ex_to_numeric(overall_coeff).
                        mul_dyn(ex_to_numeric(c1).power(ex_to_numeric(c2)));
}

bool mul::can_make_flat(const expair & p) const
{
    GINAC_ASSERT(is_ex_exactly_of_type(p.coeff,numeric));
    // this assertion will probably fail somewhere
    // it would require a more careful make_flat, obeying the power laws
    // probably should return true only if p.coeff is integer
    return ex_to_numeric(p.coeff).is_equal(_num1());
}

ex mul::expand(unsigned options) const
{
    exvector sub_expanded_seq;
    intvector positions_of_adds;
    intvector number_of_add_operands;

    epvector * expanded_seqp=expandchildren(options);

    const epvector & expanded_seq = expanded_seqp==0 ? seq : *expanded_seqp;

    positions_of_adds.resize(expanded_seq.size());
    number_of_add_operands.resize(expanded_seq.size());

    int number_of_adds=0;
    int number_of_expanded_terms=1;

    unsigned current_position=0;
    epvector::const_iterator last=expanded_seq.end();
    for (epvector::const_iterator cit=expanded_seq.begin(); cit!=last; ++cit) {
        if (is_ex_exactly_of_type((*cit).rest,add)&&
            (ex_to_numeric((*cit).coeff).is_equal(_num1()))) {
            positions_of_adds[number_of_adds]=current_position;
            const add & expanded_addref=ex_to_add((*cit).rest);
            unsigned addref_nops=expanded_addref.nops();
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
            const add & addref=ex_to_add(expanded_seq[positions_of_adds[l]].rest);
            GINAC_ASSERT(term[positions_of_adds[l]].coeff.compare(_ex1())==0);
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
        const ex & factor=recombine_pair_to_ex(*cit);
        const ex & expanded_factor=factor.expand(options);
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
const type_info & typeid_mul=typeid(some_mul);

#ifndef NO_NAMESPACE_GINAC
} // namespace GiNaC
#endif // ndef NO_NAMESPACE_GINAC
