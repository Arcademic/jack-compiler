#ifndef COMPILER_CPP
#define COMPILER_CPP

#include "constants.h"
#include "tokenizer.cpp"
#include "symbol_table.cpp"
#include <sstream>
#include <iostream>
#include <cassert>

using namespace std;


class Compiler
{
public:
    Compiler(Tokenizer& t) {
        this->t = &t;
    }

    string compile() {
        indent = "";
        writing_enabled = true;
        bool compile_error = !compile_class();
        output << flush;

        if (compile_error) {
            cout << "Compilation error." << endl;
        }
        return output.str();
    }

private:
    Tokenizer* t;
    string indent;
    string class_name;
    bool writing_enabled;

    stringstream output;

    SymbolTable class_table;
    SymbolTable subroutine_table;

    bool compile_class() {
        open_xml_tag("class");

        if (t->peek() != "class") return false;
        write_xml("keyword");

        class_name = t->peek();
        if (!compile_identifier()) return false;

        if (t->peek() != "{") return false;
        write_xml("symbol");
        
        while (
            t->peek() == "static" ||
            t->peek() == "field")
        {
            if (!compile_class_var_dec()) return false;
        }

        while (
            t->peek() == "constructor" ||
            t->peek() == "function" ||
            t->peek() == "method")
        {
            if (!compile_subroutine_dec()) return false;
        }

        if (t->peek() != "}") return false;
        write_xml("symbol");

        cout << "Class symbol table: " << endl;
        class_table.print();

        close_xml_tag("class");
        return true;
    }

    bool compile_class_var_dec() {
        open_xml_tag("classVarDec");

        string kind = t->peek();
        if (
            !(kind == "static" ||
            kind == "field"))
        {
            return false;
        }
        write_xml("keyword");

        string type = t->peek();
        if (!compile_type()) return false;

        string name = t->peek();
        if (!compile_identifier()) return false;

        class_table.define(name, type, kind);

        while (t->peek() == ",") {
            if (t->peek() != ",") return false;
            write_xml("symbol");

            name = t->peek();
            if (!compile_identifier()) return false;

            class_table.define(name, type, kind);
        }

        if (t->peek() != ";") return false;
        write_xml("symbol");

        close_xml_tag("classVarDec");
        return true;
    }

    bool compile_subroutine_dec() {
        open_xml_tag("subroutineDec");

        subroutine_table.reset();
        subroutine_table.define("this", class_name, "arg"); // only for methods?

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
            if (!compile_type()) return false;
        }

        string subroutine_name = t->peek();
        if (!compile_identifier()) return false;

        if (t->peek() != "(") return false;
        write_xml("symbol");

        if (!compile_parameter_list()) return false;

        if (t->peek() != ")") return false;
        write_xml("symbol");

        if (!compile_subroutine_body()) return false;

        cout << "Subroutine symbol table: " << subroutine_name << endl;
        subroutine_table.print();

        write_function(subroutine_name, subroutine_table.var_count("var"));

