#include "HighScoreManager.h"
#include <fstream>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <sstream>

void HighScoreManager::load() {
    highScores.clear();
    std::ifstream fin("high_scores.txt");
    std::string line;
    while (std::getline(fin, line)) {
        if (line.empty()) continue;
        size_t pos = line.find(',');
        if (pos != std::string::npos) {
            int score = std::stoi(line.substr(0, pos));
            std::string dateTime = line.substr(pos + 1);
            HighScoreEntry entry;
            entry.score = score;
            entry.time = dateTime;
            highScores.push_back(entry);
        }
    }
    fin.close();
}

void HighScoreManager::save() {
    std::ofstream fout("high_scores.txt");
    for (const auto& entry : highScores) {
        fout << entry.score << "," << entry.time << std::endl;
    }
    fout.close();
}

void HighScoreManager::tryAdd(int score, const std::string& dateTime) {
    load();
    HighScoreEntry entry;
    entry.score = score;
    entry.time = dateTime;
    highScores.push_back(entry);
    std::sort(highScores.begin(), highScores.end(), compareHighScores);
    if (highScores.size() > 10) highScores.resize(10);
    save();
}

const std::vector<HighScoreEntry>& HighScoreManager::getHighScores() const {
    return highScores;
}

std::string HighScoreManager::getCurrentDateTime() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::tm tm;
    localtime_s(&tm, &time_t);
    std::stringstream ss;
    ss << std::put_time(&tm, "%d/%m/%Y %H:%M:%S");
    return ss.str();
}