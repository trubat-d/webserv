#ifndef WEB_GIT_PARSER_HPP
#define WEB_GIT_PARSER_HPP
#include "Includer.hpp"



size_t smallest_pos(std::string buffer, std::string delimiter_list);
std::vector<std::string> split_del(std::string buffer, std::string delimiter_list);
std::string full_trim(std::string buffer, std::string delimiters);
bool is_valid_delimiter(char c, std::string delimiters);
bool is_only_wp(std::string str);
char *concatToCharset(const char* str1, std::string str2);
std::vector<std::string> *configLabelsGen();
std::vector<std::string> *configOptionsGen();

typedef std::pair<std::string, std::vector<std::string> > t_pair;

typedef struct s_node
{
	std::vector <struct s_node *> branches;
	std::vector<std::pair<std::string, std::vector<std::string> > *> config;
	std::string title;
	std::string scope;
} t_node;

class Parser
{
private:
	t_node *tree;
	std::vector<std::string> *config_options;
	std::vector<std::string> *config_labels;
	const std::string whitespaces;
	Parser(const Parser & ref);
	Parser & operator=(const Parser & ref);
public:
	Parser();
	~Parser();
	t_node *tree_config(std::vector<std::string> &buffer);
	int analyzer(t_node *head, std::vector<std::string> &buffer);
	char getLastChar(std::string str);
	char removeLastChar(std::string &line);
	void deleteParsingTable(t_node *head);
	void print_config(t_node &head, int k);
	void configPrinter();
	bool countChar(std::vector<std::string> buffer);
};

#endif