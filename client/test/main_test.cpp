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

    SECTION("Tree gen ./Prova") {
        RawEndpoint re;
        Client fede(re);
        CHECK(fede.GenerateTree(RELATIVE_DPATH) == "ce\n"
                                                   "dede/\n"
                                                   "dede/c.txt\n"
                                                   "marco/\n"
                                                   "marco/c.txt\n"
                                                   "provaaa.txt\n");
    }

    SECTION("Tree gen Prova") {
        RawEndpoint re;
        Client fede(re);
        CHECK(fede.GenerateTree(RELATIVE_PATH) == "ce\n"
                                                  "dede/\n"
                                                  "dede/c.txt\n"
                                                  "marco/\n"
                                                  "marco/c.txt\n"
                                                  "provaaa.txt\n");
    }

    SECTION("Tree gen Prova- absolute path") {
        RawEndpoint re;
        Client fede(re);
        CHECK(fede.GenerateTree(ABSOLUTE_PATH) == "ce\n"
                                                  "dede/\n"
                                                  "dede/c.txt\n"
                                                  "marco/\n"
                                                  "marco/c.txt\n"
                                                  "provaaa.txt\n");
    }

    SECTION("Tree gen No valid path") {
        RawEndpoint re;
        Client fede(re);
        CHECK_THROWS(fede.GenerateTree("./INVALIDPATH"));
    }

    SECTION("Tree gen empty folder") {
        RawEndpoint re;
        Client fede(re);
        CHECK(fede.GenerateTree(PATH_EMPTY_FOLDER) == "");
    }

}
// END - Tree Generation __________________________________________

// Patch Generation _______________________________________________
TEST_CASE("Computing Diffs") {
    RawEndpoint re;
    Client fede(re);

    SECTION( "Generating diff on same folder " ) {
        auto folder1 = fede.GenerateTree(RELATIVE_DPATH);
        auto folder2 = fede.GenerateTree(RELATIVE_DPATH);
        auto patch = fede.GeneratePatch(RELATIVE_DPATH, folder1, folder2);
        REQUIRE( patch.common_.size() != 0);
        REQUIRE( patch.added_.size() == 0);
        REQUIRE( patch.removed_.size() == 0);
    }

    SECTION( "Generating diff where folder2 is empty" ) {
        auto folder1 = fede.GenerateTree(RELATIVE_DPATH);
        auto folder2 = fede.GenerateTree(PATH_EMPTY_FOLDER);
        auto patch = fede.GeneratePatch(RELATIVE_DPATH, folder1, folder2);
        REQUIRE( patch.common_.size() == 0);
        REQUIRE( patch.added_.size() != 0);
        REQUIRE( patch.removed_.size() == 0);
    }

    SECTION( "Generating diff where folder1 is empty" ) {
        auto folder1 = fede.GenerateTree(PATH_EMPTY_FOLDER);
        auto folder2 = fede.GenerateTree(RELATIVE_DPATH);
        auto patch = fede.GeneratePatch(RELATIVE_DPATH, folder1, folder2);
        REQUIRE( patch.common_.size() == 0);
        REQUIRE( patch.added_.size() == 0);
        REQUIRE( patch.removed_.size() != 0);
    }

}
// END - Patch Generation _________________________________________

// Process Computations ___________________________________________
TEST_CASE("Process Computations") {
    RawEndpoint re;
    Client fede(re);

    SECTION( "Process removed with empty folder in client" ) {
        auto folder1 = fede.GenerateTree(PATH_EMPTY_FOLDER);
        auto folder2 = fede.GenerateTree(RELATIVE_DPATH);
        auto patch = fede.GeneratePatch(RELATIVE_DPATH, folder1, folder2);
        REQUIRE( patch.removed_.size() != 0);
        REQUIRE( patch.added_.size() == 0);
        REQUIRE( patch.common_.size() == 0);
        fede.ProcessRemoved(patch);
        REQUIRE( patch.to_be_deleted_ == "ce\n"
                         "dede/\n"
                         "dede/c.txt\n"
                         "marco/\n"
                         "marco/c.txt\n"
                         "provaaa.txt\n");
    }

    SECTION( "Process New with empty folder in server " ) {
        auto folder1 = fede.GenerateTree(RELATIVE_DPATH);
        auto folder2 = fede.GenerateTree(PATH_EMPTY_FOLDER);
        auto patch = fede.GeneratePatch(RELATIVE_DPATH, folder1, folder2);
        REQUIRE( patch.removed_.size() == 0);
        REQUIRE( patch.added_.size() != 0);
        REQUIRE( patch.common_.size() == 0);
        fede.ProcessNew(patch);
        REQUIRE( patch.to_be_sent_map_.size() == patch.added_.size());
    }


}
// END - Process Computations _____________________________________

// Other ___________________________________________
TEST_CASE("Other") {
//TODO Test prettyprint
}
// END - Other _____________________________________
