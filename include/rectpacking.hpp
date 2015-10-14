#ifndef RECTPACKING
#define RECTPACKING
#include <string>
#include <vector>

struct Rect {
    int x;
    int y;
    int width;
    int height;
    bool rotate;

    inline Rect():
        x(0),y(0),width(0),height(0),rotate(false){}
    inline void init(int x,int y, int w, int h)
    {
        this->x         = x;
        this->y         = y;
        this->width     = w;
        this->height    = h;
        this->rotate    = false;
    }
// valid
    inline operator bool()const{return width>0 && height>0;}
    virtual ~Rect(){}
};

class Packer
{
protected:
    int imageWidth;
    int imageHeight;
public:
    Packer():imageWidth(0),imageHeight(0){}
    virtual ~Packer(){}
    inline int getImageWidth()const{return imageWidth;}
    inline int getImageHeight()const{return imageHeight;}
    inline void setImageSize(int w, int h){imageWidth=w;imageHeight=h;}
    virtual void init(int width, int height){setImageSize(width, height);}
    virtual float getOccupancy()const=0;
    virtual Rect insert(int w, int h, bool mayRotate)=0;
};

struct Tag{
    std::string name;
    void *ptr;
    Tag():name(""),ptr(nullptr){}
};
struct RectTag : public Rect
{
    Tag tag;
};

// calculate rectangle one time
class PackSorter
{
protected:
    std::vector<RectTag> rects;
    std::vector<RectTag> result;
    int imageWidth;
    int imageHeight;
public:
    PackSorter():imageWidth(0),imageHeight(0){}
    virtual ~PackSorter(){}
    inline int getImageWidth()const{return imageWidth;}
    inline int getImageHeight()const{return imageHeight;}
    inline void setImageSize(int w, int h){imageWidth=w;imageHeight=h;}
    virtual float getOccupancy()const=0;
    virtual void reset()
    {
        rects.clear();
        result.clear();
    }

    void addRect(int w, int h, const Tag &tag)
    {
        RectTag rt;
        rt.width = w;
        rt.height = h;
        rt.tag = tag;
        rects.push_back(rt);
    }
    // true: success, false: failed, not enough room
    virtual bool pack(bool mayRotate)=0;
    const std::vector<RectTag> &getResult()const{return result;}
};

#endif // RECTPACKING

