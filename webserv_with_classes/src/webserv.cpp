#include "../inc/header.hpp"
#include "../inc/Server.hpp"

int main(int argc, char **argv, char **env) {
    (void)argc;
    (void)argv;
    (void)env;

	try
	{
		Sender sender(std::string("www"), std::string("errwww"));
		Webserv server(sender);
		server.parse(std::string("linux.conf"));
		server.data_structure();
		//server.check();
        server.Start();
        while (true) {
            server.Wait();
            server.ManageConnexion();
		}
		server.clean();
        server.Stop();
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
