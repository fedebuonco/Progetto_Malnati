#include <database.h>
#include <filesystem>
#include <SQLiteCpp/Transaction.h>

/// Takes in the db_path and opens a connection to it.
/// \param db_path
DatabaseConnection::DatabaseConnection(std::filesystem::path db_path, std::filesystem::path folder_watched) :
    hash_db_(db_path.string(), SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE),
    folder_watched_(folder_watched){
    // We create the table if not exist.

    std::string table_create = " CREATE TABLE IF NOT EXISTS files "
                               " (filename TEXT PRIMARY KEY, hash TEXT, lmt TEXT , status TEXT )";

    int result_create = hash_db_.exec(table_create);

    std::cout << " Executed Create " << table_create << std::endl;
    std::cout << " With Result =  " << result_create << std::endl;

}

/// Checks if a row is present in the db by scanning using tuple filename - lastModTime
/// \return bool true if present, false if not
bool DatabaseConnection::AlreadyHashed(std::string filename, std::string lmt){

    // Compile a SQL query, containing 2 parameters
    SQLite::Statement   query(hash_db_, "SELECT * "
                                                    "FROM files "
                                                    "WHERE filename = ? "
                                                    "AND lmt = ?  ");
    // Bind to ? of the query
    query.bind(1, filename);
    query.bind(2, lmt);

    while (query.executeStep())
    {
        // Demonstrate how to get some typed column value
        std::string     fn = query.getColumn(0);
        std::string     hs = query.getColumn(1);
        std::string     lt  = query.getColumn(2);

        std::cout << "AUTH DB READ: " << fn <<
                                  " " << hs <<
                                  " " << lt << std::endl;
        // We can return true, as the tuple already exists.
        return true;
    }

    // Here if we did not find any row.
    // The file could be new or changed and so the lmt is different.
    return false;

}

/// Insert a row in the db. If the db contains a tuple path_str - lmt_str
/// then that row is updated with the
/// new hash, otherwise we simply add a new row to the db.
/// \param path_str The filename path we are inserting
/// \param hash The hash of the file we are inserting
/// \param lmt_str The last modified time of the version of the file we are inserting.
void DatabaseConnection::InsertDB(std::string path_str, std::string hash, std::string lmt_str){

    // Compile a SQL query, containing 2 parameters
    SQLite::Statement   query(hash_db_, "SELECT * "
                                        "FROM files "
                                        "WHERE filename = ? ");

    // Bind to ? of the query
    query.bind(1, path_str);

    while (query.executeStep())
    {
        // Demonstrate how to get some typed column value
        std::string     old_fn = query.getColumn(0);
        std::string     old_hs = query.getColumn(1);
        std::string     old_lt  = query.getColumn(2);

        std::cout << "AUTH DB READ: "
                  " " << old_fn <<
                  " " << old_hs <<
                  " " << old_lt << std::endl;

        // So the row with this filename and lmt already exists, we must update it
        // TODO update hash.

        // Begin transaction
        SQLite::Transaction transaction(hash_db_);

        std::string sql_update = "UPDATE files "
                                 " SET filename = \"" + old_fn +
                                 "\" , hash = \""     + hash +
                                 "\" , lmt = \""      + lmt_str +
                                 "\" , status = \""      + "NEW" +
                                 "\" WHERE filename =  \""     + old_fn + "\"";

        int result_update = hash_db_.exec(sql_update);


        std::cout << " Executed Update " << sql_update << std::endl;
        std::cout << " With Result =  " << result_update << std::endl;

        // Commit transaction
        transaction.commit();

        return;
    }

    // Here we arrive only if the tuple is not found, so we must simply insert a new row
    // TODO Insert new row.



    // Begin transaction
    SQLite::Transaction transaction(hash_db_);

    std::string sql_insert = "INSERT INTO files "
                             "VALUES ( \""     + path_str +
                                     "\" , \"" + hash +
                                     "\" , \"" + lmt_str +
                                     "\" , \"" + "NEW" +
                                     "\") ";

    int result_insert = hash_db_.exec(sql_insert);


    std::cout << " Executed Update " << sql_insert << std::endl;
    std::cout << " With Result =  " << result_insert << std::endl;

    // Commit transaction
    transaction.commit();

    return;

}

/// We check the db against the file in the folder,
/// if a row doesn't have the counterpart in the folder is removed.
void DatabaseConnection::CleanOldRows(){
    // Compile a SQL query, containing 2 parameters
    SQLite::Statement   query(hash_db_, "SELECT * "
                                                    "FROM files ");
    while (query.executeStep())
    {
        // Demonstrate how to get some typed column value
        std::string     fn = query.getColumn(0);

        // We get the filename and check it against the fs, if it does not exist we delete the row in
        // We build the path
        std::filesystem::path full_path = folder_watched_ / fn;
        if(!std::filesystem::exists(full_path)){
            std::string row_delete =   " DELETE FROM files WHERE filename='"+ fn +"'";
            int result_create = hash_db_.exec(row_delete);
            std::cout << "Deleted from DB row " << fn << std::endl;
        }

    }
}