#include <Buffer.hpp>
#include <string.h>
int LineReader::readLine(unsigned char *p, size_t n, bool skipRemaining)
{
    auto const SIZE = buffer_.size;
    if(pos_>=SIZE){
        if(p&&n>0)
            *p='\0';
        return -1; // end
    }
    auto end=pos_;
    auto q = buffer_.addr;
    for(; end<SIZE; end++){
        if(q[end]=='\r' || q[end]=='\n')
            break;
    }
    auto len = end - pos_;
    if(n<len)
        len = n;
    if(len>0)
        memcpy(p, &q[pos_], len);
    /// when buffer size is not enough
    if(len == n && len>0){
        p[--len] = '\0';
        pos_ += len;
        if(!skipRemaining)
            goto out;
    }
    /// append '\0' and move pos_ to the next line
    if(len<n)
        p[len] = '\0';

    if(end<SIZE){
        pos_ = end+1;
        if(pos_<SIZE && q[end]=='\r' && q[pos_]=='\n')
            pos_++;
    }else{
        pos_ = SIZE;
    }
out:
    return len;
}
