#include <array>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <functional>
#include <unordered_map>

#include "directory.hpp"

using namespace std;

// handlers
string Directory::handleSet(const vector<string>& args) {
	database[args[1]] = args[2];
	return "OK";
}

string Directory::handleGet(const vector<string>& args) {
	if (database.find(args[1]) != database.end()) {
		return database[args[1]];
	}
	return "Key not found";
}

string Directory::handleDel(const vector<string>& args) {
	if (database.erase(args[1])) {
		return "OK";
	}
	return "Key not found";
}

string Directory::handleExit(const vector<string>& args) {
	exit(0);
}

// constructor
Directory::Directory() {
	commandHandlers["set"] = { 3, [this](const vector<string>& args) { return handleSet(args); } };
	commandHandlers["get"] = { 2, [this](const vector<string>& args) { return handleGet(args); } };
	commandHandlers["del"] = { 2, [this](const vector<string>& args) { return handleDel(args); } };
	commandHandlers["exit"] = { 1, [this](const vector<string>& args) { return handleExit(args); } };
}


// execute the command
string Directory::execute(const vector<string>& args) {
	if (args.empty()) return "";

	string command = args[0];

	if (commandHandlers.find(command) != commandHandlers.end()) {
		Command cmd = commandHandlers[command];

		if (args.size() != cmd.req_args) {
			return "Error: wrong number of arguments for '" + command + "' command";
		}
		return cmd.handler(args);

	}
	return "Unknown command";
}

// print
void Directory::debugPrint() {
	for (const auto& [k, v] : database) {
		cout << k << ": " << v << "\n";
	}
}

// Parser function
vector<string> Parser::parse(const string& input) {
	istringstream ss(input);
	vector<string> args;
	string w;

	while (ss >> w) {
		args.push_back(w);
	}
	return args;
}
