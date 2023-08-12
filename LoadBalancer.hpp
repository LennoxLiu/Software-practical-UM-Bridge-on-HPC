#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <tuple>
#include <memory>
#include <mutex>
#include "lib/umbridge.h"

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
    const std::string command = "scontrol show job " + job_id + " | grep -oP '(?<=JobState=)[^ ]+'";
    // std::cout << "Checking runtime: " << command << std::endl;
    std::string job_status;

    do
    {
        job_status = getCommandOutput(command);

        // Delete the line break
        if (!job_status.empty())
            job_status.pop_back();

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

    const std::string command = "while [ ! -f " + filename + " ]; do sleep 0.1; done";
    // std::cout << "Waiting for file: " << command << std::endl;
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

        std::cout << "job_id: " << job_id << std::endl;
        ++i;

    } while (waitForJobState(job_id, "RUNNING") == false && i < 3 && waitForFile("./urls/url-" + job_id + ".txt", 20) == false);
    // Wait to start all nodes on the cluster, call scontrol for every 1 sceond to check
    // Also wait until job is running and url file is written
    // Try maximum 3 times

    // Check if the job is running
    if (waitForJobState(job_id, "RUNNING") == false || waitForFile("./urls/url-" + job_id + ".txt", 10) == false)
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

class SingleSlurmJob
{
public:
    SingleSlurmJob(std::string model_name = "forward")
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
        std::cout << "Using model: " << model_name << std::endl;

        // Start a client, using unique pointer
        client_ptr = std::make_unique<umbridge::HTTPModel>(server_url, model_name); // use the first model avaliable on server by default
    }

    ~SingleSlurmJob()
    {
        std::cout << "Cancelling job..." << std::endl;
        // Cancel the SLURM job
        std::system(("scancel " + job_id).c_str());

        std::cout << "Deleting URL file..." << std::endl;
        // Delete the url text file
        std::system(("rm ./urls/url-" + job_id + ".txt").c_str());
    }

    std::unique_ptr<umbridge::HTTPModel> client_ptr;

    mutable std::mutex busy_mutex;

private:
    std::string job_id;
};

class LoadBalancer : public umbridge::Model
{
public:
    LoadBalancer(std::string name = "forward") : umbridge::Model(name)  
    {
        // Start a single slurm job by default. This could be made configurable if desired.
        slurm_jobs.push_back(std::make_unique<SingleSlurmJob>(name));
    }

    std::vector<std::size_t> GetInputSizes(const json &config_json = json::parse("{}")) const override
    {
        // get size from the dummy server, can only make sense after starting a server
        std::unique_lock<std::mutex> job_lock;
        auto& slurm_job_ptr = getIdleJob(job_lock); // Find an idle job or create a new one.
        return slurm_job_ptr->client_ptr->GetInputSizes(config_json);
    }

    std::vector<std::size_t> GetOutputSizes(const json &config_json = json::parse("{}")) const override
    {
        // get size from the dummy server, can only make sense after starting a server
        std::unique_lock<std::mutex> job_lock;
        auto& slurm_job_ptr = getIdleJob(job_lock); // Find an idle job or create a new one.
        return slurm_job_ptr->client_ptr->GetOutputSizes(config_json);
    }

    std::vector<std::vector<double>> Evaluate(const std::vector<std::vector<double>> &inputs, json config_json = json::parse("{}")) override
    {
        std::cout << "Request received in Load Balancer." << std::endl;

        std::unique_lock<std::mutex> job_lock;
        auto& slurm_job_ptr = getIdleJob(job_lock); // Find an idle job or create a new one.

        // Pass the arguments and get the output
        return slurm_job_ptr->client_ptr->Evaluate(inputs, config_json);
    }

