/** @file viewgar.cpp
 *
 *  GiNaC archive file viewer. */

/*
 *  GiNaC Copyright (C) 1999-2005 Johannes Gutenberg University Mainz, Germany
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

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <stdexcept>

#include "ginac.h"

using namespace std;
using namespace GiNaC;

int main(int argc, char **argv)
{
	if (argc < 2) {
		cerr << "Usage: " << argv[0] << " [-d] file..." << endl;
		exit(1);
	}
	--argc; ++argv;

	bool dump_mode = false;
	try {
		lst l;
		while (argc) {
			if (strcmp(*argv, "-d") == 0) {
				dump_mode = true;
				--argc; ++argv;
			}
			std::ifstream f(*argv);
			archive ar;
			f >> ar;
			if (dump_mode) {
				ar.printraw(std::cout);
				std::cout << std::endl;
			} else {
				for (unsigned int i=0; i<ar.num_expressions(); ++i) {
					std::string name;
					ex e = ar.unarchive_ex(l, name, i);
					std::cout << name << " = " << e << std::endl;
				}
			}
			--argc; ++argv;
		}
	} catch (std::exception &e) {
		std::cerr << *argv << ": " << e.what() << std::endl;
	}
}
