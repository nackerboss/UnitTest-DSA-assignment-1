
#ifndef TESTING
#define TESTING

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>>

const std::string RED = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string RESET = "\033[0m";

//AI ahh code
std::string printDiffHighlighter(const std::string& oldStr, const std::string& newStr) {
    int m = oldStr.length();
    int n = newStr.length();
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1));
    for (int i = 0; i <= m; ++i) {
        for (int j = 0; j <= n; ++j) {
            if (i == 0 || j == 0) dp[i][j] = 0;
            else if (oldStr[i - 1] == newStr[j - 1]) dp[i][j] = dp[i - 1][j - 1] + 1;
            else dp[i][j] = std::max(dp[i - 1][j], dp[i][j - 1]);
        }
    }
    int i = m, j = n;
    std::string finalOld = "";
    std::string finalNew = "";

    while (i > 0 && j > 0) {
        if (oldStr[i - 1] == newStr[j - 1]) {
            std::string commonChar = GREEN + std::string(1, oldStr[i - 1]) + RESET;
            finalOld = commonChar + finalOld;
            finalNew = commonChar + finalNew;
            i--; j--;
        } 
        else if (dp[i - 1][j] > dp[i][j - 1]) {
            finalOld = RED + oldStr[i - 1] + RESET + finalOld;
            i--;
        } 
        else {
            finalNew = RED + newStr[j - 1] + RESET + finalNew;
            j--;
        }
    }
    while (i > 0) {
        finalOld = RED + oldStr[i - 1] + RESET + finalOld;
        i--;
    }
    while (j > 0) {
        finalNew = RED + newStr[j - 1] + RESET + finalNew;
        j--;
    }
    return finalOld + "" + finalNew;
}

class TestHelper
{
private:
    std::string folderPath;
    std::string inputFile;
    std::string outputFile;
    std::string answerFile;
    static char FileSep;
public:
    TestHelper(std::string folderPath = "testcase", std::string inputFile = "input.txt", std::string outputFile = "output.txt", std::string answerFile = "answer.txt")
    {
        this->folderPath = folderPath;
        this->inputFile = inputFile;
        this->outputFile = outputFile;
        this->answerFile = answerFile;
    }
    void DEFAULTTESTCASE(std::string folder = "1")
    {
        std::ifstream input(this->folderPath+FileSep+folder+FileSep+this->inputFile);
        if (!input.is_open()) throw std::runtime_error("input file not found");
        std::ofstream output(this->folderPath+FileSep+folder+FileSep+this->outputFile);
        if (!output.is_open()) throw std::runtime_error("output file not found");
        std::ifstream answer(this->folderPath+FileSep+folder+FileSep+this->answerFile);
        if (!answer.is_open()) throw std::runtime_error("answer file not found");
        std::streambuf* OGCin = std::cin.rdbuf();
        std::streambuf* OGCout = std::cout.rdbuf();
        std::cin.rdbuf(input.rdbuf());
        std::cout.rdbuf(output.rdbuf());
        /*input code*/

        std::cin.rdbuf(OGCin);
        /*process*/

        /*output to file for manual checking*/

        std::cout.rdbuf(OGCout);
        /*check output*/
        std::cin.rdbuf(answer.rdbuf());
        /*get answer*/

        /*check diff*/
    }
    ~TestHelper() = default;
};
#endif