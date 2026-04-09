#include <iostream>
#include <string>

#include "Commands/commands.h"

enum Command {
    UNKNOWN,
    HELP,
    VERSION,
    IP,
    STORAGE

};

Command getCommand(std::string _ARGUMENT) {
    if (_ARGUMENT == "--help" || _ARGUMENT == "-h") return HELP;
    if (_ARGUMENT == "--version" || _ARGUMENT == "-v") return VERSION;
    if (_ARGUMENT == "ip") return IP;
    if (_ARGUMENT == "storage") return STORAGE;
    return UNKNOWN;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Utilisation: monapp [options]" << std::endl;
        return 0;
    }

    // On utilise une boucle for classique, mais on va pouvoir manipuler 'i'
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        Command cmd = getCommand(arg);

        switch (cmd) {
        case UNKNOWN:
            std::cout << "Error : " << arg << " is not valid." << std::endl;
            break;
        case HELP:
            Commands::help();
            break;
        case VERSION:
            Commands::version();
            break;
        case IP:
            Commands::ip();
            break;
        case STORAGE:
            Commands::storage();
            break;
        }
    }
    return 0;
}