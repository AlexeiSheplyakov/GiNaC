/** @file lst.h
 *
 *  Definition of GiNaC's lst. 
 *  This file was generated automatically by container.pl.
 *  Please do not modify it directly, edit the perl script instead!
 *  container.pl options: $CONTAINER=lst
 *                        $STLHEADER=list
 *                        $reserve=0
 *                        $prepend=1
 *                        $let_op=1
 *                        $open_bracket=[
 *                        $close_bracket=] */

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

#ifndef __GINAC_LST_H__
#define __GINAC_LST_H__

#include <list>
#include <ginac/basic.h>

typedef list<ex> exlist;

class lst : public basic
{

public:
    lst();
    ~lst();
    lst(lst const & other);
    lst const & operator=(lst const & other);
protected:
    void copy(lst const & other);
    void destroy(bool call_parent);

public:
    lst(exlist const & s, bool discardable=0);
    lst(exlist * vp); // vp will be deleted
    explicit lst(ex const & e1);
    explicit lst(ex const & e1, ex const & e2);
    explicit lst(ex const & e1, ex const & e2, ex const & e3);
    explicit lst(ex const & e1, ex const & e2, ex const & e3,
             ex const & e4);
    explicit lst(ex const & e1, ex const & e2, ex const & e3,
             ex const & e4, ex const & e5);
    explicit lst(ex const & e1, ex const & e2, ex const & e3,
             ex const & e4, ex const & e5, ex const & e6);
    explicit lst(ex const & e1, ex const & e2, ex const & e3,
             ex const & e4, ex const & e5, ex const & e6,
             ex const & e7);
    explicit lst(ex const & e1, ex const & e2, ex const & e3,
             ex const & e4, ex const & e5, ex const & e6,
             ex const & e7, ex const & e8);
    explicit lst(ex const & e1, ex const & e2, ex const & e3,
             ex const & e4, ex const & e5, ex const & e6,
             ex const & e7, ex const & e8, ex const & e9);
    explicit lst(ex const & e1, ex const & e2, ex const & e3,
             ex const & e4, ex const & e5, ex const & e6,
             ex const & e7, ex const & e8, ex const & e9,
             ex const &e10);

public:
    basic * duplicate() const;
    void printraw(ostream & os) const;
    void print(ostream & os, unsigned upper_precedence=0) const;
    void printtree(ostream & os, unsigned indent) const;
    bool info(unsigned inf) const;
    int nops() const;
    ex & let_op(int const i);
    ex expand(unsigned options=0) const;
    bool has(ex const & other) const;
    ex eval(int level=0) const;
    ex evalf(int level=0) const;
    ex normal(lst &sym_lst, lst &repl_lst, int level=0) const;
    ex diff(symbol const & s) const;
    ex subs(lst const & ls, lst const & lr) const;
protected:
    int compare_same_type(basic const & other) const;
    bool is_equal_same_type(basic const & other) const;
    unsigned return_type(void) const;

    // new virtual functions which can be overridden by derived classes
public:
    virtual lst & append(ex const & b);
    virtual lst & prepend(ex const & b);

protected:
    virtual void printseq(ostream & os, char openbracket, char delim,
                          char closebracket, unsigned this_precedence,
                          unsigned upper_precedence=0) const;
    virtual ex thislst(exlist const & v) const;
    virtual ex thislst(exlist * vp) const;

protected:
    bool is_canonical() const;
    exlist evalchildren(int level) const;
    exlist evalfchildren(int level) const;
    exlist normalchildren(int level) const;
    exlist diffchildren(symbol const & s) const;
    exlist * subschildren(lst const & ls, lst const & lr) const;

protected:
    exlist seq;
    static unsigned precedence;
};

// global constants

extern const lst some_lst;
extern type_info const & typeid_lst;

// macros

#define ex_to_lst(X) (static_cast<lst const &>(*(X).bp))

#endif // ndef __GINAC_LST_H__

