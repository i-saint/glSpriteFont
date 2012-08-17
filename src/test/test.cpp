#include <string>
#include <windows.h>
#include <SDL/SDL.h>
#include <GL/glew.h>
#include "../glSpriteFont.h"

#ifdef _WIN32
#   pragma comment(lib,"SDL.lib")
#   pragma comment(lib,"SDLmain.lib")
#   pragma comment(lib,"opengl32.lib")
#   pragma comment(lib,"glSpriteFont.lib")
#endif


class App
{
private:
    bool m_end_flag;
    size_t m_width;
    size_t m_height;
    glIFontRenderer *m_font;

public:
    App(size_t width=640, size_t height=480)
        : m_end_flag(false)
        , m_width(width)
        , m_height(height)
    {
        if(SDL_Init(SDL_INIT_VIDEO)<0) {
            throw std::runtime_error("SDL_Init()");
        }
        if(SDL_SetVideoMode(m_width, m_height, 0, SDL_OPENGL)==0) {
            throw std::runtime_error("SDL_SetVideoMode()");
        }

        m_font = CreateGLSpriteFont("font.sff", "font.png");
        if(m_font != NULL) {
            m_font->setScreen(0.0f, float(width), float(height), 0.0f);
        }
    }

    ~App()
    {
        if(m_font) { m_font->release(); }
        SDL_Quit();
    }


    void exec()
    {
        while(!m_end_flag) {
            update();
            draw();
        }
    }

    void handleSDLEvent()
    {
        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            if(e.type==SDL_QUIT) {
                m_end_flag = true;
            }
            else if(e.type==SDL_KEYUP) {
                if(e.key.keysym.sym==SDLK_ESCAPE) {
                    m_end_flag = true;
                }
            }
        }
    }

    void update()
    {
        handleSDLEvent();
    }

    void draw()
    {
        glViewport(0,0, m_width,m_height);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        if(m_font) {
            drawFont();
        }

        glDisable(GL_BLEND);

        SDL_GL_SwapBuffers();
    }

    void drawFont()
    {
        m_font->setMonospace(false);
        m_font->setSpacing(1.0f);
        m_font->setSize(24.0f);

        m_font->setColor(1.0f, 1.0f, 1.0f, 1.0f);
        m_font->addText(10.0f, 10.0f, "ABCDEFGあいうえお");
        m_font->addText(10.0f, 40.0f, "やったー！日本語出たよー！");

        m_font->setColor(0.6f, 0.6f, 1.0f, 1.0f);
        m_font->addText(10.0f, 70.0f, L"文字色変更");

        m_font->setColor(1.0f, 0.0f, 0.0f, 1.0f);
        m_font->setSize(30.0f);
        m_font->addText(10.0f, 100.0f, L"文字サイズ変更");

        m_font->setColor(1.0f, 1.0f, 1.0f, 1.0f);
        m_font->setSize(24.0f);
        m_font->setMonospace(true);
        m_font->setSpacing(0.75f);
        m_font->addText(10.0f, 130.0f, L"以下等幅");
        m_font->addText(10.0f, 160.0f, "ABCDEFGあいうえお");

        m_font->setSpacing(1.2f);
        m_font->addText(10.0f, 190.0f, L"スペース幅変更");

        m_font->flush();
    }
};


int main(int argc, char *argv[])
{
    try {
        App *app = new App();
        app->exec();
        delete app;
    }
    catch(const std::runtime_error& e) {
        puts(e.what());
    }

    return 0;
}
