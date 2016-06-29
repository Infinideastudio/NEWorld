#include <iostream>
#include <string>
#include <boost/dll/shared_library.hpp>

typedef void __cdecl Main(int, char**);

int main(int argc, char** argv)
{
    std::cout << "Welcome to NEWorld Minimal Launcher!" << std::endl;
    std::cout << "Enter 'client' if you wanted to run client" << std::endl;
    std::cout << "Enter 'server' if you wanted to run server" << std::endl;
    std::string in;
    std::cin >> in;
    if (in == "server")
        boost::dll::shared_library
        (
            "NEWorldServer",
            boost::dll::load_mode::append_decorations
        )
        .get<Main>("main")(argc, argv);
    else boost::dll::shared_library
        (
            "NEWorld",
            boost::dll::load_mode::append_decorations
        )
        .get<Main>("main")(argc, argv);
    return 0;
}
