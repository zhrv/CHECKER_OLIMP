#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <cstdlib>
#include <cassert>
#include "checker.h"
#include <windows.h>
using namespace std;

//todo: исправить ворнинги!
//todo: оптимизировать все места чтения из файлов где встречается такой код: while (file.get(ch)) { answer += ch; }. необходимо читать сразу большимим кусками.
//todo: избавится от текущей-самописной системы конфигов. сделать json.

int main(int argc, char *argv[])
{
	string			main_config_file_path;
	string			problem_name;
	string			compiler_name;
	string			profile_path;
	string			source_name;


	if (argc != 5)	{
		assert(false);		
	}

	checker::get_full_path_name(TEXT("main_conf.txt"), main_config_file_path);
	problem_name		= argv[1];
	compiler_name		= argv[2];
	checker::get_full_path_name(	  argv[3], profile_path);
	source_name			= argv[4];

/*
	//debug.
	checker::get_full_path_name(TEXT("main_conf.txt"), main_config_file_path);
	problem_name		= "1";
	compiler_name		= "cpp";
	checker::get_full_path_name(	  "C:\\vs\\checker_rc\\checker_rc\\users", profile_path);
	source_name			= "main.cpp";
	//
*/
	string			report_str;

	checker::check(main_config_file_path, problem_name, compiler_name, profile_path, source_name, report_str);
	cout << "report\n" << report_str << endl;

	string		    	report_path = profile_path + '\\' + problem_name + ".txt";
	ofstream		file_report(report_path);
	if (!file_report)	assert(false);
	file_report << "report\n" << report_str << endl;
	file_report.close();
	
	return 0;
}

