#include <File.hpp>
#include <Log.hpp>
#include <stdlib.h>
#include <string.h>

class File_stdio : public File
{
    public:
        File_stdio(FILE *fp):
            fp_(fp){}
        virtual void Close()override;
        virtual int Read(void *buf, size_t count)override;
        virtual bool Seek(long offset, SEEK whence)override;
        virtual uint64_t Length()const;
    private:
        FILE *fp_;
};


void File_stdio::Close()
{
    if(fp_==nullptr){
        // bad file or closed more than once
        Log::e("%s : close file more than once", __func__);
        return ;
    }
    fclose(fp_);
    fp_ = nullptr;
}
int File_stdio::Read(void *buf, size_t count)
{
    if(fp_==nullptr){
        Log::e("read file on a nil file");
        return -1;
    }
    return (int)fread(buf, 1, count, fp_);
}
bool File_stdio::Seek(long offset, SEEK whence)
{
    if(fp_==nullptr)
        return false;
    int e=-1;
    switch(whence){
        case SEEK::SET:
            e = fseek(fp_, offset, SEEK_SET);
            break;
        case SEEK::CUR:
            e = fseek(fp_, offset, SEEK_CUR);
            break;
        case SEEK::END:
            e = fseek(fp_, offset, SEEK_END);
            break;
        default:
        Log::e("BAD SEEK value");
    }
    return e==0;
}
uint64_t File_stdio::Length()const
{
    if(fp_ == nullptr)
        return 0;
    fpos_t old;
    long L=0;
    auto e = fgetpos(fp_, &old);
    if(e)
        goto out;
    e = fseek(fp_, 0, SEEK_END);
    if(e)
        goto out1;
    L = ftell(fp_);
    if(L==-1L){
        Log::e("ftelll failed");
        L=0;
    }
out1:
    fsetpos(fp_, &old);
out:
    return L;
}

File * FileUtils::Open_stdio(const char *filename)
{
    char buffer[4096];
    FILE *fp=nullptr;
    const size_t n = strlen(filename);
    const char *mode = "rb";
    if(filename[0] == '/'){
        fp = fopen(filename, mode);
        goto out;
    }
    for(auto &x : FileUtils::searchPaths_){
        size_t sn = x.size();
        if(n+sn +2> sizeof(buffer)){
            Log::d("too long path(%s)+filename(%s)", x.c_str(), filename);
            continue;
        }
        auto p = buffer + sn;
        memcpy(buffer, x.c_str(), sn);
        *p++ = '/';
        memcpy(p, filename, n+1);

        fp = fopen(buffer, mode);
        if(fp!=nullptr)
            break;
    }
out:
    if(fp)
        return MM<File_stdio>::New(fp);
    Log::e("open file <%s> failed", filename);
    return nullptr;
}

void FileUtils::addSearchPath(const char *path)
{
    if(path==nullptr || path[0]=='\0')
        return;
    size_t n = strlen(path);
    char buffer[4096];
    const char *p=path;
    if(p[0]=='~' && (n==1 || p[1]=='/')){
        auto home = getenv("HOME");
        size_t len;
        if(home==nullptr){
            Log::d("expected ~, but not set");
            return;
        }
        len = strlen(home);
        if(len==0 || len+n > sizeof(buffer)){
            Log::d("tooo long for path=%s", path);
            return ;
        }
        memcpy(buffer, home, len);
        if(n>1)
            memcpy(buffer+len, &path[1], n);

        p = buffer;
    }
    searchPaths_.push_back(p);
}
