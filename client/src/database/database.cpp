#include <database.h>
#include <filesystem>
#include <SQLiteCpp/Transaction.h>
#include <config.h>

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
        // Retrieve filename fn, hash hs, and last modified time lt.
        std::string     fn = query.getColumn(0);
        std::string     hs = query.getColumn(1);
        std::string     lt  = query.getColumn(2);

        if(DEBUG) std::cout << "TUPLE DB READ: " << fn << " " << hs << " " << lt << std::endl;
        // We can return true, as the tuple already exists.
        return true;
    }

    // Here if we did not find any row. The file could be new or changed and so the lmt is different.
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

    SQLite::Statement   query(hash_db_, "SELECT * "
                                                    "FROM files ");
    while (query.executeStep())
    {
        // We get the filename from the DB
        std::string fn = query.getColumn(0);

        // We check if the filename from the DB has the corresponding file in the folder.
        std::filesystem::path full_path = folder_watched_ / fn;
        if(!std::filesystem::exists(full_path)){
            //Inside the folder we don't have this file anymore (i.e. this file was deleted when the program was not running).
            SQLite::Statement   row_delete(hash_db_, " DELETE FROM files WHERE filename= ? ;");
            row_delete.bind(1, fn);

            row_delete.exec();

            if(DEBUG) std::cout << "Deleted from DB row " << fn << std::endl;
        }
    }
}

/// Goes in the db and changes the status to "SENDING" but does it only if the current status is "NEW"
/// \param filename of the file that we want to put in sending
/// \return true if we succesfully switched the status to "NEW" to "SENDING"
bool DatabaseConnection::ChangeStatusToSending(const std::string& filename) {

    // Compile a SQL query, containing 1 parameters
    SQLite::Statement   query(hash_db_, "SELECT * "
                                        "FROM files "
                                        "WHERE filename = ? ");

    // Bind to ? of the query
    query.bind(1, filename);

    while (query.executeStep()) {
        // Demonstrate how to get some typed column value
        std::string current_status = query.getColumn(3);


        std::cout << "AUTH DB READ: "
                     " " << filename <<
                  " " << "with status: " <<
                  " " << current_status << std::endl;

        if (current_status == "NEW"){
            // we can put it in sending, so we update the row to notify future query for this file.

            SQLite::Transaction transaction(hash_db_);

            std::string sql_update_status = " UPDATE files "
                                            " SET status = \'SENDING\' "
                                            " WHERE filename =  \"" + filename + "\"";

            int result_update = hash_db_.exec(sql_update_status);


            std::cout << " Executed Update " << sql_update_status << std::endl;
            std::cout << " With Result =  " << result_update << std::endl;

            // Commit transaction
            transaction.commit();
            return true;
        } else {
            return false;
        }


    }

}

/// Goes in the db and changes the status to "SENT" but does it only if the current status is "SENDING"
/// \param filename of the file that we want to put in as SENT
/// \return true if we succesfully switched the status to "SENDING" to "SENT"
bool DatabaseConnection::ChangeStatusToSent(const std::string& filename) {

    // Compile a SQL query, containing 1 parameters
    SQLite::Statement   query(hash_db_, "SELECT * "
                                        "FROM files "
                                        "WHERE filename = ? ");

    // Bind to ? of the query
    query.bind(1, filename);

    while (query.executeStep()) {
        // Demonstrate how to get some typed column value
        std::string current_status = query.getColumn(3);


        std::cout << "AUTH DB READ: "
                     " " << filename <<
                  " " << "with status: " <<
                  " " << current_status << std::endl;

        if (current_status == "SENDING"){


            SQLite::Transaction transaction(hash_db_);
//TODO use the correct bind dont format string manually
            std::string sql_update_status = " UPDATE files "
                                            " SET status = \'SENT\' "
                                            " WHERE filename =  \"" + filename + "\"";

            int result_update = hash_db_.exec(sql_update_status);

            std::cout << " Executed Update " << sql_update_status << std::endl;
            std::cout << " With Result =  " << result_update << std::endl;

            // Commit transaction
            transaction.commit();
            return true;
        } else {
            return false;
        }


    }

}


void DatabaseConnection::GetMetadata(const std::string& filename, std::string& hash, std::string& lmt){
    // Compile a SQL query, containing 1 parameters
    SQLite::Statement   query(hash_db_, "SELECT * "
                                        "FROM files "
                                        "WHERE filename = ? "
                                        );
    // Bind to ? of the query
    query.bind(1, filename);

    while (query.executeStep())
    {
        // Retrieve filename fn, hash hs, and last modified time lt.
        std::string     hs = query.getColumn(1);
        std::string     lt = query.getColumn(2);
        hash = hs;
        lmt = lt;
        if(DEBUG) std::cout << "TUPLE DB READ: " << filename << " " << hash << " " << lmt << std::endl;

    }

}