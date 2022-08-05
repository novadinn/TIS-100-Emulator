#include "graphics.h"

namespace {
    const int kScreenWidth = 1366;
    const int kScreenHeight = 768;
}

std::string Graphics::kMainFontPath = "PerfectDOSVGA437";
int Graphics::kMainFontSize = 15;

Graphics::Graphics() {
    window_ = SDL_CreateWindow("TIS Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			       kScreenWidth, kScreenHeight, 0);
    renderer_ = SDL_CreateRenderer(window_, -1, 0);
}

Graphics::~Graphics() {
    for (std::map<std::string, SDL_Texture*>::iterator iter = sprite_sheets_.begin();
	 iter != sprite_sheets_.end();
	 ++iter) {
        SDL_DestroyTexture(iter->second);
    }

    SDL_DestroyRenderer(renderer_);
    SDL_DestroyWindow(window_);
}

SDL_Texture* Graphics::loadImage(const std::string& path) {
    const std::string file_path = "content/" + path + ".bmp";
    if (sprite_sheets_.count(file_path) == 0) {
        SDL_Surface* image = SDL_LoadBMP(file_path.c_str());
        sprite_sheets_[file_path] = SDL_CreateTextureFromSurface(renderer_, image);
        SDL_FreeSurface(image);
    }
    return sprite_sheets_[file_path];
}

TTF_Font* Graphics::loadFont(const std::string& path, int size) {
    const std::string file_path = "content/" + path + ".ttf";
    const std::string memory_path = "content/" + path + std::to_string(size) + ".ttf";
    if (font_sheets_.count(memory_path) == 0) {
        TTF_Font* font = TTF_OpenFont(file_path.c_str(), size);
        font_sheets_[memory_path] = font;
    }
    return font_sheets_[memory_path];
}

void Graphics::blitSurface(SDL_Texture* src, SDL_Rect* src_rect, SDL_Rect* dst_rect) {
    if (src_rect) {
        dst_rect->w = src_rect->w;
        dst_rect->h = src_rect->h;
    } else {
        uint32_t format;
        int access, w, h;
        SDL_QueryTexture(src, &format, &access, &w, &h);
        dst_rect->w = w;
        dst_rect->h = h;
    }

    SDL_RenderCopy(renderer_, src, src_rect, dst_rect);
}

void Graphics::textureDimensions(SDL_Texture* texture, int& w, int& h) {
    uint32_t format;
    int access;
    SDL_QueryTexture(texture, &format, &access, &w, &h);
}

SDL_Texture* Graphics::createTextTexture(TTF_Font* font, const std::string& text, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_ , surface);
    SDL_FreeSurface(surface);
    return texture;
}

void Graphics::destroyTexture(SDL_Texture* texture) {
    if(texture != NULL) {
	SDL_DestroyTexture(texture);
	texture = NULL;
    }
}

void Graphics::getWindowPosition(int& x, int& y) const {
    SDL_GetWindowPosition(window_, &x, &y);
}

void Graphics::clear() {
    SDL_RenderClear(renderer_);
}

void Graphics::flip() {
    SDL_RenderPresent(renderer_);
}
