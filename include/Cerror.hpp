#ifndef CERROR
#define CERROR
class Cerror
{
protected:
    const char * err_;
public:
    Cerror(const char *e):err_(e){}
    inline const char *err()const{return err_;}
    inline operator bool()const{return err_==nullptr;}



    // predefined error
    static Cerror Eexist;
    static Cerror Eperm;
    static Cerror Enospace;
    static Cerror Eio;
};

#endif // CERROR

