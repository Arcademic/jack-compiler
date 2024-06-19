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
        this->t = &t;
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
    Tokenizer* t;
    string indent;
    string token;

    stringstream output;

    void parse_class() {
        inc_indent();

        assert(t->peek() == "class");
        output << indent << "<keyword> " << t->peek() << " </keyword>\n";
        t->advance();

        parse_identifier();

        assert(t->peek() == "{");
        output << indent << "<symbol> " << t->peek() << " </symbol>\n";
        t->advance();
        
        while (t->peek() == "static" || t->peek() == "field") {
            parse_class_var_dec();
        }

        parse_subroutine_dec();

        assert(t->peek() == "}");
        output << indent << "<symbol> " << t->peek() << " </symbol>\n";
        t->advance();

        dec_indent();
    }

    void parse_class_var_dec() {
        output << indent << "<classVarDec>\n";
        inc_indent();

        assert(t->peek() == "static" || t->peek() == "field");
        output << indent << "<keyword> " << t->peek() << " </keyword>\n";
        t->advance();

        dec_indent();
        output << indent << "</classVarDec>\n";
    }

    void parse_subroutine_dec() {
        inc_indent();

        assert(t->peek() == "constructor" || t->peek() == "function" || t->peek() == "method");
        output << indent << "<keyword> " << t->peek() << " </keyword>\n";
        t->advance();

        dec_indent();
    }

    void parse_identifier() {
        assert(regex_match(t->peek(), IDENTIFIER));
        output << indent << "<identifier> " << t->peek() << " </identifier>\n";
        t->advance();
    }

    void parse_type() {
        if (t->peek() == "int" || t->peek() == "char" || t->peek() == "boolean") {
            output << indent << "<keyword> " << t->peek() << " </keyword>\n";
            t->advance();
        } else {
            parse_identifier();
        }
    }

    void inc_indent() {
        indent += "  ";
    }

    void dec_indent() {
        indent = indent.substr(0, indent.size() - 2);
    }

};


#endif // PARSER_CPP