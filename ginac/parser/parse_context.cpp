#include "parse_context.h"
#include <sstream>
#include <stdexcept>
namespace GiNaC
{

symbol
find_or_insert_symbol(const std::string& name, symtab& syms, const bool strict)
{
	symtab::const_iterator p = syms.find(name);
	if (p != syms.end()) {
		if (is_a<symbol>(p->second))
			return ex_to<symbol>(p->second);
		else
			throw std::invalid_argument(
				std::string("find_or_insert_symbol: name \"")
				+ name + "\" does not correspond to a symbol");
	}


	if (strict)
		throw std::invalid_argument(
				std::string("find_or_insert_symbol: symbol \"") 
				+ name + "\" not found");

	const symbol sy(name);
	syms[name] = sy;
	return sy;
}

}

