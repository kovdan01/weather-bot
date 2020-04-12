#include "tgbot/tools/StringTools.h"

#include <cstdlib>
#include <iomanip>
#include <cstdio>

using namespace std;

namespace StringTools {

bool startsWith(const string& str1, const string& str2) {
    if (str1.length() < str2.length()) {
        return false;
    }
    string::const_iterator it1(str1.begin());
    string::const_iterator end1(str1.end());
    string::const_iterator it2(str2.begin());
    string::const_iterator end2(str2.end());
    while (it1 != end1 && it2 != end2) {
        if (*it1 != *it2) {
            return false;
        }
        ++it1;
        ++it2;
    }
    return true;
}

bool endsWith(const string& str1, const string& str2) {
    if (str1.length() < str2.length()) {
        return false;
    }
    string::const_iterator it1(str1.end());
    string::const_iterator begin1(str1.begin());
    string::const_iterator it2(str2.end());
    string::const_iterator begin2(str2.begin());
    --begin1;
    --begin2;
    while (it1 != begin1 && it2 != begin2) {
        if (*it1 != *it2) {
            return false;
        }
        --it1;
        --it2;
    }
    return true;
}

void split(const string& str, char delimiter, vector<string>& dest) {
    istringstream stream(str);
    string s;
    while (getline(stream, s, delimiter)) {
        dest.push_back(s);
    }
}

string generateRandomString(std::size_t length) {
    static const string chars("qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM1234567890-=[]\\;',./!@#$%^&*()_+{}|:\"<>?`~");
    static const std::size_t charsLen = chars.length();
    string result;
    for (std::size_t i = 0; i < length; ++i) {
        result += chars[rand() % charsLen];
    }
    return result;
}


string urlEncode(const string& value, const std::string& additionalLegitChars) {
    static const string legitPunctuation = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_.-~:";
    std::stringstream ss;
    for (auto const &c : value) {
        if ((legitPunctuation.find(c) == std::string::npos) && (additionalLegitChars.find(c) == std::string::npos)) {
            ss << '%' << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (unsigned int)(unsigned char)c;
        } else {
            ss << c;
        }
    }

    return ss.str();
}

string urlDecode(const string& value) {
    string result;
    for (std::size_t i = 0, count = value.length(); i < count; ++i) {
        const char c = value[i];
        if (c == '%') {
            int t = 0;
            sscanf(value.substr(i + 1, 2).c_str(), "%x", &t);
            result += (char) t;
            i += 2;
        } else {
            result += c;
        }
    }
    return result;
}

}
