#include "slurm-server.hpp"
#include <iostream>
#include <string>
#include <chrono>
#include <thread>

// Needed for HTTPS, implies the need for openssl, may be omitted if HTTP suffices
#define CPPHTTPLIB_OPENSSL_SUPPORT

#include "../lib/umbridge.h"

int main (void ){
    
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
    SlurmModel bModel;
    
    umbridge::serveModels({&bModel}, "localhost", port);
}