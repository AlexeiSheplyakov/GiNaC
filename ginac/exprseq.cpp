/** @file exprseq.cpp
 *
 *  Implementation of GiNaC's exprseq. */

/*
 *  This file was generated automatically by container.pl.
 *  Please do not modify it directly, edit the perl script instead!
 *  container.pl options: $CONTAINER=exprseq
 *                        $STLHEADER=vector
 *                        $reserve=1
 *                        $prepend=0
 *                        $let_op=0
 *                        $open_bracket=(
 *                        $close_bracket=)
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

#include <iostream>
#include <stdexcept>

#include "exprseq.h"
#include "ex.h"
#include "debugmsg.h"

namespace GiNaC {

#define RESERVE(s,size) (s).reserve(size)

//////////
// default constructor, destructor, copy constructor assignment operator and helpers
//////////

// public

exprseq::exprseq() : basic(TINFO_exprseq)
{
    debugmsg("exprseq default constructor",LOGLEVEL_CONSTRUCT);
}

exprseq::~exprseq()
{
    debugmsg("exprseq destructor",LOGLEVEL_DESTRUCT);
    destroy(0);
}

exprseq::exprseq(exprseq const & other)
{
    debugmsg("exprseq copy constructor",LOGLEVEL_CONSTRUCT);
    copy(other);
}

exprseq const & exprseq::operator=(exprseq const & other)
{
    debugmsg("exprseq operator=",LOGLEVEL_ASSIGNMENT);
    if (this != &other) {
        destroy(1);
        copy(other);
    }
    return *this;
}

// protected

void exprseq::copy(exprseq const & other)
{
    basic::copy(other);
    seq=other.seq;
}

void exprseq::destroy(bool call_parent)
{
    seq.clear();
    if (call_parent) basic::destroy(call_parent);
}

//////////
// other constructors
//////////

// public

exprseq::exprseq(exvector const & s, bool discardable) :  basic(TINFO_exprseq)
{
    debugmsg("exprseq constructor from exvector",
             LOGLEVEL_CONSTRUCT);
    if (discardable) {
        seq.swap(const_cast<exvector &>(s));
    } else {
        seq=s;
    }
}

exprseq::exprseq(exvector * vp) : basic(TINFO_exprseq)
{
    debugmsg("exprseq constructor from exvector *",LOGLEVEL_CONSTRUCT);
    GINAC_ASSERT(vp!=0);
    seq.swap(*vp);
    delete vp;
}

exprseq::exprseq(ex const & e1) :  basic(TINFO_exprseq)
{
    debugmsg("exprseq constructor from 1 ex",
             LOGLEVEL_CONSTRUCT);
    RESERVE(seq,1);
    seq.push_back(e1);
}

exprseq::exprseq(ex const & e1, ex const & e2) : basic(TINFO_exprseq)
{
    debugmsg("exprseq constructor from 2 ex",
             LOGLEVEL_CONSTRUCT);
    RESERVE(seq,2);
    seq.push_back(e1);
    seq.push_back(e2);
}

exprseq::exprseq(ex const & e1, ex const & e2, ex const & e3)
    : basic(TINFO_exprseq)
{
    debugmsg("exprseq constructor from 3 ex",
             LOGLEVEL_CONSTRUCT);
    RESERVE(seq,3);
    seq.push_back(e1);
    seq.push_back(e2);
    seq.push_back(e3);
}

exprseq::exprseq(ex const & e1, ex const & e2, ex const & e3,
                     ex const & e4) : basic(TINFO_exprseq)
{
    debugmsg("exprseq constructor from 4 ex",
             LOGLEVEL_CONSTRUCT);
    RESERVE(seq,4);
    seq.push_back(e1);
    seq.push_back(e2);
    seq.push_back(e3);
    seq.push_back(e4);
}

exprseq::exprseq(ex const & e1, ex const & e2, ex const & e3,
                     ex const & e4, ex const & e5) : basic(TINFO_exprseq)
{
    debugmsg("exprseq constructor from 5 ex",
             LOGLEVEL_CONSTRUCT);
    RESERVE(seq,5);
    seq.push_back(e1);
    seq.push_back(e2);
    seq.push_back(e3);
    seq.push_back(e4);
    seq.push_back(e5);
}

exprseq::exprseq(ex const & e1, ex const & e2, ex const & e3,
                     ex const & e4, ex const & e5, ex const & e6)
    : basic(TINFO_exprseq)
{
    debugmsg("exprseq constructor from 6 ex",
             LOGLEVEL_CONSTRUCT);
    RESERVE(seq,6);
    seq.push_back(e1);
    seq.push_back(e2);
    seq.push_back(e3);
    seq.push_back(e4);
    seq.push_back(e5);
    seq.push_back(e6);
}

exprseq::exprseq(ex const & e1, ex const & e2, ex const & e3,
                     ex const & e4, ex const & e5, ex const & e6,
                     ex const & e7) : basic(TINFO_exprseq)
{
    debugmsg("exprseq constructor from 7 ex",
             LOGLEVEL_CONSTRUCT);
    RESERVE(seq,7);
    seq.push_back(e1);
    seq.push_back(e2);
    seq.push_back(e3);
    seq.push_back(e4);
    seq.push_back(e5);
    seq.push_back(e6);
    seq.push_back(e7);
}

exprseq::exprseq(ex const & e1, ex const & e2, ex const & e3,
                     ex const & e4, ex const & e5, ex const & e6,
                     ex const & e7, ex const & e8) : basic(TINFO_exprseq)
{
    debugmsg("exprseq constructor from 8 ex",
             LOGLEVEL_CONSTRUCT);
    RESERVE(seq,8);
    seq.push_back(e1);
    seq.push_back(e2);
    seq.push_back(e3);
    seq.push_back(e4);
    seq.push_back(e5);
    seq.push_back(e6);
    seq.push_back(e7);
    seq.push_back(e8);
}

exprseq::exprseq(ex const & e1, ex const & e2, ex const & e3,
                     ex const & e4, ex const & e5, ex const & e6,
                     ex const & e7, ex const & e8, ex const & e9)
    : basic(TINFO_exprseq)
{
    debugmsg("exprseq constructor from 9 ex",
             LOGLEVEL_CONSTRUCT);
    RESERVE(seq,9);
    seq.push_back(e1);
    seq.push_back(e2);
    seq.push_back(e3);
    seq.push_back(e4);
    seq.push_back(e5);
    seq.push_back(e6);
    seq.push_back(e7);
    seq.push_back(e8);
    seq.push_back(e9);
}

exprseq::exprseq(ex const & e1, ex const & e2, ex const & e3,
                     ex const & e4, ex const & e5, ex const & e6,
                     ex const & e7, ex const & e8, ex const & e9,
                     ex const &e10)
    : basic(TINFO_exprseq)
{
    debugmsg("exprseq constructor from 10 ex",
             LOGLEVEL_CONSTRUCT);
    RESERVE(seq,10);
    seq.push_back(e1);
    seq.push_back(e2);
    seq.push_back(e3);
    seq.push_back(e4);
    seq.push_back(e5);
    seq.push_back(e6);
    seq.push_back(e7);
    seq.push_back(e8);
    seq.push_back(e9);
    seq.push_back(e10);
}

//////////
// functions overriding virtual functions from bases classes
//////////

// public

basic * exprseq::duplicate() const
{
    debugmsg("exprseq duplicate",LOGLEVEL_DUPLICATE);
    return new exprseq(*this);
}

void exprseq::printraw(ostream & os) const
{
    debugmsg("exprseq printraw",LOGLEVEL_PRINT);

    os << "exprseq(";
    for (exvector::const_iterator cit=seq.begin(); cit!=seq.end(); ++cit) {
        (*cit).bp->printraw(os);
        os << ",";
    }
    os << ")";
}

void exprseq::print(ostream & os, unsigned upper_precedence) const
{
    debugmsg("exprseq print",LOGLEVEL_PRINT);
    // always print brackets around seq, ignore upper_precedence
    printseq(os,'(',',',')',precedence,precedence+1);
}

void exprseq::printtree(ostream & os, unsigned indent) const
{
    debugmsg("exprseq printtree",LOGLEVEL_PRINT);

    os << string(indent,' ') << "type=" << typeid(*this).name()
       << ", hash=" << hashvalue << " (0x" << hex << hashvalue << dec << ")"
       << ", flags=" << flags
       << ", nops=" << nops() << endl;
    for (exvector::const_iterator cit=seq.begin(); cit!=seq.end(); ++cit) {
        (*cit).printtree(os,indent+delta_indent);
    }
    os << string(indent+delta_indent,' ') << "=====" << endl;
}

// exprseq::info() will be implemented by user elsewhere";

int exprseq::nops() const
{
    return seq.size();
}

// exprseq::let_op() will be implemented by user elsewhere

ex exprseq::expand(unsigned options) const
{
    exvector s;
    RESERVE(s,seq.size());
    for (exvector::const_iterator it=seq.begin(); it!=seq.end(); ++it) {
        s.push_back((*it).expand(options));
    }

    return thisexprseq(s);
}

// a exprseq 'has' an expression if it is this expression itself or a child 'has' it

bool exprseq::has(ex const & other) const
{
    GINAC_ASSERT(other.bp!=0);
    if (is_equal(*other.bp)) return true;
    for (exvector::const_iterator it=seq.begin(); it!=seq.end(); ++it) {
        if ((*it).has(other)) return true;
    }
    return false;
}

ex exprseq::eval(int level) const
{
    if (level==1) {
        return this->hold();
    }
    return thisexprseq(evalchildren(level));
}

ex exprseq::evalf(int level) const
{
    return thisexprseq(evalfchildren(level));
}

/** Implementation of ex::normal() for exprseqs. It normalizes the arguments
 *  and replaces the exprseq by a temporary symbol.
 *  @see ex::normal */
