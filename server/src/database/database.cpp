


#include "../../includes/database/database.h"
#include <string>
#include <SQLiteCpp/Database.h>
#include <iostream>

bool Database::auth(std::string username, std::string attemp_hash_password) {

    try {
        // Open a database file
        SQLite::Database    db("../backupROOT/authDB.db");

        // Compile a SQL query, containing one parameter (index 1)
        SQLite::Statement   query(db, "SELECT * FROM user WHERE username = ?");

        // Bind the integer value 6 to the first parameter of the SQL query
        query.bind(1, username);

        // Loop to execute the query step by step, to get rows of result
        while (query.executeStep())
        {
            // Demonstrate how to get some typed column value
            int             id          = query.getColumn(0);
            std::string     username    = query.getColumn(1);
            std::string     hashpass    = query.getColumn(2);
            std::string     folderName  = query.getColumn(3);

            std::cout << "AUTH DB READ  row: " << id << ", " << username << ", " << hashpass << ", " << folderName << std::endl;

            if(hashpass==attemp_hash_password){
                return true;
            }

        }

    }
    catch (std::exception& e)
    {
        std::cerr << "exception: " << e.what() << std::endl;
        //TODO Controllare come rimandare indietro errore; sicuramente non dobbiamo terminare.
    }


    return false;
}

std::string Database::getUserPath(std::string username) {

    try {
        // Open a database file
        SQLite::Database    db("../backupROOT/authDB.db");

        // Compile a SQL query, containing one parameter (index 1)
        SQLite::Statement   query(db, "SELECT folderName FROM user WHERE username = ?");

        // Bind the integer value 6 to the first parameter of the SQL query
        query.bind(1, username);

        // Loop to execute the query step by step, to get rows of result
        while (query.executeStep())
        {
            // Demonstrate how to get some typed column value
            std::string   folder          = query.getColumn(0);

            return folder;

        }

    }
    catch (std::exception& e)
    {
        std::cerr << "exception: " << e.what() << std::endl;
        //TODO Controllare come rimandare indietro errore; sicuramente non dobbiamo terminare.
    }

}
