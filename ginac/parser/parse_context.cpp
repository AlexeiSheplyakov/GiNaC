#include "parse_context.hpp"
#include <sstream>
#include <stdexcept>
namespace GiNaC
{

const symbol&
find_or_insert_symbol(const std::string& name, symtab& syms, const bool strict)
{
	symtab::const_iterator p = syms.find(name);
	if (p != syms.end())
		return p->second.first;

	if (strict)
		throw std::invalid_argument(
				std::string("find_or_insert_symbol: symbol \"") 
				+ name + "\" not found");

	// false means this symbol was created by parser 
	const std::pair<symbol, bool> tmp = std::make_pair(symbol(name), false);

	symtab::iterator i = syms.insert(symtab::value_type(name, tmp)).first;
	return i->second.first;
}

}

