#ifndef _MATCHER_
#define _MATCHER_

#include <string>
#include <list>

#include "reader.h"
#include "executor.h"

namespace checker {

//checking_map: как провер€ть.
enum checking_map {
	ALL,		// провер€емы результата должен полностью(с точностью до символа) сответствовать файлу ответа. 
	WNL,		// символ '\n' в конце провер€емого результата не оказывает вли€ние на результат тестировани€.
};

//todo: optimize.
bool strings_match(std::string str1, std::string str2, enum checking_map check_mode = checking_map::WNL);
bool answers_match(const std::string& report_program, const std::list<std::string>&	answer_list, enum checking_map check_mode = checking_map::WNL);

// провер€ть использу€ testlib checker.
/*
todo:
	написано на скорую руку. в конифгах не хранитьс€ адрес где лежит testlib'овский чекер, поэтому предполагаетс€ что он называетс€ check.exe и лежит в той же папке где
	лежат тесты к текущей задаче. так что если что то не так, то все вылетит с assert. это конечно нужно переписать. система конфигов должна быть переделана, они должны
	хранитьс€ в формате json.
*/
bool answers_match(std::string quest_file_path, std::string answer_file_path, std::string report_program_file_path, std::string testlib_checker_report_file_path);
bool parse_testlib_checker_report(const std::string& testlib_checker_report);

} // namespace checker

#endif // _MATCHER_