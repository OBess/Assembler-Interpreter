#pragma once
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>
#include <stack>
#include <regex>

namespace Interpreter {
    //prototypes
    void execute(std::unordered_map<std::string, int>& variables, std::vector<std::string>& line, size_t& i, int flags[4], const std::vector<std::pair<std::string, size_t>>& labels, std::stack<int>& stack);
    void assembler(const std::vector<std::string>& vprogram, const std::vector<std::pair<std::string, size_t>>& labels);
    void assembler_interpreter(std::string& program);
    std::vector<std::string> split(const std::string& str, const char& delimiter);
    std::vector<std::string> split(const std::string& str);

    //variables
    static std::regex regex_label("(\\w+:)"), regex_comment("(;\\s*\\w.*)");
    static std::string line = "", token_nl = "\n";
    static std::vector<std::string> vprogram;
    static std::vector<std::pair<std::string, size_t>> labels;
    static std::vector<std::string> vars;
    static std::stringstream ss;

    static size_t end_program = 100000;
    static std::string msg = "", tmsg = "";
    static bool q = false;

    //functions
    void assembler_interpreter(std::string& program)
    {
        program = std::regex_replace(program, regex_comment, "");
        ss = std::stringstream(program);
        size_t i = 0;
        while (std::getline(ss, line))
        {
            if (line.length() == 0) continue;
            vprogram.push_back(line);
            std::sregex_iterator iter(line.begin(), line.end(), regex_label);
            std::sregex_iterator end;
            if (iter != end && ((*iter)[0]).str().length() == line.length()) labels.push_back({ ((*iter)[0]).str().substr(0, ((*iter)[0]).str().length() - 1), i });
            ++i;
        }
        assembler(vprogram, labels);
        vprogram.clear();
        labels.clear();
    }
    void assembler(const std::vector<std::string>& vprogram, const std::vector<std::pair<std::string, size_t>>& labels)
    {
        std::unordered_map<std::string, int> variables;
        std::vector<std::string> line;
        std::stack<int> labels_ret;
        int flags[4] = { 0,0,0,0 }; //0 = equ; 1 = greater; 2 = less; 3 = nequ
        size_t i = 0;
        for (i = 0; i < vprogram.size(); ++i)
        {
            line = split(vprogram[i]);
            try
            {
                execute(variables, line, i, flags, labels, labels_ret);
            }
            catch (const std::string& err)
            {
                std::cout << err << '\n';
                break;
            }
        }
    }
    void execute(std::unordered_map<std::string, int>& variables, std::vector<std::string>& line, size_t& i, int flags[4], const std::vector<std::pair<std::string, size_t>>& labels, std::stack<int>& labels_ret)
    {
        if (line.size() > 3 && line[0] != "msg")
            throw std::string("Invalid line: " + std::to_string(i + 1));
        if (line.size() == 2)
        {
            std::vector<std::string> v_tmp = split(line[1], ',');
            line.erase(line.begin() + 1);
            for (auto v : v_tmp)
                line.push_back(v);
        }
        else if (line.size() == 3)
        {
            std::vector<std::string> v_tmp = split(line[1], ',');
            line.erase(line.begin() + 1);
            for (auto v : v_tmp)
                line.insert(line.begin() + 1, v);
        }
        if (line[0] == "mov")
        {
            if (line[2] > "9")
            {
                std::unordered_map<std::string, int>::iterator a = variables.find(line[1]);
                if (a == variables.end())
                {
                    variables.insert({ line[1], 0 });
                    a = variables.find(line[1]);
                }
                std::unordered_map<std::string, int>::const_iterator b = variables.find(line[2]);
                a->second = b->second;
            }
            else if (line[1] > "9" && line[2] > "9")
            {
                std::unordered_map<std::string, int>::iterator a = variables.find(line[1]);
                std::unordered_map<std::string, int>::const_iterator b = variables.find(line[2]);
                a->second = b->second;
            }
            else
            {
                std::unordered_map<std::string, int>::iterator a = variables.find(line[1]);
                if (a == variables.end())
                    variables.insert({ line[1], std::stoi(line[2]) });
                else
                    a->second = std::stoi(line[2]);
            }
        }
        else if (line[0] == "inc")
        {
            std::unordered_map<std::string, int>::iterator a = variables.find(line[1]);
            ++a->second;
        }
        else if (line[0] == "dec")
        {
            std::unordered_map<std::string, int>::iterator a = variables.find(line[1]);
            --a->second;
        }
        else if (line[0] == "add")
        {
            std::unordered_map<std::string, int>::iterator a = variables.find(line[1]);
            std::unordered_map<std::string, int>::const_iterator b = variables.find(line[2]);
            a->second += (b == variables.end()) ? std::stoi(line[2]) : b->second;
        }
        else if (line[0] == "sub")
        {
            std::unordered_map<std::string, int>::iterator a = variables.find(line[1]);
            std::unordered_map<std::string, int>::const_iterator b = variables.find(line[2]);
            a->second -= (b == variables.end()) ? std::stoi(line[2]) : b->second;
        }
        else if (line[0] == "mul")
        {
            std::unordered_map<std::string, int>::iterator a = variables.find(line[1]);
            std::unordered_map<std::string, int>::const_iterator b = variables.find(line[2]);
            a->second *= (b == variables.end()) ? std::stoi(line[2]) : b->second;
        }
        else if (line[0] == "div")
        {
            std::unordered_map<std::string, int>::iterator a = variables.find(line[1]);
            std::unordered_map<std::string, int>::const_iterator b = variables.find(line[2]);
            a->second /= (b == variables.end()) ? std::stoi(line[2]) : b->second;
        }
        else if (line[0] == "jmp")
        {
            auto a = std::find_if(labels.begin(), labels.end(), [line](std::pair<std::string, int> p) {return p.first == line[1]; });
            if (a != labels.end())
                i = a->second;
            else
                throw std::string("Invalid line: " + std::to_string(i + 1));
        }
        else if (line[0] == "cmp")
        {
            for (int i = 0; i < 4; ++i) flags[i] = 0;
            int an, bn;
            std::unordered_map<std::string, int>::iterator a = variables.find(line[1]);
            an = (a == variables.end()) ? std::stoi(line[1]) : a->second;
            std::unordered_map<std::string, int>::const_iterator b = variables.find(line[2]);
            bn = (b == variables.end()) ? std::stoi(line[2]) : b->second;
            if (an == bn)
                flags[0] = 1;
            else
                flags[3] = 1;
            if (an > bn)
                flags[1] = 1;
            if (an < bn)
                flags[2] = 1;
        }
        else if (line[0] == "jne")
        {
            if (flags[3])
            {
                auto a = std::find_if(labels.begin(), labels.end(), [line](std::pair<std::string, int> p) {return p.first == line[1]; });
                if (a != labels.end())
                    i = a->second;
                else
                    throw std::string("Invalid line: " + std::to_string(i + 1));
            }
        }
        else if (line[0] == "je")
        {
            if (flags[0])
            {
                auto a = std::find_if(labels.begin(), labels.end(), [line](std::pair<std::string, int> p) {return p.first == line[1]; });
                if (a != labels.end())
                    i = a->second;
                else
                    throw std::string("Invalid line: " + std::to_string(i + 1));
            }
        }
        else if (line[0] == "jge")
        {
            if (flags[0] || flags[1])
            {
                auto a = std::find_if(labels.begin(), labels.end(), [line](std::pair<std::string, int> p) {return p.first == line[1]; });
                if (a != labels.end())
                    i = a->second;
                else
                    throw std::string("Invalid line: " + std::to_string(i + 1));
            }
        }
        else if (line[0] == "jg")
        {
            if (flags[1])
            {
                auto a = std::find_if(labels.begin(), labels.end(), [line](std::pair<std::string, int> p) {return p.first == line[1]; });
                if (a != labels.end())
                    i = a->second;
                else
                    throw std::string("Invalid line: " + std::to_string(i + 1));
            }
        }
        else if (line[0] == "jle")
        {
            if (flags[0] || flags[2])
            {
                auto a = std::find_if(labels.begin(), labels.end(), [line](std::pair<std::string, int> p) {return p.first == line[1]; });
                if (a != labels.end())
                    i = a->second;
                else
                    throw std::string("Invalid line: " + std::to_string(i + 1));
            }
        }
        else if (line[0] == "jl")
        {
            if (flags[2])
            {
                auto a = std::find_if(labels.begin(), labels.end(), [line](std::pair<std::string, int> p) {return p.first == line[1]; });
                if (a != labels.end())
                    i = a->second;
                else
                    throw std::string("Invalid line: " + std::to_string(i + 1));
            }
        }
        else if (line[0] == "call")
        {
            labels_ret.push(i);
            auto a = std::find_if(labels.begin(), labels.end(), [line](std::pair<std::string, int> p) {return p.first == line[1]; });
            if (a != labels.end())
                i = a->second;
            else
                throw std::string("Invalid line: " + std::to_string(i + 1));
        }
        else if (line[0] == "ret")
        {
            i = labels_ret.top();
            labels_ret.pop();
        }
        else if (line[0] == "msg")
        {
            msg = "";
            tmsg = "";
            q = false;
            size_t i = 0;
            for (i = 1; i < line.size(); ++i)
                tmsg += line[i] + ' ';
            tmsg = tmsg.substr(0, tmsg.length() - 1);
            for (i = 0; i < tmsg.size(); ++i)
            {
                if (tmsg[i] == '\'' || tmsg[i] == '\"') { q = !q; continue; }
                if (q)
                    msg += tmsg[i];
                else
                    msg += (variables.find(std::string(1, tmsg[i])) != variables.end()) ? std::to_string(variables.find(std::string(1, tmsg[i]))->second) : "";
            }
            i = msg.find("\\n");
            while (i != std::string::npos)
            {
                std::cout << msg.substr(0, i) << '\n';
                msg.erase(0, i + 2);
                i = msg.find("\\n");
            }
            std::cout << msg;
        }
        else if (line[0] == "end")
            i = end_program - 1;
    }

    //useful functions
    std::vector<std::string> split(const std::string& str, const char& delimiter)
    {
        std::vector<std::string> v;
        std::string line = "";
        for (const char& c : str)
        {
            if (c == delimiter)
            {
                if (line != "")
                    v.push_back(line);
                line.clear();
                continue;
            }
            line += c;
        }
        if (line != "")
            v.push_back(line);
        return v;
    }
    std::vector<std::string> split(const std::string& str)
    {
        std::vector<std::string> v;
        std::stringstream ss(str);
        std::string temp;
        while (ss >> temp)
            v.push_back(temp);
        return v;
    }
};


