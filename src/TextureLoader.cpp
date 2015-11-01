#include <config.hpp>
#include <Buffer.hpp>
#include <File.hpp>
#include <TextureLoader.hpp>
#include <functional>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <string.h>

using namespace std::placeholders;

Image_RGBA8 Image_RGBA8::xxCreate(const std::function<unsigned char *(int *w,int *h, int *comp)> &fn)
{
    int w, h, comp;
    unsigned char *data = fn(&w, &h, &comp);
    Assert(data != nullptr, "Image::xxCreate() failed");
    return Image_RGBA8(data, w, h, (CompType)comp);
}

Image_RGBA8 Image_RGBA8::create(const char *filename, CompType ct)
{
    auto file = FileUtils::Open(filename);
    auto NIL = Image_RGBA8(nullptr, 0, 0, CompType::Default);
    if(!file)
        return NIL;
    auto len = file->Length();
    if(len==0 || len>(128UL<<20))
        return NIL;
    BufferA buffer(len);
    auto n = file->Read(buffer.Addr(), buffer.Size());
    if(n<(int)buffer.Size())
        return NIL;
    return xxCreate(std::bind(stbi_load_from_memory, buffer.Addr(), buffer.Size(), _1, _2, _3, (int)ct));
}
static int xxRead(void *user, char *data, int size)
{
    auto fn = reinterpret_cast<ImageReader *>(user);
    return fn->Read(data, size);
}
static void xxSkip(void *user, int n)
{
    auto fn = reinterpret_cast<ImageReader *>(user);
    fn->Skip(n);
}
static int xxEof(void *user)
{
    auto fn = reinterpret_cast<ImageReader *>(user);
    return fn->IsEof();
}
Image_RGBA8 Image_RGBA8::create(const ImageReader &reader, CompType ct)
{
    stbi_io_callbacks callback;
    callback.read = xxRead;
    callback.skip = xxSkip;
    callback.eof  = xxEof;
    return xxCreate(std::bind(stbi_load_from_callbacks,
                &callback, (void*)&reader, _1, _2, _3, (int)ct));
}
Image_RGBA8 Image_RGBA8::create(const void *data, size_t len, CompType ct)
{
    auto p = (const unsigned char *)data;
    auto &&fn = std::bind(stbi_load_from_memory, p,(int)len, _1, _2, _3, (int)ct);
    return xxCreate(fn);
}
Image_RGBA8 Image_RGBA8::create(FILE *file, CompType ct)
{
    return xxCreate(std::bind(stbi_load_from_file,file,_1,_2,_3,(int)ct));
}
Image_RGBA8::Image_RGBA8(unsigned char *data, int w, int h, CompType comp):
    data_(data)
    , width_(w)
    , height_(h)
    , comp_(comp)
{}
Image_RGBA8::Image_RGBA8(Image_RGBA8 &&m):
    data_(m.data_)
    , width_(m.width_)
    , height_(m.height_)
    , comp_(m.comp_)
{
    m.data_ = nullptr;
    m.width_ = m.height_ = 0;
}
Image_RGBA8::~Image_RGBA8()
{
    if(data_ != nullptr){
        stbi_image_free(data_);
        data_=nullptr;
    }
}
Image_RGBA8 &Image_RGBA8::operator=(Image_RGBA8 &&m)
{
    auto p = data_;
    auto q = m.data_;
    data_ = q;
    m.data_ = nullptr;
    width_ = m.width_;
    height_ = m.height_;
    m.width_ = m.height_ = 0;
    if(p != nullptr)
        stbi_image_free(p);
    return *this;
}
void Image_RGBA8::reverseY()
{
    auto bytesPerLine = width_*(int)comp_;
    unsigned char tmp[bytesPerLine];
    auto n = height_/2;
    auto off = 0;
    auto tail = (height_-1)*bytesPerLine;
    for(int i=0; i<n; i++){
        ::memcpy(tmp, &data_[off], bytesPerLine);
        ::mempcpy(&data_[off], &data_[tail], bytesPerLine);
        ::memcpy(&data_[tail], tmp, bytesPerLine);
        off += bytesPerLine;
        tail-= bytesPerLine;
    }
}

//int main()
//{
//    auto img = Image::create("map.png", CompType::GREY);
//    img = Image::create("", 0, CompType::GREY);
//    auto file = fopen("map.png", "rb");
//    img = Image::create(file);
//
//    ImageReader ir;
//    fseek(file, 0, SEEK_SET);
//    ir.Read = [file](char *buffer, int size)->int{
//        auto n = (int)fread(buffer, 1, size, file);
//        return n;
//    };
//    ir.Skip = [file](int n){
//        fseek(file, n, SEEK_CUR);
//    };
//    ir.IsEof = [file]()->bool{
//        return feof(file) != 0;
//    };
//    img = Image::create(ir);
//
//    fclose(file);
//    return 0;
//}
