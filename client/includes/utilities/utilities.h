#pragma once

#include <string>
#include <iostream>

constexpr unsigned int str2int(const char* str, int h = 0)
{
    return !str[h] ? 5381 : (str2int(str, h+1) * 33) ^ str[h];
}

/**
 * Print help menu of the program
 * @param program_name
 */
static void show_usage(const std::string& program_name)
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

