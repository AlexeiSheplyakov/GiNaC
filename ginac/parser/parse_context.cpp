/** @file parse_context.cpp
 *
 *  Implementation of the parser context. */

/*
 *  GiNaC Copyright (C) 1999-2010 Johannes Gutenberg University Mainz, Germany
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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "parse_context.h"

#include "function.h"

#include <sstream>
#include <stdexcept>

namespace GiNaC {

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

const prototype_table& get_default_reader(bool force_init)
{
	using std::make_pair;
	static bool initialized = false;
	static prototype_table reader;
	if ( !initialized || force_init ) {
		std::vector<function_options> flist = function::get_registered_functions();
		std::vector<function_options>::iterator i = flist.begin(), end = flist.end();
		for ( ; i != end; ++i ) {
			std::string name = i->get_name();
			unsigned narg = i->get_nparams();
			reader[make_pair(name, narg)] = function::find_function(name, narg);
		}
		initialized = true;
	}
	return reader;
}

} // namespace GiNaC
