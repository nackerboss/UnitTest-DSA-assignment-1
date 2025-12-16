#ifndef TESTING
#define TESTING

#include <string>
#include <iostream>
#include <fstream>

class TestHelper
{
private:
    std::string folderPath;
    std::string inputFile;
    std::string outputFile;
    std::string answerFile;
    static char FileSep;
public:
    TestHelper(std::string folderPath_ = "testcase", std::string inputFile_ = "input.txt", std::string outputFile_ = "output.txt", std::string answerFile_ = "answer.txt")
   : folderPath(std::move(folderPath_)),
     inputFile(std::move(inputFile_)),
     outputFile(std::move(outputFile_)),
     answerFile(std::move(answerFile_))
   {
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

        /*check diff*/ }
    ~TestHelper() = default;
};
#endif
