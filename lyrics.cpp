#include "include/lyrics.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

string cleanToken(const string& token) {

  if (token.empty()) {
    return "";
  }

  int start = 0;
  int end = token.size() - 1;

  for (int i = 0; i < token.length(); i++) {
    if (!ispunct(token[i])) {
      start = i;
      break;
    }
    if (i == token.length() - 1) {
      return "";
    }
  }
    for (int i = token.length() - 1; i >= 0; i--) {
    if (!ispunct(token[i])) {
      end = i;
      break;
    }
  }

  int letterCount = 0;
  string cleanedToken = "";
  for (int i = start; i <= end; i++) {

    if (isalpha(tolower(token[i]))) {
      letterCount++;
    }

    cleanedToken += tolower(token[i]);
  }

  if (letterCount == 0) {
    return "";
  }
  else {
    return cleanedToken;
  }
}

set<string> gatherTokens(const string& text) {
  set<string> allWords;
  stringstream textStream(text);
  string currentWord;

  while (textStream >> currentWord) {
    string cleanedWord = cleanToken(currentWord);
    if (!cleanedWord.empty()) {
      allWords.insert(cleanedWord);
    }
  }
  return allWords;
}

int buildIndices(const string& filename,
                 map<string, set<string>>& inverted_index,
                 map<string, string>& artist_index) {
  
  ifstream inputFile(filename);
  
  int songsCount = 0;
  songsCount = 0;

  string songTitle;
  string artist;
  string lyrics;

  while(true){

    if(!getline(inputFile, songTitle)) {
      break;
    }

    if (songTitle.empty()) {
      continue;
    }

    if(!getline(inputFile, artist)) {
      break;
    }

    if(!getline(inputFile, lyrics)) {
      break;
    }

    artist_index[songTitle] = artist;

    set<string> wordsFromLyrics = gatherTokens(lyrics);

    for (const string& word : wordsFromLyrics) {
      inverted_index[word].insert(songTitle);
    }

    songsCount = songsCount + 1;
  }
  
  return songsCount;
}

set<string> findQueryMatches(const map<string, set<string>>& index,
                             const string& sentence) {
  stringstream inputFile(sentence);
  string currentWord;

  set<string> result;
  bool isFirstWord = true;

  while (inputFile >> currentWord) {
    char sign = 0;

    if (currentWord[0] == '+' || currentWord[0] == '-') {
      sign = currentWord[0];
      currentWord = currentWord.substr(1);
    }

    string cleanedWord = cleanToken(currentWord);

    if (cleanedWord.empty() && cleanedWord == "") {
      continue;
    }

    set<string>matches;
    auto it = index.find(cleanedWord);
    if(it != index.end()) {
      matches = it->second;
    }

    if (isFirstWord) {
      if (sign == '-') {
        for (auto i = index.begin(); i != index.end(); i++) {
          const set<string>& songs = i->second;
          result.insert(songs.begin(), songs.end());
        }
        for (const auto& s : matches) {
          result.erase(s);
        }
      }
      else {
        result = matches;
      }
      isFirstWord = false;
      continue;
    }
    set<string> temp;

    if (sign == '+') {
      for (const auto& song : result) {
        if (matches.find(song) != matches.end()) {
          temp.insert(song);
        }
      }
      result = temp;
    }
    else if (sign == '-') {
      for (const auto& song : result) {
        if (matches.find(song) == matches.end()) {
          temp.insert(song);
        }
      }
      result = temp;
    }
    else {
      temp = result;
      for (const auto& song : matches) {
        temp.insert(song);
      }
      result = temp;
    }
  }

  return result;
}

void searchEngine(const string& filename) {
  map<string, set<string>> inverted_index;
  map<string, string> artist_index;

  int numSongs = buildIndices(filename, inverted_index, artist_index);

  if (numSongs == 0) {
    cout << "Invalid filename." << endl;
    cout << "Stand by while building indices..." << endl;
    cout << "Indexed 0 songs containing 0 unique terms and 0 artists." << endl;
  }
  else {
    cout << "Stand by while building indices..." << endl;
    set<string> distinctArtists;
    for (const auto& k : artist_index) {
      distinctArtists.insert(k.second);
    }
    cout << "Indexed " << numSongs 
    << " songs containing " << inverted_index.size() 
  << " unique terms and " << distinctArtists.size() << " artists." << endl;
  }

  while (true) {
    cout << "Enter query sentence (press enter to quit): ";
    string querySentence;
    getline(cin, querySentence);
  
    if (querySentence.empty()) {
      cout << "Thank you for searching our Lyrics DB!" << endl;
      break;
    }

    set<string>matches = findQueryMatches(inverted_index, querySentence);
    cout << "Found " << matches.size() << " matching songs" << endl;

    vector<string> sorted(matches.begin(), matches.end());
    sort(sorted.begin(), sorted.end());

    for (const auto& title : sorted) {
      cout << title << " by " << artist_index[title] << endl;
    }
  }
}
