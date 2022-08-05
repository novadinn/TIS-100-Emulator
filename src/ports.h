#pragma once

#include "nodes.h"

enum PortDirection {
    LEFT,
    RIGHT,
    UP,
    DOWN
};

struct Port : public MessageHandler {
    Port(Graphics& gfx, PortDirection direction, Node* src, Node* target, int x, int y);

    bool acceptMessage(Node* src, int val) override;
    void requestMessage(Node* src, std::function<void(int)> callback) override;
    void reset() override;
    static void processMessages();
    
    void draw();
private:
    Sprite sprite_;
    Text text_;
    
    Node* source_;
    Node* target_;

    struct SenderData {
	Node* sender;
	Node* receiver;
	Text* text;
	int val;

	inline bool operator==(const SenderData& o) {
	    return sender == o.sender && receiver == o.receiver;
	}
    };
    struct ReceiverData {
	Node* sender;
	Node* receiver;
	std::function<void(int)> callback;

	inline bool operator==(const ReceiverData& o) {
	    return sender == o.sender && receiver == o.receiver;
	}
    };

    static std::vector<SenderData> senders_;
    static std::vector<ReceiverData> receivers_;
    
    PortDirection direction_;
    int x_, y_;
};

struct DummyPort : public MessageHandler {
    bool acceptMessage(Node* src, int val) override;
    void requestMessage(Node* src, std::function<void(int)> callback) override;
    void reset() override;
};