        close_xml_tag("subroutineDec");
        return true;
    }

    bool compile_parameter_list() {
        open_xml_tag("parameterList");

        if (t->peek() != ")") {
            string type = t->peek();
            if (!compile_type()) return false;

            string name = t->peek();
            if (!compile_identifier()) return false;

            subroutine_table.define(name, type, "arg");

            while (t->peek() == ",") {
                if (t->peek() != ",") return false;
                write_xml("symbol");

                string type = t->peek();
                if (!compile_type()) return false;

                string name = t->peek();
                if (!compile_identifier()) return false;

                subroutine_table.define(name, type, "arg");
            }
        }

        close_xml_tag("parameterList");
        return true;
    }

    bool compile_subroutine_body() {
        open_xml_tag("subroutineBody");

        if (t->peek() != "{") return false;
        write_xml("symbol");

        while (t->peek() == "var") {
            if (!compile_var_dec()) return false;
        }

        if (!compile_statements()) return false;

        if (t->peek() != "}") return false;
        write_xml("symbol");

        close_xml_tag("subroutineBody");
        return true;
    }

    bool compile_statements() {
        open_xml_tag("statements");

        while (
            t->peek() == "let" ||
            t->peek() == "if" ||
            t->peek() == "while" ||
            t->peek() == "do" ||
            t->peek() == "return")
        {
            if (!compile_statement()) return false;
        }

        close_xml_tag("statements");
        return true;
    }

    bool compile_statement() {
        if (t->peek() == "let") {
            if (!compile_let_statement()) return false;
        }
        else if (t->peek() == "if") {
            if (!compile_if_statement()) return false;
        }
        else if (t->peek() == "while") {
            if (!compile_while_statement()) return false;
        }
        else if (t->peek() == "do") {
            if (!compile_do_statement()) return false;
        }
        else if (t->peek() == "return") {
            if (!compile_return_statement()) return false;
        } else {
            return false;
        }

        return true;
    }

    bool compile_let_statement() {
        open_xml_tag("letStatement");

        if (t->peek() != "let") return false;
        write_xml("keyword");

        if (!compile_identifier()) return false;

        if (t->peek() == "[") {
            if (t->peek() != "[") return false;
            write_xml("symbol");

            if (!compile_expression()) return false;

            if (t->peek() != "]") return false;
            write_xml("symbol");
        }

        if (t->peek() != "=") return false;
        write_xml("symbol");

        if (!compile_expression()) return false;

        if (t->peek() != ";") return false;
        write_xml("symbol");

        close_xml_tag("letStatement");
        return true;
    }

    bool compile_if_statement() {
        open_xml_tag("ifStatement");

        if (t->peek() != "if") return false;
        write_xml("keyword");

        if (t->peek() != "(") return false;
        write_xml("symbol");

        if (!compile_expression()) return false;

        if (t->peek() != ")") return false;
        write_xml("symbol");

        if (t->peek() != "{") return false;
        write_xml("symbol");

        if (!compile_statements()) return false;

        if (t->peek() != "}") return false;
        write_xml("symbol");

        if (t->peek() == "else") {
            if (t->peek() != "else") return false;
            write_xml("keyword");

            if (t->peek() != "{") return false;
            write_xml("symbol");

            if (!compile_statements()) return false;

            if (t->peek() != "}") return false;
            write_xml("symbol");
        }

        close_xml_tag("ifStatement");
        return true;
    }

    bool compile_while_statement() {
        open_xml_tag("whileStatement");

        if (t->peek() != "while") return false;
        write_xml("keyword");

        if (t->peek() != "(") return false;
        write_xml("symbol");

        if (!compile_expression()) return false;

        if (t->peek() != ")") return false;
        write_xml("symbol");

        if (t->peek() != "{") return false;
        write_xml("symbol");

        if (!compile_statements()) return false;

        if (t->peek() != "}") return false;
        write_xml("symbol");

        close_xml_tag("whileStatement");
        return true;
    }

    bool compile_do_statement() {
        open_xml_tag("doStatement");

        if (t->peek() != "do") return false;
        write_xml("keyword");

        if (!compile_subroutine_call()) return false;

        if (t->peek() != ";") return false;
        write_xml("symbol");

        close_xml_tag("doStatement");
        return true;
    }

    bool compile_return_statement() {
        open_xml_tag("returnStatement");

        if (t->peek() != "return") return false;
        write_xml("keyword");

        if (t->peek() != ";") {
            if (!compile_expression()) return false;
        }

        if (t->peek() != ";") return false;
        write_xml("symbol");

        close_xml_tag("returnStatement");
        return true;
    }

    bool compile_expression_list() {
        open_xml_tag("expressionList");

        if (is_expression()) {
            if (!compile_expression()) return false;

            while (t->peek() == ",") {
                if (t->peek() != ",") return false;
                write_xml("symbol");

                if (!compile_expression()) return false;
            }
        }

        close_xml_tag("expressionList");
        return true;
    }
    
    bool compile_expression() {
        open_xml_tag("expression");

        if (!compile_term()) return false;

        while (regex_match(t->peek(), OP)) {
            if (!compile_op()) return false;

            if (!compile_term()) return false;
        }

        close_xml_tag("expression");
        return true;
    }

    bool is_expression() {
        t->save_state();
        writing_enabled = false;

        if (!compile_expression()) {
            t->restore_state();
            writing_enabled = true;
            return false;
        }

        t->restore_state();
        writing_enabled = true;
        return true;
    }

    bool compile_term() {
        open_xml_tag("term");

        if (regex_match(t->peek(), INTEGER_CONSTANT)) {
            write_xml("integerConstant");
        } else if (t->peek() == "\"") {
            t->advance();
            if (!(regex_match(t->peek(), STRING_CONSTANT))) return false;
            write_xml("stringConstant");
            if (t->peek() != "\"") return false;
            t->advance();
        } else if (regex_match(t->peek(), KEYWORD_CONSTANT)) {
            write_xml("keyword");
        } else if (regex_match(t->peek(), UNARY_OP)) {
            write_xml("symbol");
            if (!compile_term()) return false;
        } else if (t->peek() == "(") {
            if (t->peek() != "(") return false;
            write_xml("symbol");

            if (!compile_expression()) return false;

            if (t->peek() != ")") return false;
            write_xml("symbol");
        } else {
            if (t->look_ahead(1) == "(" || t->look_ahead(1) == ".") {
                if (!compile_subroutine_call()) return false;
            } else {
                if (!compile_identifier()) return false;

                if (t->peek() == "[") {
                    if (t->peek() != "[") return false;
                    write_xml("symbol");

                    if (!compile_expression()) return false;

                    if (t->peek() != "]") return false;
                    write_xml("symbol");
                }
            }
        }

        close_xml_tag("term");
        return true;
    }

    bool compile_op() {
        if (!(regex_match(t->peek(), OP))) return false;
        write_xml("symbol");

        return true;
    }

    bool compile_subroutine_call() {
        if (!compile_identifier()) return false;

        if (t->peek() == ".") {
            if (t->peek() != ".") return false;
            write_xml("symbol");

            if (!compile_identifier()) return false;
        }

        if (t->peek() != "(") return false;
        write_xml("symbol");

        if (!compile_expression_list()) return false;

        if (t->peek() != ")") return false;
        write_xml("symbol");

        return true;
    }

    bool compile_var_dec() {
        open_xml_tag("varDec");

        string kind = t->peek();
        if (kind != "var") {
            return false;
        } 
        write_xml("keyword");

        string type = t->peek();
        if (!compile_type()) return false;

        string name = t->peek();
        if (!compile_identifier()) return false;

        subroutine_table.define(name, type, kind);

        while (t->peek() == ",") {
            if (t->peek() != ",") {
                return false;
            }
            write_xml("symbol");

            name = t->peek();
            if (!compile_identifier()) return false;

            subroutine_table.define(name, type, kind);
        }

        if (t->peek() != ";") return false;
        write_xml("symbol");

        close_xml_tag("varDec");
        return true;
    }

    bool compile_identifier() {
        if (!regex_match(t->peek(), IDENTIFIER)) {
            return false;
        }

        string name = t->peek();
        if (writing_enabled) {
            if (subroutine_table.contains(name)) {
                cout << "Found identifier in method level table: " << endl;
                cout << name << endl;
                cout << subroutine_table.type_of(name) << endl;
                cout << subroutine_table.kind_of(name) << endl;
            } else if (class_table.contains(name)) {
                cout << "Found identifier in class level table: " << endl;
                cout << name << endl;
                cout << subroutine_table.type_of(name) << endl;
                cout << subroutine_table.kind_of(name) << endl;
            }
        }

        write_xml("identifier");

        return true;
    }

    bool compile_type() {
        if (
            t->peek() == "int" ||
            t->peek() == "char" ||
            t->peek() == "boolean")
        {
            write_xml("keyword");
            return true;
        }

        if (!compile_identifier()) return false;

        return true;
    }

    // WRITE VM CODE

    void write_function(string name, int n_locals) {
        output << "function " << class_name << '.' << name << " " << to_string(n_locals) << '\n';
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

        
        if (writing_enabled) {
            // output << indent << "<" << tag << "> " << token << " </" << tag << ">\n";
        }
        if (advance) t->advance();
    }

    void open_xml_tag(string tag, bool increase_indent = true) {
        if (writing_enabled) {
            // output << indent << "<" << tag << ">\n";
        }
        if (increase_indent && writing_enabled) inc_indent();
    }

    void close_xml_tag(string tag, bool decrease_indent = true) {
        if (decrease_indent && writing_enabled) dec_indent();
        if (writing_enabled) {
            // output << indent << "</" << tag << ">\n";
        }
    }
};


#endif // COMPILER_CPP