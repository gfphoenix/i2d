#ifndef _TEXTURE_LOADER_HPP
#define _TEXTURE_LOADER_HPP
#include <functional>
class ImageReader
{
    public:
        std::function<int(char*, int)> Read;
        std::function<void(int)> Skip;
        std::function<bool()> IsEof;
};
// component types
enum class CompType
{
    // needed by stb library
    Default = 0,
    LUMINANCE,
    LUMINANCE_ALPHA,
    RGB,
    RGBA,
};
class Image_RGBA8 final
{
    private:
        std::string resource_name;
        unsigned char *data_;
        int width_;
        int height_;
        CompType comp_;
static Image_RGBA8 xxCreate(const std::function<unsigned char *(int *w,int *h, int *comp)> &fn);
        Image_RGBA8(const Image_RGBA8 &m)=delete;
        Image_RGBA8 &operator=(const Image_RGBA8 &)=delete;
        Image_RGBA8(unsigned char *data, int w,int h, CompType comp);
            static void *operator new(std::size_t n)=delete;
            static void *operator new[](std::size_t n)=delete;
            static void operator delete(void *ptr)=delete;
            static void operator delete[](void *ptr)=delete;
    public:
        // this method is platform specific, and should have special care.
        // for android, files from assets, there is one api for accessing it.
        // so, take it carefully
        static Image_RGBA8 create(const char *filename, CompType type);
        static Image_RGBA8 create(const ImageReader &reader, CompType type);
        static Image_RGBA8 create(const void *data, size_t len, CompType type); // decode image from memory
        static Image_RGBA8 create(FILE *file, CompType type);
        inline static Image_RGBA8 create(const char *filename){return create(filename,CompType::Default);}
        inline static Image_RGBA8 create(const ImageReader &reader){return create(reader,CompType::Default);}
        inline static Image_RGBA8 create(const void *data, size_t len){return create(data,len,CompType::Default);}
        inline static Image_RGBA8 create(FILE *file){return create(file,CompType::Default);}

        ~Image_RGBA8();
        // neither copy constructor nor assign operator is provided
        Image_RGBA8(Image_RGBA8 &&m);
        Image_RGBA8 &operator=(Image_RGBA8 &&);
        inline operator bool()const{return data_!=nullptr;}

        inline int getWidth()const{return width_;}
        inline int getHeight()const{return height_;}
        inline CompType getCompType()const{return comp_;}
        inline void *getData()const{return data_;}
        void reverseY();
};
#endif /* _TEXTURE_LOADER_HPP */
