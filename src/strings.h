#pragma once

#include <vector>
#include <algorithm>
#include <string>
#include <cctype>
#include <sstream>

static inline std::string ltrim(std::string s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
                                  return !std::isspace(ch);
                                  }));
  return s;
}

static inline std::string rtrim(std::string s) {
  s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
                       return !std::isspace(ch);
                       }).base(), s.end());
  return s;
}

static inline std::string trim(std::string s) {
  return rtrim(ltrim(s));
}

static inline std::vector<std::string> tokenize(const std::string& str,
                                                char separator = ',',
                                                bool trim_tokens = false) {
  std::vector<std::string> results;
  std::stringstream ss{str};
  while (ss.good()) {
    std::string token;
    std::getline(ss, token, separator);
    if (trim_tokens)
      token = trim(token);
    results.emplace_back(token);
  }
  return results;
}

static inline std::string tolower(std::string str) {
  std::transform(str.begin(), str.end(), str.begin(),
                 [](unsigned char c){ return std::tolower(c); });
  return str;
}
