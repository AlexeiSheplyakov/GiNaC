/** @file symbol.h
 *
 *  Interface to GiNaC's symbolic objects. */

#ifndef _SYMBOL_H_
#define _SYMBOL_H_

#include <string>

class symbol;

#include "basic.h"
#include "ex.h"

/** Basic CAS symbol.  It has a name because it must know how to output itself.
 *  It may be assigned an expression, but this feature is only intended for
 *  programs like 'ginsh' that want to associate symbols with expressions.
 *  If you want to replace symbols by expressions in your code, you should
 *  use ex::subs() or use objects of class ex instead of class symbol in the
 *  first place. */
class symbol : public basic
{
// types
    /** Symbols as keys to expressions. */
    class assigned_ex_info {
    public:
        assigned_ex_info();     //!< Default ctor
        bool is_assigned;       //!< True if there is an expression assigned
        ex assigned_expression; //!< The actual expression
        unsigned refcount;      //!< Yet another refcounter. PLEASE EXPLAIN!
    };
    
// member functions

    // default constructor, destructor, copy constructor assignment operator and helpers
public:
    symbol();
    ~symbol();
    symbol(symbol const & other);
protected:
    void copy(symbol const & other); 
    void destroy(bool call_parent);

    // other constructors
public:
    explicit symbol(string const & initname);

    // functions overriding virtual functions from base classes
public:
    basic * duplicate() const;
    void printraw(ostream & os) const;
    void printtree(ostream & os, unsigned indent) const;
    void print(ostream & os, unsigned upper_precedence=0) const;
    void printcsrc(ostream & os, unsigned type, unsigned upper_precedence=0) const;
    bool info(unsigned inf) const;
    ex expand(unsigned options=0) const;
    bool has(ex const & other) const;
    int degree(symbol const & s) const;
    int ldegree(symbol const & s) const;
    ex coeff(symbol const & s, int const n = 1) const;
    ex eval(int level = 0) const;
    ex diff(symbol const & s) const;
    ex normal(lst &sym_lst, lst &repl_lst, int level=0) const;
    ex subs(lst const & ls, lst const & lr) const;
protected:
    int compare_same_type(basic const & other) const;
    bool is_equal_same_type(basic const & other) const;
    unsigned return_type(void) const;
    unsigned return_type_tinfo(void) const;
    unsigned calchash(void) const;
    
    // non-virtual functions in this class
public:
    void assign(ex const & value);
    void unassign(void);
    ex diff(symbol const & s, unsigned nth) const;
    void setname(string const & n) {name=n;}
    string getname(void) const {return name;}
private:
    string & autoname_prefix(void);

// member variables

protected:
    assigned_ex_info * asexinfop;
    unsigned serial;  //!< unique serial number for comparision
    string name;
private:
    static unsigned next_serial;
};

// global constants

extern const symbol some_symbol;
extern type_info const & typeid_symbol;

// macros

#define ex_to_symbol(X) static_cast<symbol const &>(*(X).bp)

// wrapper functions around member functions
inline void unassign(symbol & symarg)
{ return symarg.unassign(); }

inline int degree(symbol const & a, symbol const & s)
{ return a.degree(s); }

inline int ldegree(symbol const & a, symbol const & s)
{ return a.ldegree(s); }

#endif // ndef _SYMBOL_H_
