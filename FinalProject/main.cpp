#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <queue>
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

    virtual bool findEntry(string countryCode, string cityCode);
    virtual void remove();
    virtual void addToCache(vector<string> newEntry);
    virtual void view();
    virtual void displayAndUpdate(vector<string> city);

};

class LFUCache {
public:
    LFUCache() {
        recent.clear();
    }
    unordered_map<string, pair<int,vector<string>>> recent;

    bool findEntry(string countryCode, string cityCode)  {
        string key = countryCode + cityCode;
        if (recent.find(key) != recent.end()) {
            return true;
        }
        return false;
    }

    void remove()  {
        pair<int, string> leastFrequent;
        leastFrequent.first = INT_MAX;
        for (auto entry : recent) {
            if (entry.second.first < leastFrequent.first) {
                leastFrequent.first = entry.second.first;
                leastFrequent.second = entry.first;
            }
        }

        recent.erase(leastFrequent.second);

    }
    void addToCache(vector<string> newEntry)  {
        //Make space for it and adjust positions
        if (recent.size() > 10) {
            remove();
        }
        bool found = findEntry(newEntry[0], newEntry[1]);
        //If the entry is already stashed in the cache
        if (found) {
            //Entry already exists; adjust its frequency
            auto entry = recent[(newEntry[0]+newEntry[1])];
            entry.first++;
            recent[(newEntry[0]+newEntry[1])] = entry;
            return;
        }

        //Add the new city
        recent.insert(make_pair((newEntry[0]+newEntry[1]), make_pair(1, newEntry)));

    }

    void view()  {
        for (auto entry : recent) {
            cout << entry.second.first << " ";
            for (int i=0; i<3;i++) {
                cout << entry.second.second[i] << " ";
            }
            cout << endl;
        }
    }

    void displayAndUpdate(vector<string> city)  {
        cout << city[1] << " Population: " << city[2] << endl;
        addToCache(city);
    }

};


class FIFOCache {
public:
    FIFOCache() {
        recent.empty();
    }
    queue<pair<string, vector<string>>>recent;

    bool findEntry(string countryCode, string cityCode)  {
        bool result = false;
        string key = countryCode + cityCode;
        //  Duplicating the queue so the order is not changed
        queue<pair<string, vector<string>>> copy = recent;
        for (int i=0; i<recent.size(); i++) {
            auto temp = copy.front();
            if (temp.first == key) {
                result = true;
            }
            copy.pop();
            copy.push(temp);
        }
        return result;
    }

    void remove()  {
        recent.pop();
    }
    void addToCache(vector<string> newEntry)  {
        //Make space for it and adjust positions
        if (recent.size() > 10) {
            remove();
        }
        bool found = findEntry(newEntry[0], newEntry[1]);
        //If the entry is already stashed in the cache
        if (found) {
            cout << "Size: " << recent.size() << endl;
            pair<string, vector<string>> saved;
            queue<pair<string, vector<string>>> helper;
            while (!recent.empty()) {
                auto temp = recent.front();
                if (temp.first == newEntry[0]+newEntry[1]) {
                    saved = recent.front();
                    recent.pop();
                    continue;
                }
                recent.pop();
                helper.push(temp);
            }
            while (!helper.empty()) {
                recent.push(helper.front());
                helper.pop();
            }
            recent.push(saved);
            return;
        }

        //Add the new city
        recent.push(make_pair((newEntry[0]+newEntry[1]), newEntry));

    }

    void view()  {
        queue<pair<string, vector<string>>> copy = recent;
        for (int i=0; i<recent.size(); i++) {
            vector<string> temp = copy.front().second;
            copy.pop();
            for (int j=0; j<3;j++) {
                cout << temp[j] << " ";
            }

            cout << endl;
        }

    }

    void displayAndUpdate(vector<string> city)  {
        cout << city[1] << " Population: " << city[2] << endl;
        addToCache(city);
    }

};

int main() {
    CSVReader reader;
    string file = "world_cities.csv";
    FIFOCache* c = new FIFOCache();
    string end = "~";
    string in = " ";
    string code, city;
    while (in != end) {
        cout << "To terminate program, type ~" << endl;
        cout << "Enter country code: ";
        getline(cin, in, '\n');
        if (in==end) {
            break;
        }
        code = in;
        cout << "Enter city name: ";
        getline(cin, in, '\n');
        if (in==end) {
            break;
        }
        city = in;

        vector<string> result = reader.readEntry(file, code, city);
        if (result.size()>0) {
            c->displayAndUpdate(result);
        }
        else {
            cout << city << " not found." << endl;
        }

        c->view();
    }
    return 0;
}