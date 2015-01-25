#ifndef _CHECKER_ASSERT_
#define _CHECKER_ASSERT_

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>

void _checker_assert(const bool expression, const char *_File, long line);

#define checker_assert(_Expression)	(void)( (!!(_Expression)) || (_checker_assert(!!(_Expression), __FILE__, __LINE__), 0) )

#endif