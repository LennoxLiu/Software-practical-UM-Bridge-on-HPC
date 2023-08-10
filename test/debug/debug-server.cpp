#include <iostream>
#include <string>
#include <chrono>
#include <ctime>
#include <thread>
#include <random>

// Needed for HTTPS, implies the need for openssl, may be omitted if HTTP suffices
#define CPPHTTPLIB_OPENSSL_SUPPORT

#include "../../lib/umbridge.h"

class ExampleModel : public umbridge::Model
{
public:
    ExampleModel(int test_delay, std::string name = "forward")
        : umbridge::Model(name),
          test_delay(test_delay),
          id(generateID())
    {
    }

    // Define input and output dimensions of model (here we have a single vector of length 1 for input; same for output)
    std::vector<std::size_t> GetInputSizes(const json &config_json) const override
    {
        return {1};
    }

    std::vector<std::size_t> GetOutputSizes(const json &config_json) const override
    {
        return {5};
    }

    std::vector<std::vector<double>> Evaluate(const std::vector<std::vector<double>> &inputs, json config) override
    {
        auto start = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        //std::cout << "Model (" << id << ") received request at " << std::ctime(&start) << std::endl;
        
        // Use an artificial delay to simulate actual work being done
        std::this_thread::sleep_for(std::chrono::milliseconds(test_delay));

        auto end = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        auto duration = std::difftime(end, start);
        //std::cout << "Model (" << id << ") finished request at " << std::ctime(&end) << "(" << std::difftime(end, start) << "s)" << std::endl;
        // Input is the time at which the request was sent from client
        std::vector<std::vector<double>> result {{(double) id, inputs[0][0], (double) start, (double) end, duration}};
        return result;
    }

    // Specify that our model supports evaluation. Jacobian support etc. may be indicated similarly.
    bool SupportsEvaluate() override
    {
        return true;
    }

private:
    int test_delay;
    int id;

    int generateID() {
        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<int> dist(0, 1000000);
        return dist(rng);
    }
};

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

    char const *delay_cstr = std::getenv("TEST_DELAY");
    int test_delay = 0;
    if (delay_cstr != NULL)
    {
        test_delay = atoi(delay_cstr);
    }
    std::cout << "Evaluation delay set to " << test_delay << " ms." << std::endl;

    // Set up and serve model
    ExampleModel model(test_delay);
    ExampleModel model2(5000, "backward");
    ExampleModel model3(10, "inward");
    ExampleModel model4(5, "outward");
    
    std::string hostname = "0.0.0.0";
    /*
    if (argc == 2)
    {
        hostname = argv[1];
    }
    else
    {
        hostname = getCommandOutput("hostname"); // get the hostname of node
        // delete the line break
        if (!hostname.empty())
            hostname.pop_back();
    }
    */
    std::cout << "Hosting server at : "
              << "http://" << hostname << ":" << port << std::endl;
    umbridge::serveModels({&model,&model2,&model3,&model4}, hostname, port); // start server at the hostname

    return 0;
}