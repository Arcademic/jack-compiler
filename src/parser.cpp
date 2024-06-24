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
        parse_class();
        output << flush;

        return output.str();
    }

private:
    Tokenizer* t;
    string indent;
    string token;

    stringstream output;

    void parse_class() {
        open_xml_tag("class");

        assert(t->peek() == "class");
        write_xml("keyword");

        parse_identifier();

        assert(t->peek() == "{");
        write_xml("symbol");
        
        while (
            t->peek() == "static" ||
            t->peek() == "field")
        {
            parse_class_var_dec();
        }

        while (
            t->peek() == "constructor" ||
            t->peek() == "function" ||
            t->peek() == "method")
        {
            parse_subroutine_dec();
        }

        assert(t->peek() == "}");
        write_xml("symbol");

        close_xml_tag("class");
    }

    void parse_class_var_dec() {
        open_xml_tag("classVarDec");

        assert(
            t->peek() == "static" ||
            t->peek() == "field");
        write_xml("keyword");

        parse_type();

        parse_identifier();

        while (t->peek() == ",") {
            assert(t->peek() == ",");
            write_xml("symbol");

            parse_identifier();
        }

        assert(t->peek() == ";");
        write_xml("symbol");

        close_xml_tag("classVarDec");
    }

    void parse_subroutine_dec() {
        open_xml_tag("subroutineDec");

        assert(
            t->peek() == "constructor" ||
            t->peek() == "function" ||
            t->peek() == "method");
        write_xml("keyword");

        if (t->peek() == "void") {
            write_xml("keyword");
        } else {
            parse_type();
        }

        parse_identifier();

        assert(t->peek() == "(");
        write_xml("symbol");

        parse_parameter_list();

        assert(t->peek() == ")");
        write_xml("symbol");

        parse_subroutine_body();

        close_xml_tag("subroutineDec");
    }

    void parse_parameter_list() {
        open_xml_tag("parameterList");

        if (t->peek() != ")") {
            parse_type();

            parse_identifier();

            while (t->peek() == ",") {
                assert(t->peek() == ",");
                write_xml("symbol");

                parse_type();

                parse_identifier();
            }
        }

        close_xml_tag("parameterList");
    }

    void parse_subroutine_body() {
        open_xml_tag("subroutineBody");

        assert(t->peek() == "{");
        write_xml("symbol");

        while (t->peek() == "var") {
            parse_var_dec();
        }

        parse_statements();

        assert(t->peek() == "}");
        write_xml("symbol");

        close_xml_tag("subroutineBody");
    }

    void parse_statements() {
        open_xml_tag("statements");

        while (
            t->peek() == "let" ||
            t->peek() == "if" ||
            t->peek() == "while" ||
            t->peek() == "do" ||
            t->peek() == "return")
        {
            parse_statement();
        }

        close_xml_tag("statements");
    }

    void parse_statement() {
        if (t->peek() == "let") {
            parse_let_statement();
        }
        else if (t->peek() == "if") {
            parse_if_statement();
        }
        else if (t->peek() == "while") {
            parse_while_statement();
        }
        else if (t->peek() == "do") {
            parse_do_statement();
        }
        else if (t->peek() == "return") {
            parse_return_statement();
        } else {
            throw runtime_error("Expected one of these keywords: let, if, while, do, return");
        }
    }

    void parse_let_statement() {
        open_xml_tag("letStatement");

        assert(t->peek() == "let");
        write_xml("keyword");

        parse_identifier();

        if (t->peek() == "[") {
            assert(t->peek() == "[");
            write_xml("symbol");

            parse_expression();

            assert(t->peek() == "]");
            write_xml("symbol");
        }

        assert(t->peek() == "=");
        write_xml("symbol");

        parse_expression();

        assert(t->peek() == ";");
        write_xml("symbol");

        close_xml_tag("letStatement");
    }

    void parse_if_statement() {
        open_xml_tag("ifStatement");

        assert(t->peek() == "if");
        write_xml("keyword");

        assert(t->peek() == "(");
        write_xml("symbol");

        parse_expression();

        assert(t->peek() == ")");
        write_xml("symbol");

        assert(t->peek() == "{");
        write_xml("symbol");

        parse_statements();

        assert(t->peek() == "}");
        write_xml("symbol");

        if (t->peek() == "else") {
            assert(t->peek() == "else");
            write_xml("keyword");

            assert(t->peek() == "{");
            write_xml("symbol");

            parse_statements();

            assert(t->peek() == "}");
            write_xml("symbol");
        }

        close_xml_tag("ifStatement");
    }

    void parse_while_statement() {
        open_xml_tag("whileStatement");

        assert(t->peek() == "while");
        write_xml("keyword");

        assert(t->peek() == "(");
        write_xml("symbol");

        parse_expression();

        assert(t->peek() == ")");
        write_xml("symbol");

        assert(t->peek() == "{");
        write_xml("symbol");

        parse_statements();

        assert(t->peek() == "}");
        write_xml("symbol");

        close_xml_tag("whileStatement");
    }

    void parse_do_statement() {
        open_xml_tag("doStatement");

        assert(t->peek() == "do");
        write_xml("keyword");

        parse_subroutine_call();

        assert(t->peek() == ";");
        write_xml("symbol");

        close_xml_tag("doStatement");
    }

    void parse_return_statement() {
        open_xml_tag("returnStatement");

        assert(t->peek() == "return");
        write_xml("keyword");

        if (t->peek() != ";") {
            parse_expression();
        }

        assert(t->peek() == ";");
        write_xml("symbol");

        close_xml_tag("returnStatement");
    }

    void parse_expression_list() {
        open_xml_tag("expressionList");

        if (true) { // TODO is expression?
            parse_expression();

            while (t->peek() == ",") {
                assert(t->peek() == ",");
                write_xml("symbol");

                parse_expression();
            }
        }

        close_xml_tag("expressionList");
    }
    
    void parse_expression() {
        open_xml_tag("expression");

        parse_term();

        while (regex_match(t->peek(), OP)) {
            parse_op();

            parse_term();
        }

        close_xml_tag("expression");
    }

    void parse_term() {
        open_xml_tag("term");

        if (regex_match(t->peek(), INTEGER_CONSTANT)) {
            write_xml("integerConstant");
        } else if (t->peek() == "\"") {
            write_xml("symbol");

            while (t->peek() != "\"") {
                assert(regex_match(t->peek(), STRING_CONSTANT));
                t->advance();
            }
            assert(t->peek() == "\"");
            write_xml("stringConstant");
        } else if (regex_match(t->peek(), KEYWORD_CONSTANT)) {
            write_xml("keyword");
        } else if (regex_match(t->peek(), UNARY_OP)) {
            write_xml("symbol");
        } else if (t->peek() == "(") {
            assert(t->peek() == "(");

            parse_expression();

            assert(t->peek() == ")s");
        } else {
            if (t->look_ahead(1) == "(" || t->look_ahead(1) == ".") {
                parse_subroutine_call();
            } else {
                parse_identifier();

                if (t->peek() == "[") {
                    assert(t->peek() == "[");
                    write_xml("symbol");

                    parse_expression();

                    assert(t->peek() == "]");
                    write_xml("symbol");
                }
            }
        }


        close_xml_tag("term");
    }

    void parse_op() {
        assert(regex_match(t->peek(), OP));
        write_xml("symbol");
    }

    void parse_subroutine_call() {
        open_xml_tag("subroutineCall");

        parse_identifier();

        if (t->peek() == ".") {
            assert(t->peek() == ".");
            write_xml("symbol");

            parse_identifier();
        }

        assert(t->peek() == "(");
        write_xml("symbol");

        parse_expression_list();

        assert(t->peek() == ")");
        write_xml("symbol");

        close_xml_tag("subroutineCall");
    }

    void parse_var_dec() {
        open_xml_tag("varDec");

        assert(t->peek() == "var");
        write_xml("keyword");

        parse_type();

        parse_identifier();

        while (t->peek() == ",") {
            assert(t->peek() == ",");
            write_xml("symbol");

            parse_identifier();
        }

        assert(t->peek() == ";");
        write_xml("symbol");

        close_xml_tag("varDec");
    }

    void parse_identifier() {
        assert(regex_match(t->peek(), IDENTIFIER));
        write_xml("identifier");
    }

    void parse_type() {
        if (
            t->peek() == "int" ||
            t->peek() == "char" ||
            t->peek() == "boolean")
        {
            write_xml("keyword");
        } else
        {
            parse_identifier();
        }
    }


    // HELPER METHODS

    void inc_indent() {
        indent += "  ";
    }

    void dec_indent() {
        indent = indent.substr(0, indent.size() - 2);
    }

    void write_xml(string tag, bool advance = true) {
        string token = t->peek();

        if (XML_SYMBOLS.find(token) != XML_SYMBOLS.end()) {
            token = XML_SYMBOLS.at(token);
        }

        // debug
        cout << indent << "<" << tag << "> " << token << " </" << tag << ">\n";
        
        output << indent << "<" << tag << "> " << token << " </" << tag << ">\n";
        if (advance) t->advance();
    }

    void open_xml_tag(string tag, bool increase_indent = true) {
        // debug
        cout << indent << "<" << tag << ">\n";

        output << indent << "<" << tag << ">\n";
        if (increase_indent) inc_indent();
    }

    void close_xml_tag(string tag, bool decrease_indent = true) {
        if (decrease_indent) dec_indent();
        output << indent << "</" << tag << ">\n";

        // debug
        output << indent << "</" << tag << ">\n";
    }
};


#endif // PARSER_CPP