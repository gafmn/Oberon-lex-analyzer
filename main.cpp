#include <iostream>
#include <string>

using namespace std;


enum ClassName {
    Eof,  // Special type meaning the token is last in the source
    Add, Sub, Mul, Divide,
    Til, Apper, Dot, Comma, Semicolon, Pipe, LefBr, LefSqBr, LefFigBr,
    Assign, Pow, Eq, NoEq, Lat, Les, Great, LesOrEq, GreatOrEq, TwoDot, Colon, RightBr, RightSqBr, 
    RightFigBr, Arr, Import, Begin, By, Case, Const, Div, Do, Else, Elif, End, False, If, In,
    Is, Mod, Modul, Nil, Of, Or, Pointer, Proc, Rec, Rep, Return, Then, To, True, Type, Until, Var,
    While, For,
    IntDec, IntHex, Real,
};


class Token {
public:
    ClassName class_name;
    string value;

public:
    Token() {
        this->class_name = ClassName::Eof;
    }

    Token(ClassName class_name, string value) {
        this->class_name = class_name;
        this->value = value;
    }

};


class Lexer {
    string src;
    string::iterator src_iter;

public:
    Lexer(string src) {
        this->src = src;
        this->src_iter = this->src.begin();
    }

    Token next() {
        Token token;

        while (src_iter != src.end()) {
            if (*src_iter == ' ' || *src_iter == '\n') {
                // Skip spaces and new line characters
            }
            else if (*src_iter == '(') {
                if (src_iter + 1 != src.end() && *(src_iter + 1) == '*') {
                    parseComment();
                }
            }
            else if (isDigit(*src_iter)) {
                token = parseNumber();
            }
            else if (isLetter(*src_iter)) {
                token = parseIdentifier();
            }

            src_iter++;
        }

        return token;
    }

private:
    bool isDigit(char c) {
        return c >= '0' && c <= '9';
    }

    bool isLetter(char c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }

    void parseComment() {
        if (src_iter + 2 == src.end()) {
            cerr << "Comment section should be enclosed" << endl;
            exit(1);
        }

        char last_char = ' ';
        while (last_char != '*' && *src_iter != ')') {
            src_iter++;

            if (src_iter == src.end()) {
                cerr << "Comment section should be enclosed" << endl;
                exit(1);
            }
        }
    }

    Token parseNumber() {
        Token token;
        // Check correctness
        int token_value = *src_iter - '0';
        while (*src_iter >= '0' && *src_iter <= '9') {
            token_value = 10 * token_value + *src_iter++ - '0';
            if (*src_iter == 'H') {
               token.class_name = ClassName::IntHex;
               token.value = token_value + 'H';
               return token;
            }
            if (*src_iter >= 'A' && *src_iter <= 'F') {
                token.class_name = ClassName::IntHex;
                token.value = token_value + *src_iter;
                return token;
            }
            // Working with real numbers
            if (*src_iter == '.') {
                double real_token_val = 0;
                double result_value  = token_value;
                while(*src_iter != ' ') {
                   real_token_val = *src_iter/10 + *src_iter++ - '0'; 
                }
                result_value = result_value + real_token_val; 
                token.class_name = ClassName::Real;
                token.value = result_value;
            }
        }
        token.class_name = ClassName::IntDec;
        token.value = token_value;
        return token;
    }

    Token parseIdentifier() {
        Token token;
        return token;
    }
};


int main() {
    string src = "";

    Lexer lexer = Lexer(src);
    Token token = lexer.next();

    while (token.class_name != ClassName::Eof) {
        cout << "Token " << token.value << endl;
        token = lexer.next();
    }

    return 0;
}
