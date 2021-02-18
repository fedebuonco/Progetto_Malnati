#include <database.h>
#include <filesystem>
#include <utility>

std::shared_mutex DatabaseConnection::db_mutex_;

/**
 * Constructor. Takes the db_path and folder_watched and create a db connection.
 * @param db_path: db path where store the hash & time
 * @param folder_watched: folder path to watched changes
 */
DatabaseConnection::DatabaseConnection(const std::filesystem::path& db_path, std::filesystem::path folder_watched) :
    hash_db_(db_path.string(), SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE),
    folder_watched_(std::move(folder_watched)){

    std::unique_lock lg(db_mutex_);

    try {
        // We create the table if not exist.
        SQLite::Statement query(hash_db_, "CREATE TABLE IF NOT EXISTS files (filename TEXT PRIMARY KEY, hash TEXT, lmt TEXT, status TEXT)");
        query.exec();
    }
    catch (std::exception& e)
    {
        std::cerr << "SQLite exception: " << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

/// Checks if a row is present in the db by scanning using tuple filename - lastModTime
/// \return bool true if present, false if not
bool DatabaseConnection::AlreadyHashed(const std::string& filename, const std::string& lmt){
    std::unique_lock lg(db_mutex_);
    try {
        SQLite::Statement query(hash_db_, "SELECT * FROM files WHERE filename = ? AND lmt = ?");
        query.bind(1, filename);
        query.bind(2, lmt);

        while (query.executeStep()) {
            // We can return true, as the tuple already exists.
            return true;
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "SQLite exception: " << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
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
void DatabaseConnection::InsertDB(const std::string& path_str, const std::string& hash, const std::string& lmt_str){
    std::unique_lock lg(db_mutex_);
    try{
        SQLite::Statement query(hash_db_, "SELECT * FROM files WHERE filename = ?");
        query.bind(1, path_str);

        while (query.executeStep()) {

            std::string     old_fn = query.getColumn(0);
            std::string     old_hs = query.getColumn(1);
            std::string     old_lt = query.getColumn(2);

            SQLite::Statement query_update(hash_db_, "UPDATE files SET filename = ?, hash = ?, lmt = ?, status = \'NEW\' WHERE filename = ?");

            query_update.bind(1, old_fn);
            query_update.bind(2, hash);
            query_update.bind(3, lmt_str);
            query_update.bind(4, old_fn);

            query_update.exec();

            return;
        }

        SQLite::Statement query_insert(hash_db_,"INSERT INTO files(filename, hash, lmt, status) VALUES ( ?, ?, ?, \'NEW\')");
        query_insert.bind(1, path_str);
        query_insert.bind(2, hash);
        query_insert.bind(3, lmt_str);

        query_insert.exec();
    }
    catch (std::exception& e)
    {
        std::cerr << "SQLite exception: " << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

/**
 * Changes all the tuple that are in SENDING to NEW
 * @return the number of tuple that have returned to 'NEW'
 */
int DatabaseConnection::SetBackToNew(){
    std::unique_lock lg(db_mutex_);

    try {
        SQLite::Statement query(hash_db_, "UPDATE files SET status = 'NEW' WHERE status = 'SENDING'");
        int cnt = query.exec();
        return cnt;
    }
    catch (std::exception& e)
    {
        std::cerr << "SQLite exception: " << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }
}


/// We check the db against the file in the folder,
/// if a row doesn't have the counterpart in the folder is removed.
void DatabaseConnection::CleanOldRows(){
    std::unique_lock lg(db_mutex_);
    try {
        SQLite::Statement query(hash_db_, "SELECT * FROM files");
        while (query.executeStep()) {
            // We get the filename from the DB
            std::string fn = query.getColumn(0);

            // We check if the filename from the DB has the corresponding file in the folder.
            std::filesystem::path full_path = folder_watched_ / fn;
            if (!std::filesystem::exists(full_path)) {
                //Inside the folder we don't have this file anymore (i.e. this file was deleted when the program was not running).
                SQLite::Statement row_delete(hash_db_, "DELETE FROM files WHERE filename= ?");
                row_delete.bind(1, fn);

                row_delete.exec();
            }
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "SQLite exception: " << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

/// Goes in the db and changes the status to "SENDING" but does it only if the current status is "NEW"
/// \param filename of the file that we want to put in sending
/// \return true if we successfully switched the status to "NEW" to "SENDING"
bool DatabaseConnection::ChangeStatusToSending(const std::string& filename) {
    std::unique_lock lg(db_mutex_);
    try {
        SQLite::Statement query(hash_db_, "SELECT * FROM files WHERE filename = ?");
        query.bind(1, filename);

        while (query.executeStep()) {

            std::string current_status = query.getColumn(3);

            if (current_status == "NEW") {

                SQLite::Statement update_to_sending(hash_db_,
                                                    " UPDATE files SET status = \'SENDING\' WHERE filename =  ?");
                update_to_sending.bind(1, filename);
                update_to_sending.exec();
                return true;
            }
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "SQLite exception: " << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }

    return false;
}

/// Goes in the db and changes the status to "SENT" but does it only if the current status is "SENDING"
/// \param filename of the file that we want to put in as SENT
/// \return true if we successfully switched the status to "SENDING" to "SENT"
bool DatabaseConnection::ChangeStatusToSent(const std::string& filename) {
    std::unique_lock lg(db_mutex_);
    try {
        SQLite::Statement query(hash_db_, "SELECT * FROM files WHERE filename = ?");
        query.bind(1, filename);

        while (query.executeStep()) {

            std::string current_status = query.getColumn(3);

            if (current_status == "SENDING") {
                SQLite::Statement update_to_sent(hash_db_, " UPDATE files SET status = \'SENT\' WHERE filename =  ?");
                update_to_sent.bind(1, filename);
                update_to_sent.exec();

                return true;
            }
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "SQLite exception: " << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }

    return false;
}


/// Goes in the db and changes the status to "NEW" but does it only if the current status is "SENDING"
/// \param filename of the file that we want to put in as NEW
/// \return true if we successfully switched the status to "SENDING" to "NEW"
bool DatabaseConnection::ChangeStatusToNew(const std::string& filename) {
    std::unique_lock lg(db_mutex_);
    try {

        SQLite::Statement query(hash_db_, "SELECT * FROM files WHERE filename = ?");
        query.bind(1, filename);

        while (query.executeStep()) {

            std::string current_status = query.getColumn(3);

            if (current_status == "SENDING") {

                SQLite::Statement update_to_new(hash_db_, " UPDATE files SET status = \'NEW\' WHERE filename =  ?");
                update_to_new.bind(1, filename);
                update_to_new.exec();

                return true;
            }

        }
    }
    catch (std::exception& e)
    {
        std::cerr << "SQLite exception: " << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }

    return false;
}

/// Goes in the db and changes the status to "NEW" but does it only if the current status is "SENDING"
/// \param filename of the file that we want to put in as NEW
/// \return true if we successfully switched the status to "SENDING" to "NEW"
bool DatabaseConnection::ChangeStatusToNotSent(const std::string& filename) {
    std::unique_lock lg(db_mutex_);
    try {

        SQLite::Statement query(hash_db_, "SELECT * FROM files WHERE filename = ?");
        query.bind(1, filename);

        while (query.executeStep()) {

            std::string current_status = query.getColumn(3);

            if (current_status == "SENDING") {

                SQLite::Statement update_to_new(hash_db_, " UPDATE files SET status = \'NOTSENT\' WHERE filename =  ?");
                update_to_new.bind(1, filename);
                update_to_new.exec();

                return true;
            }

        }
    }
    catch (std::exception& e)
    {
        std::cerr << "SQLite exception: " << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }

    return false;
}


void DatabaseConnection::GetMetadata(const std::string& filename, std::string& hash, std::string& lmt){
    std::unique_lock lg(db_mutex_);
    try{

        SQLite::Statement query(hash_db_, "SELECT * FROM files WHERE filename = ?");
        query.bind(1, filename);

        while (query.executeStep())
        {
            // Retrieve filename fn, hash hs, and last modified time lt.
            std::string     hs = query.getColumn(1);
            std::string     lt = query.getColumn(2);
            hash = hs;
            lmt = lt;

        }
    }
    catch (std::exception& e)
    {
        std::cerr << "SQLite exception: " << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }
}


bool DatabaseConnection::AlignStatus(const std::vector<std::pair<std::string, unsigned  long>>& sfileslmt){
    std::unique_lock lg(db_mutex_);
    for (const auto& element : sfileslmt){
        try{

            SQLite::Statement update_to_sent(hash_db_, " UPDATE files SET status = \'SENT\' WHERE filename =  ? AND lmt = ?");
            update_to_sent.bind(1, element.first);
            update_to_sent.bind(2, std::to_string(element.second));
            update_to_sent.exec();

        }
        catch (std::exception& e)
        {
            std::cerr << "SQLite exception: " << e.what() << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }

}

