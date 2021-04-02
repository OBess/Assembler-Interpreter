#include <iostream>

#include <vector>
#include <regex>

#include <string>
#include <fstream>
#include <streambuf>

#include <windows.h>
#include <Lmcons.h>

#include "Interpreter.h"

int main()
{
    //settings and variables
    setlocale(LC_ALL, "Russian");
    std::ifstream file;
    std::regex regex_file_name("(.*.asm+)");
    std::vector<std::string> commands;
    std::string greeting = "> ", input = "help", program = "";
    bool quit = false;

    //main loop
    do
    {
        //execute commands
        commands = Interpreter::split(input);

        if (commands[0] == "Hello") 
        {
            TCHAR username[UNLEN + 1];
            DWORD size = UNLEN + 1;
            if (GetUserName((TCHAR*)username, &size))
                std::wcout << L"Hello, " << username << L"!\n";
            else
                std::cout << "Hello, unnamed person!\n";
        }
        else if (commands[0] == "help")
            std::cout << "Help:\n1. help - display all commands\n2. exe [name].asm - executre code file\n3. cls - clear the screen\n4. quit - exit the program\n\n";
        else if (commands[0] == "quit")
        {
            quit = true;
            break;
        }            
        else if (commands[0] == "exe")
        {
            if (commands.size() > 1 && std::regex_match(commands[1], regex_file_name))
            {
                file.open(commands[1]);
                if(file)
                {
                    file.seekg(0, std::ios::end);
                    program.reserve(file.tellg());
                    file.seekg(0, std::ios::beg);
                    program.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                    file.close();
                    Interpreter::assembler_interpreter(program);
                    std::cout << '\n';
                    program.clear();
                }
                else
                    std::cout << "Invalid file name!\n\n";
            }
            else
                std::cout << "Invalid file name!\n\n";
        }
        else if (commands[0] == "cls")
            system("CLS");
        else
            std::cout << "Invalid command!\n\n";

        //input commands
        std::cout << greeting;
        std::getline(std::cin, input);
    } while (!quit);
    return 0;
}