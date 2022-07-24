
#include <iostream>

#include "CoGenerator.h"
#include <filesystem>
#include <string>

CoGenerator<std::string> ListFiles(std::string_view path)
{
    for (const auto& path : std::filesystem::recursive_directory_iterator(path))
    {
        co_yield path.path().string();
    }
}

CoGenerator<int> Test()
{
    co_yield 1;
    co_yield 2;
    co_yield 3;
}


int main()
{
    CoGenerator gen = Test();
    for (int ii : gen)
    {
        std::cout << ii << std::endl;
    }

    for (const auto& file : ListFiles("."))
    {
        std::cout << file << std::endl;
    }

    return 0;
}
