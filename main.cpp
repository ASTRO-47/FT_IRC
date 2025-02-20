#include "server.hpp"

// void f()
// {
//     system("leaks ircserv");
// }


// int main(int ac , char *av[])
// {
//     // atexit(f);
//     if (ac != 3)
//         return (std::cerr << "invalid number of args\n", 1);
//     Server server;
//     try
//     {
//         server.server_setup(av[1], av[2]);
//         server.multiplexing_func();
//     }
//     catch (std::exception &e)
//     {
//         std::cout << "ERROR: " << e.what() << std::endl;
//     }

//     return 0;
// }


std::string trim(const std::string& str) 
{
    const std::string whitespace = " \t\n\r";
    size_t first = str.find_first_not_of(whitespace);
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(whitespace);
    return str.substr(first, last - first + 1);
}

std::vector<std::string> parse_irc_command(const std::string& input)
{
    std::vector<std::string> commands;
    std::stringstream ss(input);
    std::string command;
    
    // Split by spaces but handle multiple spaces
    while (std::getline(ss, command, ' ')) 
    {
        // Trim each command and add if not empty
        std::string trimmed = trim(command);
        if (!trimmed.empty())
        {
            commands.push_back(trimmed);
        }
    }
    
    return commands;
}
int main()
{
    std::vector<std::string> imad = parse_irc_command("hello world how the hell");
    // for (auto it = imad.begin();it != imad.end();it++)
    // {
    //     std::cout << *it << std::endl;
    // }
}