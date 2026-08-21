#pragma once

#include <array>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <memory>
#include <functional>
#include <unordered_map>


struct Command {
	int req_args;
	std::function<std::string(const std::vector<std::string>&)> handler;
};

class dataVal {
public:
    virtual ~dataVal() = default;

	virtual std::string print() const = 0;
	virtual std::string get_type() const = 0;

    virtual void send_to(std::ostream& os) const = 0;
};

class strVal : public dataVal {
    std::string _value;
public:
	strVal(const std::string& val) : _value(val) {}

	std::string print() const override { return _value; }
	std::string get_type() const override { return "string"; }

    void send_to(std::ostream& os) const override { os << _value; }
};

class intVal : public dataVal {
    int _value;
public:
    intVal(const int& val) : _value(val) {}

    void incr() { _value++; }

    std::string print() const override { return std::to_string(_value); }
    std::string get_type() const override { return "int"; }

    void send_to(std::ostream& os) const override { os << _value; }
};

class listVal : public dataVal {
    std::vector<std::string> _value;
public:
    listVal() = default;

    void lpush(const std::string& val) { _value.insert(_value.begin(), val); }

    void rpush(const std::string& val) { _value.push_back(val); }

    std::string print() const override {
        std::string res = "["; //cout << "[";
        for (size_t i = 0; i < _value.size(); ++i) {
            res += _value[i] + (i < _value.size() - 1 ? ", " : "");
        }
        res += "]"; //cout << "]";
        return res;
    }

    std::string get_type() const override { return "list"; }

    void send_to(std::ostream& os) const override {
        for (size_t i = 0; i < _value.size(); ++i) {
            os << _value[i] << (i < _value.size() - 1 ? "," : "");
        }
    }
};

class Directory {
private:
    std::unordered_map<std::string, std::shared_ptr<dataVal>> database;
    std::unordered_map<std::string, Command> commandHandlers;

    // handler 
    std::string handleSet(const std::vector<std::string>& args);
    std::string handleGet(const std::vector<std::string>& args);
    std::string handleDel(const std::vector<std::string>& args);
    std::string handleCopy(const std::vector<std::string>& args);

    std::string handleLPush(const std::vector<std::string>& args);
    std::string handleLRange(const std::vector<std::string>& args);

    std::string handleIncr(const std::vector<std::string>& args);

    std::string handleSave(const std::vector<std::string>& args);
    std::string handleLoad(const std::vector<std::string>& args);
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
