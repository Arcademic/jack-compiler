#ifndef PARSER_CPP
#define PARSER_CPP

#include "constants.h"
#include "tokenizer.cpp"
#include <sstream>
#include <iostream>
#include <cassert>

using namespace std;


class Parser
{
public:
    Parser(Tokenizer& t) {
        parse();
    }

    string parse() {
        indent = "";
        output << "<class>\n";
        parse_class();
        output << "</class>\n";
        output << flush;

        return output.str();
    }

private:
    stringstream tokens;
    string token;
    string indent;

    stringstream output;

    void advance() {
        tokens >> token;
    }

    void parse_class() {
        inc_indent();

        advance();
        assert(token == "class");
        output << indent << "<keyword> " << token << " </keyword>\n";

        parse_identifier();

        advance();
        assert(token == "{");
        output << indent << "<symbol> " << token << " </symbol>\n";

        advance();
        tokens.putback(token[0]);
        
        parse_class_var_dec();

        parse_subroutine_dec();

        advance();
        assert(token == "}");
        output << indent << "<symbol> " << token << " </symbol>\n";

        dec_indent();
    }

    void parse_class_var_dec() {
        output << indent << "<classVarDec>\n";
        inc_indent();

        advance();
        assert(token == "static" || token == "field");
        output << indent << "<keyword> " << token << " </keyword>\n";

        dec_indent();
        output << indent << "</classVarDec>\n";
    }

    void parse_subroutine_dec() {
        inc_indent();

        advance();
        assert(token == "constructor" || token == "function" || token == "method");
        output << indent << "<keyword> " << token << " </keyword>\n";

        dec_indent();
    }

    void parse_identifier() {
        advance();
        assert(regex_match(token, IDENTIFIER));
        output << indent << "<identifier> " << token << " </identifier>\n";
    }

    void inc_indent() {
        indent += "  ";
    }

    void dec_indent() {
        indent = indent.substr(0, indent.size() - 2);
    }

};


#endif // PARSER_CPP