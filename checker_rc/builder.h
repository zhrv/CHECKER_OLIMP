#ifndef _BUILDER_
#define _BUILDER_

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <string>
#include "reader.h"

namespace checker {

//parse_compiler_config: ���������� ������� ���������� �����������.
//						 �� ������� ������ ������� ������ ���  MSVS2012 C++.
//todo: ������� ���������.
/*
	compiler_config:
					$1 -- ���� �� ���������.
					$2 -- ���� �� ������������ �����.
*/
void parse_compiler_config(const std::string& profile_path, const std::string& source_name,
						   const std::string& compiler_name, const std::string& compiler_config,
						   std::string& compiler_command);

//build_from_source: �������������� ����.
//					 �� ������� ������ ������� ������ ���  MSVS2012 C++ � FPC.
bool build_from_source(const std::string& profile_path, const std::string& source_name,
					   const std::string& compiler_name, const std::string& compiler_config,
					   std::string& executable_file_path);

} // namespace checker

#endif // _BUILDER_