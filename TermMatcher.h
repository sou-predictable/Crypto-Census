#ifndef TERMMATCHER_H
#define TERMMATCHER_H

#include "Config.h"
#include <fstream>
#include <string>
#include <unordered_set>
#include <vector>

/**
 * TermMatcher is used to determine if a certain number of unique terms exist within a vector of strings.
 */
class TermMatcher {
    public:

        // Default constructor
        TermMatcher() = default;

        /**
         * Constructor with int input. Also calls setTerms.
         * 
         * @param numTerms the number of terms that must be discovered before a positive classification is returned.
         */
        TermMatcher(int numTerms);

        /**
         * Constructor with a config input. Also calls setTerms.
         * 
         * @overload
         */
        TermMatcher(Config* configs);

        /**
         * Enumerates a vector of strings and uses std::string.find() to check whether a number of unique terms exists within the vector.
         * 
         * The terms provided should be capitalized if the search is case insensitive. TermMatcher automatically extracts terms from the 
         * "terms.txt" file in the project's directory.
         * 
         * @param data the vector of strings to be checked.
         * @param caseSensitive whether case sensitivity applies. 
         * @returns true if the number of unique terms matched is greater than the number of required terms; returns false otherwise
         * @returns true if terms list is empty
         */
        bool matchTerms(std::vector<std::string> data, bool caseSensitive);

    private:

        /**
         * Extracts terms from "terms.txt".
         * 
         * "terms.txt" must exist in the same directory as the executable.
         */
        void setTerms();

        std::unordered_set<std::string> terms;
        int numRequiredTerms;
};

#endif