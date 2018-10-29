/*
 * LIBFSAPFS_LZVN (un)compression functions
 *
 * Copyright (C) 2018, Joachim Metz <joachim.metz@gmail.com>
 *
 * Refer to AUTHORS for acknowledgements.
 *
 * This software is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this software.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <common.h>
#include <byte_stream.h>
#include <memory.h>
#include <types.h>

#include "libfsapfs_lzvn.h"
#include "libfsapfs_libcerror.h"

enum LIBFSAPFS_LZVN_OPPCODE_TYPES
{
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_LARGE,
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_MEDIUM,
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_PREVIOUS,
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,
	LIBFSAPFS_LZVN_OPPCODE_TYPE_END_OF_STREAM,
	LIBFSAPFS_LZVN_OPPCODE_TYPE_INVALID,
	LIBFSAPFS_LZVN_OPPCODE_TYPE_LITERAL_LARGE,
	LIBFSAPFS_LZVN_OPPCODE_TYPE_LITERAL_SMALL,
	LIBFSAPFS_LZVN_OPPCODE_TYPE_MATCH_LARGE,
	LIBFSAPFS_LZVN_OPPCODE_TYPE_MATCH_SMALL,
	LIBFSAPFS_LZVN_OPPCODE_TYPE_NONE,
};

/* Lookup table to map an oppcode to its type
 */
