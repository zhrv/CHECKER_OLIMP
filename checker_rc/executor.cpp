#include "executor.h"

namespace checker {

	request::request() : time_out(0), memory(0)
	{
		report_program.clear();
		correct = request_type::CRASH;
	}

//executor: класс запускает на выполнение исполняемый файл. 

	executor::executor() : time_limit(0), memory_limit(0), request_report()
	{
		executable_file_path.clear();
	}


	//set_config: утсанавливает конфигурации, что бы знать что запускать, что подавать на чтение и откуда брать результаты работы. 
	void executor::set_config(const std::string& executable_file_directory, const std::string& executable_file_path, const std::string& input_file_path, const std::string& output_file_path, const size_t time_limit, const size_t memory_limit)
	{
		this->executable_file_directory = executable_file_directory;
		this->executable_file_path  = executable_file_path;
		this->input_file_path = input_file_path;
		this->output_file_path = output_file_path;
		this->time_limit   = time_limit;
		this->memory_limit = memory_limit;
	}

	//create_child_process: создать дочерний процесс.
	bool executor::create_child_process()
	{
		bool						 success = false; 

		// Set up members of the PROCESS_INFORMATION structure. 
 
		ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION) );

		// Set up members of the STARTUPINFO structure. 
		// This structure specifies the STDIN and STDOUT handles for redirection.
 
		ZeroMemory(&siStartInfo, sizeof(STARTUPINFO) );
		siStartInfo.cb = sizeof(STARTUPINFO);
		
		// Create the child process. 

		success = CreateProcess(
			executable_file_path.c_str(),				//application name
			nullptr,									// command line 
			nullptr,									// process security attributes 
			nullptr,									// primary thread security attributes 
			false,										// handles are inherited 
			0,											// creation flags.							DEBUG_PROCESS - запуск процесса, для чекера тестируемая программа будет являться отладочной.
			nullptr,									// use parent's environment 
			executable_file_directory.c_str(),			// use parent's current directory 
			&siStartInfo,								// STARTUPINFO pointer 
			&piProcInfo);								// receives PROCESS_INFORMATION 
   
		return success;
	}

	DWORD executor::get_exit_code()
	{
		DWORD			exit_code;
		GetExitCodeProcess(piProcInfo.hProcess, &exit_code);		
		return exit_code;
	}

	bool executor::child_process_active()
	{
		return get_exit_code() == STILL_ACTIVE ? true : false;
	}

	void executor::kill_child_process()
	{
		if (child_process_active())		TerminateProcess(piProcInfo.hProcess, NO_ERROR);
	}

	void executor::close_handles()
	{
		// Close handles to the child process and its primary thread.
		// Some applications might keep these handles to monitor the status
		// of the child process, for example. 
		CloseHandle(piProcInfo.hProcess);
		CloseHandle(piProcInfo.hThread);
		CloseHandle(siStartInfo.hStdOutput);
	}
	
	bool executor::make_input_file(const std::string& quest)
	{
		std::ofstream	file(input_file_path);
		size_t			it;

		if (!file)	return false;

		for (it = 0; it < quest.length(); ++it)		file.put(quest[it]);

		file.close();
		return true;
	}

	bool executor::read_output_file()
	{
		std::ifstream		file(output_file_path);
		char				ch;

		if (!file)	return false;

		while (file.get(ch)) 	request_report.report_program += ch;
		
		file.close();
		return true;
	}

	void executor::remove_in_out_files()
	{
		remove(input_file_path.c_str());
		remove(output_file_path.c_str());
	}

	// по 100 нано секунд.
	ULONGLONG executor::FILETIME_TO_ULONGLONG(FILETIME ft)
	{
		// Copy the time into a quadword.
		return (((ULONGLONG) ft.dwHighDateTime) << 32) + ft.dwLowDateTime;
	}

	size_t executor::get_memory_usage()
	{
		size_t						memory = -1;
		PROCESS_MEMORY_COUNTERS		pmc;

		if (GetProcessMemoryInfo(piProcInfo.hProcess, &pmc, sizeof(pmc)) == false)		return -1;

		/*в pmc вся информация*/
		memory = pmc.WorkingSetSize;

/*
        printf( "\tPageFaultCount: %d\n", pmc.PageFaultCount / 1024 );
        printf( "\tPeakWorkingSetSize: %d\n", 
                  pmc.PeakWorkingSetSize / 1024);
        printf( "\tWorkingSetSize: %d\n", pmc.WorkingSetSize / 1024);
        printf( "\tQuotaPeakPagedPoolUsage: %d\n", 
                  pmc.QuotaPeakPagedPoolUsage / 1024);
        printf( "\tQuotaPagedPoolUsage: %d\n", 
                  pmc.QuotaPagedPoolUsage / 1024);
        printf( "\tQuotaPeakNonPagedPoolUsage: %d\n", 
                  pmc.QuotaPeakNonPagedPoolUsage / 1024);
        printf( "\tQuotaNonPagedPoolUsage: %d\n", 
                  pmc.QuotaNonPagedPoolUsage / 1024);
        printf( "\tPagefileUsage: %d\n", pmc.PagefileUsage / 1024); 
        printf( "\tPeakPagefileUsage: %d\n", 
                  pmc.PeakPagefileUsage / 1024);
		printf("\n------------------------------------------------------\n");
*/
		return memory;
	}

	size_t executor::get_process_kernel_plus_user_time(FILETIME kernel_time, FILETIME user_time)
	{
		size_t k_t = FILETIME_TO_ULONGLONG(kernel_time);
		size_t u_t =  FILETIME_TO_ULONGLONG(user_time);

		size_t kernel_t = k_t / 10000;
		size_t user_t =   u_t / 10000;

		size_t result = user_t + kernel_t;

		return result;
	}

	size_t executor::get_process_kernel_plus_user_time()
	{
		FILETIME ProcessStartTime, ProcessEndTime, KernelTime, UserTime;
		GetProcessTimes(piProcInfo.hProcess, &ProcessStartTime, &ProcessEndTime, &KernelTime, &UserTime);
		return get_process_kernel_plus_user_time(KernelTime, UserTime);
	}

	//execute: возвращает информацию о том как отработал проверяемый файл. quest -- подается на вход проверяемой программе.
	request& executor::execute(const std::string& quest)
	{
		size_t work_time	= 0;
		size_t timer		= 0;
		size_t memory		= 0;
		size_t memory_usage = 0;

		request_report.correct = request_type::CORRECT;
		request_report.report_program.clear();
		request_report.memory = request_report.time_out = 0;
		remove_in_out_files();

		if (make_input_file(quest) == false) {
			request_report.correct = CCTF;						// checker_assert(false);
			return request_report;								// возможно нужно кинуть exception.
		}
		
		if (create_child_process() == false) {
			request_report.correct = CPF;
			return request_report;
		}

		size_t time_start = GetTickCount();
		while (true) {
			DWORD exit_code;
			exit_code = get_exit_code();
			if (exit_code == STILL_ACTIVE) {
				//мониторинг по времени.	
				work_time = get_process_kernel_plus_user_time();
				timer = GetTickCount() - time_start;
				if (work_time > time_limit || abs((long long) timer - work_time) > eps_time) {
					request_report.correct = TLE;
					kill_child_process();
					break;
				}
				//мониторинг по памяти.
				memory_usage = get_memory_usage();
				memory = max(memory, memory_usage);
				if (memory == -1 || memory > memory_limit) {
					request_report.correct = MLE;
					kill_child_process();
					break;
				}
			} else if (exit_code == 0) {
				break;
			} else {
				request_report.correct = CRASH;
				break;
			}
		}

		//todo:
		/*
				Sleep(100);
				это конечно костыль, и очень большой. на тот случай если проверяемое приложение не закрыло дескрипторы файлов и после завершения приложения операионная система тоже не успела
				это сделать. пока я не нашел решения как с этим бороться. эту задачу нужно решить!
		*/
		Sleep(100);
		close_handles();

		if (request_report.correct == CORRECT) {
			if (read_output_file() == false) {
				request_report.correct = NIF;
				return request_report;
			}
		}

		request_report.time_out = work_time;
		request_report.memory = memory;
		return request_report;
	}

	int executor::execute(std::string program_path, const std::list<std::string>& argv)
	{
		PROCESS_INFORMATION			piProcInfo; 
		STARTUPINFO					siStartInfo;
		size_t						work_time	 = 0;
		size_t						timer		 = 0;
		const static size_t			time_limit   = 10000; //ms.
		size_t						return_code	 = 0;
		std::string					application_name, application_directory;
		
		checker_assert(!(program_path.length() == 0));

		checker::get_directory_and_file(program_path, application_directory, application_name);
		
		ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION) );
		ZeroMemory(&siStartInfo, sizeof(STARTUPINFO) );
		siStartInfo.cb = sizeof(STARTUPINFO);
		std::string		command_line_string = application_name;
		for (std::list<std::string>::const_iterator it = argv.begin(); it != argv.end(); ++it) {
			command_line_string += " \""+ *it + "\"";
		}
		command_line_string = application_directory + "\\" + command_line_string;
		char *command_line = _strdup(command_line_string.c_str());
		bool success = CreateProcess(
			nullptr,									//application name
			command_line,								// command line 
			nullptr,									// process security attributes 
			nullptr,									// primary thread security attributes 
			false,										// handles are inherited 
			0,											// creation flags.	DEBUG_PROCESS - запуск процесса, для чекера тестируемая программа будет являться отладочной.
			nullptr,									// use parent's environment 
			nullptr,									// use parent's current directory 
			&siStartInfo,								// STARTUPINFO pointer 
			&piProcInfo);								// receives PROCESS_INFORMATION

		size_t time_start = GetTickCount();
		DWORD exit_code = 0;
		while (true) {
			GetExitCodeProcess(piProcInfo.hProcess, &exit_code);	
			if (exit_code == STILL_ACTIVE) {
				//мониторинг по времени.
				FILETIME ProcessStartTime, ProcessEndTime, KernelTime, UserTime;
				GetProcessTimes(piProcInfo.hProcess, &ProcessStartTime, &ProcessEndTime, &KernelTime, &UserTime);
				size_t k_t = FILETIME_TO_ULONGLONG(KernelTime);
				size_t u_t =  FILETIME_TO_ULONGLONG(UserTime);
				size_t kernel_t = k_t / 10000;
				size_t user_t =   u_t / 10000;
				work_time = user_t + kernel_t;
				timer = GetTickCount() - time_start;
				if (work_time > time_limit || abs((long long) timer - work_time) > eps_time) {
					return_code = -1;
					DWORD exit_code;
					GetExitCodeProcess(piProcInfo.hProcess, &exit_code);
					if (exit_code == STILL_ACTIVE)		TerminateProcess(piProcInfo.hProcess, NO_ERROR);
					break;
				}
			} else if (exit_code >= 0 && exit_code <= 4) {
				return_code = exit_code;
				break;
			} else {
				return_code = -1;
				break;
			}
		}

		CloseHandle(piProcInfo.hProcess);
		CloseHandle(piProcInfo.hThread);
		
		free(command_line);

		return return_code;
	}

} // namespace checker