ex exprseq::normal(lst &sym_lst, lst &repl_lst, int level) const
{
    ex n=thisexprseq(normalchildren(level));
    return n.bp->basic::normal(sym_lst,repl_lst,level);
}

ex exprseq::diff(symbol const & s) const
{
    return thisexprseq(diffchildren(s));
}

ex exprseq::subs(lst const & ls, lst const & lr) const
{
    exvector * vp=subschildren(ls,lr);
    if (vp==0) {
        return *this;
    }
    return thisexprseq(vp);
}

// protected

int exprseq::compare_same_type(basic const & other) const
{
    GINAC_ASSERT(is_of_type(other,exprseq));
    exprseq const & o=static_cast<exprseq const &>
                                    (const_cast<basic &>(other));
    int cmpval;
    exvector::const_iterator it1=seq.begin();
    exvector::const_iterator it2=o.seq.begin();

    for (; (it1!=seq.end())&&(it2!=o.seq.end()); ++it1, ++it2) {
        cmpval=(*it1).compare(*it2);
        if (cmpval!=0) return cmpval;
    }

    if (it1==seq.end()) {
        return (it2==o.seq.end() ? 0 : -1);
    }

    return 1;
}

bool exprseq::is_equal_same_type(basic const & other) const
{
    GINAC_ASSERT(is_of_type(other,exprseq));
    exprseq const & o=static_cast<exprseq const &>
                                    (const_cast<basic &>(other));
    if (seq.size()!=o.seq.size()) return false;

    exvector::const_iterator it1=seq.begin();
    exvector::const_iterator it2=o.seq.begin();

    for (; it1!=seq.end(); ++it1, ++it2) {
	if (!(*it1).is_equal(*it2)) return false;
    }

    return true;
}

