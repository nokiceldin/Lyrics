#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "include/lyrics.h"

using namespace std;
using namespace testing;

// CleanToken:
TEST(CleanToken, normalword) {
  EXPECT_EQ(cleanToken("eldin"),"eldin");
}
TEST(CleanToken, removeDot) {
  EXPECT_EQ(cleanToken("tennis."),"tennis");
}
TEST(CleanToken, upperToLower) {
  EXPECT_EQ(cleanToken("PROGRAMMING"), "programming");
}
TEST(CleanToken, removePunctations) {
  EXPECT_EQ(cleanToken("!!yoo!!"), "yoo");
}
TEST(CleanToken, dotInMiddle) {
  EXPECT_EQ(cleanToken("eldin.nokic"),"eldin.nokic");
}
TEST(CleanToken, onlyPunctations) {
  EXPECT_EQ(cleanToken(".!?"), "");
}
TEST(CleanToken, manyPunctsAndUpperToLower) {
  EXPECT_EQ(cleanToken("....EN....."),"en");
}
TEST(CleanToken, onlyDigits) {
  EXPECT_EQ(cleanToken("2004"), "");
}
TEST(CleanToken, onlyPunctation) {
  EXPECT_EQ(cleanToken(","),"");
}
TEST(CleanToken, oneLetterString) {
  EXPECT_EQ(cleanToken("e"),"e");
}
TEST(CleanToken, mixedLetterDigits) {
  EXPECT_EQ(cleanToken("eldin04"), "eldin04");
}
TEST(CleanToken, oneLetterWithEdges) {
  EXPECT_EQ(cleanToken(",,,e,,,"),"e");
}
TEST(CleanToken, mixedKeepsWord) {
  EXPECT_EQ(cleanToken("4ldin"),"4ldin");
}
TEST(CleanToken, keepMiddlePunctation) {
  EXPECT_EQ(cleanToken("tennis-player"),"tennis-player");
}
TEST(CleanToken, keepMiddlePunctationRemoveStartAndEndPuncts) {
  EXPECT_EQ(cleanToken("..tennis-player.."),"tennis-player");
}
TEST(CleanToken, empty) {
  EXPECT_EQ(cleanToken(""),"");
}
TEST(CleanToken, mixedEverything) {
  EXPECT_EQ(cleanToken(".!t4?NNis-play.er!!!"),"t4?nnis-play.er");
}
TEST(CleanToken, PunctStartAndMiddle) {
  EXPECT_EQ(cleanToken(".mon.ster"),"mon.ster");
}
TEST(CleanToken, PunctEndAndMiddle) {
  EXPECT_EQ(cleanToken("mon.sters."),"mon.sters");
}

//GatherTokens:
TEST(GatherTokens, myName) {
  set<string> expected = {"eldin", "nokic"};
  EXPECT_EQ(gatherTokens("eldin nokic"), expected);
}
TEST(GatherTokens, spacesBefore) {
  set<string> expected = {"yolo"};
  EXPECT_EQ(gatherTokens("    yolo"), expected);
}
TEST(GatherTokens, duplicates) {
  set<string> expected = {"cs", "program"};
  EXPECT_EQ(gatherTokens("cs cs cs cs program cs program"), expected);
}
TEST(GatherTokens, spacesAfter) {
  set<string> expected = {"testing"};
  EXPECT_EQ(gatherTokens("testing     "), expected);
}
TEST(GatherTokens, spaceAndNewlines) {
  set<string> expected = {"my", "name", "is"};
  EXPECT_EQ(gatherTokens("  my\tname\nis  "), expected);
}
TEST(GatherTokens, spacesBetween) {
  set<string> expected = {"a", "b", "c"};
  EXPECT_EQ(gatherTokens("a   b   c"), expected);
}
TEST(GatherTokens, UppercaseAndPunctation) {
  set<string> expected = {"my", "life"};
  EXPECT_EQ(gatherTokens("My, MY my!!! life"), expected);
}
TEST(GatherTokens, mixedLettersDigitsKept) {
  set<string> expected = {"4ldin", "n8kic"};
  EXPECT_EQ(gatherTokens("4ldin n8kic"), expected);
}
TEST(GatherTokens, mixedPunctationsToEmptySet) {
  set<string> expected = {};
  EXPECT_EQ(gatherTokens("!.,!!!...,,??"), expected);
}
TEST(GatherTokens, emptySet) {
  set<string> expected = {};
  EXPECT_EQ(gatherTokens(""), expected);
}
TEST(GatherTokens, letterWithEdges) {
  set<string> expected = {"e", "n"};
  EXPECT_EQ(gatherTokens("!..e!. n"), expected);
}
TEST(GatherTokens, punctsAtEnd) {
  set<string> expected = {"byebye"};
  EXPECT_EQ(gatherTokens("byebye..."), expected);
}
TEST(GatherTokens, onlySpaces) {
  set<string> expected = {};
  EXPECT_EQ(gatherTokens("   \n    \t \n "), expected);
}

