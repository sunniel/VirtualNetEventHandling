//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "Util.h"

string util::getHostName(string& fullName) {
    int end = fullName.find("[");
    string hostName = fullName.substr(0, end);
    return hostName;
}

int util::getHostIndex(string& fullName) {
    int i = -1;
    size_t start = fullName.find("[") + 1;
    size_t end = fullName.find("]");
    if (start != string::npos && end != string::npos) {
        size_t range = end - start;
        string index = fullName.substr(start, range);
        if (!index.empty()) {
            i = util::strToInt(index);
        }
    }
    return i;
}

int util::strToInt(string& str) {
    int value = INT_MIN;
    stringstream ss(str);
    ss >> value;
    return value;
}

unsigned long util::strToLong(string& str) {
    unsigned long value = INT_MIN;
    stringstream ss(str);
    ss >> value;
    return value;
}

string util::intToStr(int i) {
    stringstream ss;
    ss << i;
    return ss.str();
}

string util::longToStr(unsigned long i) {
    stringstream ss;
    ss << i;
    return ss.str();
}

string util::longToString(unsigned long l) {
    stringstream ss;
    ss << l;
    return ss.str();
}

bool util::strStartWith(string str, string substr) {
    return strncmp(str.c_str(), substr.c_str(), strlen(substr.c_str())) == 0;
}

map<string, int> util::intersectMaps(map<string, int> map1
        , map<string, int> map2) {
    map<string, int> map3;
    for (map<string, int>::iterator it = map1.begin(); it != map1.end(); it++) {
        string key = it->first;
        int value = it->second;
        if (map2.find(key) != map2.end()) {
            map3[key] = value;
        }
    }
    return map3;
}

void util::splitString(string& str, string delimitor,
        vector<string>& container) {
    cStringTokenizer tokenizer(str.c_str(), delimitor.c_str());
    while (tokenizer.hasMoreTokens()) {
        const char* token = tokenizer.nextToken();
//        EV << "entries contains: " << token << endl;
        container.push_back(token);
    }
}

string util::convertListToStr(vector<string>& list, int num...) {
    stringstream ss;

    va_list args;
    va_start(args, num);
    for(int i = 0; i < num; i++) {
        const char* token = va_arg(args, const char*);
        ss << token << ";";
    }
    va_end(args);

    for (size_t i = 0; i < list.size(); i++) {
        ss << list[i] << ";";
    }
    string content = ss.str();
    return content;
}

string util::convertMapKeyToStr(map<string, int>& m, int num...) {
    stringstream ss;

    va_list args;
    va_start(args, num);
    for(int i = 0; i < num; i++) {
        const char* token = va_arg(args, const char*);
        ss << token << ";";
    }
    va_end(args);

    for (map<string, int>::iterator it = m.begin(); it != m.end(); ++it) {
        string token = it->first;
        ss << token << ";";
    }
    string content = ss.str();
    return content;
}

string util::convertMapToStr(map<int, string>& m) {
    stringstream ss;
    for (map<int, string>::iterator it = m.begin(); it != m.end(); ++it) {
        string id = intToStr(it->first);
        string value = it->second;
        ss << id << ":" << value << ";";
    }
    string content = ss.str();
    return content;
}

string util::convertMapToStr(map<string, int>& m) {
    stringstream ss;
    for (map<string, int>::iterator it = m.begin(); it != m.end(); ++it) {
        string first = it->first;
        string second = intToStr(it->second);
        ss << first << ":" << second << ";";
    }
    string content = ss.str();
    return content;
}

string util::convertMapToStr(map<unsigned long, string>& m) {
    stringstream ss;
    for (map<unsigned long, string>::iterator it = m.begin(); it != m.end();
            ++it) {
        string id = longToStr(it->first);
        string value = it->second;
        ss << id << ":" << value << ";";
    }
    string content = ss.str();
    return content;
}

string util::convertMapToStr(map<unsigned long, int>& m) {
    stringstream ss;
    for (map<unsigned long, int>::iterator it = m.begin(); it != m.end();
            ++it) {
        string id = longToStr(it->first);
        string value = intToStr(it->second);
        ss << id << ":" << value << ";";
    }
    string content = ss.str();
    return content;
}

string util::convertMapToStr(map<int, map<unsigned long, string> >& m) {
    stringstream ss;
    for (map<int, map<unsigned long, string> >::iterator it = m.begin();
            it != m.end(); ++it) {
        string round = intToStr(it->first);
        ss << round << ":";
        if (!it->second.empty()) {
            for (map<unsigned long, string>::iterator it2 = it->second.begin();
                    it2 != it->second.end(); ++it2) {
                string id = longToStr(it2->first);
                string event = it2->second;
                ss << id << "-" << event << ",";
            }
        }
        ss << ";";
    }
    string content = ss.str();
    return content;
}

void util::convertStrToMap(string& str, map<int, string>& m) {
    // empty map first;
    m.clear();
    vector<string> tokens;
    splitString(str, ";", tokens);
    for (size_t i = 0; i < tokens.size(); i++) {
        string token = tokens[i];
        vector<string> pair;
        splitString(token, ":", pair);
        int id = strToInt(pair[0]);
        if(pair.size() > 1){
            m[id] = pair[1];
        }
        else{
            m[id] = "";
        }
    }
}

void util::convertStrToMap(string& str, map<unsigned long, string>& m) {
    // empty map first;
    m.clear();
    vector<string> tokens;
    splitString(str, ";", tokens);
    for (size_t i = 0; i < tokens.size(); i++) {
        string token = tokens[i];
        vector<string> pair;
        splitString(token, ":", pair);
        unsigned long id = strToLong(pair[0]);
        if(pair.size() > 1){
            m[id] = pair[1];
        }
        else{
            m[id] = "";
        }
    }
}

void util::convertStrToMap(string& str, map<unsigned long, int>& m) {
    // empty map first;
    m.clear();
    vector<string> tokens;
    splitString(str, ";", tokens);
    for (size_t i = 0; i < tokens.size(); i++) {
        string token = tokens[i];
        vector<string> pair;
        splitString(token, ":", pair);
        unsigned long id = strToInt(pair[0]);
        m[id] = strToInt(pair[1]);
    }
}

void util::convertStrToMap(string& str,
        map<int, map<unsigned long, string> >& m) {
    // empty map first;
    m.clear();
    vector<string> L1Tokens;
    splitString(str, ";", L1Tokens);
    for (size_t i = 0; i < L1Tokens.size(); i++) {
        string token = L1Tokens[i];
        vector<string> pair1;
        splitString(token, ":", pair1);
        int round = strToInt(pair1[0]);
        string roundEvents = pair1[1];
        vector<string> L2Tokens;
        splitString(roundEvents, ",", L2Tokens);
        for (size_t i = 0; i < L2Tokens.size(); i++) {
            string token2 = L1Tokens[i];
            vector<string> pair2;
            splitString(token2, "-", pair2);
            unsigned long senderId = strToLong(pair2[0]);
            string event = pair2[1];
            if(pair2.size() > 1){
                m[round][senderId] = event;
            }
            else{
                m[round][senderId] = "";
            }
        }
    }
}

vector<string> util::loadMapKey(map<string, int>& m, int num...) {
    vector<string> list;

    va_list args;
    va_start(args, num);
    for(int i = 0; i < num; i++) {
        const char* token = va_arg(args, const char*);
        list.push_back(token);
    }
    va_end(args);

    for (map<string, int>::iterator it = m.begin(); it != m.end(); ++it) {
        string key = it->first;
        list.push_back(key);
    }
    return list;
}
