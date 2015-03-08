#include "data.h"
#include "writer.h"
#include "stringbuffer.h"




/****************************
*	struct db
*****************************/

MYSQL *db::conn = 0;
MYSQL  db::mysql;
stdstr db::host;
stdstr db::user;
stdstr db::passwd;
stdstr db::dbname;
int db::port;

void db::init(stdstr host, stdstr user, stdstr passwd, stdstr dbname, int port)
{
	db::host = host;
	db::user = user;
	db::passwd = passwd;
	db::dbname = dbname;
	db::port = port;
}

void db::connect()
{
	mysql_init(&db::mysql);
	db::conn = mysql_real_connect(&db::mysql, db::host.c_str(), db::user.c_str(), db::passwd.c_str(), db::dbname.c_str(), db::port, 0, 0);
	if (db::conn == NULL) {
		throw exception(mysql_error(&db::mysql));
	}
}


db::rows db::q(stdstr sql)
{
	MYSQL_RES *result;
	MYSQL_ROW myrow;

	int query_state;	query_state = mysql_query(db::conn, sql.c_str());
	if (query_state != 0) {
		throw exception(mysql_error(db::conn));
	}

	db::rows rs;
	result = mysql_store_result(db::conn);
	unsigned int fld_count = mysql_num_fields(result);
	while ((myrow = mysql_fetch_row(result)) != NULL) {
		db::row r;
		for (unsigned int i = 0; i < fld_count; i++) {
			stdstr fld = "";
			if (myrow[i] != NULL) {
				fld = stdstr(myrow[i]);
			}
			r.push_back(fld);
		}
		rs.push_back(r);
	}
	mysql_free_result(result);

	return rs;
}

void db::exec(stdstr sql)
{
	int query_state;	query_state = mysql_query(db::conn, sql.c_str());
	if (query_state != 0) {
		throw exception(mysql_error(db::conn));
	}

}

long long db::stoi(stdstr str)
{
	if (str == "") return 0;
	return std::stoll(str);
}

/****************************
*	struct solution
*****************************/


const int solution::STATUS_NOT_CHECKED = 0;
const int solution::STATUS_CHECKED = 1;


solution::solution(int id)
{
	db::rows r = db::q("SELECT id, uid, tid, lid, created_at, status, score, code, file, hash, result FROM solutions WHERE id = " + std::to_string(id));
	if (r.size() > 0) {
		this->id = db::stoi(r[0][0]);
		uid = db::stoi(r[0][1]);
		tid = db::stoi(r[0][2]);
		lid = db::stoi(r[0][3]);
		created_at = db::stoi(r[0][4]);
		status = db::stoi(r[0][5]);
		score = db::stoi(r[0][6]);
		code = stdstr(r[0][7]);
		file = stdstr(r[0][8]);
		hash = stdstr(r[0][9]);
		result = stdstr(r[0][10]);
		pt = ptask(new task(tid));
		pl = plang(new lang(lid));
		indexes tis = test::get_ids(id);
		for (indexes::iterator i = tis.begin(); i != tis.end(); i++) {
			tsts.push_back(test(*i));
		}
	}
	else {
		throw exception("solution #" + std::to_string(id) + " not exist");
	}
}



indexes solution::get_not_checked_ids()
{
	indexes result;
	stdstr sql = "select id from solutions where status = " + std::to_string(solution::STATUS_NOT_CHECKED) + " limit 10";
	db::rows r = db::q(sql);
	for (db::rows::iterator i = r.begin(); i != r.end(); i++) {
		result.push_back(db::stoi((*i)[0]));
	}
	return result;
}

stdstr solution::get_cfg_json()
{
	assert(false); // @todo
	return "";
}

stdstr solution::get_src_ext()
{
	return checker::clngs[pl->identifier].ext;
}

stdstr solution::create_src(stdstr name)
{
	std::ofstream f(name, std::ifstream::binary);
	const char* c_str = code.c_str();
	f.write(c_str, strlen(c_str));
	f.close();
	return name;
}

