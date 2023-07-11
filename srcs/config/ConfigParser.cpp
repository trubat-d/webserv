#include "Includer.hpp"

int main(int ac, char ** av)
{
	try
	{
		if (ac == 2)
		{
			Parser config_parser;
			std::ifstream file;
			file.open("./conf/" + std::string(av[1]), std::ifstream::in);
			if (file.fail())
				return 1;
			std::vector<std::string> buffer;
			std::string temp;
			while (std::getline(file, temp))
			{
				if (temp.empty() || is_only_wp(temp))
					continue;
				buffer.push_back(temp);
			}
			config_parser.tree_config(buffer);
			config_parser.configPrinter();
			file.close();
			if (file.fail())
				return 1;
		}
	} catch (Error::ConfigParsingException & e)
	{
		std::cerr << e.what() << std::endl;
	}
	return 0;
}

std::vector<std::string> *configOptionsGen()
{
	std::vector<std::string> *config_options = new std::vector<std::string>();
	config_options->push_back("worker_connections");
	config_options->push_back("listen");
	config_options->push_back("server_name");
	config_options->push_back("error_page");
	config_options->push_back("client_max_body_size");
	config_options->push_back("root");
	config_options->push_back("auto_index");
	config_options->push_back("upload_store");
	config_options->push_back("allow");
	config_options->push_back("deny");
	config_options->push_back("index");
	config_options->push_back("fastcgi_pass");
	config_options->push_back("include");
	config_options->push_back("fastcgi_param");
	config_options->push_back("return");
	config_options->push_back("alias");
	config_options->push_back("autoindex");
	return config_options;
}

std::vector<std::string> *configLabelsGen()
{
	std::vector<std::string> *config_labels = new std::vector<std::string>();
	config_labels->push_back("events");
	config_labels->push_back("http");
	config_labels->push_back("server");
	config_labels->push_back("location");
	return config_labels;
}

std::string full_trim(std::string buffer, std::string delimiters)
{
	for(size_t i = 0; i < buffer.size() && is_valid_delimiter(buffer[i], delimiters); i++)
	{
		buffer.erase(buffer.begin());
		i--;
	}
	for(int i = buffer.size()-1; i >= 0 && is_valid_delimiter(buffer[i], delimiters); i--)
	{
		buffer.erase(i);
	}
	return buffer;
}

bool is_valid_delimiter(char c, std::string delimiters)
{
	for (size_t i = 0; i < delimiters.size(); i++)
	{
		if(c == delimiters[i])
			return true;
	}
	return false;
}

std::vector<std::string> split_del(std::string buffer, std::string delimiter_list)
{
	std::vector<std::string> tab;
	size_t pos = 0;
	std::string token;
	while (pos != std::string::npos)
	{
		pos = smallest_pos(buffer, delimiter_list);
		token = buffer.substr(0, pos);
		if(!token.empty())
			tab.push_back(token);
		buffer.erase(0, pos + 1);
	}
	return tab;
}

size_t smallest_pos(std::string buffer, std::string delimiter_list)
{
	size_t temp;
	size_t pos = buffer.find(delimiter_list[0]);;
	for (size_t i = 0; i < delimiter_list.size(); i++)
	{
		temp = buffer.find(delimiter_list[i]);
		if(temp == std::string::npos)
			continue;
		if(temp < pos)
			pos = temp;
	}
	return pos;
}

bool is_only_wp(std::string str)
{
	for(size_t i = 0; i < str.length(); i++)
	{
		if(std::string(" \t\n\v\r\f").find(str[i]) == std::string::npos)
		{
			return false;
		}
	}
	return true;
}