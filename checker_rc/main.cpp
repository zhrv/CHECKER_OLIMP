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
int main(int argc, char *argv[])
{
	string			main_config_file_path;
	string			profile_path;
	string			source_name;

	if (argc != 4)	{
		assert(false);		
	}
	checker::get_full_path_name(argv[1], main_config_file_path);
	checker::get_full_path_name(argv[2], profile_path);
	source_name				=	argv[3];


/*
	//c:\\users\\nill\\checker_rc\\users\\1\\1\\config_mingw_cpp.json c:\\users\\nill\\checker_rc\\users\\1\\1 main.cpp
	checker::get_full_path_name("c:\\users\\nill\\checker_rc\\users\\1\\1\\config_mingw_cpp.json", main_config_file_path);
	checker::get_full_path_name("c:\\users\\nill\\checker_rc\\users\\1\\1", profile_path);
	source_name = "main.cpp";
*/	
	checker::verdict v = checker::check(main_config_file_path, profile_path, source_name);
	cout << checker::serizlize(v) << endl;

	return 0;
}