stdstr solution::create_cfg_json(stdstr name)
{
	rapidjson::StringBuffer s;
	rapidjson::Writer<rapidjson::StringBuffer> writer(s);
	writer.StartObject();
	{
		writer.String("problem");
		writer.StartObject();
		{
			writer.String("name");
			writer.String(pt->title.c_str());
			writer.String("config");
			writer.StartObject();
			{
				writer.String("time_limit");
				writer.Uint64(pt->time_limit);
				writer.String("memory_limit");
				writer.Uint64(pt->memory_limit);
				writer.String("input");
				writer.String(pt->input.c_str());
				writer.String("output");
				writer.String(pt->output.c_str());
				writer.String("test_dir");
				writer.String(get_tests_dir().c_str());
				writer.String("checker");
				writer.String(pt->checker.c_str());
				writer.String("tests");
				writer.StartArray();
				for (auto itg = pt->chgs.begin(); itg != pt->chgs.end(); ++itg) {
					for (auto itc = itg->chts.begin(); itc != itg->chts.end(); ++itc) {
						writer.StartObject();
						writer.String("id");
						writer.Uint64(itc->id);
						writer.String("scores");
						writer.Int(itc->scores);
						writer.String("input");
						writer.String(itc->input.c_str());
						writer.String("outputs");
						writer.StartArray();
						writer.String(itc->output.c_str());
						writer.EndArray();
						writer.EndObject();
					}
				}
				writer.EndArray();
			}
			writer.EndObject();
		}
		writer.EndObject();
		writer.String("language");
		writer.StartObject();
		{
			writer.String("name");
			writer.String(pl->identifier.c_str());
			writer.String("template");
			writer.String(checker::clngs[pl->identifier].cmd.c_str());

		}
		writer.EndObject();
	}
	writer.EndObject();

	const char* c_str = s.GetString();
	std::ofstream f(name, std::ifstream::binary);
	f.write(c_str, strlen(c_str));
	f.close();
	
	return name;
}

stdstr solution::get_tests_dir() 
{
	stdstr res = checker::tests_dir + std::to_string(pt->cid) + "\\" + std::to_string(tid);
	return res;
}

void solution::save_result_to_db()
{
	stdstr sql = "UPDATE solutions SET result = '" + result + "', status = 1 WHERE id = " + std::to_string(id);
	db::exec(sql);
}



/****************************
*	struct task
*****************************/


task::task(int id)
{
	db::rows r = db::q("SELECT id, cid, time_limit, memory_limit, title, content, input, output, checker FROM tasks WHERE id = " + std::to_string(id));
	if (r.size() > 0) {
		this->id = db::stoi(r[0][0]);
		cid = db::stoi(r[0][1]);
		time_limit = db::stoi(r[0][2]);
		memory_limit = db::stoi(r[0][3]);
		title = stdstr(r[0][4]);
		content = stdstr(r[0][5]);
		input = stdstr(r[0][6]);
		output = stdstr(r[0][7]);
		checker = stdstr(r[0][8]);
		indexes inds = ch_group::get_ids(id);
		for (indexes::iterator i = inds.begin(); i != inds.end(); i++) {
			chgs.push_back(ch_group(*i));
		}

		//task = new task(tid);
	}
	else {
		throw exception("task #" + std::to_string(id) + " not exist");
	}
}






/****************************
*	struct ch_group
*****************************/

ch_group::ch_group(int id)
{
	//int id, tid, method, scores;
	//stdstr name;
	db::rows r = db::q("SELECT id, tid, method, scores, name FROM checkergroups WHERE id = " + std::to_string(id));
	if (r.size() > 0) {
		this->id = db::stoi(r[0][0]);
		tid = db::stoi(r[0][1]);
		method = db::stoi(r[0][2]);
		scores = db::stoi(r[0][3]);
		name = stdstr(r[0][4]);
		indexes inds = ch_test::get_ids(id);
		for (indexes::iterator i = inds.begin(); i != inds.end(); i++) {
			chts.push_back(ch_test(*i));
		}
	}
	else {
		throw exception("checker group #" + std::to_string(id) + " not exist");
	}
}

