#pragma once

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include <map>
#include <string>

struct Graphics {
    Graphics();
    ~Graphics();

    SDL_Texture* loadImage(const std::string& file_name);
    TTF_Font* loadFont(const std::string& file_name, int font_size);
    
    void blitSurface(SDL_Texture* source, SDL_Rect* source_rectangle, SDL_Rect* destination_rectangle);
    void textureDimensions(SDL_Texture* texture, int& w, int& h);
    
    SDL_Texture* createTextTexture(TTF_Font* font, const std::string& text, SDL_Color color);
    void destroyTexture(SDL_Texture* texture);
    
    void getWindowPosition(int& x, int& y) const;
    
    void clear();
    void flip();

    static std::string kMainFontPath;
    static int kMainFontSize;
private:
    SDL_Window* window_;
    SDL_Renderer* renderer_;

    std::map<std::string, SDL_Texture*> sprite_sheets_;
    std::map<std::string, TTF_Font*> font_sheets_;
};
