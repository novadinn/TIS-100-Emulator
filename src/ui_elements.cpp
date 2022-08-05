#include "ui_elements.h"

Sprite::Sprite(Graphics& gfx, const std::string& path) :
    gfx_(gfx) {
    sprite_sheet_ = gfx_.loadImage(path);

    gfx_.textureDimensions(sprite_sheet_, source_rectangle_.w, source_rectangle_.h);
    source_rectangle_.x = 0; // NOTE: this should be done by hand!
    source_rectangle_.y = 0;
}

void Sprite::draw(int x, int y) {
    SDL_Rect destination_rectangle;
    destination_rectangle.x = x;
    destination_rectangle.y = y;
    gfx_.blitSurface(sprite_sheet_, &source_rectangle_, &destination_rectangle);
}

Text::Text(Graphics& gfx, const std::string& path, int size, int mw,
	   const std::string& init_text, int r, int g, int b) :
    gfx_(gfx), max_width_(mw) {
    font_ = gfx_.loadFont(path, size);
    color_ = {(Uint8)r, (Uint8)g, (Uint8)b };
    setText(init_text);
}

Text::Text(const Text& o) :
    gfx_(o.gfx_) {
    text_ = o.text_;
    color_ = o.color_;
    max_width_ = o.max_width_;
    font_ = o.font_;
    updateTexture();
    source_rectangle_ = o.source_rectangle_;
}

Text::~Text() {
    gfx_.destroyTexture(sprite_sheet_);
}

bool Text::addChar(char sym,int x) {
    if(text_.length() < max_width_) {
	text_ = text_.insert(x, &sym);
	updateTexture();
	return true;
    }
    return false;
}

bool Text::deleteChar(int x) {
    if((x-1) < text_.length() && x > 0) {
	text_.erase(x-1, 1);
	updateTexture();
	return true;
    }
    return false;
}

bool Text::addText(std::string r) {
    std::string n = text_ + r;
    if(n.length() <= max_width_) {
	setText(n);
	return true;
    }
    return false;
}

void Text::setText(std::string nt) {
    if(nt.length() <= max_width_) {
	text_ = nt;
	updateTexture();
    }
}

std::string Text::getText() const {
    return text_;
}

void Text::draw(int x, int y) {
    SDL_Rect destination_rectangle;
    destination_rectangle.x = x;
    destination_rectangle.y = y;
    gfx_.blitSurface(sprite_sheet_, &source_rectangle_, &destination_rectangle);
}

void Text::drawCenteredX(int x, int y, int w) {
    int tw;
    int th;
    gfx_.textureDimensions(sprite_sheet_, tw, th);
    int xo = (w - tw)/2;
    SDL_Rect destination_rectangle;
    destination_rectangle.x = x + xo;
    destination_rectangle.y = y;
    gfx_.blitSurface(sprite_sheet_, &source_rectangle_, &destination_rectangle);
}

void Text::drawCenteredY(int x, int y, int h) {
    int tw;
    int th;
    gfx_.textureDimensions(sprite_sheet_, tw, th);
    int yo = (h - th*2)/2;
    SDL_Rect destination_rectangle;
    destination_rectangle.x = x;
    destination_rectangle.y = y + yo;
    gfx_.blitSurface(sprite_sheet_, &source_rectangle_, &destination_rectangle);
}

void Text::drawCentered(int x, int y, int w, int h) {
    int tw;
    int th;
    gfx_.textureDimensions(sprite_sheet_, tw, th);
    int xo = (w - tw)/2;
    int yo = (h - th)/2;
    SDL_Rect destination_rectangle;
    destination_rectangle.x = x + xo;
    destination_rectangle.y = y + yo;
    gfx_.blitSurface(sprite_sheet_, &source_rectangle_, &destination_rectangle);
}

void Text::updateTexture() {
    gfx_.destroyTexture(sprite_sheet_);
    sprite_sheet_ = gfx_.createTextTexture(font_, text_, color_);
    gfx_.textureDimensions(sprite_sheet_, source_rectangle_.w, source_rectangle_.h);
    source_rectangle_.x = 0;
    source_rectangle_.y = 0;
}

TextBox::TextBox(Graphics& gfx, const std::string path, int size,
		 int tw, int th, int r, int g, int b) :
    text_width_(tw), text_height_(th) {
    for(int i = 0; i < th; ++i) {
	lines.push_back(Text(gfx, path, size, tw, "", r, g, b));
    }
}

void TextBox::setTexts(std::vector<std::string> texts) {
    for(int i = 0; i < lines.size() && i < texts.size(); ++i) {
	lines[i].setText(texts[i]);
    }
}

void TextBox::setLine(int i, std::string txt) {
    if(i < lines.size())
	lines[i].setText(txt);
}

std::string TextBox::readLine(int i) const {
    if(i < lines.size())
	return lines[i].getText();
    return "";
}

void TextBox::clear() {
    for(int i = 0; i < lines.size(); ++i) {
	lines[i].setText("");
    }
}

std::vector<Text> TextBox::filterLines(std::function<bool(std::string)> pred) const {
    std::vector<Text> result;
    for(int i = 0; i < lines.size(); ++i) {
	std::string str = lines[i].getText();
	if(pred(str))
	    result.push_back(lines[i]);
    }
    return result;
}

int TextBox::findMatch(std::string str) {
    int index = -1;
    for(int i = 0; i < lines.size(); ++i) {
	std::string line = lines[i].getText();
	size_t found = line.find(str);
	if (found != std::string::npos) {
	    index = i;
	    break;
	}
    }
    return index;
}

void TextBox::draw(int x, int y, int dst) {
    for(int i = 0; i < lines.size(); ++i) {
	lines[i].draw(x, y+(i*dst));
    }
}