indexes ch_group::get_ids(int sid)
{
	indexes result;
	stdstr sql = "select id from checkergroups where tid = " + std::to_string(sid);
	db::rows r = db::q(sql);
	for (db::rows::iterator i = r.begin(); i != r.end(); i++) {
		result.push_back(db::stoi((*i)[0]));
	}
	return result;

}


/****************************
*	struct ch_test
*****************************/

ch_test::ch_test(int id)
{
	//int id, gid, scores;
	//stdstr input, output;
	db::rows r = db::q("SELECT id, gid, scores, input, output FROM checkertests WHERE id = " + std::to_string(id));
	if (r.size() > 0) {
		this->id = db::stoi(r[0][0]);
		gid = db::stoi(r[0][1]);
		scores = db::stoi(r[0][2]);
		input = stdstr(r[0][3]);
		output = stdstr(r[0][4]);
	}
	else {
		throw exception("checker group #" + std::to_string(id) + " not exist");
	}
}

indexes ch_test::get_ids(int gid)
{
	indexes result;
	stdstr sql = "select id from checkertests where gid = " + std::to_string(gid);
	db::rows r = db::q(sql);
	for (db::rows::iterator i = r.begin(); i != r.end(); i++) {
		result.push_back(db::stoi((*i)[0]));
	}
	return result;
}



/****************************
*	struct lang
*****************************/


lang::lang(int id)
{
	//int id;
	//stdstr name, compiler, extension, identifier;
	db::rows r = db::q("SELECT id, name, compiler, extension, identifier FROM langs WHERE id = " + std::to_string(id));
	if (r.size() > 0) {
		this->id = db::stoi(r[0][0]);
		name = stdstr(r[0][1]);
		compiler = stdstr(r[0][2]);
		extension = stdstr(r[0][3]);
		identifier = stdstr(r[0][4]);
	}
	else {
		throw exception("lang #" + std::to_string(id) + " not exist");
	}
}



/****************************
*	struct test
*****************************/


test::test(int id)
{
	db::rows r = db::q("SELECT id, sid, num, res, cid FROM tests WHERE id = " + std::to_string(id));
	if (r.size() > 0) {
		this->id = db::stoi(r[0][0]);
		sid = db::stoi(r[0][1]);
		num = db::stoi(r[0][2]);
		res = db::stoi(r[0][3]);
		cid = db::stoi(r[0][4]);
	}
	else {
		throw exception("test #" + std::to_string(id) + " not exist");
	}
}

indexes test::get_ids(int sid)
{
	indexes result;
	stdstr sql = "select id from tests where sid = " + std::to_string(sid);
	db::rows r = db::q(sql);
	for (db::rows::iterator i = r.begin(); i != r.end(); i++) {
		result.push_back(db::stoi((*i)[0]));
	}
	return result;
}



/****************************
*	struct checker
*****************************/

stdstr checker::ch_bin;
stdstr checker::tests_dir;
checker::clng_map checker::clngs;

checker::checker(psolution ps, stdstr src_name, stdstr cfg_name, stdstr res_name) : sol(ps), src(src_name), cfg(cfg_name), res(res_name)
{
	src += sol->get_src_ext();
}

void checker::run()
{

	stdstr src_name = sol->create_src(src);
	stdstr cfg_name = sol->create_cfg_json(cfg);
	stdstr cmd = checker::ch_bin + " " + cfg + " . " + src + " " + res;
	system(cmd.c_str());
	remove(src_name.c_str());
	remove(cfg_name.c_str());
	std::ifstream f(res);
	sol->result.clear();
	char ch;
	while (f.get(ch)) {
		sol->result += ch;
	}
	f.close();
	remove(res.c_str());
	sol->save_result_to_db();
}
