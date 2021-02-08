#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "client.h"
#include "config.h"
//Unit Test


//DEFINES _______________________________________
#define RELATIVE_PATH "Prova"
#define RELATIVE_DPATH "./Prova"
#define ABSOLUTE_PATH  "/home/fede/Documents/Progetto_Malnati/client/test/cmake-build-debug/Prova"
#define PATH_EMPTY_FOLDER "./empty"
#define RELATIVE_PATH_2 "Prova2"

// Tree Generation _______________________________________________
TEST_CASE("Tree Generations"){
    RawEndpoint re;
    Client client(re);

    SECTION("Tree gen ./Prova") {
        CHECK(client.GenerateTree(RELATIVE_DPATH) == "ce\n"
                                                   "dede/\n"
                                                   "dede/c.txt\n"
                                                   "marco/\n"
                                                   "marco/c.txt\n"
                                                   "provaaa.txt\n");
    }

    SECTION("Tree gen Prova") {
        CHECK(client.GenerateTree(RELATIVE_PATH) == "ce\n"
                                                  "dede/\n"
                                                  "dede/c.txt\n"
                                                  "marco/\n"
                                                  "marco/c.txt\n"
                                                  "provaaa.txt\n");
    }

    SECTION("Tree gen Prova- absolute path") {
        CHECK(client.GenerateTree(ABSOLUTE_PATH) == "ce\n"
                                                  "dede/\n"
                                                  "dede/c.txt\n"
                                                  "marco/\n"
                                                  "marco/c.txt\n"
                                                  "provaaa.txt\n");
    }

    SECTION("Tree gen No valid path") {
        CHECK_THROWS(client.GenerateTree("./INVALIDPATH"));
    }

    SECTION("Tree gen empty folder") {
        CHECK(client.GenerateTree(PATH_EMPTY_FOLDER) == "");
    }

}
// END - Tree Generation __________________________________________

// Patch Generation _______________________________________________
TEST_CASE("Computing Diffs") {
    RawEndpoint re;
    Client client(re);

    SECTION( "Generating diff on same folder " ) {
        auto folder1 = client.GenerateTree(RELATIVE_DPATH);
        auto folder2 = client.GenerateTree(RELATIVE_DPATH);
        auto patch = client.GeneratePatch(RELATIVE_DPATH, folder1, folder2);
        REQUIRE( patch.common_.size() != 0);
        REQUIRE( patch.added_.size() == 0);
        REQUIRE( patch.removed_.size() == 0);
    }

    SECTION( "Generating diff where folder2 is empty" ) {
        auto folder1 = client.GenerateTree(RELATIVE_DPATH);
        auto folder2 = client.GenerateTree(PATH_EMPTY_FOLDER);
        auto patch = client.GeneratePatch(RELATIVE_DPATH, folder1, folder2);
        REQUIRE( patch.common_.size() == 0);
        REQUIRE( patch.added_.size() != 0);
        REQUIRE( patch.removed_.size() == 0);
    }

    SECTION( "Generating diff where folder1 is empty" ) {
        auto folder1 = client.GenerateTree(PATH_EMPTY_FOLDER);
        auto folder2 = client.GenerateTree(RELATIVE_DPATH);
        auto patch = client.GeneratePatch(RELATIVE_DPATH, folder1, folder2);
        REQUIRE( patch.common_.size() == 0);
        REQUIRE( patch.added_.size() == 0);
        REQUIRE( patch.removed_.size() != 0);
    }

}
// END - Patch Generation _________________________________________

