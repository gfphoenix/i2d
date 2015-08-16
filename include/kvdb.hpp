#ifndef KVDB
#define KVDB
#include "Cerror.hpp"
#include <stdint.h>
#include <string>
#include <vector>
class KV
{
public:
    Cerror setBool(const char *key, bool value)=0;
    Cerror setInt(const char *key, int value)=0;
    Cerror setLong(const char *key, long value)=0;
    Cerror setU64(const char *key, uint64_t value)=0;
    Cerror setFloat(const char *key, double value)=0;
    Cerror setString(const char *key, const std::string &value)=0;
    Cerror setString(const char *key, std::string &&value)=0;

    bool getBool(const char *key, bool defValue)=0;
    int getInt(const char *key, int defValue)=0;
    long getLong(const char *key, long defValue)=0;
    uint64_t getU64(const char *key, uint64_t defValue)=0;
    double getFloat(const char *key, double defValue)=0;
    std::string getString(const char *key, const std::string &defValue)=0;

    std::vector<std::string> getKeys()=0;
    bool hasKey(const char *key);
    Cerror remove(const char *key)=0;
};

#endif // KVDB

