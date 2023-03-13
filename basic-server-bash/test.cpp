#include "um-bridge-server.hpp"
#include <iostream>

int main (void ){
    BashCommandModel bModel;
    bModel.Evaluate();
    std::cout<<"test finished."<<std::endl;
}