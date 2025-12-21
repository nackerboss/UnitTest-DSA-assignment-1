#include "../../../utils/Diff.hpp"
#include <fstream>

std::string FileToStr(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open the file " << filename << std::endl;
        throw std::runtime_error("File not found");
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    file.close();
    return content;
}

int main(int argc, char *argv[])
{
    std::string lorem1 = FileToStr("lorem1.txt");
    std::string lorem2 = FileToStr("lorem2.txt");

    LDiff diff(lorem1, lorem2);

    diff.print();
    return 0;
}