void TextBox::drawCenteredX(int x, int y, int dst, int w) {
    for(int i = 0; i < lines.size(); ++i) {
	lines[i].drawCenteredX(x, y+(i*dst), w);
    }
}

void TextBox::drawCenteredY(int x, int y, int dst, int h) {
    for(int i = 0; i < lines.size(); ++i) {
	lines[i].drawCenteredY(x, y+(i*dst), h);
    }
}

void TextBox::drawCentered(int x, int y, int dst, int w, int h) {
    for(int i = 0; i < lines.size(); ++i) {
	lines[i].drawCentered(x, y+(i*dst), w, h);
    }
}

namespace {
    const std::string kCursorFilePath = "cursor";
}

InputTextBox::InputTextBox(Graphics& gfx, const std::string path, int size,
			   int tw, int th, int r, int g, int b,
			   Rectangle rect) :
    TextBox(gfx, path, size, tw, th, r, g, b),
    cursor_(gfx, kCursorFilePath),
    rectangle_(rect),
    active(false) {}

void InputTextBox::moveCursorLeft() {
    if(cursor_.x > 0) {
	--cursor_.x;
    	cursor_.resetTimer();
    }
}

void InputTextBox::moveCursorRight() {
    if(cursor_.x < lines[currentLineIndex()].getText().length()) {
	++cursor_.x;
    	cursor_.resetTimer();
    }
}

void InputTextBox::moveCursorUp() {
    if(cursor_.y > 0) {
	--cursor_.y;
	cursor_.x = lines[currentLineIndex()].getText().length();
	cursor_.resetTimer();
    }
}

void InputTextBox::moveCursorDown() {
    if(!lastLine()) {
	++cursor_.y;
	cursor_.x = lines[currentLineIndex()].getText().length();
	cursor_.resetTimer();
    }
}

void InputTextBox::moveCursorLineStart() {
    cursor_.x = 0;
    cursor_.resetTimer();
}

void InputTextBox::moveCursorLineEnd() {
    cursor_.x = lines[currentLineIndex()].getText().length();
    cursor_.resetTimer();
}

void InputTextBox::moveCursorBoxStart() {
    gotoLine(0);
}

void InputTextBox::moveCursorBoxEnd() {
    gotoLine(text_height_-1);
}

void InputTextBox::gotoLine(int i) {
    if(i > -1 && i < text_height_) {
	cursor_.y = i;
	cursor_.x = lines[currentLineIndex()].getText().length();
	cursor_.resetTimer();
    }
}

std::string InputTextBox::readLine() const {
    return lines[currentLineIndex()].getText();
}

int InputTextBox::currentLineIndex() const {
    return cursor_.y;
}

bool InputTextBox::lastLine() const {
    return currentLineIndex() == text_height_-1;
}

void InputTextBox::typeSym(char sym) {
    if(lines[currentLineIndex()].addChar(sym, cursor_.x)) {
	cursor_.resetTimer();
	moveCursorRight();
    }
}

void InputTextBox::deleteSym() {
    if(lines[currentLineIndex()].deleteChar(cursor_.x)) {
	cursor_.resetTimer();
	moveCursorLeft();
    } else if (cursor_.y > 0) {
	if(lines[currentLineIndex()-1].addText(lines[currentLineIndex()].getText())) {
	    std::string t = "";
	    for(int i = text_height_-1; i > currentLineIndex()-1; --i) {
		std::string nt = lines[i].getText();
		lines[i].setText(t);
		t = nt;
	    }
	    moveCursorUp();
	    cursor_.resetTimer();
	}
    }
}

void InputTextBox::newline() {
    if(lines[text_height_-1].getText().length() == 0 && currentLineIndex() < text_height_-1) {
	std::string t = lines[currentLineIndex()].getText();
	for(int i = currentLineIndex()+1; i < text_height_; ++i) {
	    std::string nt = lines[i].getText();
	    lines[i].setText(t);
	    t = nt;
	}
	lines[currentLineIndex()+1].setText(
	    lines[currentLineIndex()].getText().substr(
		cursor_.x, lines[currentLineIndex()].getText().length()));
	lines[currentLineIndex()].setText(lines[currentLineIndex()].getText().substr(0, cursor_.x));
	moveCursorDown();
	cursor_.resetTimer();
    }
}

bool InputTextBox::raise(int x, int y) {
    bool state = rectangle_.pointIntersection(x, y);
    active = state;
    if(active)
	cursor_.resetTimer();
    return state;
}

void InputTextBox::update(int dt) {
    cursor_.update(dt);
}

void InputTextBox::draw(int dst) {
    TextBox::draw(rectangle_.left(), rectangle_.top(), dst);
    if(active)
	cursor_.draw(rectangle_.left(), rectangle_.top(),
		     rectangle_.width()/text_width_, rectangle_.height()/text_height_);
}

void InputTextBox::drawCenteredX(int dst) {
    TextBox::drawCenteredX(rectangle_.left(), rectangle_.top(), dst, rectangle_.width());
    if(active)
	cursor_.draw(rectangle_.left(), rectangle_.top(),
		     rectangle_.width()/text_width_, rectangle_.height()/text_height_);
}

namespace {
    const int kFlashTime = 700;
}

InputTextBox::Cursor::Cursor(Graphics& gfx, const std::string path) :
    sprite_(gfx, path), x(0), y(0), timer_(0) {}


void InputTextBox::Cursor::resetTimer() {
    timer_ = 0;
}

void InputTextBox::Cursor::update(int dt) {
    timer_ += dt;
}

void InputTextBox::Cursor::draw(int ox, int oy, int w, int h) {
    if (timer_ / kFlashTime % 2 == 1) return;
    sprite_.draw(ox + x*w, oy + y*h);
}
