#include "CompareScore.h"

bool compareHighScores(const HighScoreEntry& a, const HighScoreEntry& b) {
	return a.score > b.score;
}