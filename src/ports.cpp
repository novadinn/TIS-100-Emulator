#include "ports.h"

#include <iostream>

namespace {
    const std::string kLeftFilePath = "left";
    const std::string kRightFilePath = "right";
    const std::string kUpFilePath = "up";
    const std::string kDownFilePath = "down";
}

std::vector<Port::SenderData> Port::senders_;
std::vector<Port::ReceiverData> Port::receivers_;

Port::Port(Graphics& gfx, PortDirection direction, Node* src, Node* target, int x, int y) :
    sprite_(gfx, direction == LEFT ? kLeftFilePath :
	    direction == RIGHT ? kRightFilePath :
	    direction == UP ? kUpFilePath : kDownFilePath),
    text_(gfx, Graphics::kMainFontPath, Graphics::kMainFontSize, 4, "", 255, 255, 255),
    source_(src), target_(target),
    direction_(direction), x_(x), y_(y) {}

bool Port::acceptMessage(Node* src, int val) {
    if(target_ == src) {
	src->setMode(Node::Mode::WRITE);
	return false;
    }
    SenderData data = { source_, target_, &text_, val };
    auto it = std::find(begin(senders_), end(senders_), data);
    if(it == std::end(senders_)) {
	senders_.push_back(data);
	return true;
    }
    return false;
}

void Port::requestMessage(Node* src, std::function<void(int)> callback) {
    Node* ttrt = target_;
    Node* tsrc = source_;
    if(src != source_) {
	ttrt = source_;
	tsrc = target_;
    }
    ReceiverData data = { ttrt, tsrc, callback };
    auto it = std::find(begin(receivers_), end(receivers_), data);
    if(it == std::end(receivers_)) {
	receivers_.push_back(data);
    }
}

void Port::reset() {
    text_.setText("");
    senders_.clear();
    receivers_.clear();
}

void Port::processMessages() {
    for(int i = 0; i < receivers_.size(); ++i) {
	Node* sender = receivers_[i].sender;
	Node* receiver = receivers_[i].receiver;
	receiver->setMode(Node::Mode::READ);
	for(int j = 0; j < senders_.size(); ++j) {
	    if(sender == senders_[j].sender && receiver == senders_[j].receiver) {
		if(senders_[j].text->getText() != "") {
		    int val = senders_[j].val;
		    receivers_[i].callback(val);
		    
		    receiver->setMode(Node::Mode::IDLE);
		    sender->setMode(Node::Mode::IDLE);
		    senders_[j].text->setText("");
		    auto rec_it = std::find(begin(receivers_), end(receivers_), receivers_[i--]);
		    receivers_.erase(rec_it);
		    auto sen_it = std::find(begin(senders_), end(senders_), senders_[j--]);
		    senders_.erase(sen_it);
		}
	    }
	}
    }

    for(int i = 0; i < senders_.size(); ++i) {
	senders_[i].sender->setMode(Node::Mode::WRITE);
	senders_[i].text->setText(std::to_string(senders_[i].val));
    }
}

void Port::draw() {
    int tox = 0;
    int toy = 0;
    if(direction_ == RIGHT) {
	toy -= 15;
    } else if(direction_ == LEFT) {
	toy += 10;
    } else if(direction_ == UP) {
	tox -= 26;
    } else {
	tox += 16;
    }
    
    sprite_.draw(x_, y_);
    if(direction_ == LEFT || direction_ == RIGHT) {
	text_.drawCenteredX(x_+tox, y_+toy, 20);
    } else {
	text_.drawCentered(x_+tox, y_+toy, 20, 20);
    }
    
}

bool DummyPort::acceptMessage(Node* src, int val) {
    src->setMode(Node::Mode::WRITE);
    return false;
}

void DummyPort::requestMessage(Node* src, std::function<void(int)> callback) {
    src->setMode(Node::Mode::READ);
}

void DummyPort::reset() {}
