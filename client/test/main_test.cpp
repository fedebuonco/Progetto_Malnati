#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "client.h"
#include "config.h"
//Unit Test



// Tree Generation _______________________________________________
TEST_CASE("Tree gen ./Prova") {
    RawEndpoint re;
    Client fede(re);
    REQUIRE( fede.GenerateTree("./Prova") ==  "provaaa.txt\n"
                                              "dede/\n"
                                              "dede/c.txt\n"
                                              "marco/\n"
                                              "marco/c.txt\n");
}

TEST_CASE("Tree gen Prova") {
    RawEndpoint re;
    Client fede(re);
    REQUIRE( fede.GenerateTree("Prova") ==  "provaaa.txt\n"
                                              "dede/\n"
                                              "dede/c.txt\n"
                                              "marco/\n"
                                              "marco/c.txt\n");
}

TEST_CASE("Tree gen Prova- absolute path") {
    RawEndpoint re;
    Client fede(re);
    REQUIRE( fede.GenerateTree("/home/fede/Documents/Progetto_Malnati/client/test/cmake-build-debug/Prova") ==  "provaaa.txt\n"
                                            "dede/\n"
                                            "dede/c.txt\n"
                                            "marco/\n"
                                            "marco/c.txt\n");
}