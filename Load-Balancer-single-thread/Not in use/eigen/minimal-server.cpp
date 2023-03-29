#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include "solver.hh"

// Needed for HTTPS, implies the need for openssl, may be omitted if HTTP suffices
#define CPPHTTPLIB_OPENSSL_SUPPORT

#include "umbridge.h"
#define N 10

class ExampleModel : public umbridge::Model
{
public:
    ExampleModel(int test_delay)
        : umbridge::Model("get_eigenvector_eigenvalue"),
          test_delay(test_delay)
    {
    }

    // Define input and output dimensions of model (here we have a single vector of length 1 for input; same for output)
    std::vector<std::size_t> GetInputSizes(const json &config_json) const override
    {
        // int N = config_json["N"];
        return {N*N};
    }

    std::vector<std::size_t> GetOutputSizes(const json &config_json) const override
    {
        return {N, 1};
    }

    std::vector<std::vector<double>> Evaluate(const std::vector<std::vector<double>> &inputs, json config) override
    {
        // Do the actual model evaluation; here we just multiply the first entry of the first input vector by two, and store the result in the output.
        // In addition, we support an artificial delay here, simulating actual work being done.

        
        const std::vector<double> &data = inputs[0];
        std::vector<std::vector<double>> entries;
        std::vector<double> temp_vec;

        // int N = config["N"];
        // Divide N*N vector into N vectors with size N
        for (auto it = data.begin(); it != data.end(); it += N)
        {
            temp_vec.assign(it, it + N);
            entries.push_back(temp_vec);
        }
        Matrix A(entries);

        std::vector<double> eigenvector = get_eigenvector(A, config["steps"]); 
        std::vector<std::vector<double>> result;
        result.push_back(eigenvector);

        std::vector<double> eigenvalue = {get_eigenvalue(A,eigenvector)};
        result.push_back(eigenvalue);

        return result;
    }

    // Specify that our model supports evaluation. Jacobian support etc. may be indicated similarly.
    bool SupportsEvaluate() override
    {
        return true;
    }

private:
    int test_delay;
};

int main()
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
    umbridge::serveModels({&model}, "localhost", port);

    return 0;
}