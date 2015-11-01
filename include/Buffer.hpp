#ifndef _BUFFER_HPP_
#define _BUFFER_HPP_
#include <stddef.h>
// only move, copy is not allowed
struct Buffer 
{
    unsigned char *addr;
    size_t size;
    Buffer(){}
private:
    Buffer(const Buffer &copy)=delete;
    Buffer &operator=(const Buffer &copy)=delete;
};
class BufferA final : public Buffer
{
protected:

public:
    explicit BufferA(size_t N)
    {
        addr    = new unsigned char[N];
        size    = N;
    }
    BufferA(BufferA &&tmp)
    {
        addr    = tmp.addr;
        size    = tmp.size;
        tmp.addr = nullptr;
        tmp.size = 0;
    }
    BufferA & operator=(BufferA &&tmp)
    {
        addr = tmp.addr;
        tmp.addr = nullptr;
        size = tmp.size;
        tmp.size = 0;
        return *this;
    }
    ~BufferA()
    {
        delete []addr;
    }
    inline unsigned char *Addr(){return addr;}
    inline const unsigned char *Addr()const{return addr;}
    inline size_t Size()const{return size;}

private:
    BufferA(const BufferA &copy)=delete;
    BufferA &operator =(const BufferA &tmp)=delete;
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
