#include "Rabin_Karp.h"
std::string FileHandler::readFile(const std::string& filename) {
    std::ifstream file(filename); 
    if (!file.is_open()) {
        throw std::runtime_error("Mistake: couldn't open the file " + filename);
    } 
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    return content;
}

void FileHandler::writeFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename); 
    if (!file.is_open()) {
        throw std::runtime_error("Mistake: couldn't write to a file " + filename);
    }
    file << content;
    file.close();
}

std::vector<int> StringSearcher::rabinKarpSearch(const std::string& text, const std::string& pattern, int hash_base, int modulus) const {
    using namespace std::chrono;
    auto start_time = high_resolution_clock::now(); 
    int pattern_length = pattern.size();
    int text_length = text.size();
    std::vector<int> matches;

    if (pattern_length > text_length) return matches;

    int pattern_hash = 0;
    int window_hash = 0;
    int window_scaler = 1;

    for (int i = 0; i < pattern_length - 1; ++i) {
        window_scaler = (window_scaler * hash_base) % modulus;
    }

    for (int i = 0; i < pattern_length; ++i) {
        pattern_hash = (hash_base * pattern_hash + pattern[i]) % modulus;
        window_hash = (hash_base * window_hash + text[i]) % modulus;
    }

    for (int i = 0; i <= text_length - pattern_length; ++i) {
        if (pattern_hash == window_hash) {
            if (text.substr(i, pattern_length) == pattern) {
                matches.push_back(i);
            }
        }

        if (i < text_length - pattern_length) {
            window_hash = (hash_base * (window_hash - text[i] * window_scaler) + text[i + pattern_length]) % modulus;
            if (window_hash < 0) {
                window_hash += modulus;
            }
        }
    }
    auto end_time = high_resolution_clock::now(); 
    duration<double> exec_time = end_time - start_time; 
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Execution time of the Rabin - Karp algorithm: " << exec_time.count() << " seconds." << std::endl;
    return matches;
}

std::string StringSearcher::getMatchesWithContext(const std::string& text, const std::vector<int>& matches, const std::string& pattern, int contextSize) const {
    std::string results;
    if (matches.empty()) { 
        results += "No matches were found.\n";
        return results;
    }

    for (int index : matches) { 
        int visible_index = 0; 
        for (int i = 0; i < index; ++i) {
            if (text[i] != '\n') {
                visible_index++;
            }
        }
        int context_start = std::max(0, visible_index - contextSize);
        int context_end = visible_index + pattern.size() + contextSize;

        std::string context; 
        int cleanIndex = 0; 
        for (int i = 0; i < static_cast<int>(text.size()); ++i) {
            if (cleanIndex >= context_start && cleanIndex < context_end) {
                context += text[i]; 
            }
            if (text[i] != '\n') {
                cleanIndex++;
            }
        }

        results += "A match was found: \"" + pattern + "\" in the position " + std::to_string(visible_index) + "\n";
        results += "Context: " + context + "\n";
    }
    return results;
}

bool MainApp::isValidUTF8(const std::string& str) const {
    static const std::regex validPattern("^[\x20-\x7E]*$"); 
    return std::regex_match(str, validPattern); 
}

std::string MainApp::toLower(const std::string& str) const {
    std::string lower_str = str;
    std::transform(lower_str.begin(), lower_str.end(), lower_str.begin(), ::tolower); 
    return lower_str;
}

void MainApp::run(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Not enough command line arguments";
        std::cerr << "Using: " << argv[0] << " <input file name> <output file name>" << std::endl;
        return;
    }

    const int hash_base = 256;
    const int modulus = 101;

    std::string inputFilename = argv[1];
    std::string outputFilename = argv[2];

    try {
        FileHandler fileHandler;
        std::string text = fileHandler.readFile(inputFilename);

        MainApp app;
        std::string lower_text = app.toLower(text);

        std::cout << "Enter the search string: ";
        std::string pattern;
        std::getline(std::cin, pattern);

        if (pattern.empty()) {
            std::cerr << "Error: The search string cannot be empty." << std::endl;
            return;
        }

        if (!isValidUTF8(pattern)) {
            std::cerr << "Error: The search string must contain only valid ASCII characters." << std::endl;
            return;
        }

        std::string lower_pattern = app.toLower(pattern);

        std::cout << "Enter the radius of visibility of the context: ";
        int radius;
        std::cin >> radius;

        if (std::cin.fail() || radius < 0) { // если радиус не число
            std::cerr << "Error: The radius must be a positive integer." << std::endl;
            return;
        }

        StringSearcher searcher;
        std::vector<int> matches = searcher.rabinKarpSearch(lower_text, lower_pattern, hash_base, modulus);
        std::string results = searcher.getMatchesWithContext(text, matches, pattern, radius);

        fileHandler.writeFile(outputFilename, results);
        std::cout << "The results are saved in: " << outputFilename << std::endl;

    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}
