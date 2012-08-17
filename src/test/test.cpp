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
        SDL_EnableUNICODE(1);
        SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
        if(SDL_SetVideoMode(m_width, m_height, 0, SDL_OPENGL)==0) {
            throw std::runtime_error("SDL_Init()");
        }

        m_font = CreateSpriteFont("font.sff", "font.png");
        if(m_font != NULL) {
            m_font->setScreen(0.0f, float(width), float(height), 0.0f);
            m_font->setMonospace(false);
            m_font->setSpacing(1.0f);
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
            std::wstring text;
            float base_pos = 40.0f;
            float color[4][4] = {
                {1.0f, 1.0f, 1.0f, 1.0f},
                {1.0f, 1.0f, 1.0f, 0.5f},
                {1.0f, 0.5f, 0.5f, 1.0f},
                {0.5f, 0.5f, 1.0f, 1.0f},
            };
            m_font->setMonospace(false);
            m_font->setSpacing(1.0f);
            m_font->setSize(24.0f);
            for(int i=0; i<4; ++i) {
                m_font->setColor(color[i][0], color[i][1], color[i][2], color[i][3]);
                m_font->addText(10.0f, base_pos, L"ABCDEFG");
                base_pos += 30.0f;
                m_font->addText(10.0f, base_pos, L"やったー！日本語出たよー！");
                base_pos += 30.0f;

                if(i==1) {
                    base_pos += 10.0f;
                    m_font->setSize(20.0f);
                    m_font->addText(10.0f, base_pos, L"以下等幅");
                    base_pos += 30.0f;
                    m_font->setMonospace(true);
                    m_font->setSpacing(0.75f);
                }
            }
            m_font->flush();
        }

        glDisable(GL_BLEND);

        SDL_GL_SwapBuffers();
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
