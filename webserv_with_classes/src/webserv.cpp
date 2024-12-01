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

    try {
        Sender sender = Sender(std::string("www"), std::string("errwww"));
        Server server = Server(sender, env);
		/*server.parse(std::string(argv[1]));
		server.data_structure();
		server.check();*/
        server.Start();
        while (true) {
            server.Wait();
            server.ManageConnexion();
        }
		//server.clean();
        server.Stop();
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
