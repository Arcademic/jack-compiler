#ifndef TOKENIZER_CPP
#define TOKENIZER_CPP

#include "constants.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <list>
#include <unordered_map>
#include <algorithm>

using namespace std;


class Tokenizer
{
public:
    Tokenizer(string path)
    {
        input_file.open(path);
        pos = 0;
        tokenize();
    }

    void tokenize()
    {
        bool is_multi_line_comment = false;
        string line;
        while (getline(input_file, line))
        {
            // remove one line comments
            auto comm_start_pos = line.find(SINGLE_LINE_COMMENT_STR);
            if (comm_start_pos != NPOS)
            {
                line = line.substr(0, comm_start_pos);
            }

            // remove multi line comments
            comm_start_pos = line.find(MULTI_LINE_COMMENT_START_STR);
            auto comm_end_pos = line.find(MULTI_LINE_COMMENT_END_STR);
            if (comm_start_pos != NPOS && comm_end_pos != NPOS)
            {
                line.erase(comm_start_pos, comm_end_pos - comm_start_pos + MULTI_LINE_COMMENT_END_STR.length());
            }
            else if (comm_start_pos != NPOS && comm_end_pos == NPOS)
            {
                line.erase(comm_start_pos);
                is_multi_line_comment = true;
                continue;
            }
            else if (comm_start_pos == NPOS && comm_end_pos != NPOS)
            {
                line.erase(0, comm_end_pos + MULTI_LINE_COMMENT_END_STR.length());
                is_multi_line_comment = false;
            }
            if (is_multi_line_comment)
                continue;

            // insert spaces around symbols, so they can be tokenized
            for (char symbol : SYMBOLS)
            {
                size_t pos = 0;
                while ((pos = line.find(symbol, pos)) != NPOS)
                {
                    line.insert(pos, " ");
                    line.insert(pos + 2, " ");
                    pos += 3;
                }
            }

            string value;
            stringstream line_stream(line);
            while (line_stream >> value)
            {
                tokens.push_back(value);
            }
        }
    }

    string advance() {
        if (pos < tokens.size()) {
            return tokens[pos++];
        } else {
            throw runtime_error("No more tokens available.");
        }
    }

    string look_ahead(int i) {
        if (pos+i < tokens.size()) {
            return tokens[pos+i];
        } else {
            throw runtime_error("Out of range.");
        }
    }

    string peek() {
        return look_ahead(0);
    }

    void save_state() {
        save_state_pos = pos;
    }

    void restore_state() {
        pos = save_state_pos;
    }

private:
    ifstream input_file;
    vector<string> tokens;
    long unsigned int pos;
    long unsigned int save_state_pos;

};

#endif // TOKENIZER_CPP