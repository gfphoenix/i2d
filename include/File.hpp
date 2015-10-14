#ifndef FILE_HPP
#define FILE_HPP
#include <stddef.h>
#include "types.hpp"
#include "Cerror.hpp"
#include <vector>
class File
{
public:
    static File* Open(const char *filename);
    static File* OpenFile(const char *filename, unsigned flags);
    int Read(void *p, size_t n);
    int Write(const void *p, size_t n);
    void Close();
    u64 length()const;

    static Cerror Copy(const char *dst, const char *src);
    static Cerror Move(const char *dst, const char *src);
    static Cerror Rm(const char *file);
    static bool isDir(const char *dir);
    static bool isStoragePathAvailable();
    static std::string getStoragePath();
    static std::vector<std::string> List(const char *dir);
};

#endif // FILE_HPP

