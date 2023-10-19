#include "LoadBalancer.hpp"
#include <iostream>
#include <string>
#include <chrono>
#include <thread>

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

    // Start: Instaltialize multiple LB classes for multiple models on the regular server

    // start a SLURM job for single request
    const std::string job_id = submitJob("sbatch regular-server.slurm");
    const std::string server_url = readUrl("./urls/url-" + job_id + ".txt"); // read server url from txt file
    // May use $SLURM_LOCALID in a .slurm file later
    std::cout << "Hosting sub-server at : " << server_url << std::endl;
    // List supported models
    std::vector<std::string> model_names = umbridge::SupportedModels(server_url);

    std::vector<LoadBalancer> LB_vector;
    for (auto model_name : model_names)
    {
        // Set up and serve model
        LB_vector.emplace_back(LoadBalancer{model_name});
    }

    // End: Instaltialize multiple LB classes for multiple models on the regular server

    // Create a new vector of pointers to LB_vector
    std::vector<umbridge::Model *> LB_ptr_vector(LB_vector.size());
    std::transform(LB_vector.begin(), LB_vector.end(), LB_ptr_vector.begin(),
                   [](LoadBalancer& obj) { return &obj; });

    std::cout << "Load balancer running and bound to 0.0.0.0:" << port << std::endl;
    umbridge::serveModels(LB_ptr_vector, "0.0.0.0", port, false);
}