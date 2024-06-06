#ifndef COURSE_PROJECT_DIALOG_H
#define COURSE_PROJECT_DIALOG_H

#include "../editions/validation/validation.h"
#include <iostream>

namespace dialog
{
    std::string dialog_commands[] =
    {
        "ADD <pool_name> <scheme_name> <collection_name> <customer_id> <customer_name> <customer_money>", 
        "READ <pool_name> <scheme_name> <collection_name> <customer_key>", 
        "READBETWEEN <pool_name> <scheme_name> <collection_name> <min customer_key> <max customer_key>", 
        "UPDATE <pool_name> <scheme_name> <collection_name> <customer_key> <cnew ustomer_name> <new customer_money>",
        "DELETE <pool_name> <scheme_name> <collection_name> <customer_key>", 
        "ADDPOOL <pool_name>", 
        "DELETEPOOL <pool_name>", 
        "ADDSCHEME <pool_name> <scheme_name>", 
        "DELETESCHEME <pool_name> <scheme_name>",  
        "ADDSCHEME <pool_name> <scheme_name>", 
        "DELETESCHEME <pool_name> <scheme_name>",  
        "INSERTFILE",
        "QUIT"
    };

    std::string start = "Welcome to the command center!\n";

    void interactive_dialog()
    {
        std::cout << "\nYou can choose these commands:\n\n";
        for (auto & item : dialog_commands) std::cout << item << std::endl;
    };

    std::string parse_file()
    {
        std::cin.sync();
        std::string file_name;
        std::cout << "ENTER FILE NAME HERE >> ";
        std::cin >> file_name;
        return file_name;
    }
};

#endif