uint8_t lzvn_oppcode_types[ 256 ] = {
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x00 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x01 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x02 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x03 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x04 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x05 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_END_OF_STREAM,	/* 0x06 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_LARGE,	/* 0x07 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x08 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x09 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x0a */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x0b */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x0c */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x0d */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_NONE,		/* 0x0e */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_LARGE,	/* 0x0f */

	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x10 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x11 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x12 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x13 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x14 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x15 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_NONE,		/* 0x16 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_LARGE,	/* 0x17 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x18 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x19 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x1a */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x1b */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x1c */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x1d */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_INVALID,		/* 0x1e */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_LARGE,	/* 0x1f */

	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x20 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x21 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x22 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x23 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x24 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x25 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_INVALID,		/* 0x26 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_LARGE,	/* 0x27 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x28 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x29 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x2a */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x2b */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x2c */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x2d */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_INVALID,		/* 0x2e */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_LARGE,	/* 0x2f */

	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x30 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x31 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x32 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x33 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x34 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x35 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_INVALID,		/* 0x36 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_LARGE,	/* 0x37 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x38 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x39 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x3a */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x3b */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x3c */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x3d */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_INVALID,		/* 0x3e */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_LARGE,	/* 0x3f */

	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x40 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x41 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x42 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x43 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x44 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x45 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_PREVIOUS,	/* 0x46 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_LARGE,	/* 0x47 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x48 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x49 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x4a */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x4b */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x4c */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x4d */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_PREVIOUS,	/* 0x4e */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_LARGE,	/* 0x4f */

	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x50 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x51 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x52 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x53 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x54 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x55 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_PREVIOUS,	/* 0x56 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_LARGE,	/* 0x57 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x58 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x59 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x5a */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x5b */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x5c */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x5d */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_PREVIOUS,	/* 0x5e */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_LARGE,	/* 0x5f */

	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x60 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x61 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x62 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x63 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x64 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x65 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_PREVIOUS,	/* 0x66 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_LARGE,	/* 0x67 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x68 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x69 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x6a */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x6b */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x6c */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x6d */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_PREVIOUS,	/* 0x6e */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_LARGE,	/* 0x6f */

	LIBFSAPFS_LZVN_OPPCODE_TYPE_INVALID,		/* 0x70 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_INVALID,		/* 0x71 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_INVALID,		/* 0x72 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_INVALID,		/* 0x73 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_INVALID,		/* 0x74 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_INVALID,		/* 0x75 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_INVALID,		/* 0x76 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_INVALID,		/* 0x77 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_INVALID,		/* 0x78 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_INVALID,		/* 0x79 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_INVALID,		/* 0x7a */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_INVALID,		/* 0x7b */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_INVALID,		/* 0x7c */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_INVALID,		/* 0x7d */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_INVALID,		/* 0x7e */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_INVALID,		/* 0x7f */

	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x80 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x81 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x82 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x83 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x84 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x85 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_PREVIOUS,	/* 0x86 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_LARGE,	/* 0x87 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x88 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x89 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x8a */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x8b */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x8c */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x8d */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_PREVIOUS,	/* 0x8e */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_LARGE,	/* 0x8f */

	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x90 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x91 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x92 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x93 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x94 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x95 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_PREVIOUS,	/* 0x96 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_LARGE,	/* 0x97 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x98 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x99 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x9a */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x9b */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x9c */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0x9d */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_PREVIOUS,	/* 0x9e */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_LARGE,	/* 0x9f */

	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_MEDIUM,	/* 0xa0 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_MEDIUM,	/* 0xa1 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_MEDIUM,	/* 0xa2 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_MEDIUM,	/* 0xa3 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_MEDIUM,	/* 0xa4 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_MEDIUM,	/* 0xa5 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_MEDIUM,	/* 0xa6 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_MEDIUM,	/* 0xa7 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_MEDIUM,	/* 0xa8 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_MEDIUM,	/* 0xa9 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_MEDIUM,	/* 0xaa */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_MEDIUM,	/* 0xab */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_MEDIUM,	/* 0xac */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_MEDIUM,	/* 0xad */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_MEDIUM,	/* 0xae */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_MEDIUM,	/* 0xaf */

	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_MEDIUM,	/* 0xb0 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_MEDIUM,	/* 0xb1 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_MEDIUM,	/* 0xb2 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_MEDIUM,	/* 0xb3 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_MEDIUM,	/* 0xb4 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_MEDIUM,	/* 0xb5 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_MEDIUM,	/* 0xb6 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_MEDIUM,	/* 0xb7 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_MEDIUM,	/* 0xb8 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_MEDIUM,	/* 0xb9 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_MEDIUM,	/* 0xba */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_MEDIUM,	/* 0xbb */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_MEDIUM,	/* 0xbc */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_MEDIUM,	/* 0xbd */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_MEDIUM,	/* 0xbe */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_MEDIUM,	/* 0xbf */

	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0xc0 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0xc1 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0xc2 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0xc3 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0xc4 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0xc5 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_PREVIOUS,	/* 0xc6 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_LARGE,	/* 0xc7 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0xc8 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0xc9 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0xca */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0xcb */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0xcc */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL,	/* 0xcd */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_PREVIOUS,	/* 0xce */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_LARGE,	/* 0xcf */

	LIBFSAPFS_LZVN_OPPCODE_TYPE_INVALID,		/* 0xd0 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_INVALID,		/* 0xd1 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_INVALID,		/* 0xd2 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_INVALID,		/* 0xd3 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_INVALID,		/* 0xd4 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_INVALID,		/* 0xd5 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_INVALID,		/* 0xd6 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_INVALID,		/* 0xd7 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_INVALID,		/* 0xd8 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_INVALID,		/* 0xd9 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_INVALID,		/* 0xda */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_INVALID,		/* 0xdb */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_INVALID,		/* 0xdc */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_INVALID,		/* 0xdd */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_INVALID,		/* 0xde */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_INVALID,		/* 0xdf */

	LIBFSAPFS_LZVN_OPPCODE_TYPE_LITERAL_LARGE,	/* 0xe0 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_LITERAL_SMALL,	/* 0xe1 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_LITERAL_SMALL,	/* 0xe2 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_LITERAL_SMALL,	/* 0xe3 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_LITERAL_SMALL,	/* 0xe4 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_LITERAL_SMALL,	/* 0xe5 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_LITERAL_SMALL,	/* 0xe6 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_LITERAL_SMALL,	/* 0xe7 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_LITERAL_SMALL,	/* 0xe8 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_LITERAL_SMALL,	/* 0xe9 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_LITERAL_SMALL,	/* 0xea */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_LITERAL_SMALL,	/* 0xeb */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_LITERAL_SMALL,	/* 0xec */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_LITERAL_SMALL,	/* 0xed */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_LITERAL_SMALL,	/* 0xee */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_LITERAL_SMALL,	/* 0xef */

	LIBFSAPFS_LZVN_OPPCODE_TYPE_MATCH_LARGE,	/* 0xf0 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_MATCH_SMALL,	/* 0xf1 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_MATCH_SMALL,	/* 0xf2 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_MATCH_SMALL,	/* 0xf3 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_MATCH_SMALL,	/* 0xf4 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_MATCH_SMALL,	/* 0xf5 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_MATCH_SMALL,	/* 0xf6 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_MATCH_SMALL,	/* 0xf7 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_MATCH_SMALL,	/* 0xf8 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_MATCH_SMALL,	/* 0xf9 */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_MATCH_SMALL,	/* 0xfa */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_MATCH_SMALL,	/* 0xfb */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_MATCH_SMALL,	/* 0xfc */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_MATCH_SMALL,	/* 0xfd */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_MATCH_SMALL,	/* 0xfe */
	LIBFSAPFS_LZVN_OPPCODE_TYPE_MATCH_SMALL,	/* 0xff */
};

