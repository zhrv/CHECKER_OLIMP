#include "builder.h"

namespace checker {

//parse_compiler_config: распарсить конфиги указанного компилятора.
//						 на текущий момент сделано только для  MSVS2012 C++.
//todo: требует доработки.
/*
	compiler_config:
					$1 -- путь до исходника.
					$2 -- путь до исполняемого файла.
*/
void parse_compiler_config(const std::string& profile_path, const std::string& source_name,
						   const std::string& compiler_name, const std::string& compiler_config,
						   std::string& compiler_command
						   )
{
	if (compiler_name == msv_cpp) {
		const std::string d1_str = "$1";
		const std::string d2_str = "$2";
		const std::string exe_str = ".exe";

		compiler_command = compiler_config;

		auto it = compiler_command.find(d1_str);
		if (it == std::string::npos)	assert(false);
		compiler_command.replace(it, d1_str.length(), restore_path(profile_path, source_name));

		it = compiler_command.find(d2_str);
		if (it == std::string::npos)	assert(false);
		compiler_command.replace(it, d1_str.length(), restore_path(profile_path, source_name + exe_str));
	} 
	if (compiler_name == fp_pas || compiler_name == abc_pas) {
		const std::string d1_str = "$1";

		compiler_command = compiler_config;
		
		auto it = compiler_command.find(d1_str);
		if (it == std::string::npos)	assert(false);
		compiler_command.replace(it, d1_str.length(), restore_path(profile_path, source_name));
	}
}

//build_from_source: скопмилировать файл.
//					 на текущий момент сделано только для  MSVS2012 C++.
bool build_from_source(const std::string& profile_path, const std::string& source_name, const std::string& compiler_name, const std::string& compiler_config, std::string& executable_file_path)
{
	if (compiler_name == msv_cpp) {
		const std::string	out_str = "/out:";
		const std::string   fatal_error_str = "fatal error";
		std::string			compiler_command;
		FILE				*pipe;
		const int			buffer_size = 512;
		char				buffer[buffer_size];
		std::string			buffer_str;

		parse_compiler_config(profile_path, source_name, compiler_name, compiler_config, compiler_command);

		pipe = _popen(compiler_command.c_str(), "rt" );
		if(pipe == nullptr)						return false;
	
		if (!buffer_str.empty())	buffer_str.clear();
		while (fgets(buffer, buffer_size, pipe) != nullptr) {
			buffer_str += buffer;
		}

		if (feof(pipe))	_pclose(pipe);
		else			return false; // assert(false);

		auto it = buffer_str.find(fatal_error_str);
		if (it != std::string::npos)			return false;
		it = buffer_str.find(out_str);
		if (it == std::string::npos)			return false;

		it += out_str.length();
		if (it < buffer_str.length()) {
			if (!executable_file_path.empty())	executable_file_path.clear();

			while (it < buffer_str.length() && buffer_str[it] != '\n') {
				executable_file_path += buffer_str[it];
				++it;
			}
		}

		return true;
	}
	else if (compiler_name == fp_pas) {
		const std::string	fatal_str = "Fatal:";
		const std::string	exe_str = ".exe";
		const std::string	pas_str = ".pas";
		FILE				*pipe;
		std::string			compiler_command;
		const int			buffer_size = 512;
		char				buffer[buffer_size];
		std::string			buffer_str;

		parse_compiler_config(profile_path, source_name, compiler_name, compiler_config, compiler_command);

		pipe = _popen(compiler_command.c_str(), "rt" );
		if(pipe == nullptr)						return false;

		if (!buffer_str.empty())				buffer_str.clear();
		while (fgets(buffer, buffer_size, pipe) != nullptr) {
			buffer_str += buffer;
		}
		
		if (buffer_str.empty()) {
			assert(false);
			return false;
		}

		if (feof(pipe))							_pclose(pipe);
		else									return false; // assert(false);

		auto it = buffer_str.find(fatal_str);
		if (it != std::string::npos)			return false;
		
		buffer_str = source_name;
		if (buffer_str.length() < 5
			&&	(	buffer_str[buffer_str.length() - 1] != 's' 
				||	buffer_str[buffer_str.length() - 2] != 'a'
				||	buffer_str[buffer_str.length() - 3] != 'p'
				||	buffer_str[buffer_str.length() - 4] != '.'
				)
			)									return false;

		buffer_str.resize(buffer_str.length() - pas_str.length());
		executable_file_path = restore_path(profile_path, buffer_str + exe_str);

		return true;
	}
	else if (compiler_name == abc_pas) {
		return true;
	}

	assert(false);
	return false;
}

} // namespace checker