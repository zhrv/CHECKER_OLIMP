#ifndef _CHECHER_
#define _CHECHER_

#include <string>
#include <list>
#include <vector>
#include <stack>
#include <cassert>

#include <functional>

#include "reader.h"
#include "builder.h"
#include "executor.h"
#include "matcher.h"

namespace checker {

struct verdict {
	bool			test_passed;
	std::string		report;
};

void check(const std::string& main_config_file_path, const std::string& problem_name, 
		   const std::string& compiler_name, const std::string& profile_path,
		   const std::string& source_name, std::string& report_str);

} // namespace checker

#endif // _CHECHER_