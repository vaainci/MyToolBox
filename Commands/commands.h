#ifndef COMMANDS_H
#define COMMANDS_H

#include <string>
#include <iostream>

class Commands
{
public:
    Commands();

    static void help();
    static void version();

    static void ip();
    static void storage();
};

#endif // COMMANDS_H
