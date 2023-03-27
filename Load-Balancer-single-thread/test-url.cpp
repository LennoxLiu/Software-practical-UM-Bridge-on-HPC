#include <iostream>
#include <regex>
#include <sstream>

std::string getURL(const std::string& input){
    std::regex server_regex("Hosting server at: (http|https)://[a-zA-Z0-9]+(\\.[a-zA-Z0-9]+)*:[0-9]+");
    std::smatch match;
    std::smatch match_url;
    std::regex url_regex("(http|https)://[a-zA-Z0-9]+(\\.[a-zA-Z0-9]+)*:[0-9]+");
    std::stringstream ss;
    if (std::regex_search(input, match, server_regex))
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
int main()
{
    std::string input = "Environment variable PORT not set! Using port 4242 as default. Hosting server at: http://localhost:4242 Listening on port 4242...";
    
    std::cout<<getURL(input);
}