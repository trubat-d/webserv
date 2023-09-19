#include "Includer.hpp"

int main(int ac, char **av)
{
	if(ac != 2)
		return 1;
	try
	{
		Parser config_parser(av[1]);
		std::vector<std::string> storage;
		config_parser.getPorts(storage, config_parser.getHead());
		std::vector<int> int_storage = Utils::vecStoI(storage);
		Socket webServerSocket(int_storage, &config_parser);
		webServerSocket.run();
	} catch (std::exception &err)
	{
		std::cout << err.what() << std::endl;
		return 1;
	}

	return 0;
}
