#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include "../lib/umbridge.h"

class BashCommandModel : public umbridge::Model {
public:
    BashCommandModel() : umbridge::Model("bash_command") {}

    std::vector<std::size_t> GetInputSizes(const json& config_json) const override {
        return {1}; // one-dimensional input vector
    }

    std::vector<std::size_t> GetOutputSizes(const json& config_json) const override {
        return {1}; // one-dimensional output vector
    }

    std::vector<std::vector<double>> Evaluate(const std::vector<std::vector<double>>& inputs, json config) override {
        const std::string command = GetCommand(inputs[0][0]); // construct bash command with input argument
        const std::string output = RunCommand(command); // execute command and get its output
        const double result = std::stod(output); // convert output string to double
        return {{result}}; // return output as vector
    }

    bool SupportsEvaluate() override {
        return true;
    }

private:
    std::string GetCommand(double input) const {
        const std::string command = "bash <command> " + std::to_string(input); // construct bash command with input argument
        return command;
    }

    std::string RunCommand(const std::string& command) const {
        std::string output;
        FILE* pipe = popen(command.c_str(), "r"); // open pipe to execute command
        if (!pipe) {
            std::cerr << "Error executing bash command" << std::endl;
            return output; // return empty string on error
        }
        char buffer[128];
        while (fgets(buffer, 128, pipe) != nullptr) {
            output += buffer; // append output of command to output string
        }
        pclose(pipe); // close pipe
        return output;
    }
};