// buildIndices
TEST(BuildIndices, FileProcessedCorrectly) {
  map<string, set<string>> inverted;
  map<string, string> artists;

  int count = buildIndices("data/tiny.txt", inverted, artists);

  EXPECT_EQ(count, 4);

  EXPECT_EQ(artists["Jingle bells"], "James Pierpont");
  EXPECT_EQ(artists["Brother John"], "Unknown");
  EXPECT_EQ(artists["Baa baa Black Sheep"], "Unknown");
  EXPECT_EQ(artists["Carol of the bells"], "Mykola Leontovych");

  EXPECT_TRUE(inverted.count("bells") > 0);
  EXPECT_TRUE(inverted["bells"].count("Jingle bells"));
  EXPECT_TRUE(inverted["bells"].count("Brother John"));
  EXPECT_TRUE(inverted["bells"].count("Carol of the bells"));
  EXPECT_FALSE(inverted["bells"].count("Baa baa Black Sheep"));
}
TEST(BuildIndices, FileNotFound) {
  map<string, set<string>> inverted;
  map<string, string> artists;

  int count = buildIndices("data/does_not_exist.txt", inverted, artists);

  EXPECT_EQ(count, 0);
  EXPECT_TRUE(inverted.empty());
  EXPECT_TRUE(artists.empty());
}

// FindQueryMatches
TEST(FindQueryMatches, SingleWord) {
  map<string, set<string>> index = {
    {"yo",{"Song1", "Song2"}}, 
    {"lo", {"Song3", "Song4"}}
  };

  set<string> result = findQueryMatches(index, "yo");
  set<string> expected = {"Song1", "Song2"};

  EXPECT_EQ(result, expected);
}
TEST(FindQueryMatches, LaterPlusNotInMap) {
  map<string, set<string>> index = {
    {"yo",{"Song1", "Song2"}}, 
  };

  set<string> result = findQueryMatches(index, "yo +lo");
  set<string> expected = {};

  EXPECT_EQ(result, expected);
}
TEST(FindQueryMatches, TwoWord) {
  map<string, set<string>> index = {
    {"yo",{"Song1"}}, 
    {"lo", {"Song2"}}
  };

  set<string> result = findQueryMatches(index, "yo lo");
  set<string> expected = {"Song1", "Song2"};

  EXPECT_EQ(result, expected);
}
TEST(FindQueryMatches, plus) {
  map<string, set<string>> index = {
    {"yo",{"Song1", "Song2"}}, 
    {"lo", {"Song2"}}
  };

  set<string> result = findQueryMatches(index, "yo +lo");
  set<string> expected = {"Song2"};

  EXPECT_EQ(result, expected);
}
TEST(FindQueryMatches, minus) {
  map<string, set<string>> index = {
    {"yo",{"Song1", "Song2"}}, 
    {"lo", {"Song2"}}
  };

  set<string> result = findQueryMatches(index, "yo -lo");
  set<string> expected = {"Song1"};

  EXPECT_EQ(result, expected);
}
TEST(FindQueryMatches, TermNotInIndex) {
  map<string, set<string>> index = {
    {"yo",{"Song1"}}, 
  };

  set<string> result = findQueryMatches(index, "lo");
  set<string> expected = {};

  EXPECT_EQ(result, expected);
}
TEST(FindQueryMatches, LaterMinusNotInMap) {
  map<string, set<string>> index = {
    {"yo",{"Song1", "Song2"}}, 
  };

  set<string> result = findQueryMatches(index, "yo -lo");
  set<string> expected = {"Song1", "Song2"};

  EXPECT_EQ(result, expected);
}
TEST(FindQueryMatches, LaterUnmodified) {
  map<string, set<string>> index = {
    {"yo",{"Song1", "Song2"}} 
  };

  set<string> result = findQueryMatches(index, "yo lo");
  set<string> expected = {"Song1", "Song2"};

  EXPECT_EQ(result, expected);
}
