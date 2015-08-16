#ifndef PS_HPP
#define PS_HPP

static inline int roundUp(int n, int N)
{
    if(n<0)
        n += (-n/N + 1)*N;
    return n % N;
}
template <typename T, size_t n>
class Q
{
public:
    inline bool isEmpty()const{return prod==cons;}
    inline bool isFull()const{return (prod+1)%n==cons;}
    bool get(T &out)
    {
        if(isEmpty())
            return false;
        out = data[cons];
        cons++;
        if(cons>= n)
            cons -= n;
        return true;
    }
    bool put(const T &x)
    {
        if(isFull())
            return false;
        data[prod] = x;
        prod++;
        if(prod>=n)
            prod -= n;
        return true;
    }

    inline Q():
        prod(0), cons(0)
    {}
    T data[n];
    int prod;
    int cons;
};

template <typename T, size_t n>
class qArray
{
    public:
        inline qArray():first(0){static_assert(n>0,"N<=0");}
        inline const T at(int idx)const
        {
            return data[roundUp(first+idx, n)];
        }
        inline T & operator [](int idx)
        {
            return data[roundUp(first+idx, n)];
        }
        inline const T & operator [](int idx)const
        {
            return data[roundUp(first+idx, n)];
        }

        void resetOrder(){first=0;}
        void move(int m)
        {
            first = roundUp(first + m, n);
        }

        T data[n];
        int first;
};
// statistics for percent second
template <typename Tp, typename Tm>
class PerSecond
{
public:
    PerSecond(Tm samplePeriod, double value):
        first(0), second(0),
        tmp(0), counter(0),
        samplePeriod(samplePeriod)
    {
        firstTime = getCurrentTime();
        secondTime = firstTime;
        tmpTime = firstTime;
        ps = value;
    }

    //template <typename T>
   // void init(double value);
    void step(Tp delta)
    {
        tmpTime = tmpTime + getCurrentTime();

        counter += delta;
        if (secondTime + samplePeriod <= tmpTime) {
            ps = (counter-first)/(tmpTime-firstTime);
            first = second;
            firstTime = secondTime;
            second = counter;
            secondTime = tmpTime;
        }
    }
    double getPs()
    {
        return ps;
    }

    virtual Tm getCurrentTime()const=0;
protected:
    Tp first;
    Tp second;
    Tp tmp;
    Tp counter;
    Tm samplePeriod;
    Tm firstTime;
    Tm secondTime;
    Tm tmpTime;

    double ps;
};

#endif // PS_HPP

