#include "../inc/header.hpp"
#include "../inc/Server.hpp"

int main(int argc, char **argv, char **env) {
    (void)argc;
    (void)argv;
    (void)env;
    Server server = Server();
    std::string path_to_html = "www";
    std::string path_to_err = "errwww";

    try {
        server.Initialize(path_to_html, path_to_err);
        server.Start();
        while (true) {
            server.Wait();
            server.ManageConnexion();
        }
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    server.Stop();
    return 0;
}