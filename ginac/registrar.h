/** @file registrar.h
 *
 *  GiNaC's class registrar (for class basic and all classes derived from it). */

/*
 *  GiNaC Copyright (C) 1999-2002 Johannes Gutenberg University Mainz, Germany
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

namespace GiNaC {

class registered_class_info;
class ex;
class archive_node;
class lst;


/** Unarchiving function (static member function of every GiNaC class). */
typedef ex (*unarch_func)(const archive_node &n, const lst &sym_lst);


/** Head of list of all registered_class_info structures. */
extern registered_class_info *first_registered_class;


/** This structure stores information about a registered GiNaC class. */
struct registered_class_info {
	registered_class_info(const char *n, const char *s, unsigned int k, unarch_func f)
		: name(n), super(s), tinfo_key(k), unarchive(f)
	{
		// Add structure to list
		next = first_registered_class;
		first_registered_class = this;
	}

	registered_class_info *next;  /**< Pointer to next registered_class_info in list. */
	const char *name;             /**< Class name. */
	const char *super;            /**< Name of superclass. */
	unsigned int tinfo_key;       /**< TINFO_* key. */
	unarch_func unarchive;        /**< Pointer to unarchiving function. */
};


/** Primary macro for inclusion in the declaration of each registered class. */
#define GINAC_DECLARE_REGISTERED_CLASS_NO_CTORS(classname, supername) \
public: \
	typedef supername inherited; \
	static registered_class_info reg_info; \
	virtual const char *class_name(void) const; \
	classname(const archive_node &n, const lst &sym_lst); \
	virtual void archive(archive_node &n) const; \
	static ex unarchive(const archive_node &n, const lst &sym_lst);

/** Macro for inclusion in the declaration of each registered class.
 *  It declares some functions that are common to all classes derived
 *  from 'basic' as well as all required stuff for the GiNaC class
 *  registry (mainly needed for archiving). */
#define GINAC_DECLARE_REGISTERED_CLASS(classname, supername) \
	GINAC_DECLARE_REGISTERED_CLASS_NO_CTORS(classname, supername) \
public: \
	classname(); \
	~classname() { destroy(false); } \
	classname(const classname & other); \
	const classname & operator=(const classname & other); \
	basic * duplicate() const; \
protected: \
	void copy(const classname & other); \
	void destroy(bool call_parent); \
	int compare_same_type(const basic & other) const; \
private:

/** Primary macro for inclusion in the implementation of each registered class. */
#define GINAC_IMPLEMENT_REGISTERED_CLASS_NO_CTORS(classname, supername) \
	registered_class_info classname::reg_info(#classname, #supername, TINFO_##classname, &classname::unarchive); \
	const char *classname::class_name(void) const {return reg_info.name;}

/** Macro for inclusion in the implementation of each registered class.
 *  It implements some functions that are the same in all classes derived
 *  from 'basic' (such as the assignment operator). */
#define GINAC_IMPLEMENT_REGISTERED_CLASS(classname, supername) \
	GINAC_IMPLEMENT_REGISTERED_CLASS_NO_CTORS(classname, supername) \
classname::classname(const classname & other) { copy(other); } \
const classname & classname::operator=(const classname & other) \
{ \
	if (this != &other) { \
		destroy(true); \
		copy(other); \
	} \
	return *this; \
} \
basic * classname::duplicate() const { \
	return new classname(*this); \
}


/** Find TINFO_* key by class name. */
extern unsigned int find_tinfo_key(const std::string &class_name);

/** Find unarchiving function by class name. */
extern unarch_func find_unarch_func(const std::string &class_name);


} // namespace GiNaC

#endif // ndef __GINAC_REGISTRAR_H__
