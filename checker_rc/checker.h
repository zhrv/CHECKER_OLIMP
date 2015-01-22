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

#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

namespace checker {

struct report_elem {
	size_t id;
	std::string result;
	size_t time;
	size_t memory;
	int score;

	report_elem() : result("bad"), time(-1), memory(-1), id(-1), score(0){}
	report_elem(std::string result_, size_t id_, size_t time_, size_t memory_, int score_) : result(result_), id(id_), time(time_), memory(memory_), score(score_) {}
};

struct verdict {
	std::string status;
	std::string error_msg;
	std::list<report_elem> report;

	verdict() : status("bad"), error_msg("unknown") {}
};

verdict check(const std::string& main_config_file_path, const std::string& profile_path, const std::string& source_name);
std::string serizlize(const verdict& v);
} // namespace checker

#endif // _CHECHER_