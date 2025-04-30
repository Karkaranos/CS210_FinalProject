#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <queue>
#include <random>
#include <time.h>
#include <iomanip>
#include<bits/stdc++.h>
using namespace std;

static int maxSize() {
    return 10;
}


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
                if (word == countryCode && match == 0) {
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

    virtual bool findEntry(string countryCode, string cityCode) {
        cout << "Base Find Entry";
    }
    virtual void remove() {
        cout << "Base Remove";
    }
    virtual void addToCache(vector<string> newEntry)
    {
        cout << "Base AddToCache";
    }
    virtual void view() {
        cout << "Base View";
    }
    void displayAndUpdate(vector<string> city) {
        string substr = city[1].substr(1, city[1].length());
        city[1] = toupper(city[1][0]);
        city[1]+=substr;
        cout << city[1] << " | Population: " << city[2] << endl;
        addToCache(city);
    }

    vector<string> format(vector<string> newEntry) {
        transform(newEntry[0].begin(), newEntry[0].end(), newEntry[0].begin(), ::toupper);
        string substr = newEntry[1].substr(1, newEntry[1].length());
        newEntry[1] = toupper(newEntry[1][0]);
        newEntry[1]+=substr;
        newEntry[2] = newEntry[2].substr(0, newEntry[2].length()-2);
        return newEntry;
    }

};

class LFUCache : public Cache {
public:
    LFUCache() {
        cout << "Created LFUCache" << endl;
        recent.clear();
    }
    unordered_map<string, pair<int,vector<string>>> recent;

    bool findEntry(string countryCode, string cityCode) override  {
        string key = countryCode + cityCode;
        if (recent.find(key) != recent.end()) {
            return true;
        }
        return false;
    }

    void remove() override {
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
    void addToCache(vector<string> newEntry) override {
        newEntry = format(newEntry);
        //Make space for it and adjust positions
        if (recent.size() > maxSize()) {
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

    void view() override {
        cout << endl << "Viewing LFU Cache..." << endl;
        cout << left << setw(8) << "Freq. | " << setw(8) << "Country" << " | " << setw(15) << "City"
            << " | Population" << endl;
        for (auto entry : recent) {
            cout << setw(6) << entry.second.first << "| " << setw(8) << entry.second.second[0] << " | "
            << setw(15) << entry.second.second[1] << " | " << entry.second.second[2] << endl;
            cout << endl;
        }
    }


};


class FIFOCache : public Cache {
public:
    FIFOCache() {
        cout << "Created FIFO Cache" << endl;
        recent.empty();
    }
    queue<pair<string, vector<string>>>recent;

    bool findEntry(string countryCode, string cityCode) override  {
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

    void remove() override {
        recent.pop();
    }
    void addToCache(vector<string> newEntry) override {
        newEntry = format(newEntry);
        //Make space for it and adjust positions
        if (recent.size() > maxSize()) {
            remove();
        }
        bool found = findEntry(newEntry[0], newEntry[1]);
        //If the entry is already stashed in the cache
        if (found) {
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

    void view() override {
        cout << endl << "Viewing FIFO Cache..." << endl;
        cout << left << "Position | " << setw(8) << "Country" << " | " << setw(15) << "City"
            << " | Population" << endl;
        queue<pair<string, vector<string>>> copy = recent;
        for (int i=0; i<recent.size(); i++) {
            vector<string> temp = copy.front().second;
            cout << setw(9) << i+1 << "| " << setw(8) << temp[0] << " | "
            << setw(15) << temp[1] << " | " << temp[2] << endl;
            copy.pop();
        }
    }


};

class RandomCache : public Cache {
public:
    vector<pair<string, vector<string>>> recent = vector<pair<string,vector<string>>>();
    int valCount;
    RandomCache() {
        cout << "Created RandomCache" << endl;
        valCount = 0;
        srand(time(NULL));
        rand();
    }

    bool findEntry(string countryCode, string cityCode) override {
        string key = countryCode + cityCode;
        for (int i=0; i<recent.size(); i++) {
            if (recent[i].first == key) {
                return true;
            }
        }
        return false;
    }

    void remove() override {
        // Not needed for this implementation

    }
    void addToCache(vector<string> newEntry) override {
        newEntry = format(newEntry);
        bool found = findEntry(newEntry[0], newEntry[1]);
        //If the entry is already stashed in the cache

        if (found) {
            return;
        }
        if (valCount >= maxSize()) {
            int removeMe = (rand() % maxSize()) -1;
            if (removeMe < 0) {
                removeMe = 0;
            }
            cout << removeMe << endl;
            recent.at(removeMe) = make_pair(newEntry[0] + newEntry[1], newEntry);
            return;
        }

        recent.push_back(make_pair(newEntry[0] + newEntry[1], newEntry));
        valCount++;

    }

    void view() override {
        cout << "Viewing Random Cache..." << endl;
        cout << left << "Index | " << setw(8) << "Country" << " | " << setw(15) << "City"
            << " | Population" << endl;
        for (int i=0; i<recent.size(); i++) {
            if (recent[i].first != "~") {
                cout << setw(6) << i+1 << "| " << setw(8) << recent[i].second[0] << " | "
                    << setw(15) << recent[i].second[1] << " | " << recent[i].second[2] << endl;
            }
            else {
                cout << setw(6) << i+1 << "| " << setw(8) << "NULL" << " | "
            << setw(15) << "NULL" << " | " << "NULL" << endl;
            }
        }
        cout << endl;
    }


};

int main() {
    CSVReader reader;
    string file = "world_cities.csv";
    Cache* c = nullptr;
    string end = "~";
    string in = " ";
    string code, city;

    while (c == nullptr)
    {
        char ci = ' ';
        cout << "Select your cache type. \n\t1 for LFU\n\t2 for FIFO\n\t3 for Random\n";
        cin >> ci;

        if (ci == '1') {
            c = new LFUCache();
        }
        else if (ci == '2') {
            c = new FIFOCache();
        }
        else if (ci == '3') {
            c = new RandomCache();
        }

        getline(cin, in, '\n');
    }


    while (in != end) {
        cout << "To terminate program, type ~" << endl;
        cout << "Enter country code: ";
        getline(cin, in, '\n');
        if (in==end) {
            break;
        }
        code = in;
        transform(code.begin(), code.end(), code.begin(), ::tolower);

        cout << "Enter city name: ";
        getline(cin, in, '\n');
        if (in==end) {
            break;
        }
        city = in;

        transform(city.begin(), city.end(), city.begin(), ::tolower);
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