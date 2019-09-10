#include <iostream>
#include <string>

using namespace std;
string empty = "";

class Token {
public:
    string class_name;
    string value;

public:
    enum ClassName{
        Add, Sub, Mul, Divide,
        Til, Apper, Dot, Comma, Semicolon, Pipe, LefBr, LefSqBr, LefFigBr,
        Assign, Pow, Eq, NoEq, Lat, Les, Great, LesOrEq, GreatOrEq, TwoDot, Colon, RightBr, RightSqBr, 
        RightFigBr, Arr, Import, Begin, By, Case, Const, Div, Do, Else, Elif, End, False, If, In,
        Is, Mod, Modul, Nil, Of, Or, Pointer, Proc, Rec, Rep, Return, Then, To, True, Type, Until, Var,
        While, For,
    };

    Token(string class_name, string value) {
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
        this->src_iter = src.begin();
    }

    Token next() {
        Token token = Token("", "");

        while (src_iter != src.end()) {
            if (isDigit(*src_iter)) {
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

    Token parseNumber() {
        Token token = Token(empty, empty);
        // Check correctness
        int token_value = *src_iter - '0';    
        while (*src_iter >= '0' && *src_iter <= '9') {
            token_value = 10 * token_value + *src_iter++ - '0';
            if (*src_iter == 'H') {
               token.class_name = "IntHex";
               token.value = token_value + 'H';
               return token;
            }
            if (*src_iter >= 'A' && *src_iter <= 'F') {
                token.class_name = "IntHex";
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
                token.class_name = "Real";
                token.value = result_value;
            }
        }
        token.class_name = "IntDec";
        token.value = token_value;
        return token;
    }

    Token parseIdentifier() {
    }
};


int main() {
    string src = "";

    Lexer lexer = Lexer(src);

    Token token =  Token(nullptr, nullptr);
    while (token.value != "") {
        token = lexer.next();
        cout << "Token " << token.value << endl;
    }

    return 0;
}
