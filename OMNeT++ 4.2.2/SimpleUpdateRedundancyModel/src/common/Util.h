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

#ifndef UTIL_H_
#define UTIL_H_

#include <string>
#include <cstring>
#include <vector>
#include <sstream>
#include <climits>
#include <map>
#include <iterator>
#include <algorithm>
#include <cstdarg>
#include <cStringTokenizer.h>
#include <cenvir.h>

using namespace std;

namespace util {

string getHostName(string& fullName);

int getHostIndex(string& fullName);

int strToInt(string& str);
unsigned long strToLong(string& str);

string intToStr(int i);
string longToStr(unsigned long i);

bool strStartWith(string str, string substr);

map<string, int> intersectMaps(map<string, int> map1, map<string, int> map2);

void splitString(string& str, string delimitor, vector<string>& container);

// num indicates the number variable arguments, and variable arguments can only be const char*
string convertListToStr(vector<string>& list, int num...);

// num indicates the number variable arguments, and variable arguments can only be const char*
string convertMapKeyToStr(map<string, int>& m, int num... );

//return an string of format id1:content1;id2:content2; ...
string convertMapToStr(map<int, string>& m);
string convertMapToStr(map<string, int>& m);
string convertMapToStr(map<unsigned long, string>& m);
string convertMapToStr(map<unsigned long, int>& m);
// return an string of format round1:id1-event1,id2-event,...;round2:id1-event1,id2-event2,...;...
string convertMapToStr(map<int, map<unsigned long, string> >& m);

// return a map from a string of format id1:content1;id2:content2; ...
void convertStrToMap(string& str, map<int, string>& m);
void convertStrToMap(string& str, map<unsigned long, string>& m);
void convertStrToMap(string& str, map<unsigned long, int>& m);
// return a map from a string of format round1:id1-event1,id2-event,...;round2:id1-event1,id2-event2,...;...
void convertStrToMap(string& str, map<int, map<unsigned long, string> >& m);

// num indicates the number variable arguments, and variable arguments can only be const char*
vector<string> loadMapKey(map<string, int>& m, int num...);

string longToString(unsigned long l);

}

#endif /* UTIL_H_ */
