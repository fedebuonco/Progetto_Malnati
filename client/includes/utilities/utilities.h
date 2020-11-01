//
// Created by fede on 9/21/20.
//

#pragma once

#include <string>
#include <iostream>

//Any source file that includes this will be able to use "DEBUG"
//The value is define inside utilities.cpp
//TODO CAPIRE MEGLIO https://stackoverflow.com/questions/10422034/when-to-use-extern-in-c/10422050#10422050
extern bool DEBUG;

constexpr unsigned int str2int(const char* str, int h = 0)
{
    return !str[h] ? 5381 : (str2int(str, h+1) * 33) ^ str[h];
}

static void show_usage(std::string program_name)
{
    std::cout << "\n\nHELP\n"
              <<"Usage: " << program_name << " <option(s)> SOURCES\n"
              << "Options:\n"
              << "\t-c,--credential\tCREDENTIAL\tSpecify the user credential. Example: \"-c username password\" \n"
              << "\t-f,--folder\tFOLDER\t\tSpecify the backup folder path\n"
              << "\t-s,--server\tSERVER\t\tSpecify the server address\n"
              << "\t-h,--help\tHELP\t\tShow this help message\n"
              << "\t-d,--debug\tDEBUG\t\tShow the debug messages during the execution"
              << std::endl;
}

