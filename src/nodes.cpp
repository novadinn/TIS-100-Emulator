#include "nodes.h"

#include "tis_syntax.h"

#include <iostream>

namespace {
    const std::string kSpriteFilePath = "node";

    const int kTextWidth = 18;
    const int kTextHeight = 15;

    const int kNodeWidth = 154;
    const int kNodeHeight = 189;
    
    const int kSpriteWidth = 3;
    const int kXOffset = 3;
    const int kDstBetweenLines = 12;
}

ComputeNode::ComputeNode(Graphics& gfx, int x, int y) :
    itb(gfx, Graphics::kMainFontPath, Graphics::kMainFontSize, kTextWidth, kTextHeight, 255, 255, 255,
	Rectangle(x+kSpriteWidth+kXOffset, y+kSpriteWidth, kNodeWidth, kNodeHeight)),
    desc_(gfx, x, y),
    sprite_(gfx, kSpriteFilePath),
    x_(x), y_(y) {}

void ComputeNode::setHandlers(MessageHandler* l, MessageHandler* r, MessageHandler* u, MessageHandler* d) {
    left_ = l;
    right_ = r;
    up_ = u;
    down_ = d;
}

void ComputeNode::startExecution() {
    itb.active = true;
    itb.moveCursorBoxStart();
    if(itb.filterLines(tis_syntax::skippableLine).size() == kTextHeight) {
	empty_node_ = true;
    }
    if(tis_syntax::skippableLine(itb.readLine())) {
	nextLine();
    }
}

void ComputeNode::tickUpdate() {
    std::string line = tis_syntax::removeLable(tis_syntax::simplifyLine(itb.readLine()));
    jumped_ = false;

    if(mode_ == IDLE) {
	std::string op = tis_syntax::operation(line);

	if(op == "ADD") {
	    std::string operand = tis_syntax::firstOperand(line);
	    if(tis_syntax::numberOperand(operand)) {
		addAcc(atoi(operand.c_str()));
	    } else if(tis_syntax::accOperand(operand)) {
		addAcc(acc_);
	    } else if(tis_syntax::portOperand(operand)) {
		MessageHandler* port = handlerFromDestination(operand);
		callback_ = [&](int val) { addAcc(val); };
		port->requestMessage(this, callback_);
	    }
	} else if(op == "SUB") {
	    std::string operand = tis_syntax::firstOperand(line);
	    if(tis_syntax::numberOperand(operand)) {
		subAcc(atoi(operand.c_str()));
	    } else if(tis_syntax::accOperand(operand)) {
		subAcc(acc_);
	    } else if(tis_syntax::portOperand(operand)) {
		MessageHandler* port = handlerFromDestination(operand);
		callback_ = [&](int val) { subAcc(val); };
		port->requestMessage(this, callback_);
	    }
	} else if(op == "NEG") {
	    neg();
	} else if(op == "SAV") {
	    sav();
	} else if(op == "SWP") {
	    swp();
	} else if(op == "MOV") {
	    std::string operand1 = tis_syntax::firstOperand(line);
	    std::string operand2 = tis_syntax::secondOperand(line);
	    if(tis_syntax::numberOperand(operand1) && tis_syntax::portOperand(operand2)) {
		MessageHandler* port = handlerFromDestination(operand2);
		port->acceptMessage(this, atoi(operand1.c_str()));
	    } else if(tis_syntax::accOperand(operand1) && tis_syntax::portOperand(operand2)) {
		MessageHandler* port = handlerFromDestination(operand2);
		port->acceptMessage(this, acc_);
	    } else if(tis_syntax::portOperand(operand1) && tis_syntax::accOperand(operand2)) {
		MessageHandler* port = handlerFromDestination(operand1);
		callback_ = [&](int val) { setAcc(val); };
		port->requestMessage(this, callback_);
	    } else if(tis_syntax::portOperand(operand1) && tis_syntax::portOperand(operand2)) {
		MessageHandler* port1 = handlerFromDestination(operand1);
		temp_port_ = handlerFromDestination(operand2);
		callback_ = [&](int val) {
		    temp_port_->acceptMessage(this, val);
		    temp_port_ = nullptr;
		};
		port1->requestMessage(this, callback_);
	    } else if(tis_syntax::portOperand(operand1) && tis_syntax::nilOperand(operand2)) {
		MessageHandler* port1 = handlerFromDestination(operand1);
		callback_ = [&](int val) {};
		port1->requestMessage(this, callback_);
	    }
	} else if(op == "JMP") {
	    jmp(line);
	} else if(op == "JEZ") {
	    if(acc_ == 0) {
		jmp(line);
	    }	
	} else if(op == "JNZ") {
	    if(acc_ != 0) {
		jmp(line);
	    }
	} else if(op == "JGZ") {
	    if(acc_ > 0) {
		jmp(line);
	    }
	} else if(op == "JLZ") {
	    if(acc_ < 0) {
		jmp(line);
	    }
	} else if(op == "JRO") {
	    std::string operand = tis_syntax::firstOperand(line);
	    if(tis_syntax::accOperand(operand)){
		jro(acc_);
	    } else if(tis_syntax::numberOperand(operand)) {
		jro(atoi(operand.c_str()));
	    }
	}
    }
}

