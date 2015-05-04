#include <memory>
#include <string>
#include <iostream>
#include <SDL2/SDL.h>

using namespace std;

class image
{
public:
    image(SDL_Renderer *rend, const string &file, int w, int h);
    bool draw(int x, int y);
    bool merge(image &layer);
    void setBlendMode(SDL_BlendMode bm);

    SDL_Surface *surface()
    { return m_surface; }

private:
    SDL_Renderer *m_renderer;
    SDL_Surface *m_surface;
    SDL_Texture *m_texture;
    string m_file;
    int m_width;
    int m_height;
};

image::image(SDL_Renderer *rend, const string &file, int w, int h)
    : m_renderer(rend), m_file(file), m_width(w), m_height(h)
{
    m_surface = SDL_LoadBMP(file.c_str());
    m_texture = SDL_CreateTextureFromSurface(m_renderer, m_surface);
}

bool image::draw(int x, int y)
{
    SDL_Rect destrect;
    destrect.x = x;
    destrect.y = y;
    destrect.w = m_width;
    destrect.h = m_height;

    int result = SDL_RenderCopy(m_renderer, m_texture, NULL, &destrect);
    if (result < 0)
    {
        cout << "SDL_RenderCopy failed: " << SDL_GetError() << " for file "
             << m_file << endl;
        return false;
    }
    return true;
}

bool image::merge(image &layer)
{
    SDL_Rect clip;
    clip.x = 75;
    clip.y = 79;
    clip.w = 55;
    clip.h = 45;

    //SDL_RenderDrawRect(m_renderer, &clip);

    SDL_Rect dest;
    dest.x = 10;
    dest.y = 100;

    int result = SDL_BlitSurface(layer.surface(), &clip, surface(), &dest);
    if (result < 0)
    {
        cout << "SDL_RenderCopy failed: " << SDL_GetError() << " for file "
             << m_file << endl;
        return false;
    }

    m_texture = SDL_CreateTextureFromSurface(m_renderer, surface());
    return true;
}

void image::setBlendMode(SDL_BlendMode bm)
{
    SDL_SetSurfaceBlendMode(surface(), bm);
}

int main()
{
    //
    // SDL initialisation
    //
    SDL_Window *window;
    SDL_Renderer *renderer;

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
        return false;
    window = SDL_CreateWindow( "Image Compositing Test",
                               SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED,
                               640, 480, 0 );
    if (window == NULL)
        return false;

    renderer = SDL_CreateRenderer(window, -1, 0);
    if (renderer == NULL)
    {
        SDL_DestroyWindow(window);
        return false;
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    //
    // Draw some images
    //
    auto_ptr<image> inner;
    auto_ptr<image> outer;
    auto_ptr<image> merged;

    inner.reset(new image(renderer, "inner.bmp", 200, 200));
    outer.reset(new image(renderer, "outer.bmp", 200, 200));

    merged.reset(new image(renderer, "outer.bmp", 200, 200));
    inner->setBlendMode(SDL_BLENDMODE_BLEND);
    merged->merge(*inner);

    for (int i = 0; i < 6; i++)
    {
        Uint8 r,g,b,a;

        SDL_RenderClear(renderer);

        SDL_Rect clip;
        clip.x = 0;
        clip.y = 0;
        clip.w = 100; //55;
        clip.h = 100; //30;

        SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(renderer, &clip);
        SDL_RenderDrawRect(renderer, &clip);
        SDL_SetRenderDrawColor(renderer, r, g, b, a);

        outer->draw(10, 240);
        //SDL_RenderPresent(renderer);

        //SDL_Delay(500);

        inner->draw(220, 240);
        //SDL_RenderPresent(renderer);

        //SDL_Delay(500);

        merged->draw(430, 240);
        SDL_RenderPresent(renderer);

        SDL_Delay(1000);
    }
}
