#ifndef TERMMATCHER_H
#define TERMMATCHER_H

#include "Config.h"
#include <string>
#include <unordered_set>
#include <vector>

/**
 * TermMatcher is used to determine if a certain number of unique terms exist within a vector of strings.
 */
class TermMatcher {
    public:
        TermMatcher() = default;
        TermMatcher(int numTerms);
        TermMatcher(Config* configs);
        /**
         * matchTerms enumerates the vector and uses the std::string.find() function to check whether a number of unique terms exists within the data.
         * @param data that is checked by the term matcher
         * @param terms the set of unique terms that could potentially be matched
         * @param numRequiredTerms the number of terms which should be matched
         * @param caseSensitive whether case sensitivity applies. The terms provided should be capitalized if the search is case inssensitive.
         * @returns true if the number of unique terms matched is greater than the number of required terms; returns false otherwise
         * @returns true if terms list is empty
         */
        bool matchTerms(std::vector<std::string> data, bool caseSensitive);
    private:
        void setTerms();
        std::unordered_set<std::string> terms;
        int numRequiredTerms;
};

#endif