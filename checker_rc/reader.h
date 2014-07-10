#ifndef _READER_
#define _READER_

#include <cassert>
#include <string>
#include <list>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include "windows.h"

namespace checker {

//get_full_path_name: �������������� �� �������������� ���� � ����������.
void get_full_path_name(const TCHAR *path, std::string& full_path);
//get_directory_and_file: �� ���� � ����� path, ���������� ���� � ����� � directory_path, � ��� ����� � file_name.
void get_directory_and_file(std::string path, std::string& directory_path, std::string& file_name);
//is_file_exist: ���� �� ������ path ����������(�����������)?
bool is_file_exist(const std::string& path);
std::string restore_path(const std::string& profile_path, const std::string& file_name);

//test_element: ���������� ����������� 1 ����.
struct test_element {
	std::string					quest;
	std::list<std::string>		answer_list;

	test_element();
};

//test_path_nballs_element: ������ ������� � �������� �������.
struct test_path_nballs_element {
	int							balls;
	std::string					quest_file;
	std::list<std::string>		answer_path_list;

	test_path_nballs_element();
};

//problem_config: ��������� ���� �������, ���� ������; � ����������� �� ������ � �������, � �����.
struct problem_store {
	size_t								time_limit;
	size_t								memory_limit;
	std::string							input_file_name;
	std::string							output_file_name;
	std::list<test_path_nballs_element>	test_path_list;

	problem_store();
	bool empty();
};

//test_reader: ����� ��� ������ 1 ����� �� �����.
class test_reader {
private:
	test_element	test;
public:
	//read: ������ �������� ������� ������ �� ������: quest_file, � �������� �������� ������ �� answer_file_list.
	test_element& read(const std::string& quest_file, const std::list<std::string>& answer_file_list);
};

//config_reader: ������ ��� ������������ �� �����.
class config_reader {
private:
	enum state_in {
		Q,
		Re,
		UNKNOWN
	}	state;
private:
	problem_store	problems;
public:
	config_reader();
private:
	void delete_first_space(std::string& str);
private:
	void read_file(const std::string& file_path, std::string& file_content);
public:
	bool empty();
	problem_store& get_problem_config();
public:
	//������ ������� ���� ��������.
	bool read_main_config(const std::string& main_config_file_path, const std::string& problem_name, const std::string& compiler_name, std::string& problem_config_path, std::string& compiler_config);
	//todo:	������ �������� ���������� � ��������� ���.
	//read: ������ �� ����� � ��������� ������, � ������� ����������� �� �������, ������, ����������� ���� ����������� ���������, 
	//		��� ����� �������������� ��� �� ���� � ��� ����� ����������� ����������� ����������, � ��� �� ������ �������� ������.
	bool read_problem_config(const std::string& configFile);
};

} // namespace checker

#endif // _READER_