unsigned exprseq::return_type(void) const
{
    return return_types::noncommutative_composite;
}

//////////
// new virtual functions which can be overridden by derived classes
//////////

// public

exprseq & exprseq::append(ex const & b)
{
    ensure_if_modifiable();
    seq.push_back(b);
    return *this;
}



// protected

void exprseq::printseq(ostream & os, char openbracket, char delim,
                         char closebracket, unsigned this_precedence,
                         unsigned upper_precedence) const
{
    if (this_precedence<=upper_precedence) os << openbracket;
    if (seq.size()!=0) {
        exvector::const_iterator it,it_last;
        it=seq.begin();
        it_last=seq.end();
        --it_last;
        for (; it!=it_last; ++it) {
            (*it).bp->print(os,this_precedence);
            os << delim;
        }
        (*it).bp->print(os,this_precedence);
    }
    if (this_precedence<=upper_precedence) os << closebracket;
}

ex exprseq::thisexprseq(exvector const & v) const
{
    return exprseq(v);
}

ex exprseq::thisexprseq(exvector * vp) const
{
    return exprseq(vp);
}

//////////
// non-virtual functions in this class
//////////

// public

// none

// protected

bool exprseq::is_canonical() const
{
    if (seq.size()<=1) { return 1; }

    exvector::const_iterator it=seq.begin();
    exvector::const_iterator it_last=it;
    for (++it; it!=seq.end(); it_last=it, ++it) {
        if ((*it_last).compare(*it)>0) {
            if ((*it_last).compare(*it)>0) {
                cout << *it_last << ">" << *it << "\n";
                return 0;
	        }
        }
    }
    return 1;
}


