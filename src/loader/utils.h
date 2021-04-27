#pragma once

#include <string>
#include <vector>

#define _HK_ENV_DELIMITER ':'

inline void hk_split_env(const std::string& str, std::vector<std::string>& data)
{
	auto start = 0U;
	auto end   = str.find(_HK_ENV_DELIMITER);
	while (end != std::string::npos) {
		data.push_back(str.substr(start, end - start));
		start = end + 1;
		end	  = str.find(_HK_ENV_DELIMITER, start);
	}

	if (end != std::string::npos)
		data.push_back(str.substr(start, end));
}