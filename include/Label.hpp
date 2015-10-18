#ifndef LABEL_HPP
#define LABEL_HPP
#include "Ref.hpp"
#include "BMFont.hpp"
#include "Node.hpp"
#include <vector>

class Renderer;
class Label : public Node
{
public:
    Label();
    virtual ~Label();
    void setString(const std::string &str);
    void setString(std::string &&str);
    const std::string &getString()const{return str_;}
    unsigned getCharSize()const{return codes_.size();}
    uint16_t getCharAt(unsigned idx){return codes_[idx];}
    template <typename T>
    inline void setBMfontSet(T bmSet)
    {
        bmSet_=std::forward<T>(bmSet);
        if(bmSet_){
            setWidth(calculateTextWidth());
            setHeight(bmSet_->getLineHeight());
        }
    }
    void DrawSelf(Renderer *renderer)override;
protected:
    int calculateTextWidth()const;
    void updateCodes();
    Ref_ptr<BMFont> bmSet_;
    std::string str_;
    std::vector<uint16_t> codes_;
};

#endif // LABEL_HPP
