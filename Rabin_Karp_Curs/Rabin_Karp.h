/**
 * @file Rabin_Karp.h
 * @brief Header file containing class definitions for working with the Rabin-Karp algorithm.
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <regex>
#include <algorithm> 
#include <chrono> 
#include <iomanip> 

 /**
  * @class FileHandler
  * @brief Class for handling file operations.
  */
class FileHandler {
public:
    /**
     * @brief Reads the content of a file.
     * @param filename Name of the file.
     * @return Content of the file.
     */
    std::string readFile(const std::string& filename);

    /**
     * @brief Writes content to a file.
     * @param filename Name of the file.
     * @param content Content to write.
     */
    void writeFile(const std::string& filename, const std::string& content);
};

/**
 * @class StringSearcher
 * @brief Class for performing substring search in text using the Rabin-Karp algorithm.
 */
class StringSearcher {
public:
    /**
     * @brief Performs substring search using the Rabin-Karp algorithm.
     * @param text Text to search in.
     * @param pattern Pattern to search for.
     * @param d The base of the numeral system.
     * @param prime A prime number for modulation.
     * @return A vector of positions of found matches.
     */
    std::vector<int> rabinKarpSearch(const std::string& text, const std::string& pattern, int hash_base, int modulus) const;

    /**
     * @brief Gets the context of found matches in the text.
     * @param text Text to search in.
     * @param matches Vector of positions of found matches.
     * @param pattern Pattern to search for.
     * @param contextSize Size of the context around the found match.
     * @return A string describing the found matches and their context.
     */
    std::string getMatchesWithContext(const std::string& text, const std::vector<int>& matches, const std::string& pattern, int contextSize) const;
};

/**
 * @class MainApp
 * @brief Main application to run the search.
 */
class MainApp {
private:
    /**
     * @brief Checks whether all characters in the string are valid ASCII characters.
     * @param str String to check.
     * @return true if all characters are valid, false otherwise.
     */
    bool isValidUTF8(const std::string& str) const;

    /**
     * @brief Converts a string to lowercase.
     * @param str String to convert.
     * @return Lowercase version of the string.
     */
    std::string toLower(const std::string& str) const;

public:
    /**
     * @brief Runs the application.
     * @param argc Number of command-line arguments.
     * @param argv Array of command-line arguments.
     */
    void run(int argc, char* argv[]);
};
