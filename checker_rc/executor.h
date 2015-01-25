#ifndef _EXECUTOR_
#define _EXECUTOR_

#include <cstdio>
#include "checker_assert.h"
#include <fstream>
#include <string>
#include <cmath>
#include <list>

#include <windows.h>	// CreateProcess, GetExitCodeProcess, GetTickCount.
#include <Psapi.h>		// GetProcessMemoryInfo. Подключить Psapi.lib

//todo:
#include <strsafe.h>	// возможно потом избавлюсь.

#include "reader.h"

namespace checker {

//request_type: типы результатов работы executor'a. то что может случиться при работе с исполняемым файлом, чтении результатов его работы, создании для него входного файла.
enum request_type {
	CRASH,				// программа упала, т.е. вернула что то отличное от (return 0)      											+
	CORRECT,			//																												+
	TLE,				// программа работает дольше отведенного времени.																+
	MLE,				// программа использует памяти больше чем задано.																+
	NIF,				// программа не создала файл, как результат работы.																+
	CPF,				// не удалось запустить процесс (либо нет исполняемого файла, либо он по каким то причинам не запускается).		+
	CCTF				// не удалось создать тестовый файл для программы.																+
};

//request: информация о результате работы тестируемой программы, времени её работы, используемой памяти.
struct request {
	std::string			report_program;		 // результат работы программы.
	size_t				time_out;			 // время работы программы.
	size_t				memory;				 // память используемая программой.
	enum request_type	correct;		

	request();
};

//executor: класс запускает на выполнение исполняемый файл. 
class executor {
private:	
	std::string		executable_file_path;
	std::string		executable_file_directory;
	std::string		input_file_path;
	std::string		output_file_path;
	size_t			time_limit;
	size_t			memory_limit;
	request			request_report;
private:
	static const size_t	eps_time = 2048;
private:
	static const size_t			 buf_size = 4096;
	PROCESS_INFORMATION			 piProcInfo; 
	STARTUPINFO					 siStartInfo;
public:
	executor();
public:
	//set_config: утсанавливает конфигурации, что бы знать что запускать, что подавать на чтение и откуда брать результаты работы. 
	void set_config(const std::string& executable_file_directory, const std::string& executable_file_path, const std::string& input_file_path, const std::string& output_file_path, const size_t time_limit, const size_t memory_limit);
private:
	//create_child_process: создать дочерний процесс.
	bool create_child_process();
	DWORD get_exit_code();
	bool child_process_active();
	void kill_child_process();
	void close_handles();
public:
	bool make_input_file(const std::string& quest);
	bool read_output_file();
	void remove_in_out_files();
private:
	// по 100 нано секунд.
	static ULONGLONG FILETIME_TO_ULONGLONG(FILETIME ft);
private:
	size_t get_memory_usage();
	size_t get_process_kernel_plus_user_time(FILETIME kernel_time, FILETIME user_time);
	size_t get_process_kernel_plus_user_time();
public:
	//execute: возвращает информацию о том как отработал проверяемый файл. quest -- подается на вход проверяемой программе.
	request& execute(const std::string& quest);
	//execute: запускает программу по адресу program_path со списком аргументов командной строки argv. возвращает результат завершения работы программы типа enum request_type.
	static int execute(std::string program_path, const std::list<std::string>& argv);
};

} // namespace checker

#endif // _EXECUTOR_