/*****************************************************************************
 *
 * This file is part of ENGERLIB (c++ mapping toolkit)
 *
 * Copyright (C) 2011 Artem Pavlenko
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *****************************************************************************/

#ifndef ENGER_LIB_CONFIG_HPP
#define ENGER_LIB_CONFIG_HPP

// Windows DLL support

#ifdef _WINDOWS
#  define ENGERLIB_EXP __declspec (dllexport)
#  define ENGERLIB_IMP __declspec (dllimport)
#  ifdef ENGERLIB_EXPORTS
#    define ENGERLIB_DECL __declspec (dllexport)
#  else
#    define ENGERLIB_DECL __declspec (dllimport)
#  endif
#  pragma warning( disable: 4251 )
#  pragma warning( disable: 4275 )
#  if (_MSC_VER >= 1400) // vc8
#    pragma warning(disable : 4996) //_CRT_SECURE_NO_DEPRECATE
#  endif
#else
#  if __GNUC__ >= 4
#  define ENGERLIB_EXP __attribute__ ((visibility ("default")))
#  define ENGERLIB_DECL __attribute__ ((visibility ("default")))
#  define ENGERLIB_IMP __attribute__ ((visibility ("default")))
#  else
#  define ENGERLIB_EXP
#  define ENGERLIB_DECL
#  define ENGERLIB_IMP
#  endif
#endif

#endif // ENGER_LIB_CONFIG_HPP