void ComputeNode::nextLine() {
    if(mode_ == IDLE && !jumped_ && !empty_node_) {
	do {
	    if(itb.lastLine()) {
		itb.moveCursorBoxStart();
	    } else {
		itb.moveCursorDown();
	    }
	} while(tis_syntax::skippableLine(itb.readLine()));
    }
}

void ComputeNode::endExecution() {
    itb.active = false;
    itb.moveCursorBoxStart();
    setMode(IDLE);
    setAcc(0);
    sav();
    left_->reset();
    right_->reset();
    up_->reset();
    down_->reset();
    jumped_ = false;
    empty_node_ = false;
    temp_port_ = nullptr;
    callback_ = [&](int val) {};
}

void ComputeNode::setMode(Mode mode) {
    mode_ = mode;
    if(mode_ == IDLE) {
	desc_.mode_value.setText("IDLE");
    } else if(mode_ == READ) {
	desc_.mode_value.setText("READ");
    } else {
	desc_.mode_value.setText("WRITE");
    }
}

void ComputeNode::draw() {
    sprite_.draw(x_, y_);
    desc_.draw();
    itb.draw(kDstBetweenLines);
}

MessageHandler* ComputeNode::handlerFromDestination(std::string name) const {
    if(name == "LEFT") {
	return left_;
    } else if(name == "RIGHT") {
	return right_;
    } else if(name == "UP") {
	return up_;
    } else if(name == "DOWN") {
	return down_;
    }
    return nullptr;
}

void ComputeNode::addAcc(int val) {
    setAcc(acc_ + val);
}

void ComputeNode::subAcc(int val) {
    setAcc(acc_ - val);
}

void ComputeNode::setAcc(int val) {
    if(val > 999) {
	acc_ = 999;
    } else if(val < -999) {
	acc_ = -999;
    } else {
	acc_ = val;
    }
    desc_.acc_value.setText(std::to_string(acc_));
}

void ComputeNode::neg() {
    setAcc(-acc_);
}

void ComputeNode::sav() {
    bak_ = acc_;
    desc_.bak_value.setText("<" + std::to_string(bak_) + ">");
}

void ComputeNode::swp() {
    int t = acc_;
    setAcc(bak_);
    bak_ = t;
    desc_.bak_value.setText("<" + std::to_string(bak_) + ">");
}

void ComputeNode::jmp(std::string line) {
    std::string label = tis_syntax::nameToLabel(tis_syntax::firstOperand(line));
    int goto_line = itb.findMatch(label);
    if(goto_line != -1) {
	itb.gotoLine(goto_line);
	if(!tis_syntax::skippableLine(itb.readLine()))
	    jumped_ = true;
    }
}

void ComputeNode::jro(int val) {
    int cur = itb.currentLineIndex();
    int nv = cur + val;
    if(nv <= 0) {
	jumped_ = true;
	return;
    } else if(nv >= kTextHeight) {
	nv = kTextHeight-1;
    }
    itb.gotoLine(nv-1);
}

namespace {
    const int kNodeOffsetX = 163;
    const int kNodeOffsetY = 7;

    const int kNameValueDstY = 13;
    const int kDescDstY = 38;
    const int kSectionWidth = 45;
}

ComputeNode::DescriptionSection::DescriptionSection(Graphics& gfx, int x, int y) :
    acc_name(gfx, Graphics::kMainFontPath, Graphics::kMainFontSize, 3, "ACC", 167, 167, 167),
    acc_value(gfx, Graphics::kMainFontPath, Graphics::kMainFontSize, 4, "0", 255, 255, 255),
    bak_name(gfx, Graphics::kMainFontPath, Graphics::kMainFontSize, 3, "BAK", 167, 167, 167),
    bak_value(gfx, Graphics::kMainFontPath, Graphics::kMainFontSize, 6, "<0>", 255, 255, 255),
    last_name(gfx, Graphics::kMainFontPath, Graphics::kMainFontSize, 4, "LAST", 167, 167, 167),
    last_value(gfx, Graphics::kMainFontPath, Graphics::kMainFontSize, 5, "N/A", 255, 255, 255),
    mode_name(gfx, Graphics::kMainFontPath, Graphics::kMainFontSize, 4, "MODE", 167, 167, 167),
    mode_value(gfx, Graphics::kMainFontPath, Graphics::kMainFontSize, 5, "IDLE", 255, 255, 255),
    idle_name(gfx, Graphics::kMainFontPath, Graphics::kMainFontSize, 4, "IDLE", 167, 167, 167),
    idle_value(gfx, Graphics::kMainFontPath, Graphics::kMainFontSize, 3, "0%", 255, 255, 255),
    x_(x), y_(y) {}

