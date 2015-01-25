#ifndef _EXECUTOR_
#define _EXECUTOR_

#include <cstdio>
#include "checker_assert.h"
#include <fstream>
#include <string>
#include <cmath>
#include <list>

#include <windows.h>	// CreateProcess, GetExitCodeProcess, GetTickCount.
#include <Psapi.h>		// GetProcessMemoryInfo. ���������� Psapi.lib

//todo:
#include <strsafe.h>	// �������� ����� ���������.

#include "reader.h"

namespace checker {

//request_type: ���� ����������� ������ executor'a. �� ��� ����� ��������� ��� ������ � ����������� ������, ������ ����������� ��� ������, �������� ��� ���� �������� �����.
enum request_type {
	CRASH,				// ��������� �����, �.�. ������� ��� �� �������� �� (return 0)      											+
	CORRECT,			//																												+
	TLE,				// ��������� �������� ������ ����������� �������.																+
	MLE,				// ��������� ���������� ������ ������ ��� ������.																+
	NIF,				// ��������� �� ������� ����, ��� ��������� ������.																+
	CPF,				// �� ������� ��������� ������� (���� ��� ������������ �����, ���� �� �� ����� �� �������� �� �����������).		+
	CCTF				// �� ������� ������� �������� ���� ��� ���������.																+
};

//request: ���������� � ���������� ������ ����������� ���������, ������� � ������, ������������ ������.
struct request {
	std::string			report_program;		 // ��������� ������ ���������.
	size_t				time_out;			 // ����� ������ ���������.
	size_t				memory;				 // ������ ������������ ����������.
	enum request_type	correct;		

	request();
};

//executor: ����� ��������� �� ���������� ����������� ����. 
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
	//set_config: ������������� ������������, ��� �� ����� ��� ���������, ��� �������� �� ������ � ������ ����� ���������� ������. 
	void set_config(const std::string& executable_file_directory, const std::string& executable_file_path, const std::string& input_file_path, const std::string& output_file_path, const size_t time_limit, const size_t memory_limit);
private:
	//create_child_process: ������� �������� �������.
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
	// �� 100 ���� ������.
	static ULONGLONG FILETIME_TO_ULONGLONG(FILETIME ft);
private:
	size_t get_memory_usage();
	size_t get_process_kernel_plus_user_time(FILETIME kernel_time, FILETIME user_time);
	size_t get_process_kernel_plus_user_time();
public:
	//execute: ���������� ���������� � ��� ��� ��������� ����������� ����. quest -- �������� �� ���� ����������� ���������.
	request& execute(const std::string& quest);
	//execute: ��������� ��������� �� ������ program_path �� ������� ���������� ��������� ������ argv. ���������� ��������� ���������� ������ ��������� ���� enum request_type.
	static int execute(std::string program_path, const std::list<std::string>& argv);
};

} // namespace checker

#endif // _EXECUTOR_