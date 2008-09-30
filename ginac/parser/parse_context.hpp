#ifndef _GINAC_PARSE_CONTEXT_HPP
#define _GINAC_PARSE_CONTEXT_HPP
#include <string>
#include <cstddef> // size_t
#include "ex.h"
#include "symbol.h"
#include <map>
#include <utility>

namespace GiNaC
{

/**
 * Establishes correspondence between the strings and expressions.
 * The parser will create missing symbols (if not instructed otherwise,
 * in which case it fails if the expression contains unknown symbols).
 */
typedef std::map<std::string, ex> symtab;

/**
 * Find the symbol with the @a name in the symbol table @a syms.
 *
 * If symbol is missing and @a strict = false, insert it, otherwise
 * throw an exception.
 */
extern symbol
find_or_insert_symbol(const std::string& name, symtab& syms,
	              const bool strict);

/**
 * Function (or class ctor) prototype
 * .first is  the name of function(or ctor),
 * .second is the number of arguments (each of type ex)
 */
typedef std::pair<std::string, std::size_t> prototype;

/**
 * A (C++) function for reading functions and classes from the stream.
 *
 * The parser uses (an associative array of) such functions to construct
 * (GiNaC) classes and functions from a sequence of characters.
 */
typedef ex (*reader_func)(const exvector& args);

/**
 * Prototype table.
 *
 * If parser sees an expression which looks like a function call (e.g.
 * foo(x+y, z^2, t)), it looks up such a table to find out which
 * function (or class) corresponds to the given name and has the given
 * number of the arguments.
 *
 * N.B.
 *
 * 1. The function don't have to return a (GiNaC) function or class, it
 *    can return any expression.
 * 2. Overloaded functions/ctors are paritally supported, i.e. there might
 *    be several functions with the same name, but they should take different
 *    number of arguments.
 * 3. User can extend the parser via custom prototype tables. It's possible
 *    to read user defined classes, create abbreviations, etc.
 */
typedef std::map<prototype, reader_func> prototype_table;

/**
 * Default prototype table.
 *
 * It supports most of builtin GiNaC functions.
 */
extern const prototype_table& get_default_reader();

}

#endif // _GINAC_PARSE_CONTEXT_HPP

