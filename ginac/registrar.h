/** @file registrar.h
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

#ifndef __GINAC_REGISTRAR_H__
#define __GINAC_REGISTRAR_H__

#include <string>
#include <list>

#include "class_info.h"

namespace GiNaC {

class ex;
class archive_node;

template <template <class> class> class container;
typedef container<std::list> lst;


/** Unarchiving function (static member function of every GiNaC class). */
typedef ex (*unarch_func)(const archive_node &n, lst &sym_lst);


/** This class stores information about a registered GiNaC class. */
class registered_class_options {
public:
	registered_class_options(const char *n, const char *p, unsigned ti, unarch_func f)
	 : name(n), parent_name(p), tinfo_key(ti), unarchive(f) {}

	const char *get_name() const { return name; }
	const char *get_parent_name() const { return parent_name; }
	unsigned get_id() const { return tinfo_key; }
	unarch_func get_unarch_func() const { return unarchive; }

private:
	const char *name;         /**< Class name. */
	const char *parent_name;  /**< Name of superclass. */
	unsigned tinfo_key;       /**< TINFO_* key. */
	unarch_func unarchive;    /**< Pointer to unarchiving function. */
};

typedef class_info<registered_class_options> registered_class_info;


/** Primary macro for inclusion in the declaration of each registered class. */
#define GINAC_DECLARE_REGISTERED_CLASS_NO_CTORS(classname, supername) \
public: \
	typedef supername inherited; \
	template <class isexaclass> friend bool is_exactly_a(const GiNaC::basic &obj); \
private: \
	static GiNaC::registered_class_info reg_info; \
public: \
	virtual const GiNaC::registered_class_info &get_class_info() const { return reg_info; } \
	virtual const char *class_name() const { return reg_info.options.get_name(); } \
	\
	classname(const GiNaC::archive_node &n, GiNaC::lst &sym_lst); \
	virtual void archive(GiNaC::archive_node &n) const; \
	static GiNaC::ex unarchive(const GiNaC::archive_node &n, GiNaC::lst &sym_lst); \
	\
	class visitor { \
	public: \
		virtual void visit(const classname &) = 0; \
	};

/** Macro for inclusion in the declaration of each registered class.
 *  It declares some functions that are common to all classes derived
 *  from 'basic' as well as all required stuff for the GiNaC class
 *  registry (mainly needed for archiving). */
#define GINAC_DECLARE_REGISTERED_CLASS(classname, supername) \
	GINAC_DECLARE_REGISTERED_CLASS_NO_CTORS(classname, supername) \
public: \
	classname(); \
	classname * duplicate() const { return new classname(*this); } \
	\
	void accept(GiNaC::visitor & v) const \
	{ \
		if (visitor *p = dynamic_cast<visitor *>(&v)) \
			p->visit(*this); \
		else \
			inherited::accept(v); \
	} \
protected: \
	int compare_same_type(const GiNaC::basic & other) const; \
private:

/** Macro for inclusion in the implementation of each registered class. */
#define GINAC_IMPLEMENT_REGISTERED_CLASS(classname, supername) \
	GiNaC::registered_class_info classname::reg_info = GiNaC::registered_class_info(GiNaC::registered_class_options(#classname, #supername, TINFO_##classname, &classname::unarchive));

/** Macro for inclusion in the implementation of each registered class.
 *  Additional options can be specified. */
#define GINAC_IMPLEMENT_REGISTERED_CLASS_OPT(classname, supername, options) \
	GiNaC::registered_class_info classname::reg_info = GiNaC::registered_class_info(GiNaC::registered_class_options(#classname, #supername, TINFO_##classname, &classname::unarchive).options);


/** Find TINFO_* key by class name. */
extern unsigned find_tinfo_key(const std::string &class_name);

/** Find unarchiving function by class name. */
extern unarch_func find_unarch_func(const std::string &class_name);


} // namespace GiNaC

#endif // ndef __GINAC_REGISTRAR_H__
