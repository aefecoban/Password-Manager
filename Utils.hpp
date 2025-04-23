#define NOMINMAX
#include <Windows.h>
#include <vector>
#include <algorithm>
#include <string>

int levenshteinDistance(const std::string& s1, const std::string& s2) {
    int len1 = s1.size();
    int len2 = s2.size();
    std::vector<std::vector<int>> dp(len1 + 1, std::vector<int>(len2 + 1));

    for (int i = 0; i <= len1; ++i)
        dp[i][0] = i;
    for (int j = 0; j <= len2; ++j)
        dp[0][j] = j;

    for (int i = 1; i <= len1; ++i) {
        for (int j = 1; j <= len2; ++j) {
            int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
            dp[i][j] = std::min({
                dp[i - 1][j] + 1,    // silme
                dp[i][j - 1] + 1,    // ekleme
                dp[i - 1][j - 1] + cost // deðiþtirme
                });
        }
    }
    return dp[len1][len2];
}

float similarity(const std::string& s1, const std::string& s2) {
    int dist = levenshteinDistance(s1, s2);
    int maxLen = std::max(s1.length(), s2.length());
    return 1.0f - (float)dist / maxLen; // 0.0 (tamamen farklý) - 1.0 (tamamen ayný)
}

std::string getExePath() {
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string fullPath(buffer);

    size_t pos = fullPath.find_last_of("\\/");
    return fullPath.substr(0, pos);
}