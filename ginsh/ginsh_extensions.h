/** @file ginsh_extensions.h
 *
 *  The contents of this file are included in the ginsh parser. This makes
 *  it possible to create customized versions of ginsh that add special
 *  functions. */

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

// Table of names and descriptors of functions to be added
static const fcn_init extended_fcns[] = {
	{NULL, f_dummy, 0} // End marker
};

// Table of help strings for functions
static const fcn_help_init extended_help[] = {
	{NULL, NULL} // End marker
};
