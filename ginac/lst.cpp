/** @file lst.cpp
 *
 *  Implementation of GiNaC's lst. 
 *  This file was generated automatically by container.pl.
 *  Please do not modify it directly, edit the perl script instead!
 *  container.pl options: $CONTAINER=lst
 *                        $STLHEADER=list
 *                        $reserve=0
 *                        $prepend=1
 *                        $let_op=1
 *                        $open_bracket=[
 *                        $close_bracket=] */

#include <iostream>
#include <stdexcept>

#include "ginac.h"

#define RESERVE(s,size) // no reserve needed for list

//////////
// default constructor, destructor, copy constructor assignment operator and helpers
//////////

// public

lst::lst() : basic(TINFO_LST)
{
    debugmsg("lst default constructor",LOGLEVEL_CONSTRUCT);
}

lst::~lst()
{
    debugmsg("lst destructor",LOGLEVEL_DESTRUCT);
    destroy(0);
}

lst::lst(lst const & other)
{
    debugmsg("lst copy constructor",LOGLEVEL_CONSTRUCT);
    copy(other);
}

lst const & lst::operator=(lst const & other)
{
    debugmsg("lst operator=",LOGLEVEL_ASSIGNMENT);
    if (this != &other) {
        destroy(1);
        copy(other);
    }
    return *this;
}

// protected

void lst::copy(lst const & other)
{
    basic::copy(other);
    seq=other.seq;
}

void lst::destroy(bool call_parent)
{
    seq.clear();
    if (call_parent) basic::destroy(call_parent);
}

//////////
// other constructors
//////////

// public

lst::lst(exlist const & s, bool discardable) :  basic(TINFO_LST)
{
    debugmsg("lst constructor from exlist",
             LOGLEVEL_CONSTRUCT);
    if (discardable) {
        seq.swap(const_cast<exlist &>(s));
    } else {
        seq=s;
    }
}

lst::lst(exlist * vp) : basic(TINFO_LST)
{
    debugmsg("lst constructor from exlist *",LOGLEVEL_CONSTRUCT);
    ASSERT(vp!=0);
    seq.swap(*vp);
    delete vp;
}

lst::lst(ex const & e1) :  basic(TINFO_LST)
{
    debugmsg("lst constructor from 1 ex",
             LOGLEVEL_CONSTRUCT);
    RESERVE(seq,1);
    seq.push_back(e1);
}

lst::lst(ex const & e1, ex const & e2) : basic(TINFO_LST)
{
    debugmsg("lst constructor from 2 ex",
             LOGLEVEL_CONSTRUCT);
    RESERVE(seq,2);
    seq.push_back(e1);
    seq.push_back(e2);
}

lst::lst(ex const & e1, ex const & e2, ex const & e3)
    : basic(TINFO_LST)
{
    debugmsg("lst constructor from 3 ex",
             LOGLEVEL_CONSTRUCT);
    RESERVE(seq,3);
    seq.push_back(e1);
    seq.push_back(e2);
    seq.push_back(e3);
}

lst::lst(ex const & e1, ex const & e2, ex const & e3,
                     ex const & e4) : basic(TINFO_LST)
{
    debugmsg("lst constructor from 4 ex",
             LOGLEVEL_CONSTRUCT);
    RESERVE(seq,4);
    seq.push_back(e1);
    seq.push_back(e2);
    seq.push_back(e3);
    seq.push_back(e4);
}

lst::lst(ex const & e1, ex const & e2, ex const & e3,
                     ex const & e4, ex const & e5) : basic(TINFO_LST)
{
    debugmsg("lst constructor from 5 ex",
             LOGLEVEL_CONSTRUCT);
    RESERVE(seq,5);
    seq.push_back(e1);
    seq.push_back(e2);
    seq.push_back(e3);
    seq.push_back(e4);
    seq.push_back(e5);
}

lst::lst(ex const & e1, ex const & e2, ex const & e3,
                     ex const & e4, ex const & e5, ex const & e6)
    : basic(TINFO_LST)
{
    debugmsg("lst constructor from 6 ex",
             LOGLEVEL_CONSTRUCT);
    RESERVE(seq,6);
    seq.push_back(e1);
    seq.push_back(e2);
    seq.push_back(e3);
    seq.push_back(e4);
    seq.push_back(e5);
    seq.push_back(e6);
}

