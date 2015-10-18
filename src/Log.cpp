#include <Cerror.hpp>
#include <Log.hpp>
#include <stdlib.h>
Logger::Logger(){}
Logger::~Logger(){}
Cerror Logger::write(const char *fmt, ...)
{
    va_list ap;
    va_start (ap, fmt);
    auto e = vwrite(fmt, ap);
    va_end(ap);
    return e;
}

Cerror Logger::vwrite(const char *fmt, va_list ap)
{
    printf("TAG=[%s] - ", tag_.c_str());
    vprintf(fmt, ap);
    return nullptr;
}
void Logger::panic(const char *fmt, ...)
{
    va_list ap;
    va_start (ap, fmt);
    panic(fmt, ap);
    va_end(ap);
}
void Logger::panic(const char *fmt, va_list ap)
{
    vwrite(fmt, ap);
    exit(0);
}

Ref_ptr<Logger> Log::log_ = nullptr;
void Log::setLogger(const Ref_ptr<Logger> &log)
{
    log_ = log;
}

Cerror Log::dumpLevel(const char *level, const char *fmt, va_list ap)
{
    auto l = log_.get();
    auto e = Cerror(nullptr);
    if(!l)
        goto out;
    l->write("level=%s : ", level);
    e = l->vwrite(fmt, ap);
    l->write("\n");
out:
    return e;
}
Cerror Log::d(const char *fmt, ...)
{
    va_list ap;
    va_start (ap, fmt);
    auto e = dumpLevel("debug", fmt, ap);
    va_end(ap);
    return e;
}
Cerror Log::i(const char *fmt, ...)
{
    va_list ap;
    va_start (ap, fmt);
    auto e = dumpLevel("info", fmt, ap);
    va_end(ap);
    return e;
}

void Log::panic(const char *fmt, ...)
{
    auto l = log_.get();
    if(!l)
        goto out;
    va_list ap;
    va_start (ap, fmt);
    l->panic(fmt, ap);
    va_end(ap);
out:
    return;
}
