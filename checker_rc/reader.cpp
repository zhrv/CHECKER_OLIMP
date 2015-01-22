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

//test_element: ���������� ����������� 1 ����.
test_element::test_element() : quest(), answer_list() { }

//test_path_nballs_element: ������ ������� � �������� �������.
test_path_nballs_element::test_path_nballs_element()
{
	id = -1;
	scores = 0;
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

//test_reader: ����� ��� ������ 1 ����� �� �����.

//read: ������ �������� ������� ������ �� ������: quest_file, � �������� �������� ������ �� answer_file_list.
test_element& test_reader::read(const std::string& quest_file, const std::list<std::string>& answer_file_list)
{
	char			ch;
	std::string		answer;
	std::ifstream	file;
		
	file.open(quest_file);
	assert(file);

	test.quest.clear();
	test.answer_list.clear();

	// ��������� ��������� test_element ������������ ������������.
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

//config_reader: ������ ��� ������������ �� �����.

config_reader::config_reader()/*: problems()*/
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

//������ ������� ���� ��������.
bool config_reader::read_main_config(const std::string& main_config_file_path, std::string& compiler_command_template, std::string& compiler_name, problem_store& problem) {
	std::string main_config;
	read_file(main_config_file_path, main_config);
	const char *json = main_config.c_str();

	rapidjson::Document document;
	document.Parse(json);
	
	//problem
	assert(document.HasMember("problem"));
	const rapidjson::Value& pv = document["problem"];
	assert(pv.IsObject());
	
	assert(pv.HasMember("name"));
	assert(pv["name"].IsString());
	std::string problem_name = pv["name"].GetString();	// name

	assert(pv.HasMember("config"));
	const rapidjson::Value& cv = pv["config"];
	assert(cv.IsObject());

	assert(cv.HasMember("time_limit"));
	assert(cv["time_limit"].IsUint());
	problem.time_limit = cv["time_limit"].GetUint(); // time_limit

	assert(cv.HasMember("memory_limit"));
	assert(cv["memory_limit"].IsUint());
	problem.memory_limit = cv["memory_limit"].GetUint(); // memory_limit

	assert(cv.HasMember("input"));
	assert(cv["input"].IsString());
	problem.input_file_name = cv["input"].GetString(); // input

	assert(cv.HasMember("output"));
	assert(cv["output"].IsString());
	problem.output_file_name = cv["output"].GetString(); // output

	assert(cv.HasMember("test_dir"));
	assert(cv["test_dir"].IsString());
	std::string test_dir = cv["test_dir"].GetString(); // test_dir

	assert(cv.HasMember("checker"));
	assert(cv["checker"].IsString());
	problem.checker_file_path = restore_path(test_dir, cv["checker"].GetString()); //checker_file_path

	assert(cv.HasMember("tests"));
	const rapidjson::Value& testsv = cv["tests"];
	assert(testsv.IsArray());
	rapidjson::SizeType testsv_size = testsv.Size();
	for (rapidjson::SizeType i = 0; i < testsv_size; ++i) {
		const rapidjson::Value& tv = testsv[i];
		test_path_nballs_element e;
		
		assert(tv.HasMember("id"));
		assert(tv["id"].IsUint64());
		e.id = tv["id"].GetUint64(); // id

		assert(tv.HasMember("scores"));
		assert(tv["scores"].IsInt());
		e.scores = tv["scores"].GetInt(); // scores
		
		assert(tv.HasMember("input"));
		assert(tv["input"].IsString());
		e.quest_file = restore_path(test_dir, tv["input"].GetString()); // input
		
		assert(tv.HasMember("outputs"));
		const rapidjson::Value& outsv = tv["outputs"];
		assert(outsv.IsArray());
		rapidjson::SizeType outsv_size = outsv.Size();
		for (rapidjson::SizeType j = 0; j < outsv_size; ++j) {
			const rapidjson::Value& ov = outsv[j];
			assert(ov.IsString());
			e.answer_path_list.push_back(restore_path(test_dir, ov.GetString())); // add to outputs
		} // outputs
		problem.test_path_list.push_back(e);
	}										// test
	

	//language
	assert(document.HasMember("language"));
	const rapidjson::Value& lv = document["language"];
	assert(lv.IsObject());

	assert(lv.HasMember("name"));
	assert(lv["name"].IsString());
	compiler_name = lv["name"].GetString(); // name

	assert(lv.HasMember("template"));
	assert(lv["template"].IsString());
	compiler_command_template = lv["template"].GetString(); // template

	return true;
}

} // namespace checker