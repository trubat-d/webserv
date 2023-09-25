#include "Parser.hpp"

Parser::~Parser()
{
	deleteParsingTable(this->tree);
	delete this->config_labels;
	delete this->config_options;
}

Parser::Parser(std::string file_name) : whitespaces(" \t\n\v\r\f")
{
	this->config_labels = configLabelsGen();
	this->config_options = configOptionsGen();
	std::ifstream file;
	file.open("./conf/" + std::string(file_name), std::ifstream::in);
	if (file.fail())
	{
		std::string error = "File not Found !";
		throw Error::ConfigParsingException(error);
	}
	std::vector<std::string> buffer;
	std::string temp;
	while (std::getline(file, temp))
	{
		if (temp.empty() || is_only_wp(temp))
			continue;
		buffer.push_back(temp);
	}
	this->tree = tree_config(buffer);
	if(!method_error_checker(*this->tree))
	{
		std::string error = "Config Method is Allowed and Denied at the same time";
		throw Error::ConfigParsingException(error);
	}
}

Parser::Parser(const Parser &ref)
{
	*this = ref;
}

Parser &Parser::operator=(const Parser &ref)
{
	this->tree = ref.tree;
	this->config_options = ref.config_options;
	this->config_labels = ref.config_labels;
	return *this;
}

t_node *Parser::tree_config(std::vector<std::string> &buffer)
{
	if(!countChar(buffer))
	{
		std::string err("} OR {");
		throw Error::ConfigParsingException(err);
	}
	this->tree = new t_node();
	this->tree->title = "base";
	analyzer(this->tree, buffer);
	return this->tree;
}

int Parser::analyzer(t_node *head, std::vector<std::string> &buffer)
{
	if(buffer.empty())
		return 0;
	std::string current_line = buffer[0];
	char identifier;
	do
	{
		current_line = buffer[0];
		std::string first_word;
		std::stringstream line(current_line);
		line >> first_word;
		identifier = getLastChar(current_line);
		if (identifier == '{')
		{
			t_node *newNode;
			try
			{
				newNode = new t_node();
			} catch (std::bad_alloc &)
			{
				throw Error::MemoryAllocationException();
			}
			newNode->title = first_word;
			if (std::find(this->config_labels->begin(), this->config_labels->end(), first_word) ==
				this->config_labels->end())
				throw Error::ConfigParsingException(first_word);
			if (first_word == "location")
			{
				for (int i = 0; line >> first_word; i++)
				{
					if (i != 0)
						newNode->scope += " ";
					newNode->scope += first_word;
				}
				removeLastChar(newNode->scope);
				newNode->scope.pop_back();
			}
			head->branches.push_back(newNode);
			buffer.erase(buffer.begin());
			if (analyzer(newNode, buffer) != 0)
			{
				throw Error::ConfigParsingException(head->title);
			}
		}
		else if (identifier == '}')
		{
			buffer.erase(buffer.begin());
			return 0;
		}
		else if (identifier == ';')
		{
			removeLastChar(current_line);
			std::stringstream line_tmp(current_line);
			std::string word, key;
			line_tmp >> key;
			if (std::find(this->config_options->begin(), this->config_options->end(), key) == this->config_options->end())
				throw Error::ConfigParsingException(word);
			while (line_tmp >> word)
			{
				head->config[key].push_back(word);
			}
			buffer.erase(buffer.begin());
		} else
		{
			throw Error::ConfigParsingException(first_word);
		}
	}
	while(identifier == ';' || (!buffer.empty() && identifier == '{'));
	return -1;
}


bool Parser::countChar(std::vector<std::string> buffer)
{
	long int line_count = 0;
	long int count = 0;
	for (size_t i = 0; i < buffer.size(); i++)
	{
		for(size_t j = 0; j < buffer[i].length(); j++)
		{
			if(buffer[i][j] == '{')
			{
				count++;
				line_count++;
			}
			else if (buffer[i][j] == '}')
			{
				count--;
				line_count++;
			}
		}
		if(line_count > 1)
			return false;
		else
			line_count = 0;
	}
	return (count == 0);
}

char Parser::getLastChar(std::string line)
{
	int i = line.size()-1;
	while(i >= 0 && this->whitespaces.find(line[i]) != std::string::npos)
	{
		i--;
	}
	if(i>=0)
		return line[i];
	return 0;
}

char Parser::removeLastChar(std::string &line)
{
	int i = line.size()-1;
	while(i >= 0 && this->whitespaces.find(line[i]) != std::string::npos)
	{
		i--;
	}
	if(i>=0)
		line.erase(i);
	return 0;
}

void Parser::deleteParsingTable(t_node *head)
{
	if(!head)
		return ;
//	for (size_t i = 0; i < head->config.size(); i++)
//	{
//		delete head->config[i];
//	}

	if(!head->branches.empty())
	{
		for(size_t i = 0; i < head->branches.size(); i++)
		{
			deleteParsingTable(head->branches[i]);
		}
	}
	delete head;
}

std::vector<std::string> Parser::getEvent(std::string event)
{
    std::vector<std::string> ret;
    t_node *main;
    if(this->tree->branches.empty())
        return (ret);
    for(size_t i = 0; i < this->tree->branches.size(); i++)
    {
        if(this->tree->branches[i]->title == "events")
        {
            main = this->tree->branches[i];
        }
    }
    if(main->config.empty())
        return ret;
    for(size_t i = 0; i < main->config.size(); i++)
    {
        if(main->config.find(event) != main->config.end())
        {
            return main->config.at(event);
        }
    }
    return ret;
}


//void Parser::configPrinter()
//{
//	print_config(*this->tree, 0);
//}
//
//
//
//void Parser::print_config(t_node &head, int k)
//{
//	(void)head;
//	(void)k;
//	for (size_t i = 0; i < head.branches.size(); i++)
//	{
//		int temp = k-1;
//		while(temp>=0){std::cout << "\t";temp--;}
//		temp = k;
//		std::cout << head.branches[i]->title << " context : ["<< head.branches[i]->scope << "]" << std::endl;
//		while(temp>=0){std::cout << "\t";temp--;}
//		std::cout << "config :" << std::endl;
//		for (size_t n = 0; n < head.branches[i]->config.size(); n++)
//		{
//			temp = k+1;
//			while(temp>=0){std::cout << "\t";temp--;}
//			std::cout << "-> " <<  head.branches[i]->config[n]->first << " ";
//			for (size_t j = 0; j < head.branches[i]->config[n]->second.size(); j++)
//			{
//				std::cout << head.branches[i]->config[n]->second[j] << " ";
//			}
//			std::cout << std::endl;
//		}
//		print_config(*head.branches[i], k+1);
//	}
//}
