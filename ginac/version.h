/** @file version.h
 *
 *  GiNaC library version information. */

/*
 *  GiNaC Copyright (C) 1999-2008 Johannes Gutenberg University Mainz, Germany
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

#ifndef GINAC_VERSION_H
#define GINAC_VERSION_H

/* Major version of GiNaC */
#define GINACLIB_MAJOR_VERSION 1

/* Minor version of GiNaC */
#define GINACLIB_MINOR_VERSION 6

/* Micro version of GiNaC */
#define GINACLIB_MICRO_VERSION 2

/*
 * GiNaC archive file version information.
 *
 * The current archive version is GINACLIB_ARCHIVE_VERSION. This is
 * the version of archives created by the current version of GiNaC.
 * Archives version (GINACLIB_ARCHIVE_VERSION - GINACLIB_ARCHIVE_AGE)
 * thru * GINACLIB_ARCHIVE_VERSION can be read by current version
 * of GiNaC.
 *
 * Backward compatibility notes:
 * If new properties have been added:
 *	GINACLIB_ARCHIVE_VERSION += 1
 *	GINACLIB_ARCHIVE_AGE += 1
 * If backwards compatibility has been broken, i.e. some properties
 * has been removed, or their type and/or meaning changed:
 *	GINACLIB_ARCHIVE_VERSION += 1
 *	GINACLIB_ARCHIVE_AGE = 0
 */
#define GINACLIB_ARCHIVE_VERSION 3
#define GINACLIB_ARCHIVE_AGE 3

namespace GiNaC {

extern const int version_major;
extern const int version_minor;
extern const int version_micro;

} // namespace GiNaC

#endif // ndef GINAC_VERSION_H
