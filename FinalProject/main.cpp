#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>>
using namespace std;
class CSVReader {
private:
    static string fileName;
    public:
    static bool openFile(const string& filename) {
        fileName = filename;
         ifstream file(filename);

        if (!file.is_open()) {
            cerr << "Error: Could not open file " << filename << endl;
            return false;
        }
        return true;
    }
    static vector<string> readEntry(const string& filename, string countryCode, string city) {
        if (!openFile(filename)) {
            return vector<string>();
        }
        ifstream file(filename);
        string line, word;

        while (getline(file, line)) {
            int match = 0;
            stringstream ss(line);
            vector<string> row;
            while (getline(ss, word, ','))
            {
                if (word == countryCode) {
                    match++;
                }
                if (word == city) {
                    match++;
                }
                row.push_back(word);
            }
            if (match >=2) {
                file.close();
                return row;
            }
        }
        file.close();
        return vector<string>();

    }
};




int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}