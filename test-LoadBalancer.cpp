#include "LoadBalancer.hpp"
#include <iostream>
#include <string>
#include <chrono>
#include <thread>

// Needed for HTTPS, implies the need for openssl, may be omitted if HTTP suffices
#define CPPHTTPLIB_OPENSSL_SUPPORT

#include "lib/umbridge.h"

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

    std::string hostname;
    if (argc == 2)
    {
        hostname = argv[1]; // Receive hostname from command line
    }
    else
    {
        // Get the hostname of node
        hostname = getCommandOutput("hostname");

        // Delete line break
        if (!hostname.empty())
            hostname.pop_back();
    }

    // Start: Instaltialize multiple LB classes for multiple models on the regular server

    // start a SLURM job for single request
    const std::string job_id = submitJob("sbatch regular-server.slurm");
    const std::string server_url = readUrl("./urls/url-" + job_id + ".txt"); // read server url from txt file
    // May use $SLURM_LOCALID in a .slurm file later
    std::cout << "Hosting sub-server at : " << server_url << std::endl;
    // List supported models
    std::vector<std::string> model_names = umbridge::SupportedModels(server_url);

    std::vector<std::unique_ptr<LoadBalancer>> LB_vector;
    for (auto model_name : model_names)
    {
        // Set up and serve model
        LB_vector.push_back(std::make_unique<LoadBalancer>(model_name));
    }

    // End: Instaltialize multiple LB classes for multiple models on the regular server

    // Create a new vector of raw pointers to LB_vector
    std::vector<umbridge::Model *> LB_ptr_vector(LB_vector.size());
    std::transform(LB_vector.begin(), LB_vector.end(), LB_ptr_vector.begin(),
                   [](auto& obj) { return obj.get(); });

    std::cout << "Hosting server at: http://" << hostname << ":" << port << std::endl;
    umbridge::serveModels(LB_ptr_vector, hostname, port, false);
}