void ComputeNode::DescriptionSection::draw() {
    int x = x_ + kNodeOffsetX;
    int y = y_ + kNodeOffsetY;
    acc_name.drawCenteredX(x, y, kSectionWidth);
    acc_value.drawCenteredX(x, y+kNameValueDstY, kSectionWidth);
    y += kDescDstY;
    bak_name.drawCenteredX(x, y, kSectionWidth);
    bak_value.drawCenteredX(x, y+kNameValueDstY, kSectionWidth);
    y += kDescDstY;
    last_name.drawCenteredX(x, y, kSectionWidth);
    last_value.drawCenteredX(x, y+kNameValueDstY, kSectionWidth);
    y += kDescDstY;
    mode_name.drawCenteredX(x, y, kSectionWidth);
    mode_value.drawCenteredX(x, y+kNameValueDstY, kSectionWidth);
    y += kDescDstY;
    idle_name.drawCenteredX(x, y, kSectionWidth);
    idle_value.drawCenteredX(x, y+kNameValueDstY, kSectionWidth);
}

namespace {
    const int kMaxNameSize = 10;

    const std::string kISpritePath = "input";
    const int kIDescWidth = 35;
    const int kDescYOffset = 4;
    const int kDescNameYOffset = -16;
}

InputNode::InputNode(Graphics& gfx, std::vector<std::string> values, std::string name, int index,
		     int x, int y, int dx, int dy) :
    index_(index),
    name_text_(gfx, Graphics::kMainFontPath, Graphics::kMainFontSize, kMaxNameSize, name, 255, 255, 255),
    desc_text_box_(gfx, Graphics::kMainFontPath, Graphics::kMainFontSize, 4, values.size(), 255, 255, 255),
    desc_sprite_(gfx, kISpritePath),
    x_(x), y_(y), desc_x_(dx), desc_y_(dy) {
    desc_text_box_.setTexts(values);
}

void InputNode::setHandler(MessageHandler* handler) {
    handler_ = handler;
}

void InputNode::tickUpdate() {
    if(handler_->acceptMessage(this, atoi(desc_text_box_.readLine(current_line_index_).c_str())))
	++current_line_index_;
}

void InputNode::endExecution() {
    current_line_index_ = 0;
}

int InputNode::index() const {
    return index_;
}

void InputNode::draw() {
    name_text_.drawCenteredX(x_, y_, kNodeWidth);
    desc_sprite_.draw(desc_x_, desc_y_);
    desc_text_box_.drawCenteredX(desc_x_, desc_y_+kDescYOffset, kDstBetweenLines, kIDescWidth);
    name_text_.drawCenteredX(desc_x_, desc_y_+kDescNameYOffset, kIDescWidth);
}

namespace {
    const std::string kOSpritePath = "output";
    const int kODescWidth = 75;

    const int kOutputBoxXOffset = 35;
    const int kOutputBoxWidth = 41;
}

OutputNode::OutputNode(Graphics& gfx, std::vector<std::string> values, std::string name, int index,
		       int x, int y, int dx, int dy) :
    index_(index),
    name_text_(gfx, Graphics::kMainFontPath, Graphics::kMainFontSize, kMaxNameSize, name, 255, 255, 255),
    desc_text_box_(gfx, Graphics::kMainFontPath, Graphics::kMainFontSize, 4, values.size(), 255, 255, 255),
    output_text_box_(gfx, Graphics::kMainFontPath, Graphics::kMainFontSize, 4, values.size(), 255, 255, 255),
    desc_sprite_(gfx, kOSpritePath),
    x_(x), y_(y), desc_x_(dx), desc_y_(dy) {
    desc_text_box_.setTexts(values);
}

void OutputNode::setHandler(MessageHandler* handler) {
    handler_ = handler;
}

void OutputNode::tickUpdate() {
    callback_ = [&](int val) {
	output_text_box_.setLine(current_line_index_, std::to_string(val));
	++current_line_index_;
    };
    handler_->requestMessage(this, callback_);
}

void OutputNode::endExecution() {
    current_line_index_ = 0;
    output_text_box_.clear();
}

int OutputNode::index() const {
    return index_;
}

void OutputNode::draw() {
    name_text_.drawCenteredX(x_, y_, kNodeWidth);
    desc_sprite_.draw(desc_x_, desc_y_);
    desc_text_box_.drawCenteredX(desc_x_, desc_y_+kDescYOffset, kDstBetweenLines, kIDescWidth);
    output_text_box_.drawCenteredX(desc_x_+kOutputBoxXOffset, desc_y_+kDescYOffset,
				   kDstBetweenLines, kOutputBoxWidth);
    name_text_.drawCenteredX(desc_x_, desc_y_+kDescNameYOffset, kODescWidth);
}

namespace {
    const std::string kDamagedSpritePath = "damaged";
}

DamagedNode::DamagedNode(Graphics& gfx, int x, int y) :
    sprite_(gfx, kDamagedSpritePath), x_(x), y_(y) {}

void DamagedNode::tickUpdate() {}

void DamagedNode::draw() {
    sprite_.draw(x_, y_);
}