// Process Computations ___________________________________________
TEST_CASE("Process Computations") {
    RawEndpoint re;
    Client client(re);

    SECTION( "Process removed with empty folder in client" ) {
        auto folder1 = client.GenerateTree(PATH_EMPTY_FOLDER);
        auto folder2 = client.GenerateTree(RELATIVE_DPATH);
        auto patch = client.GeneratePatch(RELATIVE_DPATH, folder1, folder2);
        REQUIRE( patch.removed_.size() != 0);
        REQUIRE( patch.added_.size() == 0);
        REQUIRE( patch.common_.size() == 0);
        client.ProcessRemoved(patch);
        REQUIRE( patch.to_be_deleted_ == "ce\n"
                         "dede/\n"
                         "dede/c.txt\n"
                         "marco/\n"
                         "marco/c.txt\n"
                         "provaaa.txt\n");
    }

    SECTION( "Process New with empty folder in server " ) {
        auto folder1 = client.GenerateTree(RELATIVE_DPATH);
        auto folder2 = client.GenerateTree(PATH_EMPTY_FOLDER);
        auto patch = client.GeneratePatch(RELATIVE_DPATH, folder1, folder2);
        REQUIRE( patch.removed_.size() == 0);
        REQUIRE( patch.added_.size() != 0);
        REQUIRE( patch.common_.size() == 0);
        client.ProcessNew(patch);
        REQUIRE( patch.to_be_sent_map_.size() == patch.added_.size());
    }


}
// END - Process Computations _____________________________________

// Other ___________________________________________
TEST_CASE("prettyPrint") {
    RawEndpoint re;
    Client client(re);

    SECTION( "Prettyprint when client is empty" ) {
        auto folder1 = client.GenerateTree(PATH_EMPTY_FOLDER);
        auto folder2 = client.GenerateTree(RELATIVE_DPATH);
        auto patch = client.GeneratePatch(RELATIVE_DPATH, folder1, folder2);
        REQUIRE( patch.removed_.size() != 0);
        REQUIRE( patch.added_.size() == 0);
        REQUIRE( patch.common_.size() == 0);
        TreeT server_th{folder2, ""};
        client.ProcessRemoved(patch);
        client.ProcessNew(patch);
        client.ProcessCommon(patch,server_th);
        REQUIRE( patch.PrettyPrint() == "\n:::::::: Changes ::::::::\n"
                                        "- ce\n"
                                        "- dede/\n"
                                        "- dede/c.txt\n"
                                        "- marco/\n"
                                        "- marco/c.txt\n"
                                        "- provaaa.txt\n"
                                        ":::::::: Deleted Files ::::::::\n"
                                        "ce\n"
                                        "dede/\n"
                                        "dede/c.txt\n"
                                        "marco/\n"
                                        "marco/c.txt\n"
                                        "provaaa.txt\n"
                                        ":::::::: Files that will be Sent - Last Modified Time ::::::::\n");

    }

    SECTION( "Prettyprint when server is empty" ) {
        auto folder1 = client.GenerateTree(RELATIVE_DPATH);
        auto folder2 = client.GenerateTree(PATH_EMPTY_FOLDER);
        auto patch = client.GeneratePatch(RELATIVE_DPATH, folder1, folder2);
        REQUIRE( patch.removed_.size() == 0);
        REQUIRE( patch.added_.size() != 0);
        REQUIRE( patch.common_.size() == 0);
        TreeT server_th{folder2, ""};
        client.ProcessRemoved(patch);
        client.ProcessNew(patch);
        client.ProcessCommon(patch,server_th);
        REQUIRE( patch.PrettyPrint() == ":::::::: Changes ::::::::\n"
                                        "+ ce\n"
                                        "+ dede/\n"
                                        "+ dede/c.txt\n"
                                        "+ marco/\n"
                                        "+ marco/c.txt\n"
                                        "+ provaaa.txt\n"
                                        ":::::::: Deleted Files ::::::::\n"
                                        ":::::::: Files that will be Sent - Last Modified Time ::::::::\n"
                                        "ce - 1605546130\n"
                                        "dede/ - 1605546130\n"
                                        "dede/c.txt - 1605546130\n"
                                        "marco/ - 1605546130\n"
                                        "marco/c.txt - 1605546130\n"
                                        "provaaa.txt - 1605546130\n"
                                        );

    }

}
// END - Other _____________________________________
