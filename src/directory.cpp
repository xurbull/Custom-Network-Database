#include <algorithm>
#include <fstream>
#include <cctype>

#include "directory.hpp"

#ifdef _WIN32
#define strcasecmp _stricmp
#endif

using namespace std;

// check
bool is_number(const string& s) {
	if (s.empty()) 
		return false;
	for (char c : s) {
		if (!isdigit(c) && c != '-') 
			return false;
	}
	return true;
}

// handlers
// 
// set logic
string Directory::handleSet(const vector<string>& args) {
	database[args.at(1)] = make_shared<strVal>(args.at(2));
	return "OK";
}

// get logic
string Directory::handleGet(const vector<string>& args) {
	if (database.find(args.at(1)) != database.end()) {
		return database[args.at(1)]->print();
	}
	return "Key not found";
}

// delete logic
string Directory::handleDel(const vector<string>& args) {
	if (database.erase(args.at(1))) {
		return "OK";
	}
	return "Key not found";
}

// copy logic by reference
string Directory::handleCopy(const vector<string>& args) {
	if (database.find(args.at(1)) != database.end()) {
		database[args.at(2)] = database[args.at(1)];
		return "OK";
	}
	return "Key not found";
}

// list push logic
string Directory::handleLPush(const vector<string>& args) {
	string key = args.at(1);
	string val = args.at(2);

	auto it = database.find(key);

	if (it == database.end()) {
		auto new_list = make_shared<listVal>();
		new_list->lpush(val);
		database[key] = new_list;
		return "OK";
	}

	if (auto list_ptr = dynamic_cast<listVal*>(it->second.get())) {
		list_ptr->lpush(val);
		return "OK";
	}

	return "WRONGTYPE Operation against a key holding the wrong kind of value";
}

// list range logic
string Directory::handleLRange(const vector<string>& args) {
	auto it = database.find(args.at(1));
	if (it == database.end()) return "(empty list)";

	if (auto list_ptr = dynamic_cast<listVal*>(it->second.get())) {
		return list_ptr->print();
	}
	return "WRONGTYPE Operation against a key holding the wrong kind of value";
}

// increment logic, the int goes as string 
string Directory::handleIncr(const vector<string>& args) {
	
	string key = args.at(1);
	auto it = database.find(key);
	if (it == database.end()) 
		return "Key not found";

	auto& db_val = it->second;

	// if val is int
	if (auto int_ptr = dynamic_cast<intVal*>(db_val.get())) {
		int_ptr->incr();
		return int_ptr->print();
	}

	// if val is string
	if (auto str_ptr = dynamic_cast<strVal*>(db_val.get())) {
		string org_str = str_ptr->print();

		if (is_number(org_str)) {
			int new_val = stoi(org_str) + 1;

			database[key] = make_shared<intVal>(new_val);
			return to_string(new_val);
		}
	}
	return "WRONGTYPE Operation against a key holding the wrong kind of value";
}

string Directory::handleClear(const std::vector<std::string>& args) {
	database.clear();
	return "OK";
}

// exit logic
string Directory::handleExit(const vector<string>& args) {
	exit(0);
	return "Exit";
}

// persistence handlers
// 
// save logic
string Directory::handleSave(const vector<string>& args) {
	ofstream os("database.txt");
	if (!os) return "Error: Could not open file";

	for (const auto& [key, db_val] : database) {
		os << key << ";" << db_val->get_type() << ";";
		db_val->send_to(os);
		os << "\n";
	}
	return "Database Saved to Disk!";
}

// load logic
string Directory::handleLoad(const vector<string>& args) {
	ifstream is("database.txt");
	if (!is) return "Error: Could not open file";

	database.clear(); 
	string line;

	while (getline(is, line)) {
		if (line.empty()) continue;
		istringstream ss(line);
		string key, type, value;

		getline(ss, key, ';');
		getline(ss, type, ';');
		getline(ss, value);

		if (type == "string") {
			database[key] = make_shared<strVal>(value);
		}
		else if (type == "int") {
			database[key] = make_shared<intVal>(stoi(value));
		}
		else if (type == "list") {
			auto new_list = make_shared<listVal>();
			istringstream list_stream(value);
			string item;
			while (getline(list_stream, item, ',')) {
				new_list->rpush(item);
			}
			database[key] = new_list;
		}
	}
	return "Database Loaded from Disk!";
}

// constructor
Directory::Directory() {
	commandHandlers["set"] = { 3, [this](const vector<string>& args) { return handleSet(args); } };
	commandHandlers["get"] = { 2, [this](const vector<string>& args) { return handleGet(args); } };
	commandHandlers["del"] = { 2, [this](const vector<string>& args) { return handleDel(args); } };
	commandHandlers["incr"] = { 2, [this](const vector<string>& args) { return handleIncr(args); } };
	commandHandlers["copy"] = { 3, [this](const vector<string>& args) { return handleCopy(args); } };
	commandHandlers["lpush"] = { 3, [this](const vector<string>& args) { return handleLPush(args); } };
	commandHandlers["lrange"] = { 2, [this](const vector<string>& args) { return handleLRange(args); } };
	commandHandlers["exit"] = { 1, [this](const vector<string>& args) { return handleExit(args); } };
	commandHandlers["save"] = { 1, [this](const vector<string>& args) { return handleSave(args); } };
	commandHandlers["load"] = { 1, [this](const vector<string>& args) { return handleLoad(args); } };
	commandHandlers["clear"] = { 1, [this](const vector<string>& args) { return handleClear(args); } };
}


// execute the command
string Directory::execute(const vector<string>& args) {
	if (args.empty()) return "";

	string command = args[0];

	transform(command.begin(), command.end(), command.begin(),
		[](unsigned char c) { return std::tolower(c); });

	if (commandHandlers.find(command) != commandHandlers.end()) {
		Command cmd = commandHandlers[command];

		if (args.size() != cmd.req_args) {
			return "Error: wrong number of arguments for '" + command + "' command";
		}
		
		try {
			return cmd.handler(args);
		}
		catch (const std::out_of_range& e) {
			return "Internal Server Error: Memory Out of Bounds";
		}
		catch (const std::exception& e) {
			return string("Internal Server Error: ") + e.what();
		}

	}
	return "Unknown command";
}

// print
void Directory::debugPrint() {
	for (const auto& [k, v] : database) {
		cout << k << ": " << v->print() << "\n";
	}
}

// parsing the input
vector<string> Parser::parse(const string& input) {
	istringstream ss(input);
	vector<string> args;
	string w;

	while (ss >> w) {
		args.push_back(w);
	}
	return args;
}
