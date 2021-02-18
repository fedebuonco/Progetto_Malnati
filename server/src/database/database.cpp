#include <../../includes/database/database.h>
#include <string>
#include <SQLiteCpp/Database.h>
#include <iostream>
#include <filesystem>
#include <server.h>
#include <sqlite3.h>

std::shared_mutex Database::db_mutex_;

/**
 * Verify if the hash of the password sent from the client is the same of the hash password store inside db.
 * @param username : user username
 * @param attempt_hash_password : hash password arrived from client
 * @param serverP
 * @return true or false depending on whether the password hash is correct or not
 */
bool Database::auth(const std::string& username, const std::string& attempt_hash_password, const std::filesystem::path& serverP) {

    std::unique_lock lg(db_mutex_);

    try {
        std::filesystem::path db_path = serverP / "backupFiles" / "authDB.db" ;
        SQLite::Database    db(db_path.string());

        SQLite::Statement   query(db, "SELECT * FROM user WHERE username = ?");
        query.bind(1, username);

        while (query.executeStep()) {
            //Retrieve the hash password stored inside the auth db
            std::string     hash_pass_DB    = query.getColumn(2);

            if(hash_pass_DB == attempt_hash_password) return true;
            else return false;
        }

    }
    catch (std::exception& e)
    {
        std::cerr << "SQLite exception: " << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }

    return false;
}

/**
 * Return the folder name of the user
 * @param username : username of the user
 * @param serverP : location of the server folder
 * @return folder of the user
 */
std::string Database::getUserPath(const std::string& username, const std::filesystem::path& serverP) {
    std::unique_lock lg(db_mutex_);

    try {
        //Open the authDB where is stored the user folder
        std::filesystem::path db_path = serverP / "backupFiles" / "authDB.db" ;
        SQLite::Database    db(db_path.string());

        SQLite::Statement   query(db, "SELECT folderName FROM user WHERE username = ?");
        query.bind(1, username);

        while (query.executeStep()) {
            //Username is unique so we have only a result; we take the first result anyway
            std::string folder = query.getColumn(0);
            return folder;
        }

    }
    catch (std::exception& e)
    {
        std::cerr << "SQLite exception: " << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }

    return "";
}

/**
 * Create a db to store the path and time of the user files
 * @param folderName : name of the server folder in which we store the user file
 * @param serverP
 */
void Database::createTable(const std::string& folderName, const std::filesystem::path& serverP) {
    std::unique_lock lg(db_mutex_);

    try {
        //Find the path of the user db
        std::string user_db_name = folderName + ".db";
        std::filesystem::path db_path = serverP / "backupFiles" / "usersTREE" / user_db_name ;

        SQLite::Database    db(db_path.string(), SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE);

        //Drop existing table and create a new one; this because a user can have only one backup folder per time
        db.exec("DROP TABLE IF EXISTS UserTree");
        db.exec("CREATE TABLE UserTree (id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, path TEXT, time TEXT)");

    }
    catch (std::exception& e)
    {
        std::cerr << "SQLite exception: " << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }

}

/**
 * Get the time stored in the user db of a given filepath
 * @param folderName : name of the server folder in which we store the user file
 * @param path : path to retrieve the time
 * @param serverP : program folder path
 * @return time store in the db of the filepath passed as argument
 */
std::string Database::getTimeFromPath(const std::string& folderName, const std::string& path, const std::filesystem::path& serverP) {
    std::unique_lock lg(db_mutex_);

    try {
        //Find the path of the user db
        std::string user_db_name = folderName + ".db";
        std::filesystem::path db_path = serverP / "backupFiles" / "usersTREE" / user_db_name ;

        SQLite::Database    db(db_path.string());

        //Find the time of a specific path
        SQLite::Statement   query(db, "SELECT time FROM UserTree WHERE path= ?");
        query.bind(1, path);

        while (query.executeStep()) {
            std::string   time     = query.getColumn(0);
            return time;
        }

    }
    catch (std::exception& e)
    {
        std::cerr << "SQLite exception: " << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }

    //If we don't find the path in the DB we return 1; this means that the client will send the file anyway
    return std::to_string(1);
}

/**
 * Delete the file passed from the user db
 * @param folderName : name of the server folder in which we store the user file
 * @param path : file to delete inside the db
 * @param serverP
 */
void Database::deleteFile(const std::string& folderName, const std::string& path, const std::filesystem::path& serverP) {
    std::unique_lock lg(db_mutex_);

    try {
        //Find the path of the user db
        std::string user_db_name = folderName + ".db";
        std::filesystem::path db_path = serverP / "backupFiles" / "usersTREE" / user_db_name;

        SQLite::Database db(db_path.string(), SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

        SQLite::Statement query(db, "DELETE FROM UserTree WHERE path= ?");
        query.bind(1, path);

        while (query.executeStep()) {
            if (DEBUG) std::cout << "Deleted ROW" << std::endl;
        }

        //If the filename doesn't exist, we don't do nothing

    }
    catch (std::exception &e) {
        std::cerr << "SQLite exception: " << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

/**
 * Insert a file path and time into the user db. If the file is already present, update only the time
 * @param userName : user username
 * @param pathName : path of the file to insert in the db
 * @param lmt      : last modified time of the file to insert in the db
 * @param serverP
 */
void Database::insertFile(const std::string& userName, const std::string& pathName, const std::string& lmt, const std::filesystem::path& serverP) {
    std::unique_lock lg(db_mutex_);

    try {
        std::filesystem::path db_path = serverP / "backupFiles" / "authDB.db";
        SQLite::Database db(db_path.string());

        //First we fine the folderName of the given Username

        SQLite::Statement query(db, "SELECT folderName FROM user WHERE username = ?");
        query.bind(1, userName);

        std::string folder;

        while (query.executeStep()) {
            std::string folderLocale = query.getColumn(0);
            folder = folderLocale;
        }

        //Now we search if the pathname is already present inside the DB (i.e. we have already an old version with an old time).

        std::string user_db_name = folder + ".db";
        std::filesystem::path db_path1 = serverP / "backupFiles" / "usersTREE" / user_db_name;
        SQLite::Database db1(db_path1.string(), SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

        SQLite::Statement query_if_present(db1, "SELECT * FROM UserTree WHERE path = ?");
        query_if_present.bind(1, pathName);

        while (query_if_present.executeStep()) {

            //We found the pathname so we update the time.

            std::string id = query_if_present.getColumn(0);

            SQLite::Statement sql_update(db1, "UPDATE UserTree SET time = ? WHERE id = ?");
            sql_update.bind(1, lmt);
            sql_update.bind(2, id);

            sql_update.exec();
            return;
        }

        //We didn't find the pathname (i.e. is a new file) and we insert the new tuple

        SQLite::Statement query1(db1, "INSERT INTO UserTree(path, time) VALUES (?,?)");
        query1.bind(1, pathName);
        query1.bind(2, lmt);

        query1.exec();

    }
    catch (std::exception& e)
    {
        std::cerr << "SQLite exception: INSERT " << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }
}