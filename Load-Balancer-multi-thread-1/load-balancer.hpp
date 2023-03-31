#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <tuple>
#include <memory>
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

// state = ["PENDING","RUNNING","COMPLETED","FAILED","CANCELLED"]
bool waitForJobState(const std::string &job_id, const std::string &state = "COMPLETED")
{
    std::string command;
    command = "scontrol show job " + job_id + " | grep -oP '(?<=JobState=)[^ ]+'";
    // std::cout << "Checking runtime: " << command << std::endl;
    std::string job_status;

    do
    {
        job_status = getCommandOutput(command);
        if (!job_status.empty())
            job_status.pop_back(); // delete the line break

        // Don't wait if there is an error or the job is ended
        if (job_status == "" || (state != "COMPLETE" && job_status == "COMPLETED") || job_status == "FAILED" || job_status == "CANCELLED")
        {
            std::cerr << "Wait for job status failure, status : " << job_status << std::endl;
            return false;
        }
        // std::cout<<"Job status: "<<job_status<<std::endl;
        sleep(1);
    } while (job_status != state);

    return true;
}

// Check for every 100 ms, wait for maximum 20 second
bool waitForFile(const std::string &filename, int time_out = 20)
{
    auto start_time = std::chrono::steady_clock::now();
    auto timeout = std::chrono::seconds(time_out); // wait for maximum 10 seconds

    std::string command = "while [ ! -f " + filename + " ]; do sleep 0.1; done";
    std::system(command.c_str());
    auto end_time = std::chrono::steady_clock::now();

    if (end_time - start_time > timeout)
    {
        std::cerr << "Timeout reached waiting for file " << filename << std::endl;
        return false;
    }

    return true;
}

// Start a slurm job and return job id
std::string submitJob(const std::string &command)
{
    std::string sbatch_command = command + " | awk '{print $4}'"; // extract job ID from sbatch output
    std::cout << "Submitting job with command: " << command << std::endl;

    std::string job_id;
    int i = 0;
    do
    {
        job_id = getCommandOutput(sbatch_command);

        // Delete the line break
        if (!job_id.empty())
            job_id.pop_back();

        ++i;

    } while (waitForJobState(job_id, "RUNNING") == false && i < 3 && waitForFile("./urls/url-" + job_id + ".txt", 20) == false);
    // Wait to start all nodes on the cluster, call scontrol for every 1 sceond to check
    // Also wait until job is running and url file is written
    // Try maximum 3 times

    // Check if the job is running
    if (waitForJobState(job_id, "RUNNING") == false || waitForFile("./urls/url-" + job_id + ".txt", 5) == false)
    {
        std::cout << "Submit job failure." << std::endl;
        exit(-1);
    }

    return job_id;
}

std::string readUrl(const std::string &filename)
{
    std::ifstream file(filename);
    std::string url;
    if (file.is_open())
    {
        std::string file_contents((std::istreambuf_iterator<char>(file)),
                                  (std::istreambuf_iterator<char>()));
        url = file_contents;
        file.close();
    }
    else
    {
        std::cerr << "Unable to open file " << filename << " ." << std::endl;
    }

    // delete the line break
    if (!url.empty())
        url.pop_back();

    return url;
}

/*
    // check whether the server starts successfully and return the url of server
    std::string checkAndGetURL(const std::string &input)
    {
        std::regex server_regex("Hosting server at: (http|https)://[a-zA-Z0-9]+(\\.[a-zA-Z0-9]+)*:[0-9]+");
        std::smatch match;
        std::smatch match_url;
        std::regex url_regex("(http|https)://[a-zA-Z0-9]+(\\.[a-zA-Z0-9]+)*:[0-9]+");
        std::stringstream ss;
        if (std::regex_search(input, match, server_regex)) // check if the server starts
        {
            ss << match[0];
            std::string input2 = ss.str();
            if (std::regex_search(input2, match_url, url_regex))
            {
                // std::cout << "Found URL: " << match_url[0] << std::endl;
                return match_url[0];
            }
        }
        return "";
    }
*/

class SingleSlurmJob
{
public:
    SingleSlurmJob()
    {
        // start a SLURM job for single request
        job_id = submitJob("sbatch regular-server.slurm");

        const std::string server_url = readUrl("./urls/url-" + job_id + ".txt"); // read server url from txt file
        // May use $SLURM_LOCALID in a .slurm file later

        std::cout << "Hosting sub-server at : " << server_url << std::endl;

        // List supported models
        std::vector<std::string> models = umbridge::SupportedModels(server_url);
        std::cout << "Supported models: " << std::endl;
        for (auto model : models)
        {
            std::cout << "  " << model << std::endl;
        }

        // Start a client, using unique pointer
        client_ptr = std::make_unique<umbridge::HTTPModel>(server_url, models[0]); // use the first model avaliable on server by default
    }

    ~SingleSlurmJob()
    {
        // Cancel the SLURM job
        std::system(("scancel " + job_id).c_str());

        // Delete the url text file
        std::system(("rm ./urls/url-" + job_id + ".txt").c_str());
    }

    std::unique_ptr<umbridge::HTTPModel> client_ptr;

private:
    std::string job_id;
};

class LoadBalancer : public umbridge::Model
{
public:
    LoadBalancer() : umbridge::Model("slurm_LB") {}

    std::vector<std::size_t> GetInputSizes(const json &config_json) const override
    {
        // get size from the dummy server, can only make sense after starting a server
        SingleSlurmJob slurm_job; // start a new SLURM job;
        return slurm_job.client_ptr->GetInputSizes(config_json);
    }

    std::vector<std::size_t> GetOutputSizes(const json &config_json) const override
    {
        SingleSlurmJob slurm_job; // start a new SLURM job;
        return slurm_job.client_ptr->GetOutputSizes(config_json);
    }

    std::vector<std::vector<double>> Evaluate(const std::vector<std::vector<double>> &inputs, json config) override
    {
        std::cout << "Request received in Load Balancer." << std::endl;

        SingleSlurmJob slurm_job; // start a new SLURM job

        // Pass the arguments and get the output
        std::vector<std::vector<double>> outputs = slurm_job.client_ptr->Evaluate(inputs, config);

        return outputs; // return output as vector
    }

    bool SupportsEvaluate() override
    {
        SingleSlurmJob slurm_job; // start a new SLURM job;
        return slurm_job.client_ptr->SupportsEvaluate();
    }

private:
};
