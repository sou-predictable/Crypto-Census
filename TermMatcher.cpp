#include "TermMatcher.h"
#include "Config.h"
#include <fstream>
#include <string>
#include <unordered_set>
#include <vector>

TermMatcher::TermMatcher(int i) {
    numRequiredTerms = i;
    setTerms();
}

TermMatcher::TermMatcher(Config* config) {
    const int defaultNumRequiredTerms = 4;
    numRequiredTerms = config->getIntConfig("TermMatcher_NumRequiredTerms", defaultNumRequiredTerms);
    if(numRequiredTerms < 0)
        numRequiredTerms = 0;
    setTerms();
}

void TermMatcher::setTerms() {
    const std::string termsFile = "terms.txt";
    std::string currentLine;
    std::ifstream exclusionInputer = std::ifstream(termsFile);
    while(std::getline(exclusionInputer, currentLine)) {
        terms.insert(currentLine);
    }
}

/**
 * matchTerms enumerates the vector and uses the std::string.find() function to check whether a number of unique terms exists within the data.
 */
bool TermMatcher::matchTerms(std::vector<std::string> data, bool caseSensitive) {
    const int maxTermSize = 5000;
    int uniqueTermsFound = 0;

    //Handle empty term list
    if(terms.size() == 0)
        return true;
    // Enumerate through all terms
    for(std::string term : terms) {
        /**
         * Check to see if the term exists within the data
         * If it does, uniqueTermsFound is incremented by 1 and checked to see if it is greater than numRequiredTerms
         */
        // Check for case sensitivity
        if(term.size() < maxTermSize) {
            if(!caseSensitive) {
                std::string tempTerm = term;
                term.clear();
                for(char c : tempTerm) {
                    // If the search is case insensitive, matchTerms capitalizes the data before the comparison
                    term += (char)toupper(c);
                }
            }
            for(std::string str : data) {
                // Check for case sensitivity
                if(!caseSensitive) {
                    std::string tempStr = str;
                    str.clear();
                    str.shrink_to_fit();
                    for(char c : tempStr) {
                        // If the search is case insensitive, matchTerms capitalizes the data before the comparison
                        str += (char)toupper(c);
                    }
                }
                if(str.find(term) != std::string::npos) {
                    uniqueTermsFound++;
                    if(uniqueTermsFound >= numRequiredTerms)
                        return true;
                    break;
                }
            }
        }
    }
    return false;
}