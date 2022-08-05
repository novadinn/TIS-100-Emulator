#pragma once

#include <string>
#include <functional>

struct Node;

struct MessageHandler {
    virtual bool acceptMessage(Node* src, int val) = 0;
    virtual void requestMessage(Node* src, std::function<void(int)> callback) = 0;
    virtual void reset() = 0;
};
