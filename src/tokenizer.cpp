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
    }

    stringstream tokenize()
    {
        bool is_multi_line_comment = false;
        stringstream tokens;
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
                tokens << value << '\n';
            }
        }
        tokens << flush;
        return tokens;
    }

private:
    ifstream input_file;
};

#endif // TOKENIZER_CPP