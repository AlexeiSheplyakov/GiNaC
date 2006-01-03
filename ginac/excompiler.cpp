/** @file excompiler.cpp
 *
 *  Class to facilitate the conversion of a ex to a function pointer suited for
 *  fast numerical integration. */

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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "excompiler.h"

#include <dlfcn.h>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "config.h"
#include "ex.h"
#include "lst.h"
#include "operators.h"
#include "relational.h"
#include "symbol.h"

namespace GiNaC {

#ifdef HAVE_LIBDL
	
class excompiler
{
	struct filedesc
	{
		void* module;
		std::string name;
	};
public:
	excompiler() {};
	~excompiler()
	{
		for (std::vector<filedesc>::const_iterator it = filelist.begin(); it != filelist.end(); ++it) {
			dlclose(it->module);
			std::string strremove = "rm " + it->name;
			system(strremove.c_str());
		}
	}
	void add(void* module, std::string name)
	{
		filedesc fd;
		fd.module = module;
		fd.name = name;
		filelist.push_back(fd);
	}
	std::vector<filedesc> filelist;
};

excompiler _exc;

FP_dim1 compile(const ex& expr, const symbol& sym)
{
	symbol argx("argx");
	ex expr_with_x = expr.subs(lst(sym==argx));

	char filename[] = "/tmp/GiNaCXXXXXX";

	int fno = mkstemp(filename);

	std::ofstream ofs(filename);

	ofs << "#include <stddef.h> " << std::endl;
	ofs << "#include <stdlib.h> " << std::endl;
	ofs << "#include <math.h> " << std::endl;
	ofs << std::endl;
	ofs << "double compiled_ex(double argx)" << std::endl;
	ofs << "{" << std::endl;
	ofs << "double res = ";
	expr_with_x.print(GiNaC::print_csrc_double(ofs));
	ofs << ";" << std::endl;
	ofs << "return(res); " << std::endl;
	ofs << "}" << std::endl;

	ofs.close();

	std::string strcompile = "ginac-excompiler " + std::string(filename);
	system(strcompile.c_str());

	std::string strremove = "rm " + std::string(filename) + " " + std::string(filename) + ".o";
	system(strremove.c_str());

	std::string strsofile = std::string(filename) + ".so";
	void* module = NULL;
	module = dlopen(strsofile.c_str(), RTLD_NOW);
	if (module == NULL)	{
		throw std::runtime_error("excompiler: could not open compiled module!");
	}

	_exc.add(module, strsofile);

	return (FP_dim1) dlsym(module, "compiled_ex");
}

FP_cuba compile(const lst& exprs, const lst& syms)
{
	lst replacements;
	for (int count=0; count<syms.nops(); ++count) {
		std::ostringstream s;
		s << "a[" << count << "]";
		replacements.append(syms.op(count) == symbol(s.str()));
	}

	std::vector<ex> expr_with_cname;
	for (int count=0; count<exprs.nops(); ++count) {
		expr_with_cname.push_back(exprs.op(count).subs(replacements));
	}

	char filename[] = "/tmp/GiNaCXXXXXX";

	int fno = mkstemp(filename);

	std::ofstream ofs(filename);

	ofs << "#include <stddef.h> " << std::endl;
	ofs << "#include <stdlib.h> " << std::endl;
	ofs << "#include <math.h> " << std::endl;
	ofs << std::endl;
	ofs << "void compiled_ex(const int* an, const double a[], const int* fn, double f[])" << std::endl;
	ofs << "{" << std::endl;
	for (int count=0; count<exprs.nops(); ++count) {
		ofs << "f[" << count << "] = ";
		expr_with_cname[count].print(GiNaC::print_csrc_double(ofs));
		ofs << ";" << std::endl;
	}
	ofs << "}" << std::endl;

	ofs.close();

	std::string strcompile = "ginac-excompiler " + std::string(filename);
	system(strcompile.c_str());

	std::string strremove = "rm " + std::string(filename) + " " + std::string(filename) + ".o";
	system(strremove.c_str());

	std::string strsofile = std::string(filename) + ".so";
	void* module = NULL;
	module = dlopen(strsofile.c_str(), RTLD_NOW);
	if (module == NULL)	{
		throw std::runtime_error("excompiler: could not open compiled module!");
	}

	_exc.add(module, strsofile);

	return (FP_cuba) dlsym(module, "compiled_ex");
}

#elif

FP_dim1 compile(const ex& expr, const symbol& sym)
{
	throw std::runtime_error("compile has been disabled because of missing libdl!");
	return NULL;
}

FP_cuba compile(const lst& exprs, const lst& syms)
{
	throw std::runtime_error("compile has been disabled because of missing libdl!");
	return NULL;
}

#endif

} // namespace GiNaC
