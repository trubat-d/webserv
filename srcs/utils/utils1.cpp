#include "Utils.hpp"

char *Utils::concatToCharset(std::string &str1, std::string &str2)
{
	str1 += str2;
	return const_cast<char *>(str1.c_str());
}

std::vector<int> Utils::vecStoI(std::vector<std::string> input)
{
	std::vector<int> result;
	for(size_t i = 0; i < input.size(); i++)
	{
		std::stringstream strm(input[i]);
		int x = 0;
		strm >> x;
		result.push_back(x);
	}
	return result;
}