#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>

std::string readFile(const std::string &path);
void writeFile(const std::string &path, const std::string &data);

class Cache
{
    public:
        explicit Cache(const std::string &path);

        bool exists() const;
        std::string load() const;
        void save(const std::string &data) const;

    private:
        std::string path;
};