lst::lst(ex const & e1, ex const & e2, ex const & e3,
                     ex const & e4, ex const & e5, ex const & e6,
                     ex const & e7) : basic(TINFO_LST)
{
    debugmsg("lst constructor from 7 ex",
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

lst::lst(ex const & e1, ex const & e2, ex const & e3,
                     ex const & e4, ex const & e5, ex const & e6,
                     ex const & e7, ex const & e8) : basic(TINFO_LST)
{
    debugmsg("lst constructor from 8 ex",
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

lst::lst(ex const & e1, ex const & e2, ex const & e3,
                     ex const & e4, ex const & e5, ex const & e6,
                     ex const & e7, ex const & e8, ex const & e9)
    : basic(TINFO_LST)
{
    debugmsg("lst constructor from 9 ex",
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

lst::lst(ex const & e1, ex const & e2, ex const & e3,
                     ex const & e4, ex const & e5, ex const & e6,
                     ex const & e7, ex const & e8, ex const & e9,
                     ex const &e10)
    : basic(TINFO_LST)
{
    debugmsg("lst constructor from 10 ex",
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

basic * lst::duplicate() const
{
    debugmsg("lst duplicate",LOGLEVEL_DUPLICATE);
    return new lst(*this);
}

void lst::printraw(ostream & os) const
{
    debugmsg("lst printraw",LOGLEVEL_PRINT);

    os << "lst(";
    for (exlist::const_iterator cit=seq.begin(); cit!=seq.end(); ++cit) {
        (*cit).bp->printraw(os);
        os << ",";
    }
    os << ")";
}

void lst::print(ostream & os, unsigned upper_precedence) const
{
    debugmsg("lst print",LOGLEVEL_PRINT);
    // always print brackets around seq, ignore upper_precedence
    printseq(os,'[',',',']',precedence,precedence+1);
}

void lst::printtree(ostream & os, unsigned indent) const
{
    debugmsg("lst printtree",LOGLEVEL_PRINT);

    os << string(indent,' ') << "type=" << typeid(*this).name()
       << ", hash=" << hashvalue << " (0x" << hex << hashvalue << dec << ")"
       << ", flags=" << flags
       << ", nops=" << nops() << endl;
    for (exlist::const_iterator cit=seq.begin(); cit!=seq.end(); ++cit) {
        (*cit).printtree(os,indent+delta_indent);
    }
    os << string(indent+delta_indent,' ') << "=====" << endl;
}

// lst::info() will be implemented by user elsewhere";

int lst::nops() const
{
    return seq.size();
}

ex & lst::let_op(int const i)
{
    ASSERT(i>=0);
    ASSERT(i<nops());

    exlist::iterator it=seq.begin();
    for (int j=0; j<i; j++) {
        ++it;
    }
    return *it;
}


ex lst::expand(unsigned options) const
{
    exlist s;
    RESERVE(s,seq.size());
    for (exlist::const_iterator it=seq.begin(); it!=seq.end(); ++it) {
        s.push_back((*it).expand(options));
    }

    return thislst(s);
}

// a lst 'has' an expression if it is this expression itself or a child 'has' it

bool lst::has(ex const & other) const
{
    ASSERT(other.bp!=0);
    if (is_equal(*other.bp)) return true;
    for (exlist::const_iterator it=seq.begin(); it!=seq.end(); ++it) {
        if ((*it).has(other)) return true;
    }
    return false;
}

ex lst::eval(int level) const
{
    if (level==1) {
        return this->hold();
    }
    return thislst(evalchildren(level));
}

ex lst::evalf(int level) const
{
    return thislst(evalfchildren(level));
}

/** Implementation of ex::normal() for lsts. It normalizes the arguments
 *  and replaces the lst by a temporary symbol.
 *  @see ex::normal */
ex lst::normal(lst &sym_lst, lst &repl_lst, int level) const
{
    ex n=thislst(normalchildren(level));
    return n.bp->basic::normal(sym_lst,repl_lst,level);
}

ex lst::diff(symbol const & s) const
{
    return thislst(diffchildren(s));
}

ex lst::subs(lst const & ls, lst const & lr) const
{
    exlist * vp=subschildren(ls,lr);
    if (vp==0) {
        return *this;
    }
    return thislst(vp);
}

// protected

int lst::compare_same_type(basic const & other) const
{
    ASSERT(is_of_type(other,lst));
    lst const & o=static_cast<lst const &>
                                    (const_cast<basic &>(other));
    int cmpval;
    exlist::const_iterator it1=seq.begin();
    exlist::const_iterator it2=o.seq.begin();

    for (; (it1!=seq.end())&&(it2!=o.seq.end()); ++it1, ++it2) {
        cmpval=(*it1).compare(*it2);
        if (cmpval!=0) return cmpval;
    }

    if (it1==seq.end()) {
        return (it2==o.seq.end() ? 0 : -1);
    }

    return 1;
}

bool lst::is_equal_same_type(basic const & other) const
{
    ASSERT(is_of_type(other,lst));
    lst const & o=static_cast<lst const &>
                                    (const_cast<basic &>(other));
    if (seq.size()!=o.seq.size()) return false;

    exlist::const_iterator it1=seq.begin();
    exlist::const_iterator it2=o.seq.begin();

    for (; it1!=seq.end(); ++it1, ++it2) {
	if (!(*it1).is_equal(*it2)) return false;
    }

    return true;
}

unsigned lst::return_type(void) const
{
    return return_types::noncommutative_composite;
}

//////////
// new virtual functions which can be overridden by derived classes
//////////

// public

lst & lst::append(ex const & b)
{
    ensure_if_modifiable();
    seq.push_back(b);
    return *this;
}

lst & lst::prepend(ex const & b)
{
    ensure_if_modifiable();
    seq.push_front(b);
    return *this;
}


// protected

void lst::printseq(ostream & os, char openbracket, char delim,
                         char closebracket, unsigned this_precedence,
                         unsigned upper_precedence) const
{
    if (this_precedence<=upper_precedence) os << openbracket;
    if (seq.size()!=0) {
        exlist::const_iterator it,it_last;
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

ex lst::thislst(exlist const & v) const
{
    return lst(v);
}

ex lst::thislst(exlist * vp) const
{
    return lst(vp);
}

//////////
// non-virtual functions in this class
//////////

// public

// none

// protected

bool lst::is_canonical() const
{
    if (seq.size()<=1) { return 1; }

    exlist::const_iterator it=seq.begin();
    exlist::const_iterator it_last=it;
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


exlist lst::evalchildren(int level) const
{
    exlist s;
    RESERVE(s,seq.size());

    if (level==1) {
        return seq;
    }
    if (level == -max_recursion_level) {
        throw(std::runtime_error("max recursion level reached"));
    }
    --level;
    for (exlist::const_iterator it=seq.begin(); it!=seq.end(); ++it) {
        s.push_back((*it).eval(level));
    }
    return s;
}

exlist lst::evalfchildren(int level) const
{
    exlist s;
    RESERVE(s,seq.size());

    if (level==1) {
        return seq;
    }
    if (level == -max_recursion_level) {
        throw(std::runtime_error("max recursion level reached"));
    }
    --level;
    for (exlist::const_iterator it=seq.begin(); it!=seq.end(); ++it) {
        s.push_back((*it).evalf(level));
    }
    return s;
}

exlist lst::normalchildren(int level) const
{
    exlist s;
    RESERVE(s,seq.size());

    if (level==1) {
        return seq;
    }
    if (level == -max_recursion_level) {
        throw(std::runtime_error("max recursion level reached"));
    }
    --level;
    for (exlist::const_iterator it=seq.begin(); it!=seq.end(); ++it) {
        s.push_back((*it).normal(level));
    }
    return s;
}

exlist lst::diffchildren(symbol const & y) const
{
    exlist s;
    RESERVE(s,seq.size());
    for (exlist::const_iterator it=seq.begin(); it!=seq.end(); ++it) {
        s.push_back((*it).diff(y));
    }
    return s;
}

/* obsolete subschildren
exlist lst::subschildren(lst const & ls, lst const & lr) const
{
    exlist s;
    RESERVE(s,seq.size());
    for (exlist::const_iterator it=seq.begin(); it!=seq.end(); ++it) {
        s.push_back((*it).subs(ls,lr));
    }
    return s;
}
*/

exlist * lst::subschildren(lst const & ls, lst const & lr) const
{
    // returns a NULL pointer if nothing had to be substituted
    // returns a pointer to a newly created epvector otherwise
    // (which has to be deleted somewhere else)

    exlist::const_iterator last=seq.end();
    exlist::const_iterator cit=seq.begin();
    while (cit!=last) {
        ex const & subsed_ex=(*cit).subs(ls,lr);
        if (!are_ex_trivially_equal(*cit,subsed_ex)) {

            // something changed, copy seq, subs and return it
            exlist *s=new exlist;
            RESERVE(*s,seq.size());

            // copy parts of seq which are known not to have changed
            exlist::const_iterator cit2=seq.begin();
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

unsigned lst::precedence=10;

//////////
// global constants
//////////

const lst some_lst;
type_info const & typeid_lst=typeid(some_lst);

