#pragma once

#include "CompareScore.h"
#include <vector>
#include <string>

class HighScoreManager {
    std::vector<HighScoreEntry> highScores;
    
public:
    void load();
    void save();
    void tryAdd(int score, const std::string& dateTime);
    const std::vector<HighScoreEntry>& getHighScores() const;
    std::string getCurrentDateTime();
};
