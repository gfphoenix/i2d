#ifndef BITS_HPP
#define BITS_HPP
void * memset (void *BLOCK, int C, size_t SIZE);
typedef unsigned long ul;
inline static int bits_(){return sizeof(ul)*8;}
template <int N>
class bits
{
private:
    ul data_[(N+bits_()-1)/bits_()];
public:
    bits(){zeros();}
    inline void zeros(){memset(data_, 0, sizeof(data_));}
    inline void ones(){memset(data_, -1, sizeof(data_));}
    inline bool isSet(int nr)const{
	int idx = nr/bits_();
	int off = nr%bits_();
	return (data_[idx]&(((ul)1)<<off)) != 0;
    }
    inline void set(int nr){
	int idx = nr/bits_();
	int off = nr%bits_();
	data_[idx] |= ((ul)1)<<off;
    }
    inline void clear(int nr){
	int idx = nr/bits_();
	int off = nr%bits_();
	data_[idx] &= ~(((ul)1)<<off);
    }
    inline void toggle(int nr){
	int idx = nr/bits_();
	int off = nr%bits_();
	data_[idx] ^= (((ul)1)<<off);
    }

    inline int size()const{return sizeof(data_)/sizeof(ul);}
    inline bool isAllZero()const{
	int i=size();
	while(i-- >0){
	    if(data[i])
		return false;
	}
	return true;
    }
};

#endif // BITS_HPP

