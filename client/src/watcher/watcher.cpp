#include "watcher.h"
#include <files.h>
#include <base64.h>
#include <sha.h>
#include <hex.h>
#include <config.h>
#include <file_sipper.h>


/// This method will assign the callback called in the Watcher::listenerFunction() to the provided function.
/// \param updateCallback The function that will be called as the callback.
void Watcher::SetUpdateCallback(const std::function<void()> &updateCallback) {
    update_callback = updateCallback;
}

/// This is the function that will be called when a change is recorded.
/// Prints the changes and then call the provided callback ( provided from Watcher::SetUpdateCallback() )
/// \param events The series of events (file deleted, added, etc.) that triggered the function.
void Watcher::listenerFunction(std::vector<pfw::EventPtr> events)
{
    for (const auto &event : events) {
        std::bitset<16> typeBits(event->type);
        std::cout << event->relativePath << " with the type: " << typeBits << std::endl;
        //TODO do not consider bakup files ( the ones ending in ~) and the dirs.
        //TODO this are for linux and currently don't know why some are not computed.
        if (typeBits == 0b00000001 || typeBits == 0b00000010 || typeBits == 0b00000011){ // Here it means we are adding a file or mod a file.
            CryptoPP::SHA256 hash;
            std::string digest;
            std::filesystem::path master_folder = std::filesystem::path ("./Prova");
            master_folder /= event->relativePath;
            if (!std::filesystem::is_directory(master_folder)) {
               try {
                   CryptoPP::FileSource f(
                           master_folder.generic_string().c_str(),
                           true,
                           new CryptoPP::HashFilter(hash,
                                                    new CryptoPP::HexEncoder(new CryptoPP::StringSink(digest))));
                   std::cout << digest << std::endl;
               } catch(...){
                   std::cerr <<master_folder.c_str() << " -- Not computed" << std::endl;
                   continue;
               }
            }

            ///
            RawEndpoint re_test;
            re_test.raw_ip_address = "127.0.0.1";
            re_test.port_num = 3343;
            try {
                FileSipper(re_test, master_folder);
            }catch(std::exception& e){
                std::cerr << "Error " << e.what() << std::endl;
            }

        }



        //TODO decide where to store them.

    }

    update_callback();

}
/// Start the watcher.
/// \param path
void Watcher::Start(std::filesystem::path path){
    _watcher = std::make_unique<pfw::FileSystemWatcher>(
            path, std::chrono::milliseconds(1),
            std::bind(&Watcher::listenerFunction, this,
                      std::placeholders::_1));
}
