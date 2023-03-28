#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cstdio>
#include "../lib/umbridge.h"

#define REGULAR_SERVER "./server.o" // only support C++ server for now

class LoadBalancer : public umbridge::Model
{
public:
    LoadBalancer() : umbridge::Model("slurm_LB") {}

    std::vector<std::size_t> GetInputSizes(const json &config_json) const override
    {
        // get size from the dummy server, can only make sense after starting a server
        return {1};
    }

    std::vector<std::size_t> GetOutputSizes(const json &config_json) const override
    {
        return {1};
    }

    std::vector<std::vector<double>> Evaluate(const std::vector<std::vector<double>> &inputs, json config) override
    {
        std::cout << "Request received in Load Balancer." << std::endl;

        // start a SLURM job for single request
        const std::string job_id = submitJob("sbatch empty_job.slurm");
        waitForJobState(job_id, "RUNNING"); // wait to start all nodes on the cluster, call scontrol for every 1 sceond to check

        const std::string node_name = getCommandOutput("scontrol show job " + job_id + " | grep -o 'NodeList=[^ ]*' | sed 's/NodeList=//'");

        std::cout << "node_name = " << node_name << std::endl;
        std::cout << "Start server: "
                  << "bash ./start_regular_server.sh " + job_id + " " + node_name + " ./server.o" << std::endl;
        // start regular server in the node and return the hostname and port
        //  the regular servers should host at the hostname instead of 0.0.0.0 or localhost
        const std::string server_url = getCommandOutput("bash ./start_regular_server.sh " + job_id + " " + node_name + " ./server.o");
        if (server_url.substr(0, 4) != "http")
        {
            std::cerr << "Start regular server failed." << std::endl;
            exit(-1);
        }

        // Start a client
        umbridge::HTTPModel client(server_url, umbridge::SupportedModels(server_url)[0]); // use the first model avaliable on server by default

        // Pass the arguments and get the output
        std::vector<std::vector<double>> outputs = client.Evaluate(inputs, config);

        // Cancel the SLURM job
        getCommandOutput("scancel " + job_id);

        return outputs; // return output as vector
    }

    bool SupportsEvaluate() override
    {
        return true;
    }

private:
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

    // return job id
    std::string submitJob(const std::string &command)
    {
        std::string sbatch_command;
        sbatch_command = command + " | awk '{print $4}'"; // extract job ID from sbatch output
        std::cout << "Submitting job with command: " << command << std::endl;

        std::string job_id = getCommandOutput(sbatch_command);
        if (!job_id.empty())
            job_id.pop_back(); // delete the line break

        return job_id;
    }

    // state = ["PENDING","RUNNING","COMPLETED"]
    void waitForJobState(const std::string &job_id, const std::string &state = "COMPLETED")
    {
        std::string command;
        command = "scontrol show job " + job_id + " | grep -oP '(?<=JobState=)[^ ]+'";
        std::cout << "Checking runtime: " << command << std::endl;
        std::string job_status;

        do
        {
            job_status = getCommandOutput(command);
            if (!job_status.empty())
                job_status.pop_back(); // delete the line break

            if (job_status == "")
            {
                std::cerr << "Wait for job completion failure." << std::endl;
                return;
            }
            // std::cout<<"Job status: "<<job_status<<std::endl;
            sleep(5);
        } while (job_status != state);
    }

    std::string getCommand(const std::vector<std::vector<double>> &input, std::string outputFile = "output.txt")
    {
        std::string command;
        command += "sbatch add_one.slurm ";
        command += writeToFile(input);
        command += " " + outputFile;
        return command;
    }

    void getOutput(std::vector<std::vector<double>> &output, const std::string filename = "output.txt")
    {
        readFromFile(output, filename);
    }

    std::string writeToFile(const std::vector<std::vector<double>> &data, const std::string &filename = "parameters.txt")
    {
        std::ofstream file(filename);

        if (!file.is_open())
        {
            std::cerr << "Error opening file" << std::endl;
            return "";
        }

        for (const auto &row : data)
        {
            for (const auto &elem : row)
            {
                file << elem << " ";
            }
            file << std::endl;
        }

        file.close();

        return filename; // return the name of output file
    }

    // write the data to output vector
    void readFromFile(std::vector<std::vector<double>> &output, const std::string &filename)
    {
        std::ifstream file(filename);

        if (!file.is_open())
        {
            std::cerr << "Error opening file" << std::endl;
            return;
        }

        std::string line;
        while (std::getline(file, line))
        {
            std::vector<double> row;
            std::stringstream ss(line);
            double elem;
            while (ss >> elem)
            {
                row.push_back(elem);
            }
            output.push_back(row);
        }

        file.close();
    }
};
