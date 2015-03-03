#include "checker_assert.h"

void _checker_assert(const bool expression, const char *_File, long line) {
	if (expression == false) {
//#if _DEBUG
		std::cout << "Assertion failed: false, file " << _File << ", line " << __LINE__ << std::endl;
//#endif
		exit(1);
	}
}