#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>
using namespace std;
class CSVReader {
private:
     string fileName;
    public:
     bool openFile(const string& filename) {
        fileName = filename;
         ifstream file(filename);

        if (!file.is_open()) {
            cerr << "Error: Could not open file " << filename << endl;
            return false;
        }
        return true;
    }
    vector<string> readEntry(const string& filename, string countryCode, string city) {
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

class Cache {
public:
    unordered_map<string, pair<int,vector<string>>> recent;

    bool findEntry(string countryCode, string cityCode) {
        string key = countryCode + cityCode;
        if (recent.find(key) != recent.end()) {
            return true;
        }
        return false;
    }
    void enqueue(vector<string> newEntry) {
        //If the entry is already stashed in the cache
        if (findEntry(newEntry[0], newEntry[1])) {
            //Entry already exists; adjust its position
            auto entry = recent[(newEntry[0]+newEntry[1])];
            int lastIndex = entry.first;
            if (lastIndex==1) {
                return;
            }
            else
            {
                for (auto entry : recent) {
                    if (entry.second.first < lastIndex) {
                        entry.second.first++;
                    }
                }
                entry.first = 1;
                recent[(newEntry[0]+newEntry[1])] = entry;
                return;
            }

        }
        //If the entry is new

        //Make space for it
        for (auto entry : recent) {
            if (entry.second.first >= 9) {
                recent.erase(entry.first);
            }
            recent[entry.second.second[0]+entry.second.second[1]] = make_pair((entry.second.first+1), entry.second.second);
        }

        recent.insert(make_pair((newEntry[0]+newEntry[1]), make_pair(1, newEntry)));

    }

    void view() {
        for (auto entry : recent) {
            cout << entry.second.first << " ";
            for (int i=0; i<3;i++) {
                cout << entry.second.second[i] << " ";
            }
            cout << endl;
        }
    }

};

void DisplayAndUpdate(vector<string> city, Cache& cache)
{
    cout << city[1] << " Population: " << city[2] << endl;
    cache.enqueue(city);

}


int main() {
    CSVReader reader;
    string file = "world_cities.csv";
    Cache cache;
    string end = "~";
    string in = " ";
    string code, city;
    while (in != end) {
        cout << "To terminate program, type ~" << endl;
        cout << "Enter country code: ";
        cin >> in;
        if (in==end) {
            break;
        }
        cin.ignore(100,'\n');
        cin.clear();
        code = in;
        cout << "Enter city name: ";
        cin >> in;
        if (in==end) {
            break;
        }
        cin.ignore(100,'\n');
        cin.clear();
        city = in;

        vector<string> result = reader.readEntry(file, code, city);
        if (result.size()>0) {
            DisplayAndUpdate(result, cache);
        }
        else {
            cout << city << " not found." << endl;
        }

        cache.view();
    }
    return 0;
}