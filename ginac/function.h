/** @file function.h
 *
 *  Interface to abstract class function (new function concept).
 *
 *  This file was generated automatically by function.pl.
 *  Please do not modify it directly, edit the perl script instead!
 *  function.pl options: $maxargs=10 */

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

#ifndef __GINAC_FUNCTION_H__
#define __GINAC_FUNCTION_H__

#include <string>
#include <vector>
#include <ginac/exprseq.h>

class function;

// the following lines have been generated for max. 10 parameters
#define DECLARE_FUNCTION_1P(NAME) \
extern unsigned function_index_##NAME; \
inline function NAME(ex const & p1) { \
    return function(function_index_##NAME, p1); \
}

#define DECLARE_FUNCTION_2P(NAME) \
extern unsigned function_index_##NAME; \
inline function NAME(ex const & p1, ex const & p2) { \
    return function(function_index_##NAME, p1, p2); \
}

#define DECLARE_FUNCTION_3P(NAME) \
extern unsigned function_index_##NAME; \
inline function NAME(ex const & p1, ex const & p2, ex const & p3) { \
    return function(function_index_##NAME, p1, p2, p3); \
}

#define DECLARE_FUNCTION_4P(NAME) \
extern unsigned function_index_##NAME; \
inline function NAME(ex const & p1, ex const & p2, ex const & p3, ex const & p4) { \
    return function(function_index_##NAME, p1, p2, p3, p4); \
}

#define DECLARE_FUNCTION_5P(NAME) \
extern unsigned function_index_##NAME; \
inline function NAME(ex const & p1, ex const & p2, ex const & p3, ex const & p4, ex const & p5) { \
    return function(function_index_##NAME, p1, p2, p3, p4, p5); \
}

#define DECLARE_FUNCTION_6P(NAME) \
extern unsigned function_index_##NAME; \
inline function NAME(ex const & p1, ex const & p2, ex const & p3, ex const & p4, ex const & p5, ex const & p6) { \
    return function(function_index_##NAME, p1, p2, p3, p4, p5, p6); \
}

#define DECLARE_FUNCTION_7P(NAME) \
extern unsigned function_index_##NAME; \
inline function NAME(ex const & p1, ex const & p2, ex const & p3, ex const & p4, ex const & p5, ex const & p6, ex const & p7) { \
    return function(function_index_##NAME, p1, p2, p3, p4, p5, p6, p7); \
}

#define DECLARE_FUNCTION_8P(NAME) \
extern unsigned function_index_##NAME; \
inline function NAME(ex const & p1, ex const & p2, ex const & p3, ex const & p4, ex const & p5, ex const & p6, ex const & p7, ex const & p8) { \
    return function(function_index_##NAME, p1, p2, p3, p4, p5, p6, p7, p8); \
}

#define DECLARE_FUNCTION_9P(NAME) \
extern unsigned function_index_##NAME; \
inline function NAME(ex const & p1, ex const & p2, ex const & p3, ex const & p4, ex const & p5, ex const & p6, ex const & p7, ex const & p8, ex const & p9) { \
    return function(function_index_##NAME, p1, p2, p3, p4, p5, p6, p7, p8, p9); \
}

#define DECLARE_FUNCTION_10P(NAME) \
extern unsigned function_index_##NAME; \
inline function NAME(ex const & p1, ex const & p2, ex const & p3, ex const & p4, ex const & p5, ex const & p6, ex const & p7, ex const & p8, ex const & p9, ex const & p10) { \
    return function(function_index_##NAME, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10); \
}


// end of generated lines

#define REGISTER_FUNCTION(NAME,E,EF,D,S) \
unsigned function_index_##NAME=function::register_new(#NAME,E,EF,D,S);

#define BEGIN_TYPECHECK \
bool automatic_typecheck=true;

#define TYPECHECK(VAR,TYPE) \
if (!is_ex_exactly_of_type(VAR,TYPE)) { \
    automatic_typecheck=false; \
} else

#define TYPECHECK_INTEGER(VAR) \
if (!(VAR).info(info_flags::integer)) { \
    automatic_typecheck=false; \
} else

#define END_TYPECHECK(RV) \
{} \
if (!automatic_typecheck) { \
    return RV.hold(); \
}

typedef ex (* eval_funcp)();
typedef ex (* evalf_funcp)();
typedef ex (* diff_funcp)();
typedef ex (* series_funcp)();

// the following lines have been generated for max. 10 parameters
typedef ex (* eval_funcp_1)(ex const &);
typedef ex (* eval_funcp_2)(ex const &, ex const &);
typedef ex (* eval_funcp_3)(ex const &, ex const &, ex const &);
typedef ex (* eval_funcp_4)(ex const &, ex const &, ex const &, ex const &);
typedef ex (* eval_funcp_5)(ex const &, ex const &, ex const &, ex const &, ex const &);
typedef ex (* eval_funcp_6)(ex const &, ex const &, ex const &, ex const &, ex const &, ex const &);
typedef ex (* eval_funcp_7)(ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, ex const &);
typedef ex (* eval_funcp_8)(ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, ex const &);
typedef ex (* eval_funcp_9)(ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, ex const &);
typedef ex (* eval_funcp_10)(ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, ex const &);

typedef ex (* evalf_funcp_1)(ex const &);
typedef ex (* evalf_funcp_2)(ex const &, ex const &);
typedef ex (* evalf_funcp_3)(ex const &, ex const &, ex const &);
typedef ex (* evalf_funcp_4)(ex const &, ex const &, ex const &, ex const &);
typedef ex (* evalf_funcp_5)(ex const &, ex const &, ex const &, ex const &, ex const &);
typedef ex (* evalf_funcp_6)(ex const &, ex const &, ex const &, ex const &, ex const &, ex const &);
typedef ex (* evalf_funcp_7)(ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, ex const &);
typedef ex (* evalf_funcp_8)(ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, ex const &);
typedef ex (* evalf_funcp_9)(ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, ex const &);
typedef ex (* evalf_funcp_10)(ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, ex const &);

typedef ex (* diff_funcp_1)(ex const &, unsigned);
typedef ex (* diff_funcp_2)(ex const &, ex const &, unsigned);
typedef ex (* diff_funcp_3)(ex const &, ex const &, ex const &, unsigned);
typedef ex (* diff_funcp_4)(ex const &, ex const &, ex const &, ex const &, unsigned);
typedef ex (* diff_funcp_5)(ex const &, ex const &, ex const &, ex const &, ex const &, unsigned);
typedef ex (* diff_funcp_6)(ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, unsigned);
typedef ex (* diff_funcp_7)(ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, unsigned);
typedef ex (* diff_funcp_8)(ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, unsigned);
typedef ex (* diff_funcp_9)(ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, unsigned);
typedef ex (* diff_funcp_10)(ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, unsigned);

typedef ex (* series_funcp_1)(ex const &, symbol const &, ex const &, int);
typedef ex (* series_funcp_2)(ex const &, ex const &, symbol const &, ex const &, int);
typedef ex (* series_funcp_3)(ex const &, ex const &, ex const &, symbol const &, ex const &, int);
typedef ex (* series_funcp_4)(ex const &, ex const &, ex const &, ex const &, symbol const &, ex const &, int);
typedef ex (* series_funcp_5)(ex const &, ex const &, ex const &, ex const &, ex const &, symbol const &, ex const &, int);
typedef ex (* series_funcp_6)(ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, symbol const &, ex const &, int);
typedef ex (* series_funcp_7)(ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, symbol const &, ex const &, int);
typedef ex (* series_funcp_8)(ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, symbol const &, ex const &, int);
typedef ex (* series_funcp_9)(ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, symbol const &, ex const &, int);
typedef ex (* series_funcp_10)(ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, ex const &, symbol const &, ex const &, int);

// end of generated lines

struct registered_function_info {
    char const * name;
    unsigned nparams;
    unsigned options;
    eval_funcp e;
    evalf_funcp ef;
    diff_funcp d;
    series_funcp s;
};

/** The class function is used to implement builtin functions like sin, cos...
    and user defined functions */
class function : public exprseq
{
    friend void ginsh_get_ginac_functions(void);

// member functions

    // default constructor, destructor, copy constructor assignment operator and helpers
public:
    function();
    ~function();
    function(function const & other);
    function const & operator=(function const & other);
protected:
    void copy(function const & other);
    void destroy(bool call_parent);

    // other constructors
public:
    function(unsigned ser);
    // the following lines have been generated for max. 10 parameters
    function(unsigned ser, ex const & param1);
    function(unsigned ser, ex const & param1, ex const & param2);
    function(unsigned ser, ex const & param1, ex const & param2, ex const & param3);
    function(unsigned ser, ex const & param1, ex const & param2, ex const & param3, ex const & param4);
    function(unsigned ser, ex const & param1, ex const & param2, ex const & param3, ex const & param4, ex const & param5);
    function(unsigned ser, ex const & param1, ex const & param2, ex const & param3, ex const & param4, ex const & param5, ex const & param6);
    function(unsigned ser, ex const & param1, ex const & param2, ex const & param3, ex const & param4, ex const & param5, ex const & param6, ex const & param7);
    function(unsigned ser, ex const & param1, ex const & param2, ex const & param3, ex const & param4, ex const & param5, ex const & param6, ex const & param7, ex const & param8);
    function(unsigned ser, ex const & param1, ex const & param2, ex const & param3, ex const & param4, ex const & param5, ex const & param6, ex const & param7, ex const & param8, ex const & param9);
    function(unsigned ser, ex const & param1, ex const & param2, ex const & param3, ex const & param4, ex const & param5, ex const & param6, ex const & param7, ex const & param8, ex const & param9, ex const & param10);

    // end of generated lines
    function(unsigned ser, exprseq const & es);
    function(unsigned ser, exvector const & v, bool discardable=0);
    function(unsigned ser, exvector * vp); // vp will be deleted

    // functions overriding virtual functions from bases classes
public:
    basic * duplicate() const;
    void printraw(ostream & os) const; 
    void print(ostream & os, unsigned upper_precedence=0) const;
    void printtree(ostream & os, unsigned indent) const;
    void printcsrc(ostream & os, unsigned type, unsigned upper_precedence=0) const;
    ex expand(unsigned options=0) const;
    ex eval(int level=0) const;
    ex evalf(int level=0) const;
    ex diff(symbol const & s) const;
    ex series(symbol const & s, ex const & point, int order) const;
    ex thisexprseq(exvector const & v) const;
    ex thisexprseq(exvector * vp) const;
protected:
    int compare_same_type(basic const & other) const;
    bool is_equal_same_type(basic const & other) const;
    unsigned return_type(void) const;
    unsigned return_type_tinfo(void) const;
    
    // new virtual functions which can be overridden by derived classes
    // none
    
    // non-virtual functions in this class
protected:
    ex pdiff(unsigned diff_param) const; // partial differentiation
    static vector<registered_function_info> & registered_functions(void);
public:
    // the following lines have been generated for max. 10 parameters
    static unsigned register_new(char const * nm, eval_funcp_1 e,
                                 evalf_funcp_1 ef=0, diff_funcp_1 d=0, series_funcp_1 s=0);
    static unsigned register_new(char const * nm, eval_funcp_2 e,
                                 evalf_funcp_2 ef=0, diff_funcp_2 d=0, series_funcp_2 s=0);
    static unsigned register_new(char const * nm, eval_funcp_3 e,
                                 evalf_funcp_3 ef=0, diff_funcp_3 d=0, series_funcp_3 s=0);
    static unsigned register_new(char const * nm, eval_funcp_4 e,
                                 evalf_funcp_4 ef=0, diff_funcp_4 d=0, series_funcp_4 s=0);
    static unsigned register_new(char const * nm, eval_funcp_5 e,
                                 evalf_funcp_5 ef=0, diff_funcp_5 d=0, series_funcp_5 s=0);
    static unsigned register_new(char const * nm, eval_funcp_6 e,
                                 evalf_funcp_6 ef=0, diff_funcp_6 d=0, series_funcp_6 s=0);
    static unsigned register_new(char const * nm, eval_funcp_7 e,
                                 evalf_funcp_7 ef=0, diff_funcp_7 d=0, series_funcp_7 s=0);
    static unsigned register_new(char const * nm, eval_funcp_8 e,
                                 evalf_funcp_8 ef=0, diff_funcp_8 d=0, series_funcp_8 s=0);
    static unsigned register_new(char const * nm, eval_funcp_9 e,
                                 evalf_funcp_9 ef=0, diff_funcp_9 d=0, series_funcp_9 s=0);
    static unsigned register_new(char const * nm, eval_funcp_10 e,
                                 evalf_funcp_10 ef=0, diff_funcp_10 d=0, series_funcp_10 s=0);

    // end of generated lines
    unsigned getserial(void) const {return serial;}
    
// member variables

protected:
    unsigned serial;
};

// utility macros

#define is_ex_the_function(OBJ, FUNCNAME) \
    (is_ex_exactly_of_type(OBJ, function) && static_cast<function *>(OBJ.bp)->getserial() == function_index_##FUNCNAME)

// global constants

extern const function some_function;
extern type_info const & typeid_function;

#endif // ndef __GINAC_FUNCTION_H__
