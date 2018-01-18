// http://github.com/jeffwalter/defer
//
// Copyright (C) 2016-2018 Jeff Walter <jeff@404ster.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

#ifndef __JWALTER_DEFER_H
#define __JWALTER_DEFER_H

#ifdef __clang__
	// Note: You may need to add -fblocks or -lBlocksRuntime to your compiler flags
	// (https://news.ycombinator.com/item?id=16176436)
	// Modified from http://fdiv.net/2015/10/08/emulating-defer-c-clang-or-gccblocks
	static inline void defer_cleanup (void (^*b) ()) { (*b) (); }
#	define defer_merge(a,b) a##b
#	define defer_varname(a) defer_merge (defer_scopevar_, a)
#	define defer __attribute__((cleanup (defer_cleanup))) __attribute__((unused)) void (^defer_varname (__COUNTER__)) () = ^

#elif __GNUC__
	// From https://news.ycombinator.com/item?id=10365956 (https://news.ycombinator.com/user?id=geocar)
#	define defer_(x) do {} while (0); \
		auto void _dtor1_##x (); \
		auto void _dtor2_##x (); \
		int __attribute__((cleanup (_dtor2_##x))) _dtorV_##x=69; \
		void _dtor2_##x () { \
			if (_dtorV_##x==42) \
				return (_dtor1_##x ()); \
		}; \
		_dtorV_##x=42; \
		void _dtor1_##x ()
#	define defer__(x) defer_(x)
#	define defer defer__(__COUNTER__)

#else
#	error Unknown compiler
#endif

#endif
