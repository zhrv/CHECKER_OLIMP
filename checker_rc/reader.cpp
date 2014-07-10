#include "reader.h"

namespace checker {

void get_full_path_name(const TCHAR *path, std::string& full_path)
{
	DWORD		retval = 0;
	TCHAR		**lppPart = {0};
	TCHAR		buff[1024];

	retval = GetFullPathName(path,
					1024,
					buff,
					lppPart);  

	full_path.assign(buff);
	assert(retval);
}

void get_directory_and_file(std::string path, std::string& directory_path, std::string& file_name)
{
	directory_path.clear(); file_name.clear();
	get_full_path_name(std::string(path).c_str(), path);
	int pos;
	for (pos = path.length() - 1; pos >= 0; --pos)
	{
		if (path[pos] == '\\')	break;
	}
	if (pos != 0 && pos < path.length()-1)	file_name = path.substr(pos+1);
	else									assert(false);
	directory_path = path.substr(0, pos);
}

bool is_file_exist(const std::string& path)
{
	std::ifstream			file(path);
	bool					result = false;
	if (file)				result = true;
	file.close();
	return result;
}

std::string restore_path(const std::string& profile_path, const std::string& file_name)
{
	std::string		temp;

	temp = profile_path + "\\" + file_name;

	return temp;
}

//test_element: структрура описывающая 1 тест.
test_element::test_element() : quest(), answer_list() { }

//test_path_nballs_element: адреса вопроса и валидных ответов.
test_path_nballs_element::test_path_nballs_element()
{
	balls = 0;
	quest_file.clear();
	answer_path_list.clear();
}

problem_store::problem_store() : time_limit(0), memory_limit(0) 
{
	test_path_list.clear();
}

bool problem_store::empty()
{
	return test_path_list.empty() && time_limit == 0 && memory_limit == 0 ? true : false;
}

//test_reader: класс для чтения 1 теста из фалов.

//read: читает тестовые входные данные из файлов: quest_file, и тестовые выходные данные из answer_file_list.
test_element& test_reader::read(const std::string& quest_file, const std::list<std::string>& answer_file_list)
{
	char			ch;
	std::string		answer;
	std::ifstream	file;
		
	file.open(quest_file);
	assert(file);

	test.quest.clear();
	test.answer_list.clear();

	// заполняем структуру test_element посимвольным копированием.
	while (file.get(ch)) {
		test.quest += ch;
	}
	file.close();
		
	for (std::list<std::string>::const_iterator i = answer_file_list.begin(); i != answer_file_list.end(); ++i) {
		file.open(*i);
		assert(file);
		answer.clear();
		while (file.get(ch)) {
			answer += ch;
		}
		file.close();
		test.answer_list.push_back(answer);
	}

	return test; 
}

//config_reader: читает все конфинурации из файла.

config_reader::config_reader(): problems()
{
	state = state_in::UNKNOWN;
}

void config_reader::delete_first_space(std::string& str)
{
	int	i = 0;
	while (isspace(str[i]))
		++i;
	str.replace(0, i, "");
}

void config_reader::read_file(const std::string& file_path, std::string& file_content)
{
	std::ifstream		file(file_path);
	char				ch;

	if (!file)			assert(false);
		
	if (!file_content.empty())	file_content.clear();

	while (file.get(ch)) 		file_content += ch;
		
	file.close();
}

bool config_reader::empty()
{
	return problems.empty();
}

problem_store& config_reader::get_problem_config()
{
	return problems;
}

//парсит главный файл конфигов.
bool config_reader::read_main_config(const std::string& main_config_file_path, const std::string& problem_name, const std::string& compiler_name, std::string& problem_config_path, std::string& compiler_config)
{
	const std::string problem_list_str  = "problem_list:";
	const std::string language_list_str = "language_list:";

	std::ifstream	in(main_config_file_path);
	std::string		strbuf;
	
	assert(in);
	std::getline(in, strbuf);
	assert(strbuf == problem_list_str);
	
	if (!problem_config_path.empty()) problem_config_path.clear();
	if (!compiler_config.empty()) compiler_config.clear();

	while (!std::cin.eof()) {
		std::getline(in, strbuf);
		if (strbuf == problem_name) {
			std::getline(in, strbuf);
			problem_config_path = strbuf;
			break;
		}
	}
	
	while (!std::cin.eof()) {
		std::getline(in, strbuf);
		if (strbuf == language_list_str)
			break;
	}

	while (!std::cin.eof()) {
		std::getline(in, strbuf);
		if (strbuf == compiler_name) {
			std::getline(in, strbuf);
			read_file(strbuf, compiler_config);
			break;
		}
	}

	return !compiler_config.empty() && !problem_config_path.empty();
}

//todo:	бажная проверка валидности и синтаксис чек.
//read: читает из файла в структуру данные, в которых ограничения по времени, памяти, исполняемый файл проверяемой программы, 
//		имя файля подающегосяхся ему на вход и имя файла создаваемое тестируемой программой, а так же адреса тестовых файлов.
bool config_reader::read_problem_config(const std::string& configFile)
{
	static const std::string		time_limit_str = "time_limit:";
	static const std::string		memory_limit_str = "memory_limit:";
	static const std::string		input_file_name_str = "input:";
	static const std::string		outout_file_name_str = "output:";
	static const std::string		q_str = "Q:";
	static const std::string		re_st = "Re:";

	std::ifstream					in(configFile);
	std::string						strbuf;
	test_path_nballs_element		element;

	assert(in);

	in >> strbuf;
	if (strbuf == time_limit_str) {
		in >> problems.time_limit;
	} else {
		assert(false);
	}
	in >> strbuf;
	if (strbuf == memory_limit_str) {
		in >> problems.memory_limit;
	} else {
		assert(false);
	}
	in >> strbuf;
	if (strbuf == input_file_name_str) {
		in >> problems.input_file_name;
	} else {
		assert(false);
	}
	in >> strbuf;
	if (strbuf == outout_file_name_str) {
		in >> problems.output_file_name;
	} else {
		assert(false);
	}

	std::getline(in, strbuf);

	while (in.good()) {
		std::getline(in, strbuf);
		switch (state) {
		case state_in::UNKNOWN:
			if (strbuf == q_str) {
				state = state_in::Q;
				break;
			} else if (strbuf == re_st) {
					state = state_in::Re;
					break;
			} else {
				assert(false);
			}
			break;
		case state_in::Q:
			{
				int balls;
				delete_first_space(strbuf);
				balls = atoi(strbuf.c_str());
				element.balls = balls;
				std::getline(in, strbuf);
				delete_first_space(strbuf);
				element.quest_file = strbuf;
				state = state_in::UNKNOWN;
			}
			break;
		case state_in::Re:
			if (strbuf != q_str) {
				if (strbuf.length() != 0) {
					delete_first_space(strbuf);
					element.answer_path_list.push_back(strbuf);
				}
			} else {
				state = state_in::Q;
				problems.test_path_list.push_back(element);
				element.quest_file.clear();
				element.answer_path_list.clear();
				break;
			}
			break;
		}
	}
	problems.test_path_list.push_back(element);
	element.quest_file.clear();
	element.answer_path_list.clear();
		
	return !empty();
}

} // namespace checker