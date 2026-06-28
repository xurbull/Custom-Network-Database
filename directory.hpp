#pragma once

#include <array>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <functional>
#include <unordered_map>

struct Command {
	int req_args;
	std::function<std::string(const std::vector<std::string>&)> handler;
};

class Directory {
private:
    std::unordered_map<std::string, std::string> database;
    std::unordered_map<std::string, Command> commandHandlers;

    // handler 
    std::string handleSet(const std::vector<std::string>& args);
    std::string handleGet(const std::vector<std::string>& args);
    std::string handleDel(const std::vector<std::string>& args);
    std::string handleExit(const std::vector<std::string>& args);

public:
    Directory();
    std::string execute(const std::vector<std::string>& args);
    void debugPrint();
};

class Parser {
public:
	std::vector<std::string> parse(const std::string& input);
};
