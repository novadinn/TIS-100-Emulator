#include "tis.h"

#include "graphics.h"
#include "input.h"
#include "ports.h"
#include "program_data.h"

#include <iostream>

namespace {
    const int kDeltaTime = 1;
    const int kGameSpeed = 1;
}

TIS::TIS() {
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    eventLoop();
}

TIS::~TIS() {
    SDL_Quit();
    TTF_Quit();
}

void TIS::eventLoop() {
    Graphics graphics;
    Input input;
    SDL_Event event;

    TextBox description_box(graphics, Graphics::kMainFontPath, Graphics::kMainFontSize,
			    100, program_data::programDescription().size(), 255, 255, 255);
    description_box.setTexts(program_data::programDescription());
    Sprite description_sprite(graphics, "description");
    Text program_name(graphics, Graphics::kMainFontPath, Graphics::kMainFontSize,
		      program_data::programName().size(), program_data::programName(), 255, 255, 255);
    
    state_ = EDIT;
    std::vector<ComputeNode*> nodes;
    std::vector<DamagedNode*> damaged_nodes;
    std::vector<program_data::NodeType> layout = program_data::getLayout();
    for(int i = 0; i < layout.size(); ++i) {
	const int sx = 355;
	const int sy = 53;
	const int ix = 257;
	const int iy = 235;
	
	if(layout[i] == program_data::TILE_COMPUTE) {
	    ComputeNode* node = new ComputeNode(graphics, sx + (i%4)*ix, sy + (i/4)*iy);
	    nodes.push_back(node);
	} else if(layout[i] == program_data::TILE_DAMAGED) {
	    DamagedNode* dnode = new DamagedNode(graphics, sx + (i%4)*ix, sy + (i/4)*iy);
	    damaged_nodes.push_back(dnode);
	}
    }
    

    std::vector<InputNode*> input_nodes;
    std::vector<program_data::InputData> input_data = program_data::getInputData();
    for(int i = 0; i < input_data.size(); ++i) {
	std::vector<std::string> values;
	int index = input_data[i].index;
	for(int j = 0; j < input_data[i].values.size(); ++j) {
	    values.push_back(std::to_string(input_data[i].values[j]));
	}
	const int x_offset = 355;
	const int y_offset = 22;
	const int x_incr = 257;
	
	const int desc_x_incr = 49;
	const int desc_x_offset = 26;
	const int desc_y_offset = 173;
	InputNode* node = new InputNode(graphics, values, input_data[i].name, index,
					x_offset+x_incr*index, y_offset,
					desc_x_offset+desc_x_incr*i, desc_y_offset);
	input_nodes.push_back(node);
    }
    std::vector<OutputNode*> output_nodes;
    std::vector<program_data::OutputData> output_data = program_data::getOutputData();
    for(int i = 0; i < output_data.size(); ++i) {
	std::vector<std::string> values;
	int index = output_data[i].index;
	for(int j = 0; j < output_data[i].values.size(); ++j) {
	    values.push_back(std::to_string(output_data[i].values[j]));
	}
	const int x_offset = 355;
	const int y_offset = 731;
	const int x_incr = 257;
	
	const int desc_input_x_incr = 49;
	const int desc_x_incr = 89;
	const int desc_x_offset = 26+desc_input_x_incr*input_nodes.size();
	const int desc_y_offset = 173;
	OutputNode* node = new OutputNode(graphics, values, output_data[i].name, index,
					  x_offset+x_incr*index, y_offset,
					  desc_x_offset+desc_x_incr*i, desc_y_offset);
	output_nodes.push_back(node);
    }

    std::vector<Node*> layout_nodes;
    int cindex = 0;
    int dindex = 0;
    for(int i = 0; i < layout.size(); ++i) {
	if(layout[i] == program_data::TILE_COMPUTE) {
	    layout_nodes.push_back(nodes[cindex++]);
	} else if(layout[i] == program_data::TILE_DAMAGED) {
	    layout_nodes.push_back(damaged_nodes[dindex++]);
	}
    }
    
    std::vector<Port*> ports;
    DummyPort* dport = new DummyPort();
    for(int i = 0; i < layout.size(); ++i) {
	if(layout[i] == program_data::TILE_COMPUTE) {	    
	    const int sx = 355;
	    const int sy = 53;
	    const int ix = 257;
	    const int iy = 235;
	
	    MessageHandler* l = dport;
	    MessageHandler* r = dport;
	    MessageHandler* u = dport;
	    MessageHandler* d = dport;

	    if((i%4) > 0) {
		const int x_offset = -33;
		const int y_offset = 111;
		Port* port = new Port(graphics, LEFT, layout_nodes[i], layout_nodes[i-1],
				      sx + (i%4)*ix+x_offset, sy + (i/4)*iy+y_offset);
		l = port;
		ports.push_back(port);
	    }
	    if((i%4) < 3) {
		const int x_offset = 225;
		const int y_offset = 76;
		Port* port = new Port(graphics, RIGHT, layout_nodes[i], layout_nodes[i+1],
				      sx + (i%4)*ix+x_offset, sy + (i/4)*iy+y_offset);
		r = port;
		ports.push_back(port);
	    }
	    if((i/4) > 0) {
		const int x_offset = 82;
		const int y_offset = -32;
		Port* port = new Port(graphics, UP, layout_nodes[i], layout_nodes[i-4],
				      sx + (i%4)*ix+x_offset, sy + (i/4)*iy+y_offset); 
		u = port;
		ports.push_back(port);
	    }
	    if((i/4) < 2) {
		const int x_offset = 122;
		const int y_offset = 204;
		Port* port = new Port(graphics, DOWN, layout_nodes[i], layout_nodes[i+4],
				      sx + (i%4)*ix+x_offset, sy + (i/4)*iy+y_offset);
		d = port;
		ports.push_back(port);
	    }
	    for(int j = 0; j < input_nodes.size(); ++j) {
		if(i == input_nodes[j]->index()) {
		    const int x_offset = 355+122;
		    const int y_offset = 22;
		    const int x_incr = 257;
		    Port* port = new Port(graphics, DOWN, input_nodes[j], layout_nodes[i],
					  x_offset+x_incr*input_nodes[j]->index(), y_offset);
		    u = port;
		    ports.push_back(port);
		    input_nodes[j]->setHandler(port);
		}
	    }
	    for(int j = 0; j < output_nodes.size(); ++j) {
		if(i == output_nodes[j]->index()+8) {
		    const int x_offset = 355+122;
		    const int y_offset = 731;
		    const int x_incr = 257;
		    Port* port = new Port(graphics, DOWN, layout_nodes[i], output_nodes[j],
					  x_offset+x_incr*output_nodes[j]->index(), y_offset);
		    d = port;
		    ports.push_back(port);
		    output_nodes[j]->setHandler(port);
		}
	    }

	    ComputeNode* t = dynamic_cast<ComputeNode*>(layout_nodes[i]);
	    t->setHandlers(l, r, u, d);
	}
    }
    int cnode = -1;
    
    int last_update_time = SDL_GetTicks();
    int frame_time_accumulator = 0;
    bool running = true;
    while (running) {
	const int start_frame_time = SDL_GetTicks();
	const int frame_time = start_frame_time - last_update_time;
	last_update_time = start_frame_time;
	frame_time_accumulator += frame_time;

	while (frame_time_accumulator >= kDeltaTime) {
	    input.beginNewFrame();
	    while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
		    running = false;
		    break;
		case SDL_KEYDOWN:
		    input.keyDownEvent(event);
		    break;
		case SDL_KEYUP:
		    input.keyUpEvent(event);
		    break;
		case SDL_MOUSEBUTTONDOWN:
		    input.mouseDownEvent(event);
		    break;
		case SDL_MOUSEBUTTONUP:
		    input.mouseUpEvent(event);
		    break;
		case SDL_TEXTINPUT:
		    if(cnode != -1 && state_ == EDIT)
			nodes[cnode]->itb.typeSym(std::toupper(event.text.text[0]));
		    break;
		default:
		    break;
		}
	    }

	    if(state_ == EDIT) {
		if(input.wasMouseButtonPressed(SDL_BUTTON_LEFT)) {
		    int x, y;
		    SDL_GetGlobalMouseState(&x, &y);
		    int xt, yt;
		    graphics.getWindowPosition(xt, yt);
		    x -= xt;
		    y -= yt;

		    cnode = -1;
		    for(int i = 0; i < nodes.size(); ++i) {
			if(nodes[i]->itb.raise(x, y)) {
			    cnode = i;
			}
		    }
		}

		if(cnode != -1) {
		    if(input.wasKeyPressed(SDLK_LEFT)) {
			nodes[cnode]->itb.moveCursorLeft();
		    }
		    if(input.wasKeyPressed(SDLK_RIGHT)) {
			nodes[cnode]->itb.moveCursorRight();
		    }
		    if(input.wasKeyPressed(SDLK_UP)) {
			nodes[cnode]->itb.moveCursorUp();
		    }
		    if(input.wasKeyPressed(SDLK_DOWN)) {
			nodes[cnode]->itb.moveCursorDown();
		    }
		    if(input.wasKeyPressed(SDLK_HOME)) {
			nodes[cnode]->itb.moveCursorLineStart();
		    }
		    if(input.wasKeyPressed(SDLK_END)) {
			nodes[cnode]->itb.moveCursorLineEnd();
		    }
		    if(input.wasKeyPressed(SDLK_BACKSPACE)) {
			nodes[cnode]->itb.deleteSym();
		    }
		    if(input.wasKeyPressed(SDLK_PAGEUP)) {
			nodes[cnode]->itb.moveCursorBoxStart();
		    }
		    if(input.wasKeyPressed(SDLK_PAGEDOWN)) {
			nodes[cnode]->itb.moveCursorBoxEnd();
		    }
		    if(input.wasKeyPressed(SDLK_RETURN)) {
			nodes[cnode]->itb.newline();
		    }
		}
	    }

	    if(input.wasKeyPressed(SDLK_F6)) {
		if(state_ == EDIT) {
		    for(int i = 0; i < nodes.size(); ++i) {
			nodes[i]->startExecution();
		    }
		    cnode = -1;
		    state_ = RUN;
		} else {
		    for(int i = 0; i < input_nodes.size(); ++i) {
			input_nodes[i]->tickUpdate();
		    }
		    for(int i = 0; i < nodes.size(); ++i) {
			nodes[i]->tickUpdate();
		    }
		    for(int i = 0; i < output_nodes.size(); ++i) {
			output_nodes[i]->tickUpdate();
		    }
		    Port::processMessages();
		    for(int i = 0; i < nodes.size(); ++i) {
			nodes[i]->nextLine();
		    }
		}
	    }
	    if(input.wasKeyPressed(SDLK_F7)) {
		if(state_ == RUN) {
		    for(int i = 0; i < nodes.size(); ++i) {
			nodes[i]->endExecution();
		    }
		    for(int i = 0; i < output_nodes.size(); ++i) {
			output_nodes[i]->endExecution();
		    }
		    for(int i = 0; i < input_nodes.size(); ++i) {
			input_nodes[i]->endExecution();
		    }
		    state_ = EDIT;
		}
	    }
	    
	    frame_time_accumulator -= kDeltaTime;
	    
	    // Update
	    if(cnode != -1 && state_ == EDIT)
		nodes[cnode]->itb.update(kDeltaTime*kGameSpeed);
	}

	// Draw
	graphics.clear();

	for(int i = 0; i < input_nodes.size(); ++i) {
	    input_nodes[i]->draw();
	}
	for(int i = 0; i < output_nodes.size(); ++i) {
	    output_nodes[i]->draw();
	}
	for(int i = 0; i < nodes.size(); ++i) {
	    nodes[i]->draw();
	}
	for(int i = 0; i < damaged_nodes.size(); ++i) {
	    damaged_nodes[i]->draw();
	}	
	for(int i = 0; i < ports.size(); ++i) {
	    ports[i]->draw();
	}

	const int desc_x = 26;
	const int desc_y = 43;
	description_sprite.draw(desc_x, desc_y);
	const int name_y_offset = 20;
	const int desc_width = 299;
	program_name.drawCenteredX(desc_x, desc_y-name_y_offset, desc_width);
	const int dst = 12;
	const int desc_height = 93;
	const int box_x_offset = 6;
	description_box.drawCenteredY(desc_x+box_x_offset, desc_y, dst, desc_height);
	
	graphics.flip();
    }

    delete dport;
    for(int i = 0; i < output_nodes.size(); ++i) {
	delete output_nodes[i];
    }
    for(int i = 0; i < input_nodes.size(); ++i) {
	delete input_nodes[i];
    }
    for(int i = 0; i < ports.size(); ++i) {
	delete ports[i];
    }
    for(int i = 0; i < damaged_nodes.size(); ++i) {
	delete damaged_nodes[i];
    }
    for(int i = 0; i < nodes.size(); ++i) {
	delete nodes[i];
    }
}
