#ifndef FILE_HPP
#define FILE_HPP
#include <stddef.h>
#include <string>
#include <vector>
#include "Ref.hpp"

class File : public Ref
{
    public:
        enum class SEEK {
            SET, CUR, END,
        };
        
        virtual void Close()=0;
        virtual int Read(void *buf, size_t count)=0;
        virtual bool Seek(long offset, SEEK whence)=0;
        virtual uint64_t Length()const=0;

        virtual ~File(){}

};

class FileUtils 
{
	public:
        // platform specific
		static bool isStorageAvailable();
        static const std::string &getStoragePath();
        static Ref_ptr<File> Open(const char *filename);

        // should be platform-independent
        static File * Open_stdio(const char *filename);
        static void addSearchPath(const char *path);
        static inline const std::vector<std::string> &getSearchPaths(){return searchPaths_;}
        template <typename T>
        static inline void setSearchPaths(T v)
        {
            searchPaths_ = std::forward<T>(v);
        }
    protected:
        static std::vector<std::string> searchPaths_;
};

#endif // FILE_HPP

