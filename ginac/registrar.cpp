/** @file registrar.cpp
 *
 *  GiNaC's class registrar (for class basic and all classes derived from it). */

/*
 *  GiNaC Copyright (C) 1999-2003 Johannes Gutenberg University Mainz, Germany
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

#include <string>
#include <stdexcept>

#include "registrar.h"

namespace GiNaC {

registered_class_info *first_registered_class = NULL;

/** Find registered_class_info strucure by class name. */
static inline registered_class_info *find_registered_class_info(const std::string &class_name)
{
	registered_class_info *p = first_registered_class;
	while (p) {
		if (class_name == p->name)
			return p;
		p = p->next;
	}
	throw (std::runtime_error("class '" + class_name + "' not registered"));
}

unsigned int find_tinfo_key(const std::string &class_name)
{
	registered_class_info *p = find_registered_class_info(class_name);
	return p->tinfo_key;
}

unarch_func find_unarch_func(const std::string &class_name)
{
	registered_class_info *p = find_registered_class_info(class_name);
	return p->unarchive;
}


} // namespace GiNaC
