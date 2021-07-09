/*
 * Copyright © 2018 Christian Persch
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef __POPPLER_MACROS_H__
#define __POPPLER_MACROS_H__

/**
 * POPPLER_PUBLIC:
 */
#if defined(_WIN32)
#    define POPPLER_PUBLIC __declspec(dllexport)
#elif defined(__GNUC__)
#    define POPPLER_PUBLIC __attribute__((visibility("default"))) extern
#else
#    define POPPLER_PUBLIC
#endif

#endif /* __POPPLER_MACROS_H__ */
