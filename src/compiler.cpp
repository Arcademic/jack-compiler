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

    int n_args_count;

    int while_label_count = 0;
    int if_label_count = 0;

    bool compile_class() {
        

        if (t->peek() != "class") return false;
        t->advance();

        class_name = t->peek();
        if (!compile_identifier()) return false;

        if (t->peek() != "{") return false;
        t->advance();
        
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
        t->advance();

        cout << "Class symbol table: " << endl;
        class_table.print();

        
        return true;
    }

    bool compile_class_var_dec() {
        

        string kind = t->peek();
        if (
            !(kind == "static" ||
            kind == "field"))
        {
            return false;
        }
        t->advance();

        string type = t->peek();
        if (!compile_type()) return false;

        string name = t->peek();
        if (!compile_identifier()) return false;

        class_table.define(name, type, kind);

        while (t->peek() == ",") {
            if (t->peek() != ",") return false;
            t->advance();

            name = t->peek();
            if (!compile_identifier()) return false;

            class_table.define(name, type, kind);
        }

        if (t->peek() != ";") return false;
        t->advance();

        
        return true;
    }

    bool compile_subroutine_dec() {
        subroutine_table.reset();
        string keyword = t->peek();
        if (keyword == "method") {
            subroutine_table.define("this", class_name, "arg");
        }

        if (
            !(keyword == "constructor" ||
            keyword == "function" ||
            keyword == "method"))
        {
            return false;
        };
        t->advance();

        if (t->peek() == "void") {
            t->advance();
        } else {
            if (!compile_type()) return false;
        }

        string subroutine_name = t->peek();
        if (!compile_identifier()) return false;

        if (t->peek() != "(") return false;
        t->advance();

        if (!compile_parameter_list()) return false;

        if (t->peek() != ")") return false;
        t->advance();

        if (!compile_subroutine_body(subroutine_name)) return false;

        cout << "Subroutine symbol table: " << subroutine_name << endl;
        subroutine_table.print();

        
        return true;
    }

    bool compile_parameter_list() {
        

        if (t->peek() != ")") {
            string type = t->peek();
            if (!compile_type()) return false;

            string name = t->peek();
            if (!compile_identifier()) return false;

            subroutine_table.define(name, type, "arg");

            while (t->peek() == ",") {
                if (t->peek() != ",") return false;
                t->advance();

                string type = t->peek();
                if (!compile_type()) return false;

                string name = t->peek();
                if (!compile_identifier()) return false;

                subroutine_table.define(name, type, "arg");
            }
        }

        
        return true;
    }

    bool compile_subroutine_body(string subroutine_name) {
        

        if (t->peek() != "{") return false;
        t->advance();

        while (t->peek() == "var") {
            if (!compile_var_dec()) return false;
        }

        write_function(subroutine_name, subroutine_table.var_count("var"));

        if (!compile_statements()) return false;

        if (t->peek() != "}") return false;
        t->advance();

        
        return true;
    }

    bool compile_statements() {
        while (
            t->peek() == "let" ||
            t->peek() == "if" ||
            t->peek() == "while" ||
            t->peek() == "do" ||
            t->peek() == "return")
        {
            if (!compile_statement()) return false;
        }
        
        return true;
    }

    bool compile_statement() {
        if (t->peek() == "let") {
            string var_name = t->look_ahead(1);
            if (!compile_let_statement()) return false;
            string segment;
            if (subroutine_table.contains(var_name)) {
                segment = KIND_TO_SEGMENT.at(subroutine_table.kind_of(var_name));
                write_pop(segment, to_string(subroutine_table.index_of(var_name)));
            } else if (class_table.contains(var_name)) {
                segment = KIND_TO_SEGMENT.at(class_table.kind_of(var_name));
                write_pop(segment, to_string(class_table.index_of(var_name)));
            } else {
                cout << "Identifier '" << var_name << "' not recognized." << endl;
            }
        }
        else if (t->peek() == "if") {
            if (!compile_if_statement()) return false;
        }
        else if (t->peek() == "while") {
            if (!compile_while_statement()) return false;
        }
        else if (t->peek() == "do") {
            if (!compile_do_statement()) return false;
            write_pop("temp", "0");
        }
        else if (t->peek() == "return") {
            if (!compile_return_statement()) return false;
        } else {
            return false;
        }

        return true;
    }

    bool compile_let_statement() {
        

        if (t->peek() != "let") return false;
        t->advance();

        if (!compile_identifier()) return false;

        if (t->peek() == "[") {
            if (t->peek() != "[") return false;
            t->advance();

            if (!compile_expression()) return false;

            if (t->peek() != "]") return false;
            t->advance();
        }

        if (t->peek() != "=") return false;
        t->advance();

        if (!compile_expression()) return false;

        if (t->peek() != ";") return false;
        t->advance();

        
        return true;
    }

    bool compile_if_statement() {
        if (t->peek() != "if") return false;
        t->advance();

        if (t->peek() != "(") return false;
        t->advance();

        if (!compile_expression()) return false;

        if (t->peek() != ")") return false;
        t->advance();

        int label_count = if_label_count++;
        string cond_true_label = "IF_TRUE";
        cond_true_label.append(to_string(label_count));
        write_if(cond_true_label);

        string cond_false_label = "IF_FALSE";
        cond_false_label.append(to_string(label_count));
        write_goto(cond_false_label);
        
        write_label(cond_true_label);

        if (t->peek() != "{") return false;
        t->advance();

        if (!compile_statements()) return false;

        if (t->peek() != "}") return false;
        t->advance();

        string end_label = "IF_END";

        if (t->peek() == "else") {
            if (t->peek() != "else") return false;
            t->advance();

            if (t->peek() != "{") return false;
            t->advance();

            end_label.append(to_string(label_count));
            write_goto(end_label);

            write_label(cond_false_label);

            if (!compile_statements()) return false;

            if (t->peek() != "}") return false;
            t->advance();
        }

        write_label(end_label);
        if_label_count = 0;

        return true;
    }

    bool compile_while_statement() {
        
        int label_count = while_label_count++;
        string cond_label = "WHILE_EXP";
        cond_label.append(to_string(label_count));
        write_label(cond_label);

        if (t->peek() != "while") return false;
        t->advance();

        if (t->peek() != "(") return false;
        t->advance();

        if (!compile_expression()) return false;

        if (t->peek() != ")") return false;
        t->advance();

        write("not");
        string end_label = "WHILE_END";
        end_label.append(to_string(label_count));
        write_if(end_label);

        if (t->peek() != "{") return false;
        t->advance();

        if (!compile_statements()) return false;

        if (t->peek() != "}") return false;
        t->advance();

        write_goto(cond_label);
        write_label(end_label);
        while_label_count = 0;
        
        return true;
    }

    bool compile_do_statement() {
        

        if (t->peek() != "do") return false;
        t->advance();

        if (!compile_subroutine_call()) return false;

        if (t->peek() != ";") return false;
        t->advance();

        

        
        return true;
    }

    bool compile_return_statement() {
        

        if (t->peek() != "return") return false;
        t->advance();



        if (t->peek() != ";") {
            if (!compile_expression()) return false;
        } else {
            write_push("constant", "0");
        }

        if (t->peek() != ";") return false;
        t->advance();

        write_return();
        
        return true;
    }

    bool compile_expression_list() {
        

        if (is_expression()) {
            if (!compile_expression()) return false;
            n_args_count++;

            while (t->peek() == ",") {
                if (t->peek() != ",") return false;
                t->advance();

                if (!compile_expression()) return false;
                n_args_count++;
            }
        }

        
        return true;
    }
    
    bool compile_expression() {
        

        if (!compile_term()) return false;

        while (regex_match(t->peek(), OP)) {
            string op = t->peek();
            if (!compile_op()) return false;

            if (!compile_term()) return false;

            write_op(op);
        }

        
        return true;
    }

    bool compile_term() {
        

        if (regex_match(t->peek(), INTEGER_CONSTANT)) {
            string integer_constant = t->peek();
            t->advance();

            write_push("constant", integer_constant);
        } else if (t->peek() == "\"") {
            t->advance();
            if (!(regex_match(t->peek(), STRING_CONSTANT))) return false;
            t->advance();
            if (t->peek() != "\"") return false;
            t->advance();
        } else if (regex_match(t->peek(), KEYWORD_CONSTANT)) {
            string keyword = t->peek();
            t->advance();
            if (keyword == "true") {
                write_push("constant", "0");
                write("not");
            } else if (keyword == "false") {
                write_push("constant", "0");
            } else if (keyword == "null") {
                write_push("constant", "0");
            } else if (keyword == "this") {
                write_push("pointer", "0");
            } else {
                cout << "Keyword constant '" << keyword << "' not recognized. Expected 'true', 'false', 'null' or 'this'." << endl;
            }
        } else if (regex_match(t->peek(), UNARY_OP)) {
            string op = t->peek();
            t->advance();
            if (!compile_term()) return false;
            write_unary_op(op);
        } else if (t->peek() == "(") {
            if (t->peek() != "(") return false;
            t->advance();

            if (!compile_expression()) return false;

            if (t->peek() != ")") return false;
            t->advance();
        } else {
            if (t->look_ahead(1) == "(" || t->look_ahead(1) == ".") {
                if (!compile_subroutine_call()) return false;
            } else {
                string var_name = t->peek();
                if (!compile_identifier()) return false;
                string segment;
                if (subroutine_table.contains(var_name)) {
                    segment = KIND_TO_SEGMENT.at(subroutine_table.kind_of(var_name));
                    write_push(segment, to_string(subroutine_table.index_of(var_name)));
                } else if (class_table.contains(var_name)) {
                    segment = KIND_TO_SEGMENT.at(class_table.kind_of(var_name));
                    write_push(segment, to_string(class_table.index_of(var_name)));
                } else {
                    cout << "Identifier '" << var_name << "' not recognized." << endl;
                }

                if (t->peek() == "[") {
                    if (t->peek() != "[") return false;
                    t->advance();

                    if (!compile_expression()) return false;

                    if (t->peek() != "]") return false;
                    t->advance();
                }
            }
        }

        
        return true;
    }

    bool compile_op() {
        if (!(regex_match(t->peek(), OP))) return false;
        t->advance();

        return true;
    }

    bool compile_subroutine_call() {
        string subroutine_name = t->peek();
        if (!compile_identifier()) return false;

        if (t->peek() == ".") {
            if (t->peek() != ".") return false;
            t->advance();

            subroutine_name.append(".").append(t->peek());
            if (!compile_identifier()) return false;
        }

        if (t->peek() != "(") return false;
        t->advance();

        n_args_count = 0;
        if (!compile_expression_list()) return false;

        if (t->peek() != ")") return false;
        t->advance();

        write_call(subroutine_name, n_args_count);

        return true;
    }

    bool compile_var_dec() {
        

        string kind = t->peek();
        if (kind != "var") {
            return false;
        } 
        t->advance();

        string type = t->peek();
        if (!compile_type()) return false;

        string name = t->peek();
        if (!compile_identifier()) return false;

        subroutine_table.define(name, type, kind);

        while (t->peek() == ",") {
            if (t->peek() != ",") {
                return false;
            }
            t->advance();

            name = t->peek();
            if (!compile_identifier()) return false;

            subroutine_table.define(name, type, kind);
        }

        if (t->peek() != ";") return false;
        t->advance();

        
        return true;
    }

    bool compile_identifier() {
        if (!regex_match(t->peek(), IDENTIFIER)) {
            return false;
        }

        t->advance();

        return true;
    }

    bool compile_type() {
        if (
            t->peek() == "int" ||
            t->peek() == "char" ||
            t->peek() == "boolean")
        {
            t->advance();
            return true;
        }

        if (!compile_identifier()) return false;

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


    // WRITE VM CODE

    void write_function(string name, int n_locals) {
        if (writing_enabled) {
            output << "function " << class_name << '.' << name << " " << to_string(n_locals) << '\n';
        }
    }

    void write_push(string segment, string index) {
        if (writing_enabled) {
            output << "push " << segment << " " << index << '\n';
        }
    }

    void write_pop(string segment, string index) {
        if (writing_enabled) {
            output << "pop " << segment << " " << index << '\n';
        }
    }

    void write_op(string op) {
        if (writing_enabled) {
            if (OP_TO_VM.find(op) != OP_TO_VM.end()) {
                output << OP_TO_VM.at(op) << '\n';
            } else if (op == "*") {
                write_call("Math.multiply", 2);
            } else if (op == "/") {
                write_call("Math.divide", 2);
            } else {
                cout << "Operation '" << op << "' not recognized." << endl;
            }
        }
    }

    void write_unary_op(string op) {
        if (writing_enabled) {
            if (UNARY_OP_TO_VM.find(op) != UNARY_OP_TO_VM.end()) {
                output << UNARY_OP_TO_VM.at(op) << '\n';
            } else {
                cout << "Unary operation '" << op << "' not recognized." << endl;
            }
        }
    }

    void write_call(string name, int n_args) {
        output << "call " << name << " " << to_string(n_args) << '\n';
    }

    void write_return() {
        output << "return" << "\n";
    }

    void write_label(string label) {
        output << "label " << label << endl;
    }

    void write_if(string label) {
        output << "if-goto " << label << endl;
    }

    void write_goto(string label) {
        output << "goto " << label << endl;
    }

    void write(string command) {
        output << command << '\n';
    }

};


#endif // COMPILER_CPP