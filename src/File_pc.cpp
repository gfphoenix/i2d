#include <File.hpp>
#include <mm.hpp>

std::vector<std::string> FileUtils::searchPaths_ = {{"."}};
static std::string storagePath__ = "~";
bool FileUtils::isStorageAvailable()
{
    return true;
}
const std::string & FileUtils::getStoragePath()
{
    return storagePath__;
}
Ref_ptr<File> FileUtils::Open(const char *filename)
{
    return FileUtils::Open_stdio(filename);
}
