#include "../inc/header.hpp"
#include "../inc/Server.hpp"

int main(int argc, char **argv, char **env) {
    (void)argc;
    (void)argv;
    (void)env;

    try {
        Sender sender = Sender(std::string("www"), std::string("errwww"));
        Server server = Server(sender);
        server.Start();
        while (true) {
            server.Wait();
            server.ManageConnexion();
        }
        server.Stop();
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}