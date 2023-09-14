#ifndef WEB_GIT_PARSER_HPP
#define WEB_GIT_PARSER_HPP

#include "Includer.hpp"



size_t smallest_pos(std::string buffer, std::string delimiter_list);
bool is_valid_delimiter(char c, std::string delimiters);
bool is_only_wp(std::string str);
std::vector<std::string> *configLabelsGen();
std::vector<std::string> *configOptionsGen();

typedef std::map<std::string, std::vector<std::string> > t_conf_map;

typedef struct s_node
{
	std::vector <struct s_node *> branches;
	std::map<std::string, std::vector<std::string> > config;
	std::string title;
	std::string scope;
} t_node;

class Parser
{
public:
	Parser();
	~Parser();
	t_node *tree_config(std::vector<std::string> &buffer);
	int analyzer(t_node *head, std::vector<std::string> &buffer);
	char getLastChar(std::string str);
	char removeLastChar(std::string &line);
	void deleteParsingTable(t_node *head);
	//void print_config(t_node &head, int k);
	//void configPrinter();
	bool countChar(std::vector<std::string> buffer);
	bool in_config(std::map<std::string, std::vector<std::string> > &temp, std::string key, std::string value) const;
	t_node *getLocationNode(t_node *head, const std::string &path, t_conf_map & ret) const;
	void join_map(t_conf_map & map1, t_conf_map & map2) const;
	void print_config(t_conf_map &maper) const;
	t_conf_map getServerConfig(std::string const & name, std::string const & port, std::string const & path) const;
	t_node *getServerNode(t_node *head, const std::string &name, const std::string &port, t_conf_map &ret) const;
private:
	std::vector<std::string> *config_options;
	t_node *tree;
	std::vector<std::string> *config_labels;
	const std::string whitespaces;
	Parser(const Parser & ref);
	Parser & operator=(const Parser & ref);
};

#endif