/* Decompresses LIBFSAPFS_LZVN compressed data
 * Returns 1 on success or -1 on error
 */
int libfsapfs_lzvn_decompress(
     const uint8_t *compressed_data,
     size_t compressed_data_size,
     uint8_t *uncompressed_data,
     size_t *uncompressed_data_size,
     libcerror_error_t **error )
{
	static char *function           = "libfsapfs_lzvn_decompress";
	size_t compressed_data_offset   = 0;
	size_t match_offset             = 0;
	size_t uncompressed_data_offset = 0;
	uint16_t distance               = 0;
	uint16_t literal_size           = 0;
	uint16_t match_size             = 0;
	uint8_t oppcode                 = 0;
	uint8_t oppcode_type            = 0;
	uint8_t oppcode_value           = 0;

	if( compressed_data == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid compressed data.",
		 function );

		return( -1 );
	}
	if( compressed_data_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid compressed data size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( uncompressed_data == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid uncompressed data.",
		 function );

		return( -1 );
	}
	if( uncompressed_data_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid uncompressed data size.",
		 function );

		return( -1 );
	}
	if( *uncompressed_data_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid uncompressed data size value exceeds maximum.",
		 function );

		return( -1 );
	}
	while( compressed_data_offset < compressed_data_size )
	{
		if( uncompressed_data_offset >= *uncompressed_data_size )
		{
			break;
		}
		if( compressed_data_offset >= compressed_data_size )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
			 "%s: compressed data size value too small.",
			 function );

			return( -1 );
		}
		oppcode = compressed_data[ compressed_data_offset++ ];

		oppcode_type = lzvn_oppcode_types[ oppcode ];

		literal_size = 0;
		match_size   = 0;

		switch( oppcode_type )
		{
			case LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_LARGE:
				if( ( compressed_data_offset + 1 ) >= compressed_data_size )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
					 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
					 "%s: compressed data size value too small.",
					 function );

					return( -1 );
				}
				oppcode_value = compressed_data[ compressed_data_offset++ ];

				literal_size = ( oppcode & 0xc0 ) >> 6;
				match_size   = ( ( oppcode & 0x38 ) >> 3 ) + 3;
				distance     = ( (uint16_t) compressed_data[ compressed_data_offset++ ] << 8 ) | oppcode_value;

				break;

			case LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_MEDIUM:
				if( ( compressed_data_offset + 1 ) >= compressed_data_size )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
					 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
					 "%s: compressed data size value too small.",
					 function );

					return( -1 );
				}
				oppcode_value = compressed_data[ compressed_data_offset++ ];

				literal_size = ( oppcode & 0x18 ) >> 3;
				match_size   = ( ( ( oppcode & 0x07 ) << 2 ) | ( oppcode_value & 0x03 ) ) + 3;
				distance     = ( (uint16_t) compressed_data[ compressed_data_offset++ ] << 6 ) | ( ( oppcode_value & 0xfc ) >> 2 );

				break;

			case LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_PREVIOUS:
				literal_size = ( oppcode & 0xc0 ) >> 6;
				match_size   = ( ( oppcode & 0x38 ) >> 3 ) + 3;

				break;

			case LIBFSAPFS_LZVN_OPPCODE_TYPE_DISTANCE_SMALL:
				if( compressed_data_offset >= compressed_data_size )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
					 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
					 "%s: compressed data size value too small.",
					 function );

					return( -1 );
				}
				literal_size = ( oppcode & 0xc0 ) >> 6;
				match_size   = ( ( oppcode & 0x38 ) >> 3 ) + 3;
				distance     = ( (uint16_t) ( oppcode & 0x07 ) << 8 ) | compressed_data[ compressed_data_offset++ ];

				break;

			case LIBFSAPFS_LZVN_OPPCODE_TYPE_LITERAL_LARGE:
				if( compressed_data_offset >= compressed_data_size )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
					 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
					 "%s: compressed data size value too small.",
					 function );

					return( -1 );
				}
				literal_size = (uint16_t) compressed_data[ compressed_data_offset++ ] + 16;

				break;

			case LIBFSAPFS_LZVN_OPPCODE_TYPE_LITERAL_SMALL:
				literal_size = oppcode & 0x0f;

				break;

			case LIBFSAPFS_LZVN_OPPCODE_TYPE_MATCH_LARGE:
				if( compressed_data_offset >= compressed_data_size )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
					 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
					 "%s: compressed data size value too small.",
					 function );

					return( -1 );
				}
				match_size = (uint16_t) compressed_data[ compressed_data_offset++ ] + 16;

				break;

			case LIBFSAPFS_LZVN_OPPCODE_TYPE_MATCH_SMALL:
				match_size = oppcode & 0x0f;

				break;

			case LIBFSAPFS_LZVN_OPPCODE_TYPE_END_OF_STREAM:
			case LIBFSAPFS_LZVN_OPPCODE_TYPE_NONE:
				break;

			case LIBFSAPFS_LZVN_OPPCODE_TYPE_INVALID:
			default:
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
				 "%s: invalid oppcode: 0x%02" PRIx8 ".",
				 function,
				 oppcode );

				return( -1 );
		}
		if( oppcode_type == LIBFSAPFS_LZVN_OPPCODE_TYPE_END_OF_STREAM )
		{
			break;
		}
		if( literal_size > 0 )
		{
			if( ( (size_t) literal_size > compressed_data_size )
			 || ( compressed_data_offset >= ( compressed_data_size - literal_size ) ) )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: literal size value exceeds compressed data size.",
				 function );

				return( -1 );
			}
			if( ( (size_t) literal_size > *uncompressed_data_size )
			 || ( uncompressed_data_offset >= ( *uncompressed_data_size - literal_size ) ) )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: literal size value exceeds uncompressed data size.",
				 function );

				return( -1 );
			}
			if( memory_copy(
			     &( uncompressed_data[ uncompressed_data_offset ] ),
			     &( compressed_data[ compressed_data_offset ] ),
			     (size_t) literal_size ) == NULL )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_MEMORY,
				 LIBCERROR_MEMORY_ERROR_COPY_FAILED,
				 "%s: unable to copy literal to uncompressed data.",
				 function );

				return( -1 );
			}
			compressed_data_offset   += (size_t) literal_size;
			uncompressed_data_offset += (size_t) literal_size;
		}
		if( match_size > 0 )
		{
			if( (size_t) distance > uncompressed_data_offset )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: distance value exceeds uncompressed data offset.",
				 function );

				return( -1 );
			}
			match_offset = uncompressed_data_offset - distance;

			if( ( (size_t) match_size > *uncompressed_data_size )
			 || ( uncompressed_data_offset >= ( *uncompressed_data_size - match_size ) ) )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: match size value exceeds uncompressed data size.",
				 function );

				return( -1 );
			}
			if( memory_copy(
			     &( uncompressed_data[ uncompressed_data_offset ] ),
			     &( uncompressed_data[ match_offset ] ),
			     (size_t) match_size ) == NULL )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_MEMORY,
				 LIBCERROR_MEMORY_ERROR_COPY_FAILED,
				 "%s: unable to copy match to uncompressed data.",
				 function );

				return( -1 );
			}
			uncompressed_data_offset += (size_t) match_size;
		}
	}
	*uncompressed_data_size = uncompressed_data_offset;

	return( 1 );
}

