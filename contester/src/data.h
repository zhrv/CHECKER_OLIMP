#ifndef _DATA_H_
#define _DATA_H_

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <ctime>
#include "mysql.h"
#include <assert.h>


typedef std::string stdstr;
typedef std::vector<int> indexes;

struct db
{
	typedef std::vector<stdstr> row;
	typedef std::vector<row> rows;

	static MYSQL *conn;
	static MYSQL mysql;



	static void init(stdstr host, stdstr user, stdstr passwd, stdstr dbname, int port);
	static void connect();
	static rows q(stdstr sql);
	static void exec(stdstr sql);
	static void close() { mysql_close(db::conn); }

	static long long stoi(stdstr str);

	static stdstr host;
	static stdstr user;
	static stdstr passwd;
	static stdstr dbname;
	static int port;

};


struct exception
{
	exception(stdstr msg) : message(msg) {}
	stdstr get_message() { return message; }
	stdstr message;
};


struct lang
{
	lang(int id);

	int id;
	stdstr name, compiler, extension, identifier;
};
typedef std::auto_ptr<lang> plang;


struct test
{
	static const int RESULT_OK		= 0;
	static const int RESULT_TLE		= 1;
	static const int RESULT_MLE		= 2;
	static const int RESULT_CRASH	= 3;
	static const int RESULT_BAD		= 4;

	test() {}
	test(int id);
	static indexes get_ids(int solution_id);

	// db fields
	int id, sid, num, res, cid;

};
typedef std::auto_ptr<test> ptest;
typedef std::vector<test> tests;


struct ch_test
{
	ch_test(int id);

	static indexes get_ids(int group_id);

	// db fields
	int id, gid, scores;
	stdstr input, output;
};
typedef std::vector<ch_test> ch_tests;


struct ch_group
{
	ch_group(int id);
	static indexes get_ids(int task_id);

	ch_tests chts;

	// db fields
	int id, tid, method, scores;
	stdstr name;
};
typedef std::vector<ch_group> ch_groups;


struct task
{
	task(int id);
	//~task(){}

	ch_groups chgs;

	// db fields
	int id, cid;
	unsigned long long time_limit, memory_limit;
	stdstr title, content, input, output, checker;
};
typedef std::auto_ptr<task> ptask;


struct solution
{
	solution(int id);
	//~solution();

	stdstr create_src(stdstr name);
	void delete_src(stdstr name);

	stdstr get_cfg_json();
	stdstr create_cfg_json(stdstr name);
	void delete_cfg_json(stdstr name);
	stdstr get_src_ext();
	stdstr get_tests_dir();

	void save_result_to_db();

	static indexes get_not_checked_ids();


	ptask pt;
	plang pl;
	tests tsts;

	// db fields
	int id, uid, tid, lid, created_at, status, score;
	stdstr code, file, hash, result;

	static const int STATUS_NOT_CHECKED;
	static const int STATUS_CHECKED;
};
typedef std::auto_ptr<solution> psolution;


struct checker
{
	struct result
	{
		stdstr json;
	};

	struct clng
	{
		stdstr ext, cmd;
	};

	typedef std::map<stdstr, clng> clng_map;

	checker(psolution ps, stdstr src_name, stdstr cfg_name, stdstr res_name);
	void run();
	result get_result() { return res_json; }

	psolution sol;
	result res_json;
	stdstr src, cfg, res;

	static stdstr ch_bin;
	static stdstr tests_dir;
	static clng_map clngs;
};
typedef std::auto_ptr<checker> pchecker;




#endif