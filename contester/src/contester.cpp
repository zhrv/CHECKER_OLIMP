#include "data.h"
#include "document.h"
#include <windows.h>

clock_t DELAY = 10000;

void print_version();
void read_config();

int main(int argc, char** argv)
{
	print_version();
	read_config();

	try {
		db::init("localhost", "root", "", "projects_contest", 3306);
		db::connect();
	}
	catch (exception e) {
		std::cerr << "DB connection error: " <<  e.get_message() << std::endl;
		return EXIT_FAILURE;
	}

	while (1) {
		clock_t start_time = clock();
		
		indexes ind = solution::get_not_checked_ids();
		for (indexes::iterator it = ind.begin(); it != ind.end(); it++) {
			try {
				psolution ps(new solution(*it));
				checker ch(ps, "solution", "solution.json", "result.json");
				ch.run();
			}
			catch (exception e) {
				std::cerr << "solution #" << *it << ": " << e.get_message() << std::endl;
			}
		}
		

		clock_t end_time = start_time+DELAY;
		if (clock() < end_time) {
			Sleep(DELAY - (clock() - start_time));
		}
		
	}
	
	db::close();
	
	return EXIT_SUCCESS;
}











void print_version()
{
	std::cout << "Contester for programming olimpiads" << std::endl;
	std::cout << "(C) V. Salnikov and R.Zhalnin (Mordovian State University, Russia)" << std::endl;
	std::cout << "Version 0.0.1dev" << std::endl << std::endl;
}

void read_config() 
{
	stdstr str;
	char ch;
	str.clear();
	std::ifstream f("contester.cfg");
	while (f.get(ch)) str += ch;
	f.close();
	const char* json = str.c_str();
	rapidjson::Document doc;
	doc.Parse(json);
	DELAY = doc["delay"].GetInt();
	checker::ch_bin = doc["checker"].GetString();
	checker::tests_dir = doc["tests_dir"].GetString();
	if (doc["langs"].IsArray()) {
		checker::clngs.clear();
		const rapidjson::Value& lv = doc["langs"];
		rapidjson::SizeType lv_size = lv.Size();
		for (rapidjson::SizeType i = 0; i < lv_size; ++i) {
			const rapidjson::Value& li = lv[i];
			checker::clng lng;
			lng.cmd = li["compiler"].GetString();
			lng.ext = li["ext"].GetString();
			checker::clngs[li["id"].GetString()] = lng;
		}
	} 
}
