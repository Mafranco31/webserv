#include "../inc/header.hpp"
#include "../inc/Server.hpp"


// Drop comments to use configuration file
int main(int argc, char **argv, char **env) {
    (void)argc;
    (void)argv;
    (void)env;
    /*if (argc != 2) {
        argv[1] = (char *)"../web_parse/linux.conf";
    }*/

	try
	{
		Sender sender(std::string("www"), std::string("errwww"));
		Webserv server(sender, env);
		server.parse(std::string("linux.conf"));
		server.data_structure();
		//server.check();
		server.ep = kqueue();
		if (server.ep == -1)
			throw Webserv::ErrorCreatingKqueue();
		for (int i = 0; i < static_cast<int>(server._port.size()); i++)
			server.sub_server.push_back(Server(server.env, server.sender, server._host[i], server._port[i], server.ep, server.nev));
		for (std::vector<Server>::iterator it = server.sub_server.begin(); it != server.sub_server.end(); it++)
			(*it).Start();
		while (true) {
			server.Wait();
			for (std::vector<Server>::iterator it = server.sub_server.begin(); it != server.sub_server.end(); it++)
			{
				//std::cout << (*it)._port << std::endl;
				(*it).ManageConnexion(server.events);
			}
		}
		server.clean();
		for (std::vector<Server>::iterator it = server.sub_server.begin(); it != server.sub_server.end(); it++)
			(*it).Stop();
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
