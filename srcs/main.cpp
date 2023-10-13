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
		if(storage.empty())
		{
			std::cerr << "Port Error" << std::endl;
			return 1;
		}
		Socket webServerSocket(Utils::vecStoI(storage), &config_parser);
		Utils::fillMime();
		webServerSocket.run();
	} catch (std::exception &err)
	{
		std::cout << err.what() << std::endl;
		return 1;
	}

	return 0;
}
