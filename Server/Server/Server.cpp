// Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>

#include "queue_names.h"

int main()
{
    std::cout << "Server running" << std::endl;

    int request_num = 0;
    std::string request_file_name = QUEUE_INPUT_DIRECTORY + "\\" + QUEUE_INPUT_FILE;
    std::string response_file_name = QUEUE_OUTPUT_DIRECTORY + "\\" + QUEUE_OUTPUT_FILE;

    bool is_running = true;
    while (is_running)
    {
        Sleep(100);

        // Look for the request file in the input queue
        std::ifstream request_file;
        request_file.open(request_file_name, std::ifstream::in);
        if (request_file.is_open())
        {
            request_num++;
            std::string request_line;
            std::string response;

            std::cout << "***** request " << request_num << " *****" << std::endl;
            int request_line_num = 0;
            while (std::getline(request_file, request_line))
            {
                request_line_num++;
                if (request_line == "EXIT")
                {
                    is_running = false;
                }

                std::cout << request_line << std::endl;
                response += request_line;
                response += "\n";
            }
            request_file.close();

            // delete the request file
            BOOL bDelete = DeleteFileA(request_file_name.c_str());
            if (bDelete == TRUE)
            {
                std::cout << "=> request processed" << std::endl;
            }

            // write the response file to the output queue
            std::ofstream response_file;
            response_file.open(response_file_name, std::ofstream::out);
            if (response_file.is_open())
            {
                response += "=> SUCCESS\n";
                response_file << response;
                response_file.flush();
                response_file.close();
            }
        }
    }

    return 0;
}
