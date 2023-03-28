#include "load-balancer.hpp"
#include <iostream>
#include <string>
#include <chrono>
#include <thread>

// Needed for HTTPS, implies the need for openssl, may be omitted if HTTP suffices
#define CPPHTTPLIB_OPENSSL_SUPPORT

#include "../lib/umbridge.h"

// run and get the result of command
std::string getCommandOutput(const std::string command)
{
    FILE *pipe = popen(command.c_str(), "r"); // execute the command and return the output as stream
    if (!pipe)
    {
        std::cerr << "Failed to execute the command: " + command << std::endl;
        return "";
    }

    char buffer[128];
    std::string output;
    while (fgets(buffer, 128, pipe))
    {
        output += buffer;
    }
    pclose(pipe);

    return output;
}

int main(int argc, char *argv[])
{

    // Read environment variables for configuration
    char const *port_cstr = std::getenv("PORT");
    int port = 0;
    if (port_cstr == NULL)
    {
        std::cout << "Environment variable PORT not set! Using port 4242 as default." << std::endl;
        port = 4242;
    }
    else
    {
        port = atoi(port_cstr);
    }

    // Set up and serve model
    LoadBalancer LB;

    std::string hostname = "localhost";
    if (argc == 2)
    {
        hostname = argv[1];
    }
    else
    {
        hostname = getCommandOutput("hostname"); // get the hostname of node
        if(!hostname.empty())
            hostname.pop_back();
    }

    std::cout << "Hosting server at: http://" << hostname << ":" << port << std::endl;
    umbridge::serveModels({&LB}, hostname, port);
}