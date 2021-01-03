#include "../../includes/database/database.h"
#include <string>
#include <SQLiteCpp/Database.h>
#include <iostream>
#include <fstream>

bool Database::auth(std::string username, std::string attemp_hash_password, std::string serverPath) {

    try {
        // Open a database file
        SQLite::Database    db( serverPath + "\\backupFiles\\authDB.db");

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

std::string Database::getUserPath(std::string username, std::string serverPath) {

    try {
        // Open a database file
        SQLite::Database    db(serverPath + "\\backupFiles\\authDB.db");

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

    return "";
}

void Database::createTable(std::string foldername, std::string serverPath) {

    try {
        // Open a database file in create/write mode
        SQLite::Database    db(serverPath + "\\backupFiles\\usersTREE\\"+foldername+".db", SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE);
        std::cout << "SQLite database file '" << db.getFilename().c_str() << "' opened successfully\n";

        // Create a new table with an explicit "id" column aliasing the underlying rowid
        db.exec("DROP TABLE IF EXISTS UserTree");
        db.exec("CREATE TABLE UserTree (id INTEGER PRIMARY KEY, path TEXT, time TEXT)");

        // first row
        //int nb = db.exec("INSERT INTO UserTree VALUES (NULL, \"PROVA\", \"1155\")");

    }
    catch (std::exception& e)
    {
        std::cerr << "exception: " << e.what() << std::endl;
        //TODO Controllare come rimandare indietro errore; sicuramente non dobbiamo terminare.
    }

}

std::string Database::getTimefromPath(std::string foldername, std::string path, std::string serverPath) {


    try {
        // Open a database file
        SQLite::Database    db(serverPath + "\\backupFiles\\usersTREE\\"+foldername+".db");

        // Compile a SQL query, containing one parameter (index 1)
        SQLite::Statement   query(db, "SELECT time FROM UserTree WHERE path= ?");

        // Bind the integer value 6 to the first parameter of the SQL query
        query.bind(1, path);

        // Loop to execute the query step by step, to get rows of result
        while (query.executeStep())
        {
            // Demonstrate how to get some typed column value
            std::string   time     = query.getColumn(0);
            return time;
        }
        //Se non trova nulla ritorna 1
        return std::to_string(1);

    }
    catch (std::exception& e)
    {
        std::cerr << "exception: " << e.what() << std::endl;
        //TODO Controllare come rimandare indietro errore; sicuramente non dobbiamo terminare.
    }

    //Se non trova nulla ritorna 1
    return std::to_string(1);

}