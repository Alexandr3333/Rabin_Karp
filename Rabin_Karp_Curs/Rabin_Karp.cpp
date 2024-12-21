#include "Rabin_Karp.h"
std::string FileHandler::readFile(const std::string& filename) {
    std::ifstream file(filename); 
    if (!file.is_open()) {
        throw std::runtime_error("Ошибка: не удалось открыть файл " + filename);
    } 
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    return content;
}

void FileHandler::writeFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename); 
    if (!file.is_open()) {
        throw std::runtime_error("Ошибка: не удалось записать в файл " + filename);
    }
    file << content;
    file.close();
}

std::vector<int> StringSearcher::rabinKarpSearch(const std::string& text, const std::string& pattern, int d, int prime) const {
    using namespace std::chrono;
    auto start_time = high_resolution_clock::now(); 
    int m = pattern.size();
    int n = text.size();
    std::vector<int> matches;

    if (m > n) return matches;

    int pattern_hash = 0;
    int window_hash = 0;
    int h = 1;

    for (int i = 0; i < m - 1; ++i) {
        h = (h * d) % prime;
    }

    for (int i = 0; i < m; ++i) {
        pattern_hash = (d * pattern_hash + pattern[i]) % prime;
        window_hash = (d * window_hash + text[i]) % prime;
    }

    for (int i = 0; i <= n - m; ++i) {
        if (pattern_hash == window_hash) {
            if (text.substr(i, m) == pattern) {
                matches.push_back(i);
            }
        }

        if (i < n - m) {
            window_hash = (d * (window_hash - text[i] * h) + text[i + m]) % prime;
            if (window_hash < 0) {
                window_hash += prime;
            }
        }
    }
    auto end_time = high_resolution_clock::now(); 
    duration<double> exec_time = end_time - start_time; 
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Время выполнения алгоритма Рабина-Карпа: " << exec_time.count() << " секунд." << std::endl;
    return matches;
}

std::string StringSearcher::getMatchesWithContext(const std::string& text, const std::vector<int>& matches, const std::string& pattern, int contextSize) const {
    std::string results;
    if (matches.empty()) { 
        results += "Совпадений не найдено.\n";
        return results;
    }

    for (int index : matches) { 
        int visible_index = 0; 
        for (int i = 0; i < index; ++i) {
            if (text[i] != '\n') {
                visible_index++;
            }
        }
        int start = std::max(0, visible_index - contextSize);
        int end = visible_index + pattern.size() + contextSize;

        std::string context; 
        int cleanIndex = 0; 
        for (int i = 0; i < static_cast<int>(text.size()); ++i) {
            if (cleanIndex >= start && cleanIndex < end) { 
                context += text[i]; 
            }
            if (text[i] != '\n') {
                cleanIndex++;
            }
        }

        results += "Найдено совпадение: \"" + pattern + "\" в позиции " + std::to_string(visible_index) + "\n";
        results += "Контекст: " + context + "\n";
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
        std::cout << "Недостаточно аргументов командной строки";
        std::cerr << "Использование: " << argv[0] << " <имя_входного_файла> <имя_выходного_файла>" << std::endl;
        return;
    }

    const int d = 256; 
    const int prime = 101;

    std::string inputFilename = argv[1];
    std::string outputFilename = argv[2];

    try {
        FileHandler fileHandler;
        std::string text = fileHandler.readFile(inputFilename);

        MainApp app;
        std::string lower_text = app.toLower(text);

        std::cout << "Введите строку для поиска: ";
        std::string pattern;
        std::getline(std::cin, pattern);

        if (pattern.empty()) {
            std::cerr << "Ошибка: строка для поиска не может быть пустой." << std::endl;
            return;
        }

        if (!isValidUTF8(pattern)) {
            std::cerr << "Ошибка: строка для поиска должна содержать только допустимые символы ASCII." << std::endl;
            return;
        }

        std::string lower_pattern = app.toLower(pattern);

        std::cout << "Введите радиус видимости контекста: ";
        int radius;
        std::cin >> radius;

        if (std::cin.fail() || radius < 0) { // если радиус не число
            std::cerr << "Ошибка: радиус должен быть положительным целым числом." << std::endl;
            return;
        }

        StringSearcher searcher;
        std::vector<int> matches = searcher.rabinKarpSearch(lower_text, lower_pattern, d, prime);
        std::string results = searcher.getMatchesWithContext(text, matches, pattern, radius);

        fileHandler.writeFile(outputFilename, results);
        std::cout << "Результаты сохранены в " << outputFilename << std::endl;

    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}
