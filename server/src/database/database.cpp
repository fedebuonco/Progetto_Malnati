#include "../../includes/database/database.h"
#include <string>
#include <SQLiteCpp/Database.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <server.h>
#include <SQLiteCpp/Transaction.h>

bool Database::auth(std::string username, std::string attemp_hash_password, std::filesystem::path serverP) {

    try {
        // Open a database file
        std::filesystem::path db_path = serverP / "backupFiles" / "authDB.db" ;
        SQLite::Database    db(db_path.string());

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

std::string Database::getUserPath(std::string username, std::filesystem::path serverP) {

    try {
        // Open a database file
        std::filesystem::path db_path = serverP / "backupFiles" / "authDB.db" ;
        SQLite::Database    db(db_path.string());

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

void Database::createTable(std::string foldername, std::filesystem::path serverP) {

    try {
        // Open a database file in create/write mode
        std::string folderext = foldername+".db";
        std::filesystem::path db_path = serverP / "backupFiles" / "usersTREE" / folderext ;
        SQLite::Database    db(db_path.string(), SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE);
        //std::cout << "SQLite database file '" << db.getFilename().c_str() << "' opened successfully\n";

        // Create a new table with an explicit "id" column aliasing the underlying rowid
        db.exec("DROP TABLE IF EXISTS UserTree");
        db.exec("CREATE TABLE UserTree (id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, path TEXT, time TEXT)");

        // first row
        //int nb = db.exec("INSERT INTO UserTree VALUES (NULL, \"PROVA\", \"1155\")");

    }
    catch (std::exception& e)
    {
        std::cerr << "exception: " << e.what() << std::endl;
        //TODO Controllare come rimandare indietro errore; sicuramente non dobbiamo terminare.
    }

}

std::string Database::getTimefromPath(std::string foldername, std::string path, std::filesystem::path serverP) {


    try {
        // Open a database file
        std::string folderext = foldername+".db";
        std::filesystem::path db_path = serverP / "backupFiles" / "usersTREE" / folderext ;
        SQLite::Database    db(db_path.string());

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

void Database::deleteFile(std::string foldername, std::string path, std::filesystem::path serverP) {

    try{
        std::string folderext = foldername+".db";
        std::filesystem::path db_path = serverP / "backupFiles" / "usersTREE" / folderext ;
        SQLite::Database    db(db_path.string(), SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE);

        // Compile a SQL query, containing one parameter (index 1)
        SQLite::Statement   query(db, "DELETE FROM UserTree WHERE path= ?");

        // Bind the integer value 6 to the first parameter of the SQL query
        query.bind(1, path);

        // Loop to execute the query step by step, to get rows of result
        while (query.executeStep())
        {
            // Demonstrate how to get some typed column value
            if(DEBUG) std::cout << "Deleted ROW" << std::endl;
        }
        //If the filename doesn't exist, we don't do nothing
    }
    catch (std::exception& e) {
        std::cerr << "exception: " << e.what() << std::endl;
        //TODO Controllare come rimandare indietro errore; sicuramente non dobbiamo terminare.
    }



}

void Database::insertFile(std::string userName, std::string pathName, std::string hash, std::string lmt, std::filesystem::path serverP) {

    try {
        std::filesystem::path db_path = serverP / "backupFiles" / "authDB.db" ;
        SQLite::Database    db(db_path.string());

        // Compile a SQL query, containing one parameter (index 1)
        SQLite::Statement   query(db, "SELECT folderName FROM user WHERE username = ?");

        // Bind the integer value 6 to the first parameter of the SQL query
        query.bind(1, userName);

        std::string folder;

        // Loop to execute the query step by step, to get rows of result
        while (query.executeStep()) {
            // Demonstrate how to get some typed column value
           std::string folderLocale = query.getColumn(0);
           folder=folderLocale;
       }

        std::string folderext = folder + ".db";
        std::filesystem::path db_path1 = serverP / "backupFiles" / "usersTREE" / folderext;
        SQLite::Database db1(db_path1.string(), SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

        //Now we look if the file is already present in the db
        // Compile a SQL query, containing 1 parameters
        SQLite::Statement   query_ifpresent(db1, "SELECT * FROM UserTree WHERE path = ?");

        // Bind to ? of the query
        query_ifpresent.bind(1, pathName);
        // Loop to execute the query step by step, to get rows of result
        while (query_ifpresent.executeStep()) {
            // Demonstrate how to get some typed column value
            std::string id = query_ifpresent.getColumn(0);
            // Begin transaction
            SQLite::Transaction transaction(db1);

            std::string sql_update = "UPDATE UserTree "
                                     " SET time = \"" + lmt +
                                     "\" WHERE id =  \""     + id + "\"";

            int result_update = db1.exec(sql_update);

            // Commit transaction
            transaction.commit();
            return;
        }

        if (DEBUG) std::cout << "SQLite database file '" << db1.getFilename().c_str() << "' opened successfully\n";

        // Compile a SQL query, containing one parameter (index 1)
        SQLite::Statement   query1(db1, "INSERT INTO UserTree(path, time) VALUES (?,?)");

        query1.bind(1, pathName);
        query1.bind(2, lmt);

        query1.exec();

    }
    catch (std::exception& e) {
        std::cerr << "exception: " << e.what() << std::endl;
        //TODO Controllare come rimandare indietro errore; sicuramente non dobbiamo terminare.
    }

}