exvector exprseq::evalchildren(int level) const
{
    exvector s;
    RESERVE(s,seq.size());

    if (level==1) {
        return seq;
    }
    if (level == -max_recursion_level) {
        throw(std::runtime_error("max recursion level reached"));
    }
    --level;
    for (exvector::const_iterator it=seq.begin(); it!=seq.end(); ++it) {
        s.push_back((*it).eval(level));
    }
    return s;
}

exvector exprseq::evalfchildren(int level) const
{
    exvector s;
    RESERVE(s,seq.size());

    if (level==1) {
        return seq;
    }
    if (level == -max_recursion_level) {
        throw(std::runtime_error("max recursion level reached"));
    }
    --level;
    for (exvector::const_iterator it=seq.begin(); it!=seq.end(); ++it) {
        s.push_back((*it).evalf(level));
    }
    return s;
}

exvector exprseq::normalchildren(int level) const
{
    exvector s;
    RESERVE(s,seq.size());

    if (level==1) {
        return seq;
    }
    if (level == -max_recursion_level) {
        throw(std::runtime_error("max recursion level reached"));
    }
    --level;
    for (exvector::const_iterator it=seq.begin(); it!=seq.end(); ++it) {
        s.push_back((*it).normal(level));
    }
    return s;
}

exvector exprseq::diffchildren(symbol const & y) const
{
    exvector s;
    RESERVE(s,seq.size());
    for (exvector::const_iterator it=seq.begin(); it!=seq.end(); ++it) {
        s.push_back((*it).diff(y));
    }
    return s;
}

/* obsolete subschildren
exvector exprseq::subschildren(lst const & ls, lst const & lr) const
{
    exvector s;
    RESERVE(s,seq.size());
    for (exvector::const_iterator it=seq.begin(); it!=seq.end(); ++it) {
        s.push_back((*it).subs(ls,lr));
    }
    return s;
}
*/

exvector * exprseq::subschildren(lst const & ls, lst const & lr) const
{
    // returns a NULL pointer if nothing had to be substituted
    // returns a pointer to a newly created epvector otherwise
    // (which has to be deleted somewhere else)

    exvector::const_iterator last=seq.end();
    exvector::const_iterator cit=seq.begin();
    while (cit!=last) {
        ex const & subsed_ex=(*cit).subs(ls,lr);
        if (!are_ex_trivially_equal(*cit,subsed_ex)) {

            // something changed, copy seq, subs and return it
            exvector *s=new exvector;
            RESERVE(*s,seq.size());

            // copy parts of seq which are known not to have changed
            exvector::const_iterator cit2=seq.begin();
            while (cit2!=cit) {
                s->push_back(*cit2);
                ++cit2;
            }
            // copy first changed element
            s->push_back(subsed_ex);
            ++cit2;
            // copy rest
            while (cit2!=last) {
                s->push_back((*cit2).subs(ls,lr));
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

unsigned exprseq::precedence=10;

//////////
// global constants
//////////

const exprseq some_exprseq;
type_info const & typeid_exprseq=typeid(some_exprseq);

} // namespace GiNaC

