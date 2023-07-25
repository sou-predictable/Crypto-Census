#ifndef OUTPUTHANDLER_H
#define OUTPUTHANDLER_H

#include <fstream>
#include <string>
#include <queue>
#include <atomic>

class OutputHandler {
    public:
        OutputHandler();
        OutputHandler(std::string outputFile);
        void writeOutput(std::string output);
        void close();
    private:
        std::ofstream outputFileStream;
};

#endif