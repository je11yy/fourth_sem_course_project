#ifndef COURSE_PROJECT_VALIDATION_H
#define COURSE_PROJECT_VALIDATION_H

#include <string>

namespace validation
{
    enum class command
    {
        ADD,
        READ,
        READ_BETWEEN,
        UPDATE,
        DELETE,
        ADD_POOL,
        DELETE_POOL,
        ADD_SCHEME,
        DELETE_SCHEME,
        ADD_COLLECTION,
        DELETE_COLLECTION
    };

    int commands_count = 11;

    std::string commands[] = 
    {
        "ADD", "READ", "READBETWEEN", "UPDATE", "DELETE", "ADDPOOL", "DELETEPOOL", "ADDSCHEME", "DELETESCHEME", "ADDCOLLECTION", "DELETECOLLECTION"
    };

    bool is_dialog_option(std::string to_validate)
    {
        if (!to_validate.compare("--dialog")) return true;
        return false;
    }

    bool is_command(std::string to_validate, command & _command)
    {
        for (auto i = 0; i < commands_count; ++i)
        {
            if (to_validate.compare(commands[i]) == 0)
            {
                _command = (command)i;
                return true;
            }
        }
        return false;
    }

    bool is_id(std::string to_validate, uint & id)
    {
        for (auto & item : to_validate) if (!isdigit(item)) return false;
        id = std::stoul(to_validate);
        return true;
    }

    bool is_name(std::string to_validate)
    {
        if (to_validate.empty()) return false;
        for (auto & item : to_validate) if (!isalpha(item)) return false;
        command _command;
        if (is_command(to_validate, _command)) return false;
        return true;
    }
};

#endif