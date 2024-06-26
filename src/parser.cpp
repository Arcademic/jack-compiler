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
    }

    string parse() {
        indent = "";
        writing_enabled = true;
        bool parse_error = !parse_class();
        output << flush;

        if (parse_error) {
            cout << "Parse error." << endl;
        }
        return output.str();
    }

private:
    Tokenizer* t;
    string indent;
    bool writing_enabled;

    stringstream output;

    bool parse_class() {
        open_xml_tag("class");

        if (t->peek() != "class") return false;
        write_xml("keyword");

        if (!parse_identifier()) return false;

        if (t->peek() != "{") return false;
        write_xml("symbol");
        
        while (
            t->peek() == "static" ||
            t->peek() == "field")
        {
            if (!parse_class_var_dec()) return false;
        }

        while (
            t->peek() == "constructor" ||
            t->peek() == "function" ||
            t->peek() == "method")
        {
            if (!parse_subroutine_dec()) return false;
        }

        if (t->peek() != "}") return false;
        write_xml("symbol");

        close_xml_tag("class");
        return true;
    }

    bool parse_class_var_dec() {
        open_xml_tag("classVarDec");

        if (
            !(t->peek() == "static" ||
            t->peek() == "field"))
        {
            return false;
        }
        write_xml("keyword");

        if (!parse_type()) return false;

        if (!parse_identifier()) return false;

        while (t->peek() == ",") {
            if (t->peek() != ",") return false;
            write_xml("symbol");

            if (!parse_identifier()) return false;
        }

        if (t->peek() != ";") return false;
        write_xml("symbol");

        close_xml_tag("classVarDec");
        return true;
    }

    bool parse_subroutine_dec() {
        open_xml_tag("subroutineDec");

        if (
            !(t->peek() == "constructor" ||
            t->peek() == "function" ||
            t->peek() == "method"))
        {
            return false;
        };
        write_xml("keyword");

        if (t->peek() == "void") {
            write_xml("keyword");
        } else {
            if (!parse_type()) return false;
        }

        if (!parse_identifier()) return false;

        if (t->peek() != "(") return false;
        write_xml("symbol");

        if (!parse_parameter_list()) return false;

        if (t->peek() != ")") return false;
        write_xml("symbol");

        if (!parse_subroutine_body()) return false;

        close_xml_tag("subroutineDec");
        return true;
    }

    bool parse_parameter_list() {
        open_xml_tag("parameterList");

        if (t->peek() != ")") {
            if (!parse_type()) return false;

            if (!parse_identifier()) return false;

            while (t->peek() == ",") {
                if (t->peek() != ",") return false;
                write_xml("symbol");

                if (!parse_type()) return false;

                if (!parse_identifier()) return false;
            }
        }

        close_xml_tag("parameterList");
        return true;
    }

    bool parse_subroutine_body() {
        open_xml_tag("subroutineBody");

        if (t->peek() != "{") return false;
        write_xml("symbol");

        while (t->peek() == "var") {
            if (!parse_var_dec()) return false;
        }

        if (!parse_statements()) return false;

        if (t->peek() != "}") return false;
        write_xml("symbol");

        close_xml_tag("subroutineBody");
        return true;
    }

    bool parse_statements() {
        open_xml_tag("statements");

        while (
            t->peek() == "let" ||
            t->peek() == "if" ||
            t->peek() == "while" ||
            t->peek() == "do" ||
            t->peek() == "return")
        {
            if (!parse_statement()) return false;
        }

        close_xml_tag("statements");
        return true;
    }

    bool parse_statement() {
        if (t->peek() == "let") {
            if (!parse_let_statement()) return false;
        }
        else if (t->peek() == "if") {
            if (!parse_if_statement()) return false;
        }
        else if (t->peek() == "while") {
            if (!parse_while_statement()) return false;
        }
        else if (t->peek() == "do") {
            if (!parse_do_statement()) return false;
        }
        else if (t->peek() == "return") {
            if (!parse_return_statement()) return false;
        } else {
            return false;
        }

        return true;
    }

    bool parse_let_statement() {
        open_xml_tag("letStatement");

        if (t->peek() != "let") return false;
        write_xml("keyword");

        if (!parse_identifier()) return false;

        if (t->peek() == "[") {
            if (t->peek() != "[") return false;
            write_xml("symbol");

            if (!parse_expression()) return false;

            if (t->peek() != "]") return false;
            write_xml("symbol");
        }

        if (t->peek() != "=") return false;
        write_xml("symbol");

        if (!parse_expression()) return false;

        if (t->peek() != ";") return false;
        write_xml("symbol");

        close_xml_tag("letStatement");
        return true;
    }

    bool parse_if_statement() {
        open_xml_tag("ifStatement");

        if (t->peek() != "if") return false;
        write_xml("keyword");

        if (t->peek() != "(") return false;
        write_xml("symbol");

        if (!parse_expression()) return false;

        if (t->peek() != ")") return false;
        write_xml("symbol");

        if (t->peek() != "{") return false;
        write_xml("symbol");

        if (!parse_statements()) return false;

        if (t->peek() != "}") return false;
        write_xml("symbol");

        if (t->peek() == "else") {
            if (t->peek() != "else") return false;
            write_xml("keyword");

            if (t->peek() != "{") return false;
            write_xml("symbol");

            if (!parse_statements()) return false;

            if (t->peek() != "}") return false;
            write_xml("symbol");
        }

        close_xml_tag("ifStatement");
        return true;
    }

    bool parse_while_statement() {
        open_xml_tag("whileStatement");

        if (t->peek() != "while") return false;
        write_xml("keyword");

        if (t->peek() != "(") return false;
        write_xml("symbol");

        if (!parse_expression()) return false;

        if (t->peek() != ")") return false;
        write_xml("symbol");

        if (t->peek() != "{") return false;
        write_xml("symbol");

        if (!parse_statements()) return false;

        if (t->peek() != "}") return false;
        write_xml("symbol");

        close_xml_tag("whileStatement");
        return true;
    }

    bool parse_do_statement() {
        open_xml_tag("doStatement");

        if (t->peek() != "do") return false;
        write_xml("keyword");

        if (!parse_subroutine_call()) return false;

        if (t->peek() != ";") return false;
        write_xml("symbol");

        close_xml_tag("doStatement");
        return true;
    }

    bool parse_return_statement() {
        open_xml_tag("returnStatement");

        if (t->peek() != "return") return false;
        write_xml("keyword");

        if (t->peek() != ";") {
            if (!parse_expression()) return false;
        }

        if (t->peek() != ";") return false;
        write_xml("symbol");

        close_xml_tag("returnStatement");
        return true;
    }

    bool parse_expression_list() {
        open_xml_tag("expressionList");

        if (is_expression()) {
            if (!parse_expression()) return false;

            while (t->peek() == ",") {
                if (t->peek() != ",") return false;
                write_xml("symbol");

                if (!parse_expression()) return false;
            }
        }

        close_xml_tag("expressionList");
        return true;
    }
    
    bool parse_expression() {
        open_xml_tag("expression");

        if (!parse_term()) return false;

        while (regex_match(t->peek(), OP)) {
            if (!parse_op()) return false;

            if (!parse_term()) return false;
        }

        close_xml_tag("expression");
        return true;
    }

    bool is_expression() {
        t->save_state();
        writing_enabled = false;

        if (!parse_expression()) {
            t->restore_state();
            return false;
        }

        t->restore_state();
        writing_enabled = true;
        return true;
    }

    bool parse_term() {
        open_xml_tag("term");

        if (regex_match(t->peek(), INTEGER_CONSTANT)) {
            write_xml("integerConstant");
        } else if (t->peek() == "\"") {
            t->advance();

            if (writing_enabled) output << indent << "<stringConstant>";
            while (t->peek() != "\"") {
                if (!(regex_match(t->peek(), STRING_CONSTANT))) return false;
                if (writing_enabled) output << " " << t->peek();
                t->advance();
            }
            if (t->peek() != "\"") return false;
            if (writing_enabled) output << " </stringConstant>\n";
            t->advance();

        } else if (regex_match(t->peek(), KEYWORD_CONSTANT)) {
            write_xml("keyword");
        } else if (regex_match(t->peek(), UNARY_OP)) {
            write_xml("symbol");
        } else if (t->peek() == "(") {
            if (t->peek() != "(") return false;

            if (!parse_expression()) return false;

            if (t->peek() != ")s") return false;
        } else {
            if (t->look_ahead(1) == "(" || t->look_ahead(1) == ".") {
                if (!parse_subroutine_call()) return false;
            } else {
                if (!parse_identifier()) return false;

                if (t->peek() == "[") {
                    if (t->peek() != "[") return false;
                    write_xml("symbol");

                    if (!parse_expression()) return false;

                    if (t->peek() != "]") return false;
                    write_xml("symbol");
                }
            }
        }

        close_xml_tag("term");
        return true;
    }

    bool parse_op() {
        if (!(regex_match(t->peek(), OP))) return false;
        write_xml("symbol");

        return true;
    }

    bool parse_subroutine_call() {
        if (!parse_identifier()) return false;

        if (t->peek() == ".") {
            if (t->peek() != ".") return false;
            write_xml("symbol");

            if (!parse_identifier()) return false;
        }

        if (t->peek() != "(") return false;
        write_xml("symbol");

        if (!parse_expression_list()) return false;

        if (t->peek() != ")") return false;
        write_xml("symbol");

        return true;
    }

    bool parse_var_dec() {
        open_xml_tag("varDec");

        if (t->peek() != "var") {
            return false;
        } 
        write_xml("keyword");

        if (!parse_type()) return false;

        if (!parse_identifier()) return false;

        while (t->peek() == ",") {
            if (t->peek() != ",") {
                return false;
            }
            write_xml("symbol");

            if (!parse_identifier()) return false;
        }

        if (t->peek() != ";") return false;
        write_xml("symbol");

        close_xml_tag("varDec");
        return true;
    }

    bool parse_identifier() {
        if (!regex_match(t->peek(), IDENTIFIER)) {
            return false;
        }
        write_xml("identifier");

        return true;
    }

    bool parse_type() {
        if (
            t->peek() == "int" ||
            t->peek() == "char" ||
            t->peek() == "boolean")
        {
            write_xml("keyword");
            return true;
        }
        
        if (!parse_identifier()) return false;

        return true;
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
        
        if (writing_enabled) {
            output << indent << "<" << tag << "> " << token << " </" << tag << ">\n";
        }
        if (advance) t->advance();
    }

    void open_xml_tag(string tag, bool increase_indent = true) {
        // debug
        cout << indent << "<" << tag << ">\n";

        if (writing_enabled) {
            output << indent << "<" << tag << ">\n";
        }
        if (increase_indent) inc_indent();
    }

    void close_xml_tag(string tag, bool decrease_indent = true) {
        if (decrease_indent) dec_indent();
        if (writing_enabled) {
            output << indent << "</" << tag << ">\n";
        }

        // debug
        cout << indent << "</" << tag << ">\n";
    }
};


#endif // PARSER_CPP