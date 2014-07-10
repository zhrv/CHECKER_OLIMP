#ifndef _MATCHER_
#define _MATCHER_

#include <string>
#include <list>

#include "reader.h"
#include "executor.h"

namespace checker {

//checking_map: ��� ���������.
enum checking_map {
	ALL,		// ���������� ���������� ������ ���������(� ��������� �� �������) �������������� ����� ������. 
	WNL,		// ������ '\n' � ����� ������������ ���������� �� ��������� ������� �� ��������� ������������.
};

//todo: optimize.
bool strings_match(std::string str1, std::string str2, enum checking_map check_mode = checking_map::WNL);
bool answers_match(const std::string& report_program, const std::list<std::string>&	answer_list, enum checking_map check_mode = checking_map::WNL);

// ��������� ��������� testlib checker.
/*
todo:
	�������� �� ������ ����. � �������� �� ��������� ����� ��� ����� testlib'������ �����, ������� �������������� ��� �� ���������� check.exe � ����� � ��� �� ����� ���
	����� ����� � ������� ������. ��� ��� ���� ��� �� �� ���, �� ��� ������� � assert. ��� ������� ����� ����������. ������� �������� ������ ���� ����������, ��� ������
	��������� � ������� json.
*/
bool answers_match(std::string quest_file_path, std::string answer_file_path, std::string report_program_file_path, std::string testlib_checker_report_file_path);
bool parse_testlib_checker_report(const std::string& testlib_checker_report);

} // namespace checker

#endif // _MATCHER_