#ifndef G_HPP
#define G_HPP
#include "Ref.hpp"
#include "Log.hpp"
// global G data
struct Gconf
{
public:
    void panic(const char *fmt, ...);
    void error(const char *fmt, ...);
    void info(const char *fmt, ...);
    void debug(const char *fmt, ...);
    void setLogger(Ref_ptr<Logger> log);
    const char *version()const;
private:
    Logger *log;
    static Ref_ptr<Logger> def;
};
extern Gconf G;
#endif // G_HPP

