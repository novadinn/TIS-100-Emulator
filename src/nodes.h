#pragma once

#include "ui_elements.h"
#include "message_handler.h"

struct Node {
    virtual ~Node() {}
    
    enum Mode {
	IDLE,
	READ,
	WRITE
    };
    virtual void setMode(Mode mode) {}
    
    virtual void tickUpdate() = 0;
};

struct ComputeNode : public Node {
    ComputeNode(Graphics& gfx, int x, int y);

    void setHandlers(MessageHandler* l, MessageHandler* r, MessageHandler* u, MessageHandler* d);

    void startExecution();
    void tickUpdate() override;
    void nextLine();
    void endExecution();

    void setMode(Mode mode) override;
    
    void draw();
    
    InputTextBox itb;
private:
    MessageHandler* handlerFromDestination(std::string name) const;
    
    void addAcc(int val);
    void subAcc(int val);
    void setAcc(int val);
    void neg();
    void sav();
    void swp();
    void jmp(std::string label);
    void jro(int val);
    
    struct DescriptionSection {
	DescriptionSection(Graphics& gfx, int x, int y);

	void resetToDefault();
	void draw();
	
	Text acc_name;
	Text acc_value;
	Text bak_name;
	Text bak_value;
	Text last_name;
	Text last_value;
	Text mode_name;
	Text mode_value;
	Text idle_name;
	Text idle_value;
    private:
	int x_, y_;
    };

    MessageHandler* left_;
    MessageHandler* right_;
    MessageHandler* up_;
    MessageHandler* down_;
    int acc_ = 0;
    int bak_ = 0;
    Mode mode_ = IDLE;
    std::function<void(int)> callback_;
    MessageHandler* temp_port_ = nullptr;
    bool jumped_ = false;
    bool empty_node_ = false;

    Sprite sprite_;
    DescriptionSection desc_;
    int x_, y_;
};

struct InputNode : public Node {
    InputNode(Graphics& gfx, std::vector<std::string> values, std::string name, int index,
	      int x, int y, int dx, int dy);

    void setHandler(MessageHandler* handler);
    
    void tickUpdate() override; // TODO: add reset to text data
    void endExecution();
    
    int index() const;
    
    void draw();
private:
    MessageHandler* handler_;
    int index_;

    Text name_text_;
    TextBox desc_text_box_;
    int current_line_index_ = 0;
    Sprite desc_sprite_;
    int x_, y_;
    int desc_x_, desc_y_;
};

struct OutputNode : public Node {
    OutputNode(Graphics& gfx, std::vector<std::string> values, std::string name, int index,
	       int x, int y, int dx, int dy);

    void setHandler(MessageHandler* handler);

    void tickUpdate() override;
    void endExecution();

    int index() const;

    void draw();
private:
    MessageHandler* handler_;
    std::function<void(int)> callback_;
    int index_;

    Text name_text_;
    TextBox desc_text_box_;
    TextBox output_text_box_;
    int current_line_index_ = 0;
    Sprite desc_sprite_;
    int x_, y_;
    int desc_x_, desc_y_;
};

struct DamagedNode : public Node {
    DamagedNode(Graphics& gfx, int x, int y);

    void tickUpdate() override;
    
    void draw();
private:
    Sprite sprite_;
    int x_, y_;
};
