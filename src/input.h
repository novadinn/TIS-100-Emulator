#pragma once

#include "SDL2/SDL.h"
#include <map>

struct Input {
    void beginNewFrame();

    void keyDownEvent(const SDL_Event& event);
    void keyUpEvent(const SDL_Event& event);

    bool wasKeyPressed(SDL_Keycode key);
    bool isKeyHeld(SDL_Keycode key);
    bool wasKeyReleased(SDL_Keycode key);

    void mouseDownEvent(const SDL_Event& event);
    void mouseUpEvent(const SDL_Event& event);

    bool wasMouseButtonPressed(Uint8 index);
    bool isMouseButtonPressed(Uint8 index);
    bool wasMouseButtonReleased(Uint8 index);
    
private:
    std::map<SDL_Keycode, bool> pressed_keys_;
    std::map<SDL_Keycode, bool> held_keys_;
    std::map<SDL_Keycode, bool> released_keys_;

    std::map<Uint8, bool> pressed_mouse_buttons_;
    std::map<Uint8, bool> held_mouse_buttons_;
    std::map<Uint8, bool> released_mouse_buttons_;
};
