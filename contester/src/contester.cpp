#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include "mysql.h"
#include <assert.h>

const clock_t	DELAY		= 5000;

typedef std::vector<int> indexes;

struct db
{
	typedef std::vector<MYSQL_ROW> rows;

	static MYSQL *conn;
	static MYSQL mysql;



	static void connect(std::string host, std::string user, std::string passwd, std::string table, int port);

	static rows q(std::string sql);

	static void close() { mysql_close(db::conn); }

};
MYSQL *db::conn = 0;
MYSQL  db::mysql;


struct exception
{
	exception(std::string msg) : message(msg) {}
	std::string get_message() { return message; }
	std::string message;
};


struct test
{
	test(MYSQL *conn);
	~test(){}
};
typedef std::auto_ptr<test> ptest;
typedef std::vector<test> tests_vector;


struct task
{
	task(MYSQL *conn);
	~task(){}
	tests_vector tests;
};
typedef std::auto_ptr<task> ptask;



struct solution
{
	solution(int id);
	~solution();

	std::string create_src(std::string name);
	void delete_src(std::string name);

	std::string get_cfg_json();
	std::string create_cfg_json(std::string name);
	void delete_cfg_json(std::string name);
	std::string get_src_ext();

	static indexes get_not_checked_ids();


	ptask task;
};
typedef std::auto_ptr<solution> psolution;

struct checker
{
	struct result
	{
		std::string json;
	};

	checker(psolution ps, std::string src_name, std::string cfg_name);
	void run();
	result get_result() { return res; }

	void save_result_to_db(){}

	psolution sol;
	result res;
	std::string src, cfg;
};
typedef std::auto_ptr<checker> pchecker;




int main(int argc, char** argv)
{


	std::string cmd = "checker_rc.exe solution.json . solution.pas";
	system(cmd.c_str());

	
	return EXIT_SUCCESS;

	db::connect("localhost", "root", "", "projects_contest", 3306);
	
	while (1) {
		clock_t start_time = clock();
		
		indexes ind = solution::get_not_checked_ids();
		for (indexes::iterator it = ind.begin(); it != ind.end(); it++) {
			try {
				psolution ps(new solution(*it));
				checker ch(ps, "solution", "solution.json");
				ch.run();
				ch.save_result_to_db();
			}
			catch (exception e) {
				std::cerr << "solution #" << *it << ": " << e.get_message() << std::endl;
			}
		}
		

		clock_t end_time = start_time+DELAY;
		while (clock() < end_time);
		
	}
	
	db::close();
	
	return 0;
}













/**
 *	struct db
 */

void db::connect(std::string host, std::string user, std::string passwd, std::string table, int port)
{
	mysql_init(&db::mysql);
	//connection = mysql_real_connect(&mysql,"host","user",
	//                   "password","database",port,"unix_socket",clientflag);
	db::conn = mysql_real_connect(&db::mysql, "localhost", "root", "", "projects_contest", 3306, 0, 0);
	if (db::conn == NULL) {
		throw exception(mysql_error(&db::mysql));
	}
}


db::rows db::q(std::string sql)
{
	MYSQL_RES *result;
	MYSQL_ROW row;

	int query_state;	query_state = mysql_query(db::conn, sql.c_str());
	if (query_state != 0) {
		throw exception(mysql_error(db::conn));
	}

	db::rows r;
	result = mysql_store_result(db::conn);
	while ((row = mysql_fetch_row(result)) != NULL) {
		r.push_back(row);
	}
	mysql_free_result(result);

	return r;
}


/**
 *	struct solution
 */

solution::solution(int id)
{
	db::rows r = db::q("select * from solutions where id = " + id);
	if (r.size() > 0) {

	}
	else {
		throw exception(std::string("solution #" + id) + " not exist");
	}
}


solution::~solution()
{

}

std::string solution::get_cfg_json()
{
	assert(false); // @todo
	return "";
}

std::string solution::get_src_ext()
{
	assert(false); // @todo
	return ".cpp";
}

indexes solution::get_not_checked_ids()
{
	assert(false); // @todo
	indexes result;
	return result;
}

std::string solution::create_src(std::string name)
{
	assert(false); // @todo 
	return "";
}

std::string solution::create_cfg_json(std::string name)
{
	return "";
}

/**  
 *	struct checker 
 */
checker::checker(psolution ps, std::string src_name, std::string cfg_name) : sol(ps), src(src_name), cfg(cfg_name)
{
	src += sol->get_src_ext();
}

void checker::run()
{

	sol->create_src(src);
	sol->create_cfg_json(cfg);
	std::string cmd = "checker.exe " + cfg + " . " + src;
	system(cmd.c_str());
}
