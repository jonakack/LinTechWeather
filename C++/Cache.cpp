#include "cache.hpp"

std::string readFile(const std::string &path)
{
    std::ifstream f(path);
    if (!f.is_open())
        throw std::runtime_error("File not found");

    std::string content((std::istreambuf_iterator<char>(f)),
                        std::istreambuf_iterator<char>());
    return content;
}

void writeFile(const std::string &path, const std::string &data)
{
    std::ofstream f(path);
    if (!f)
        throw std::runtime_error("Cannot write file");
    f << data;
}

// --- Cache class ---

Cache::Cache(const std::string &path)
    : path(path)
{
}

bool Cache::exists() const
{
    std::ifstream f(path);
    return f.good();
}

std::string Cache::load() const
{
    return readFile(path);
}

void Cache::save(const std::string &data) const
{
    writeFile(path, data);
}

bool Cache::isold() const
{
    
}