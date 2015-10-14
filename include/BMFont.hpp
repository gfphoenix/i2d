#ifndef BMFONT_HPP
#define BMFONT_HPP
#include "Ref.hpp"
#include "BMFontSet.hpp"
#include "Node.hpp"
#include <vector>

class Renderer;
class BMFont : public Node
{
public:
    BMFont();
    virtual ~BMFont();
    void setString(const std::string &str);
    void setString(std::string &&str);
    const std::string &getString()const{return str_;}
    unsigned getCharSize()const{return codes_.size();}
    uint16_t getCharAt(unsigned idx){return codes_[idx];}
    template <typename T>
    void setBMfontSet(T bmSet){bmSet_=std::forward<T>(bmSet);}
    void DrawSelf(Renderer *renderer)override;
protected:
    void updateCodes();
    Ref_ptr<BMFontSet> bmSet_;
    std::string str_;
    std::vector<uint16_t> codes_;
};

#endif // BMFONT_HPP
