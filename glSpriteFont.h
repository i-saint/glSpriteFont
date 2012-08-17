#ifndef __glSpriteFont_h__
#define __glSpriteFont_h__

#ifndef glIFR_InterModule
    #define glIFR_InterModule __declspec(dllimport)
#endif // glIFR_InterModule

class glIFR_InterModule glIFontRenderer
{
protected:
    virtual ~glIFontRenderer() {}
public:
    virtual void release()=0;
    virtual void setScreen(float left, float right, float bottom, float top)=0;
    virtual void setColor(float r, float g, float b, float a)=0;
    virtual void setSize(float size)=0;
    virtual void setSpacing(float space)=0; // 文字幅の倍率
    virtual void setMonospace(bool v)=0; // 等幅にするか

    virtual void addText(float x, float y, const char *text, size_t len=0)=0;   // len==0 だと strlen で自動的に計算します
    virtual void addText(float x, float y, const wchar_t *text, size_t len=0)=0;// wchar_t 版。こっちの方が速いのでできるだけこっち使いましょう
    virtual void flush()=0;
};

glIFR_InterModule glIFontRenderer* CreateGLSpriteFont(const char *path_to_sff, const char *path_to_image);

#endif // __glSpriteFont_h__
