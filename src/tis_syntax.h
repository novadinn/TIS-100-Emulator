#pragma once

#include <string>
#include <iostream>

namespace tis_syntax {
    
    inline std::string simplifyLine(std::string t) {
	int l = t.length();
	int c = std::count(t.begin(), t.end(), ' ');
	std::remove(t.begin(), t.end(), ' ');
	t.resize(l - c);
	return t;
    }
    
    inline std::string operation(std::string line) {
	return line.substr(0, 3);
    }

    inline std::string firstOperand(std::string line) {
	int i = line.find(",");
	std::string rest;
	if(i > 0) {
	    int l = line.substr(0, i).length();
	    rest = line.substr(3, l-3);
	} else {
	    rest = line.substr(3);
	}
	return rest;
    }

    inline std::string secondOperand(std::string line) {
	int i = line.find(",");
	std::string rest = line.substr(i+1);
	return rest;
    }

    inline bool portOperand(std::string operand) {
	if(operand == "LEFT" || operand == "RIGHT" || operand == "UP" || operand == "DOWN")
	    return true;
	return false;
    }

    inline bool numberOperand(std::string operand) {
	for (int i = 0; i < operand.length(); ++i) {
	    if (std::isdigit(operand[i]) == 0 && operand[0] != '-')
		return false;
	}
	return true;
    }

    inline bool accOperand(std::string operand) {
	return operand == "ACC";
    }
    
    inline bool nilOperand(std::string operand) {
	return operand == "NIL";
    }

    inline std::string removeLable(std::string t) {
	int i = t.find(":");
	return t.substr(i+1);
    }

    inline bool label(std::string line) {
	return line.size() > 1 && line[line.size()-1] == ':';
    }
    
    inline std::string nameToLabel(std::string label) {
	return label + ":";
    }

    inline bool skippableLine(std::string line) {
	line = simplifyLine(line);
	return line == "" || label(line);
    }
}
