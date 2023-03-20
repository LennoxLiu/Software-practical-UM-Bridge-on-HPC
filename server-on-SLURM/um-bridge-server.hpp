#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <unistd.h>
#include "../lib/umbridge.h"

#define N 3

class BashCommandModel : public umbridge::Model
{
public:
    BashCommandModel() : umbridge::Model("bash_command") {}

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
        const std::string command = getCommand(inputs, "output.txt"); // construct bash command with input argument
        const std::string job_id = submitJob(command);
                                                   // execute command and write its result to a file
        waitForJobCompletion(job_id); //call scontrol for every 1 sceond to check

        std::vector<std::vector<double>> output;
        getOutput(output, "output.txt"); // get output from output file

        return output; // return output as vector
    }

    bool SupportsEvaluate() override
    {
        return true;
    }

private:
    std::string submitJob(const std::string &command)
    {
        std::cout << "Submitting job with command: " << command << std::endl;
        std::string sbatch_command;
        sbatch_command = command + " | awk '{print $4}'"; // extract job ID from sbatch output
        std::string job_id;
        std::ifstream infile(std::system(sbatch_command.c_str())); //can not get the id correctly yet
        if (infile >> job_id)
        {
            std::cout << "Job submitted with ID: " << job_id << std::endl;
            return job_id;
        }
        else
        {
            std::cerr << "Failed to submit job" << std::endl;
            exit(1);
        }
    }

    void waitForJobCompletion(const std::string &job_id)
    {
        std::string command;
        command = "scontrol show job " + job_id + " | grep JobState";
        std::string job_status;
        do
        {
            job_status.clear();
            std::ifstream infile(std::system(command.c_str()));
            if (infile >> job_status)
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
