#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <cstdlib>
#include "checker_assert.h"
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
	string			result_name;

	if (argc != 5)	{
		checker_assert(false);		
	}
	checker::get_full_path_name(argv[1], main_config_file_path);
	checker::get_full_path_name(argv[2], profile_path);
	source_name				  = argv[3];
	result_name               = argv[4];

	checker::verdict v = checker::check(main_config_file_path, profile_path, source_name);

	ofstream fout(result_name);
	fout << checker::serizlize(v) << endl;
	fout.close();
	return 0;
}

