#include "checker.h"

namespace checker {

/*
		{
			"problem": {
				"name": "problem_a",
				"config": {
					"time_limit": 1000,
					"memory_limit": 31457280,
					"input": "race.in",
					"output": "race.out",
					"test_dir": "C:\\Users\\vs\\checker_rc\\testnconfigs\\problem_a\\",
					"checker": "check.exe",
					"tests": [
						{
							"scores": 10,
							"input": "01",
							"outputs": [
								"01.a",
								"01_2.a"
							]
						}
					]
				}
			},
			"language": {
				"name": "cpp",
				"template": "\"C:\\Program Files (x86)\\Microsoft Visual Studio 11.0\\VC\\vcvarsall.bat\" & cl /W4 /F268435456 /EHsc /FoRelease /O2 /Fe$2 $1"
			}
		}
*/

	void check(const std::string& main_config_file_path, const std::string& profile_path, const std::string& source_name, std::string& report_str) {

		config_reader	reader;

		std::string		compiler_command_template;
		std::string		compiler_name;
		problem_store 	problems;

		bool result = false;

		result = reader.read_main_config(main_config_file_path, compiler_command_template, compiler_name, problems);
		assert(result);

		std::string		executable_file_path;
		result = build_from_source(profile_path, source_name, compiler_name, compiler_command_template, executable_file_path);
		if (!result) {
			report_str = "compilation_error";
			return;
		}

		executor		executor_machine;
		//todo: передавать только: profile_path, executable_name, problems.input_file_name, problems.output_file_name, problems.time_limit, problems.memory_limit. а все нужные restore_path делать внутри set_config().
		executor_machine.set_config(profile_path, executable_file_path, restore_path(profile_path, problems.input_file_name), restore_path(profile_path, problems.output_file_name), problems.time_limit, problems.memory_limit);
		test_reader			t_reader;
		test_element		test;
		request				request_el;
		//size_t			on_test;

		static char			buff[256];
		verdict				temp;
		std::vector<verdict>		verd;
		int				scores;

		scores = 0; temp.test_passed = false; temp.report = "DEADBEAF\n"; verd.push_back(temp);

		for (auto it = problems.test_path_list.begin(); it != problems.test_path_list.end(); ++it) {
			//читаем один тест -- вопрос и множество ответов в структуру test_element.
			test = t_reader.read(it->quest_file, it->answer_path_list);

			//тестируем программу прогон€€ еЄ на вышепрочитанном тесте.
			request_el = executor_machine.execute(test.quest);

			//**************************************************************//	
			temp.test_passed = false;
			switch (request_el.correct) {
			case CORRECT:
				if (!answers_match(it->quest_file,
					*it->answer_path_list.begin(),
					restore_path(profile_path, problems.output_file_name),
					restore_path(profile_path, source_name + "testlib_checker.report"))) {
					temp.report = std::string("incorrect\n");
				}
				else {
					scores += it->scores;
					sprintf(buff, "%d ", request_el.time_out);
					temp.report = std::string("correct ") + std::string(buff);
					sprintf(buff, "%d\n", request_el.memory);
					temp.report += std::string(buff);
					temp.test_passed = true;
				}
				break;
			case TLE:
				temp.report = "tle\n";
				break;
			case MLE:
				temp.report = "mle\n";
				break;
			default:
				temp.report = "crash\n";
			}
			verd.push_back(temp);
			//**************************************************************//		
		}

		//**************************************************************//
		report_str.clear();
		temp.test_passed = false; temp.report = "DEADBEAF\n"; verd.push_back(temp);

		std::vector<size_t>	temp_v;
		for (size_t i = 1; i != verd.size(); ++i) {
			if (verd[i - 1].test_passed == false && verd[i].test_passed == true)		temp_v.push_back(i);
			if (verd[i - 1].test_passed == true && verd[i].test_passed == false)		temp_v.push_back(i - 1);
		}

		if (temp_v.empty())		report_str = "0 ";
		else {
			for (size_t i = 0; i < temp_v.size(); i += 2) {
				if (temp_v[i] != temp_v[i + 1]) {
					sprintf(buff, "%d - ", temp_v[i]);
					report_str += std::string(buff);
					sprintf(buff, "%d ", temp_v[i + 1]);
					report_str += std::string(buff);
				}
				else {
					sprintf(buff, "%d ", temp_v[i]);
					report_str += std::string(buff);
				}
			}
		}
		sprintf(buff, ";%d\n", scores);
		report_str += buff;

		for (size_t i = 1; i < verd.size() - 1; ++i) {
			sprintf(buff, "%d ", i);
			report_str += buff + verd[i].report;
		}
		//**************************************************************//
	}

/*
void check(const std::string& main_config_file_path, const std::string& problem_name, 
		   const std::string& compiler_name, const std::string& profile_path,
		   const std::string& source_name, std::string& report_str)
{
	config_reader	reader;
	executor		executor_machine;

	std::string		problem_config_path;
	std::string		compiler_command_template;
	std::string		executable_file_path;

	bool			result = false;

	result = reader.read_main_config(main_config_file_path, problem_name, compiler_name,
		problem_config_path, compiler_command_template);
	assert(result);
	
	result = build_from_source(profile_path, source_name, compiler_name, compiler_command_template, executable_file_path);
	if (!result) {
		report_str = "compilation_error";
		return;
	}

	result = reader.read_problem_config(problem_config_path);
	assert(result);

	problem_store problems = reader.get_problem_config();
	//todo: передавать только: profile_path, executable_name, problems.input_file_name, problems.output_file_name, problems.time_limit, problems.memory_limit. а все нужные restore_path делать внутри set_config().
	executor_machine.set_config(profile_path, executable_file_path, restore_path(profile_path, problems.input_file_name), restore_path(profile_path, problems.output_file_name), problems.time_limit, problems.memory_limit);
	
	test_reader			t_reader;
	test_element			test;
	request				request_el;
	//size_t			on_test;

	static char			buff[256];
	verdict				temp;
	std::vector<verdict>		verd;
	int				balls;

	balls = 0; temp.test_passed = false; temp.report = "DEADBEAF\n"; verd.push_back(temp);

	for (auto it =  problems.test_path_list.begin(); it != problems.test_path_list.end(); ++it) {
		//читаем один тест -- вопрос и множество ответов в структуру test_element.
		test = t_reader.read(it->quest_file, it->answer_path_list);
		
		//тестируем программу прогон€€ еЄ на вышепрочитанном тесте.
		request_el = executor_machine.execute(test.quest);

	//**************************************************************	
		temp.test_passed = false;
		switch (request_el.correct) {
		case CORRECT:
			if (!answers_match(it->quest_file, 
								*it->answer_path_list.begin(), 
								restore_path(profile_path, problems.output_file_name), 
								restore_path(profile_path, source_name + "testlib_checker.report"))) {
				temp.report = std::string("incorrect\n");
			} else {
				balls += it->balls;
				sprintf(buff, "%d ", request_el.time_out);
				temp.report = std::string("correct ") + std::string(buff);
				sprintf(buff, "%d\n", request_el.memory);
				temp.report += std::string(buff);
				temp.test_passed = true;
			}
			break;
		case TLE:
			temp.report = "tle\n";
			break;
		case MLE:
			temp.report = "mle\n";
			break;
		default:
			temp.report = "crash\n";
		}
		verd.push_back(temp);
	//**************************************************************		
	}

	//**************************************************************
	report_str.clear();
	temp.test_passed = false; temp.report = "DEADBEAF\n"; verd.push_back(temp);

	std::vector<size_t>	temp_v;
	for (size_t i = 1; i != verd.size(); ++i) {
		if (verd[i-1].test_passed == false && verd[i].test_passed == true)		temp_v.push_back(i);
		if (verd[i-1].test_passed == true  && verd[i].test_passed == false)		temp_v.push_back(i-1);
	}

	if (temp_v.empty())		report_str = "0 ";
	else {
		for (size_t i = 0; i < temp_v.size(); i += 2) {
			if (temp_v[i] != temp_v[i+1]) {
				sprintf(buff, "%d - ", temp_v[i]);
				report_str += std::string(buff);
				sprintf(buff, "%d ", temp_v[i+1]);
				report_str += std::string(buff);
			} else {
				sprintf(buff, "%d ", temp_v[i]);
				report_str += std::string(buff);
			}
		}
	}
	sprintf(buff, ";%d\n", balls);
	report_str += buff;

	for (size_t i = 1; i < verd.size()-1; ++i) {
		sprintf(buff, "%d ", i);
		report_str += buff + verd[i].report;
	}
	//**************************************************************
}
*/

/*
	//for ACM rules.
	auto attach_to_report = [](std::string& report_str, const request& request_el, const size_t on_test, const test_element& test) -> bool {
		static char buff[256];

		switch (request_el.correct) {
		case CORRECT:
			sprintf(buff, "%d\n", request_el.time_out);
			report_str = std::string("correct\ntime\n") + std::string(buff);
			sprintf(buff, "%d\n", request_el.memory);
			report_str += std::string("memory\n") + std::string(buff);
			break;
		case TLE:
			report_str = "tle";
			return false;
		case MLE:
			report_str = "mle";
			return false;
		default:
			report_str = "crash";
			return false;
		}

		//провер€ем корректность ответа тестируемой программы.
		if (!answers_match(request_el.report_program, test.answer_list)) {
			//todo: если вдруг на одном из тестов тестируема€ программа отработала неверно, т.е. еЄ отчет не совпадает с ожидаемым.
			//сформируем отчет и выйдем из функции.
			sprintf(buff, "%d\n", on_test);
			report_str = std::string("incorrect\n") + std::string(buff);

			return false;
		}

		return true;
	};
*/

} // namespace checker
