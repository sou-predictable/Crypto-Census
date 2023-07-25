#include "OutputHandler.h"
#include <fstream>
#include <string>
#include <thread>
#include <queue>
#include <atomic>

OutputHandler::OutputHandler() {
    outputFileStream = std::ofstream("output.txt", std::ofstream::out);
}

OutputHandler::OutputHandler(std::string outputFile) {
    outputFileStream = std::ofstream(outputFile.c_str(), std::ofstream::out);
}

void OutputHandler::writeOutput(std::string output) {
    outputFileStream << output << std::endl;
}

void OutputHandler::close() {
    outputFileStream.close();
}