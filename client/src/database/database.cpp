#include <database.h>
#include <filesystem>
#include <utility>
#include <SQLiteCpp/Transaction.h>
#include <config.h>


/**
 * Constructor. Takes the db_path and folder_watched and create a db connection.
 * @param db_path db path where store the hash & time
 * @param folder_watched folder path to watched changes
 */
DatabaseConnection::DatabaseConnection(const std::filesystem::path& db_path, std::filesystem::path folder_watched) :
    hash_db_(db_path.string(), SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE),
    folder_watched_(std::move(folder_watched)){

    // We create the table if not exist.
    std::string table_create = " CREATE TABLE IF NOT EXISTS files "
                               " (filename TEXT PRIMARY KEY, hash TEXT, lmt TEXT , status TEXT )";

    hash_db_.exec(table_create);
}

/// Checks if a row is present in the db by scanning using tuple filename - lastModTime
/// \return bool true if present, false if not
bool DatabaseConnection::AlreadyHashed(const std::string& filename, const std::string& lmt){

    try {
        SQLite::Statement query(hash_db_,    "SELECT * FROM files WHERE filename = ? AND lmt = ?");
        query.bind(1, filename);
        query.bind(2, lmt);

        while (query.executeStep()) {
            // Retrieve filename fn, hash hs, and last modified time lt.
            std::string fn = query.getColumn(0);
            std::string hs = query.getColumn(1);
            std::string lt = query.getColumn(2);

            // We can return true, as the tuple already exists.
            return true;
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "SQLite exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    // Here if we did not find any row. The file could be new or changed and so the lmt is different.
    return false;
}

/// Insert a row in the db. If the db contains a tuple path_str - lmt_str then that row is updated with the
/// new hash, otherwise we simply add a new row to the db.
/// \param path_str The filename path we are inserting
/// \param hash The hash of the file we are inserting
/// \param lmt_str The last modified time of the version of the file we are inserting.
void DatabaseConnection::InsertDB(const std::string& path_str, const std::string& hash, const std::string& lmt_str){

    SQLite::Statement query(hash_db_, "SELECT * FROM files WHERE filename = ?");
    query.bind(1, path_str);

    while (query.executeStep()) {

        std::string     old_fn = query.getColumn(0);
        std::string     old_hs = query.getColumn(1);
        std::string     old_lt  = query.getColumn(2);

        SQLite::Statement query_update(hash_db_, "UPDATE files SET filename = ?, hash = ?, lmt = ?, status = NEW WHERE filename = ?");

        query_update.bind(1, old_fn);
        query_update.bind(1, hash);
        query_update.bind(1, lmt_str);
        query_update.bind(1, old_fn);

        query_update.exec(); //TODO: Try this

        return;
    }

    std::string sql_insert = "INSERT INTO files "
                             "VALUES ( \""     + path_str +
                                     "\" , \"" + hash +
                                     "\" , \"" + lmt_str +
                                     "\" , \"" + "NEW" +
                                     "\") ";

    hash_db_.exec(sql_insert);
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

            SQLite::Statement   update_to_sending(hash_db_, " UPDATE files "
                                                     " SET status = \'SENDING\' "
                                                     " WHERE filename =  ? " );
            update_to_sending.bind(1, filename);
            int result_update = update_to_sending.exec();

            std::cout << " Executed Update to Sending" << std::endl;
            std::cout << " With Result =  " << result_update << std::endl;

            return true;
        } else {
            return false;
        }


    }
    return true;
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

    std::cout << "Sto per cambiare il file " << filename  << " a SENT"  << std::endl;


    while (query.executeStep()) {
        // Demonstrate how to get some typed column value
        std::string current_status = query.getColumn(3);


        std::cout << "Il file " << filename  << " era in "  << current_status << std::endl;

        if (current_status == "SENDING"){


            SQLite::Statement   update_to_sent(hash_db_, " UPDATE files "
                                                        " SET status = \'SENT\' "
                                                        " WHERE filename =  ? " );
            update_to_sent.bind(1, filename);
            int result_update = update_to_sent.exec();

            std::cout << " Executed Update to Sent" << std::endl;
            std::cout << " With Result =  " << result_update << std::endl;

            return true;
        } else {
            return false;
        }


    }
    return false;
}


/// Goes in the db and changes the status to "NEW" but does it only if the current status is "SENDING"
/// \param filename of the file that we want to put in as NEW
/// \return true if we succesfully switched the status to "SENDING" to "NEW"
bool DatabaseConnection::ChangeStatusToNew(const std::string& filename) {

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


            SQLite::Statement   update_to_sent(hash_db_, " UPDATE files "
                                                         " SET status = \'NEW\' "
                                                         " WHERE filename =  ? " );
            update_to_sent.bind(1, filename);
            int result_update = update_to_sent.exec();

            std::cout << " Executed Update to Sent" << std::endl;
            std::cout << " With Result =  " << result_update << std::endl;

            return true;
        } else {
            return false;
        }


    }
    return false;
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