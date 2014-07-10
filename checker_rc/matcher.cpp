#include "matcher.h"

namespace checker {

//todo: optimize.
bool strings_match(std::string str1, std::string str2, enum checking_map check_mode)
{
	switch (check_mode) {
	case checking_map::ALL:
		if (str2 == str1) {
			return true;
		}
		break;
	case checking_map::WNL:
		if (!str1.empty()) {
			if (str1[str1.length()-1] == '\n') {
				str1.resize(str1.length()-1);
			}
			if (!str2.empty()) {
				if (str2[str2.length()-1]  == '\n') {
					str2.resize(str2.length()-1);
				}
			}
			if (str2 == str1) {
				return true;
			}
		}
		break;
	}

	return	false;
}

bool answers_match(const std::string& report_program, const std::list<std::string>&	answer_list, enum checking_map check_mode)
{
	bool		result = false;

	for (auto it_test_answ = answer_list.begin(); it_test_answ != answer_list.end(); ++it_test_answ) {
		if (strings_match(report_program, *it_test_answ, check_mode)) {
			result = true;
			break;
		}
	}

	return result;
}

bool parse_testlib_checker_report(const std::string& testlib_checker_report)
{
	const std::string true_report = ".Testlib Result Number = 0";
	if (testlib_checker_report.find(true_report) == std::string::npos)
		return false;
	return true;
}

bool answers_match(std::string quest_file_path, std::string answer_file_path, std::string report_program_file_path, std::string testlib_checker_report_file_path)
{
	bool					result = false;
	std::string				dir_tests, dir_temp, temp;
	const std::string		check_name = "check.exe";
	
	get_full_path_name(std::string(quest_file_path).c_str(), quest_file_path);
	get_full_path_name(std::string(answer_file_path).c_str(), answer_file_path);
	get_full_path_name(std::string(report_program_file_path).c_str(), report_program_file_path);
	get_full_path_name(std::string(testlib_checker_report_file_path).c_str(), testlib_checker_report_file_path);

	assert(is_file_exist(quest_file_path));
	assert(is_file_exist(answer_file_path));

	get_directory_and_file(quest_file_path, dir_tests, temp);
	get_directory_and_file(answer_file_path, dir_temp, temp);

	if (dir_tests != dir_temp) {
		assert(false);
		return false;
	}

	std::string				check_path = dir_tests + '\\' + check_name;
	std::list<std::string>	argv;

	argv.push_back(quest_file_path); argv.push_back(report_program_file_path); argv.push_back(answer_file_path); argv.push_back(testlib_checker_report_file_path);
	int return_code = executor::execute(check_path, argv);
	if (!(return_code >= 0 && return_code <= 4)) {
		fprintf(stderr, "with exit code = %d on quest_file_path = %s", return_code, quest_file_path.c_str());
		assert(false);
		return false;
	}
	
	std::string				testlib_checker_report;
	std::ifstream			file(testlib_checker_report_file_path);
	char					ch;
	if (!file) {
		assert(false);
		return false;
	}
	while (file.get(ch)) 	testlib_checker_report += ch;
	file.close();

	result = parse_testlib_checker_report(testlib_checker_report);
	remove(testlib_checker_report_file_path.c_str());

	return result;
}

} // namespace checker