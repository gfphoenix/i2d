#ifndef _BUFFER_HPP_
#define _BUFFER_HPP_
#include <stddef.h>
// only move, copy is not allowed
struct Buffer 
{
    unsigned char *addr;
    size_t size;
};

class LineReader final
{
    public:
        LineReader(const Buffer &buffer):
            buffer_(buffer), pos_(0){}
        int readLine(unsigned char *p, size_t n, bool skipRemaining=true);
        inline unsigned int length()const{return buffer_.size;}
        inline long getPos()const{return pos_;}
        inline void setPos(long pos){pos_=pos;}
        inline void rewind(){pos_=0;}
        inline int readLine(Buffer &b)
        {
            return readLine(b.addr, b.size, true);
        }
        inline void skipLine()
        {
            readLine(nullptr, 0);
        }

    private:
        const Buffer &buffer_;
        long pos_;
};
#endif // _BUFFER_HPP_
