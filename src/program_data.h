#pragma once

#include <string>
#include <vector>

namespace program_data {
    std::string programName() {
	return "--- DOUBLE ---";
    }
    std::vector<std::string> programDescription() {
	return { "> READ VALUES FROM IN.A",
	    "> DOUBLE VALUE",
	    "> WRITE VALUE TO OUT.A"};
    }

    std::vector<int> input1Values() {
	std::vector<int> result;
	for(int i = 0; i < 39; ++i) {
	    result.push_back(i);
	}
	return result;
    }

    std::vector<int> output1Values() {
	std::vector<int> result;
	std::vector<int> input = input1Values();
	for(int i = 0; i < input.size(); ++i) {
	    result.push_back(input[i] * 2);
	}
	return result;
    }

    struct InputData {
	std::string name;
	std::vector<int> values;
	int index;
    };
    std::vector<InputData> getInputData() {
	return { { "IN.A", input1Values(), 0 } };
    }

    struct OutputData {
	std::string name;
	std::vector<int> values;
	int index;
    };
    std::vector<OutputData> getOutputData() {
	return { { "OUT.A", output1Values(), 0 } };
    }

    enum NodeType {
	TILE_COMPUTE,
	TILE_DAMAGED
	// TODO: add TILE_MEMORY
    };
    std::vector<NodeType> getLayout() {
	return { TILE_COMPUTE, TILE_COMPUTE, TILE_DAMAGED, TILE_COMPUTE,
	    TILE_DAMAGED, TILE_COMPUTE, TILE_COMPUTE, TILE_COMPUTE,
	    TILE_COMPUTE, TILE_COMPUTE, TILE_COMPUTE, TILE_COMPUTE };
    }
}
