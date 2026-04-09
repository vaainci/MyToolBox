#include <iostream>
#include <string>

// On définit nos commandes possibles
enum Command {
    HELP,
    VERSION,
    UNKNOWN
};

// Petite fonction pour transformer le texte en Enum
Command getCommand(std::string _ARGUMENT) {
    if (_ARGUMENT == "--help" || _ARGUMENT == "-h") return HELP;
    if (_ARGUMENT == "--version" || _ARGUMENT == "-v") return VERSION;
    return UNKNOWN;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Utilisation: monapp [options]" << std::endl;
        return 0;
    }
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        Command cmd = getCommand(arg);

        switch (cmd) {
        case HELP:
            std::cout << "Help : use --version to see the current version." << std::endl;
            break;
        case VERSION:
            std::cout << "MyToolBox v0.0.0" << std::endl;
            break;
        case UNKNOWN:
            std::cout << "Error : " << arg << " is not a valid argument (--help)." << std::endl;
            break;
        }
    }

    return 0;
}