#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cstdio>
#include "../lib/umbridge.h"

#define N 3

class SlurmModel : public umbridge::Model
{
public:
    SlurmModel() : umbridge::Model("slurm_command") {}

    std::vector<std::size_t> GetInputSizes(const json &config_json) const override
    {
        return {N, N};
    }

    std::vector<std::size_t> GetOutputSizes(const json &config_json) const override
    {
        return {N, N};
    }

    std::vector<std::vector<double>> Evaluate(const std::vector<std::vector<double>> &inputs, json config) override
    {
        std::cout<<"Request received in server."<<std::endl;
        const std::string command = getCommand(inputs, "output.txt"); // construct bash command with input argument
        const std::string job_id = submitJob(command);
        // execute command and write its result to a file
        waitForJobCompletion(job_id); // call scontrol for every 1 sceond to check

        std::vector<std::vector<double>> output;
        getOutput(output, "output.txt"); // get output from output file

        return output; // return output as vector
    }

    bool SupportsEvaluate() override
    {
        return true;
    }

private:
    std::string getCommandOutput(const std::string command)
    {
        FILE *pipe = popen(command.c_str(), "r"); // execute the command and return the output as stream
        if (!pipe)
        {
            std::cerr << "Failed to execute the command." << std::endl;
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

    //return job id
    std::string submitJob(const std::string &command)
    {
        std::cout << "Submitting job with command: " << command << std::endl;
        std::string sbatch_command;
        sbatch_command = command + " | awk '{print $4}'"; // extract job ID from sbatch output
        std::string job_id = getCommandOutput(sbatch_command);
        if (!job_id.empty())
            job_id.pop_back(); //delete the line break
    }

    void waitForJobCompletion(const std::string &job_id)
    {
        std::string command;
        command = "scontrol show job " + job_id + " | grep JobState";
        std::cout<<"Checking runtime: "<<command<<std::endl;
        std::string job_status;

        do
        {
            job_status = getCommandOutput(command);
            if (job_status == "")
            {
                std::cerr << "Wait for job completion failure." << std::endl;
                return;
            }
            else
            {
                job_status = job_status.substr(9); // remove "JobState="
            }
            sleep(1);
        } while (job_status != "COMPLETED");
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
