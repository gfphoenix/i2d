#ifndef LOG_HPP_
#define LOG_HPP_
#include "Ref.hpp"
#include "Cerror.hpp"
#include <string>
class Logger : public Ref
{
public:
    inline void setTag(const std::string &tag){tag_=tag;}
    const std::string &getTag()const{return tag_;}
    Logger();
    virtual ~Logger();
    virtual Cerror write(const char *fmt, ...);
    virtual Cerror vwrite(const char *fmt, va_list ap);
    virtual void panic(const char *fmt, ...);
    virtual void panic(const char *fmt, va_list ap);

private:
    std::string tag_;
};

class Log
{
private:
    Log()=delete;
    static Ref_ptr<Logger> log_;
public:
    static void setLogger(const Ref_ptr<Logger> &log);
    static Cerror d(const char *fmt, ...);
    static Cerror i(const char *fmt, ...);
    static Cerror e(const char *fmt, ...);
    static void panic(const char *fmt, ...);
protected:
    static Cerror dumpLevel(const char *level, const char *fmt, va_list ap);
};

#endif // LOG

