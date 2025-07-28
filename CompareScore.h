#pragma once
#include <SFML/Graphics.hpp>

struct HighScoreEntry {
    int score;
    std::string time;
};

bool compareHighScores(const HighScoreEntry& a, const HighScoreEntry& b);