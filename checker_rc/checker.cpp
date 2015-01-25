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
				"test_dir": "C:\\Users\\nill\\checker_rc\\testnconfigs\\a_tests",
				"checker": "check.exe",
				"tests": [
					{
						"id": 123,
						"scores": 10,
						"input": "01",
						"outputs": [
							"01.a"
						]
					},
					{
						"id": 321,
						"scores": 10,
						"input": "02",
						"outputs": [
							"02.a"
						]
					}
				]
			}
		},
		"language": {
		"name": "msv_cpp",
		"template": "\"C:\\Program Files (x86)\\Microsoft Visual Studio 11.0\\VC\\vcvarsall.bat\" & cl /W4 /F268435456 /EHsc /FoRelease /O2 /Fe$2 $1"
		}
	}
*/

verdict check(const std::string& main_config_file_path, const std::string& profile_path, const std::string& source_name) {
	verdict			result;

	config_reader	reader;
	std::string		compiler_command_template;
	std::string		compiler_name;
	problem_store 	problems;
			
	bool result_flag = false;
	result_flag = reader.read_main_config(main_config_file_path, compiler_command_template, compiler_name, problems);
	checker_assert(result_flag);

	std::string		executable_file_path;
	result_flag = build_from_source(profile_path, source_name, compiler_name, compiler_command_template, executable_file_path);
	if (!result_flag) {
		result.error_msg = "compilation_error";
		result.status = "bad";
		return result;
	}

	executor		executor_machine;
	//todo: передавать только: profile_path, executable_name, problems.input_file_name, problems.output_file_name, problems.time_limit, problems.memory_limit. а все нужные restore_path делать внутри set_config().
	executor_machine.set_config(profile_path, executable_file_path, restore_path(profile_path, problems.input_file_name), restore_path(profile_path, problems.output_file_name), problems.time_limit, problems.memory_limit);
	test_reader			t_reader;
	test_element		test;
	request				request_el;
	//size_t			on_test;

	static char			buff[256];

	result.status = "ok";
	result.error_msg = "";
	for (auto it = problems.test_path_list.begin(); it != problems.test_path_list.end(); ++it) {
		//читаем один тест -- вопрос и множество ответов в структуру test_element.
		test = t_reader.read(it->quest_file, it->answer_path_list);
		//тестируем программу прогоняя её на вышепрочитанном тесте.
		request_el = executor_machine.execute(test.quest);

		//**************************************************************//	
		switch (request_el.correct) {
		case CORRECT:
			if (!answers_match(it->quest_file,
				*it->answer_path_list.begin(),
				restore_path(profile_path, problems.output_file_name),
				restore_path(profile_path, source_name + "testlib_checker.report"))) {
				result.report.push_back(report_elem("bad", it->id, request_el.time_out, request_el.memory, 0));
			}
			else {
				result.report.push_back(report_elem("ok", it->id, request_el.time_out, request_el.memory, it->scores));
			}
			break;
		case TLE:
			result.report.push_back(report_elem("tle", it->id, -1, request_el.memory, 0));
			break;
		case MLE:
			result.report.push_back(report_elem("mle", it->id, request_el.time_out, -1, 0));
			break;
		default:
			result.report.push_back(report_elem("crash", it->id, -1, -1, 0));
		}
		//**************************************************************//		
	}

	return result;
}

std::string serizlize(const verdict& v) {
	rapidjson::StringBuffer s;
	rapidjson::Writer<rapidjson::StringBuffer> writer(s);
	writer.StartObject();
	writer.String("status");
	writer.String(v.status.c_str());
	writer.String("error_msg");
	writer.String(v.error_msg.c_str());
	writer.String("report");
	writer.StartArray();
	for (auto it = v.report.begin(); it != v.report.end(); ++it) {
		writer.StartObject();
		writer.String("id");
		writer.Uint64(it->id);
		writer.String("result");
		writer.String(it->result.c_str());
		writer.String("time");
		writer.Uint64(it->time);
		writer.String("memory");
		writer.Uint64(it->memory);
		writer.String("score");
		writer.Int(it->score);
		writer.EndObject();
	}
	writer.EndArray();
	writer.EndObject();
	return  s.GetString();
}

} // namespace checker
