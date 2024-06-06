#include "handler/handler.h"

int main(int argc, char * argv[])
{
    std::string error = "Invalid command line arguments. Try again with <command file or --dialog> <mode> <index>;\n<mode> is --memory-cache or --file-system\n<index> is --id, --name, --balance or nothing\n";
    if (argc != 3 && argc != 4)
    {
        std::cerr << error;
        return -1;
    }
    std::string dialog;
    bool is_dialog = false;

    std::string file_name;
    file_name.assign(argv[1]);
    std::string mode;
    mode.assign(argv[2]);

    if (validation::is_dialog_option(file_name)) is_dialog = true;

    database::key_type type = database::key_type::ID;
    if (argc == 4)
    {
        std::string index;
        index.assign(argv[3]);
        if (!index.compare("--name")) type = database::key_type::NAME;
        else if (!index.compare("--balance")) type = database::key_type::BALANCE;
        else if (index.compare("--id"))
        {
            std::cerr << error;
            return -1;
        }
    }
    
    if (!mode.compare("--memory-cache")) 
    {
        try
        {
            program::program(file_name, database::mode::IN_MEMORY_CACHE, type, is_dialog);
        }
        catch(const std::logic_error& e)
        {
            std::cerr << "\n\n" << e.what() << "\n\n";
            return -1;
        }
    }
    else if (!mode.compare("--file-system"))
    {
        try
        {
            program::program(file_name, database::mode::FILE_SYSTEM, type, is_dialog);
        }
        catch(const std::logic_error& e)
        {
            std::cerr << "\n\n" << e.what() << "\n\n";
            return -1;
        }
    }
    else
    {
        std::cerr << error;
        return -1;
    }
    
    boost::filesystem::remove_all("database");
    std::cout << "Program has finished correcly\n";
    return 0;
}