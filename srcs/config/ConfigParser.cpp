#include "Includer.hpp"
#include "Parser.hpp"

//
//int main(int ac, char ** av)
//{
//	try
//	{
//		if (ac == 2)
//		{
//			Parser config_parser("default.conf");
//			std::ifstream file;
//			file.open("./conf/" + std::string(av[1]), std::ifstream::in);
//			if (file.fail())
//				return 1;
//			std::vector<std::string> buffer;
//			std::string temp;
//			while (std::getline(file, temp))
//			{
//				if (temp.empty() || is_only_wp(temp))
//					continue;
//				buffer.push_back(temp);
//			}
//			config_parser.tree_config(buffer);
////			config_parser.configPrinter();
//			std::string name = "example.com";
//			std::string port = "80";
//			std::string path = "/kapouet/trob";
//			std::map<std::string, std::vector<std::string> > serv_cfg = config_parser.getServerConfig(name, port, path);
//			config_parser.print_config(serv_cfg);
//			file.close();
//			if (file.fail())
//				return 1;
//		}
//	} catch (Error::ConfigParsingException & e)
//	{
//		std::cerr << e.what() << std::endl;
//	}
//	return 0;
//}

std::vector<std::string> *configOptionsGen()
{
	std::vector<std::string> *config_options = new std::vector<std::string>();
//	std::string test[] = {"worker_connections", "listen"};
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
	config_options->push_back("return");
	config_options->push_back("alias");
	config_options->push_back("autoindex");
	config_options->push_back("uri_max_size");
	config_options->push_back("cgi");
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

t_conf_map Parser::getServerConfig(std::string const & name , std::string const & port, std::string const & path) const
{
	t_conf_map ret;
	join_map(ret,this->tree->config);

	t_node *server = getServerNode(this->tree, name, port, ret);
	getLocationNode(server, path, ret);
	return ret;
}

t_node *Parser::getHead()
{
	return this->tree;
}

t_node *Parser::getServerNode(t_node *head, std::string const & name, std::string const & port, t_conf_map & ret) const
{
	t_node *server_node = NULL;
	if(!head)
		return NULL;
	else
	{
		for (size_t i = 0; i < head->branches.size(); i++)
		{
			if(head->branches[i]->title == "server")
			{
				if(in_config(head->branches[i]->config, "listen", port) \
				&& in_config(head->branches[i]->config, "server_name", name))
				{
					join_map(ret, head->branches[i]->config);
					return head->branches[i];
				}
			}
			else
			{
				server_node = getServerNode(head->branches[i], name, port, ret);
				if(server_node)
					join_map(ret, head->branches[i]->config);
			}
			if(server_node != NULL)
				return server_node;
		}
	}
	return NULL;
}

bool Parser::in_config(std::map<std::string, std::vector<std::string> > & temp,std::string key, std::string value) const
{
	return (std::find(temp[key].begin(), temp[key].end(), value) != temp[key].end());
}

t_node *Parser::getLocationNode(t_node *head, std::string const & path, t_conf_map & ret) const
{
	t_node *longest_path = NULL;
	if(!head)
		return head;
	else
	{
		for (size_t i = 0; i < head->branches.size(); i++)
		{
			if(head->branches[i]->title != "location")
				continue;
			size_t title_size = head->branches[i]->scope.length();
			if(!path.compare(0,title_size, head->branches[i]->scope))
			{
				if(!longest_path || title_size > longest_path->scope.length())
				{
					longest_path = head->branches[i];
				}
			}
		}
		if(longest_path == NULL)
		{
				return head;
		}
		else
		{
			if(longest_path->branches.empty())
			{
				join_map(ret, longest_path->config);
				return longest_path;
			}
			else
			{
				join_map(ret, longest_path->config);
				longest_path = getLocationNode(longest_path, path, ret);
				return longest_path;
			}
		}
	}
}

void Parser::join_map(t_conf_map & map1, t_conf_map & map2) const
{
	for (t_conf_map::iterator it = map2.begin(); it != map2.end(); it++)
	{
		if(map1.find(it->first) == map1.end())
		{
            if(it->first == "allow")
            {
                if(map1.find("deny") != map1.end())
                {
                    for(size_t i = 0; i < it->second.size(); i++)
                    {
                        if(std::find(map1.at("deny").begin(), map1.at("deny").end(),it->second[i]) != map1.at("deny").end())
                        {
                            std::cout << "removed :" << it->second[i] << "from deny" << std::endl;
                            std::remove(map1.at("deny").begin(), map1.at("deny").end(),it->second[i]);
                        }
                    }
                }
		    }
            else if (it->first == "deny")
            {
                if(map1.find("allow") != map1.end())
                {
                    for(size_t i = 0; i < it->second.size(); i++)
                    {
                        if(std::find(map1.at("allow").begin(), map1.at("allow").end(),it->second[i]) != map1.at("allow").end())
                        {
                            std::cout << "removed :" << it->second[i] << "from allow" << std::endl;
                            std::remove(map1.at("allow").begin(), map1.at("allow").end(),it->second[i]);
                        }
                    }
                }
            }
			map1[it->first] = it->second;
		}
		else
		{
			map1.at(it->first) = map2.at(it->first);
		}
	}
}

void Parser::getPorts(std::vector<std::string> &storage, t_node* last_loc)
{
	if(last_loc->branches.empty())
		return ;
    if(last_loc->config.find("listen") != last_loc->config.end())
    {
        for(size_t i = 0; i < last_loc->config.at("listen").size(); i++)
        {
            if(std::find(storage.begin(), storage.end(), last_loc->config.at("listen")[i]) == storage.end())
            {
                storage.push_back(last_loc->config.at("listen")[i]);
            }
			else
			{
				storage.clear();
				return ;
			}
        }
    }
	for (size_t i = 0; i < last_loc->branches.size(); i++)
	{
		getPorts(storage, last_loc->branches[i]);
	}
}

void Parser::print_config(t_conf_map & maper) const
{
	std::cout << " PRINTING CONFIG 1"<< std::endl;
	for(t_conf_map::iterator it = maper.begin(); it != maper.end(); it++)
	{
		std::cout << "key : [" << it->first;
		std::cout << "] Values : [";
		for (size_t i = 0 ; i < it->second.size(); i++)
		{
			std::cout << it->second[i] << " ";
		}
		std::cout << "]" << std::endl;
	}
	std::cout << " END OF PRINTING CONFIG "<< std::endl;
}

bool Parser::method_error_checker(t_node &head) const
{
	bool val = true;
	if(head.config.find("allow") != head.config.end() && head.config.find("deny") != head.config.end())
	{
		for(size_t i = 0; i < head.config.at("allow").size(); i++)
		{
			if(std::find(head.config.at("deny").begin(), head.config.at("deny").end(),head.config.at("allow")[i]) != head.config.at("deny").end())
			{
				val = false;
				break;
			}
		}
	}
	else
	{
		for(size_t i = 0; i < head.branches.size(); i++)
		{
//			std::cout << head.branches[i]->title << std::endl;
			val = val && method_error_checker(*head.branches[i]);
		}
	}
	return val;
}








