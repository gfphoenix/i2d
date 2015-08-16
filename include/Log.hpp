#ifndef LOG
#define LOG
#include "Ref.hpp"
#include <string>
class Logger : public Ref
{
public:
    inline void setTag(const std::string &tag){tag_=tag;}
    const std::string &getTag()const{return tag_;}
    void panic(const char *fmt, ...);
    void error(const char *fmt, ...);
    void info(const char *fmt, ...);
    void debug(const char *fmt, ...);

private:
    static Ref_ptr<Logger> stdLog;
    std::string tag_;
};

#endif // LOG

