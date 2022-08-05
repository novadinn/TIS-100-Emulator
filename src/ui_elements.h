#pragma once

#include <string>
#include <vector>
#include <functional>

#include "graphics.h"
#include "rectangle.h"

struct Sprite {
    Sprite(Graphics& gfx, const std::string& path);
    
    void draw(int x, int y);
private:
    SDL_Texture* sprite_sheet_;
    SDL_Rect source_rectangle_;
    Graphics& gfx_;
};

struct Text {
    Text(Graphics& gfx, const std::string& path, int size, int mw,
	 const std::string& init_text, int r, int g, int b);
    Text(const Text& t1);
    ~Text();

    bool addChar(char sym, int x);
    bool deleteChar(int x);
    bool addText(std::string r);

    void setText(std::string nt);
    std::string getText() const;
    
    void draw(int x, int y);
    void drawCenteredX(int x, int y, int w);
    void drawCenteredY(int x, int y, int h);
    void drawCentered(int x, int y, int w, int h);
private:
    void updateTexture();
    
    Graphics& gfx_;
    std::string text_;
    SDL_Color color_;
    int max_width_;
    
    TTF_Font* font_;
    SDL_Texture* sprite_sheet_ = NULL;
    SDL_Rect source_rectangle_;
};

struct TextBox {
    TextBox(Graphics& gfx, const std::string path, int size,
	    int tw, int th, int r, int g, int b);
    virtual ~TextBox() {}

    void setTexts(std::vector<std::string> texts);
    void setLine(int i, std::string txt);
    std::string readLine(int i) const;
    void clear();
    
    std::vector<Text> filterLines(std::function<bool(std::string)> pred) const;
    int findMatch(std::string str);
    
    void draw(int x, int y, int dst);
    void drawCenteredX(int x, int y, int dst, int w);
    void drawCenteredY(int x, int y, int dst, int h);
    void drawCentered(int x, int y, int dst, int w, int h);
    
    std::vector<Text> lines;
protected:
    int text_width_;
    int text_height_;
};

struct InputTextBox : public TextBox {
    InputTextBox(Graphics& gfx, const std::string path, int size,
		 int tw, int th, int r, int g, int b,
		 Rectangle rect);

    void moveCursorLeft();
    void moveCursorRight();
    void moveCursorUp();
    void moveCursorDown();
    void moveCursorLineStart();
    void moveCursorLineEnd();
    void moveCursorBoxStart();
    void moveCursorBoxEnd();
    void gotoLine(int i);
    
    std::string readLine() const;
    int currentLineIndex() const;
    bool lastLine() const;
    
    void typeSym(char sym);
    void deleteSym();
    void newline();
    bool raise(int x, int y);
    
    void update(int dt);
    
    void draw(int dst);
    void drawCenteredX(int dst);
    
    bool active;
private:
    struct Cursor {
	Cursor(Graphics& graphics, const std::string file_name);
	
	void resetTimer();
	void update(int dt);
	
	void draw(int ox, int oy, int w, int h);

	int x, y;
    private:
	Sprite sprite_;
	int timer_;
    };

    Cursor cursor_;
    Rectangle rectangle_;
};