    std::vector<double> Gradient(unsigned int outWrt,
                                 unsigned int inWrt,
                                 const std::vector<std::vector<double>> &inputs,
                                 const std::vector<double> &sens,
                                 json config_json = json::parse("{}")) override
    {
        std::unique_lock<std::mutex> job_lock;
        auto& slurm_job_ptr = getIdleJob(job_lock); // Find an idle job or create a new one.
        return slurm_job_ptr->client_ptr->Gradient(outWrt, inWrt, inputs, sens, config_json);
    }

    std::vector<double> ApplyJacobian(unsigned int outWrt,
                                      unsigned int inWrt,
                                      const std::vector<std::vector<double>> &inputs,
                                      const std::vector<double> &vec,
                                      json config_json = json::parse("{}")) override
    {
        std::unique_lock<std::mutex> job_lock;
        auto& slurm_job_ptr = getIdleJob(job_lock); // Find an idle job or create a new one.
        return slurm_job_ptr->client_ptr->ApplyJacobian(outWrt, inWrt, inputs, vec, config_json);
    }

    std::vector<double> ApplyHessian(unsigned int outWrt,
                                     unsigned int inWrt1,
                                     unsigned int inWrt2,
                                     const std::vector<std::vector<double>> &inputs,
                                     const std::vector<double> &sens,
                                     const std::vector<double> &vec,
                                     json config_json = json::parse("{}"))
    {
        std::unique_lock<std::mutex> job_lock;
        auto& slurm_job_ptr = getIdleJob(job_lock); // Find an idle job or create a new one.
        return slurm_job_ptr->client_ptr->ApplyHessian(outWrt, inWrt1, inWrt2, inputs, sens, vec, config_json);
    }

    bool SupportsEvaluate() override
    {
        std::unique_lock<std::mutex> job_lock;
        auto& slurm_job_ptr = getIdleJob(job_lock); // Find an idle job or create a new one.
        return slurm_job_ptr->client_ptr->SupportsEvaluate();
    }

    bool SupportsGradient() override
    {
        std::unique_lock<std::mutex> job_lock;
        auto& slurm_job_ptr = getIdleJob(job_lock); // Find an idle job or create a new one.
        return slurm_job_ptr->client_ptr->SupportsGradient();
    }

    bool SupportsApplyJacobian() override
    {
        std::unique_lock<std::mutex> job_lock;
        auto& slurm_job_ptr = getIdleJob(job_lock); // Find an idle job or create a new one.
        return slurm_job_ptr->client_ptr->SupportsApplyJacobian();
    }

    bool SupportsApplyHessian() override
    {
        std::unique_lock<std::mutex> job_lock;
        auto& slurm_job_ptr = getIdleJob(job_lock); // Find an idle job or create a new one.
        return slurm_job_ptr->client_ptr->SupportsApplyHessian();
    }

private:
    mutable std::mutex slurm_jobs_mutex;
    mutable std::vector<std::unique_ptr<SingleSlurmJob>> slurm_jobs;
    
    std::unique_ptr<SingleSlurmJob>& getIdleJob(std::unique_lock<std::mutex>& job_lock) const
    {
        std::unique_lock<std::mutex> guard(slurm_jobs_mutex); // Consider using a r/w-lock
        // Check if there is an idle job using a linear search
        // this can be improved if necessary for performance.
        for (auto& job : slurm_jobs) 
        {
            std::unique_lock<std::mutex> busy_lock(job->busy_mutex, std::try_to_lock);
            if (busy_lock) 
            {
                busy_lock.swap(job_lock);
                return job;
            }
        }

        // Create a new job if no idle jobs available
        guard.unlock();
        auto new_job = std::make_unique<SingleSlurmJob>(Model::name);
        std::unique_lock<std::mutex> busy_lock(new_job->busy_mutex);
        busy_lock.swap(job_lock);
    
        // Add new job to the list of jobs
        // Might be able to make this async?
        guard.lock();
        slurm_jobs.push_back(std::move(new_job));

        return slurm_jobs.back();
    }
};
