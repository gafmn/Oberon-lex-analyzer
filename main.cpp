#include <iostream>
#include <string>

using namespace std;

#define MAX_HASH 997


enum ClassName {
    None,  // Special class meaning that token is empty
    Ident,  // Identificator
    Not, And, Or,  // Logical operators
    Import, Module,  // Module keywords
    True, False, Nil,  // Special values
    Proc, Begin, End, Ret,  // Procedure keywords
    Colon, Semicolon, Assign,  // Special symbols
    If, Elif, Else, Case, Then,  // Condition keywords
    Add, Sub, Mul, Div, DivInt, Mod,  // Math operations
    While, For, To, By, Do, Rep, Until,  // Loop keywords
    Type, Pointer, Var, Const, Arr, Of, Rec,  // Type keywords
    IntDec, IntHex, IntExp, Real, Str, StrHex,  // Literals
    Eq, NoEq, Less, LessOrEq, Great, GreatOrEq, Is, In,  // Comparison operators
    LefBr, RightBr, LefSqBr, RightSqBr, LefFigBr, RightFigBr,  // Brackets
    BoolType, CharType, IntType, RealType, ByteType, SetType,  // Basic types
};

class Node {
public:
    string value;
    ClassName class_name;
    Node* next; 

    Node() {
        next = NULL;    
    }

    Node(string value, ClassName class_name) {
        this->value = value;
        this->class_name = class_name;
        this->next = NULL;
    }
};

class SymbolTable {
public:
    Node* nodes[MAX_HASH];

public:
    SymbolTable() {
       for (int i = 0; i < MAX_HASH; i++) 
            nodes[i] = NULL;
    }

    int hashf (string node_value) {
        int hash_sum = 0;
       
        for (char sym : node_value) {
            hash_sum += 29 * (sym + 281);
            hash_sum %= MAX_HASH;
        }

        return hash_sum;
    }

    bool insert (string value, ClassName class_name) {
        int hash_id = hashf(value);
        Node* node = new Node(value, class_name);
        if (nodes[hash_id] == NULL) {
            nodes[hash_id] = node;
            
            return true;
        }else {
            Node* start = nodes[hash_id];
            if (start->value == value) 
                return false;
            while (start->next != NULL) 
                start = start->next;
            start->next = node;
            return true;
        }
        return false;
    }

    ClassName find (string value) {
        int hash_id = hashf(value);
        Node* start = nodes[hash_id];

        if (start == NULL) {
            return ClassName::None;
        }

        while (start != NULL) {
            if (start->value == value) {
                return start->class_name;
            }
            start = start->next;
        }
        return ClassName::None;
    }

};

class Token {
public:
    ClassName class_name;
    string value;

public:
    Token() {
        this->class_name = ClassName::None;
    }

    Token(ClassName class_name, string value) {
        this->class_name = class_name;
        this->value = value;
    }

};


class Lexer {
private:
    string src;
    string::iterator src_iter;

public:
    SymbolTable symbol_table;

public:
    Lexer(string src) {
        this->src = src;
        this->src_iter = this->src.begin();

        createSymbolTable();
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
                return parseNumber();
            }
            else if (isLetter(*src_iter)) {
                return parseIdentifier();
            }
            else if (*src_iter == '"') {
                return parseString();
            }
            else if (isOtherSymbol(*src_iter)) {
                return parseOtherSymbol();
            }
            src_iter++;
        }

        return token;
    }



private:
    void createSymbolTable() {
        // Feel the table keywords
        pair<string, ClassName> keywords[] = {
            {"ARRAY", ClassName::Arr}, {"OF", ClassName::Of},
            {"PROCEDURE", ClassName::Proc}, {"BEGIN", ClassName::Begin},
            {"END", ClassName::End}, {"RETURN", ClassName::Ret},
            {"RECORD", ClassName::Rec},
            {"POINTER", ClassName::Pointer}, {"TO", ClassName::To},
            {"IN", ClassName::In}, {"IS", ClassName::Is}, {"OR", ClassName::Or},
            {"DIV", ClassName::Div}, {"MOD", ClassName::Mod},
            {"FALSE", ClassName::False}, {"TRUE", ClassName::True},
            {"NIL", ClassName::Nil},
            {"IF", ClassName::If}, {"THEN", ClassName::Then},
            {"ELSIF", ClassName::Elif}, {"ELSE", ClassName::Else},
            {"CASE", ClassName::Case}, {"OF", ClassName::Of},
            {"WHILE", ClassName::While}, {"DO", ClassName::Do},
            {"REPEAT", ClassName::Rep}, {"UNTIL", ClassName::Until},
            {"FOR", ClassName::For}, {"BY", ClassName::By},
            {"CONST", ClassName::Const}, {"TYPE", ClassName::Type},
            {"VAR", ClassName::Var},
            {"MODULE", ClassName::Module}, {"IMPORT", ClassName::Import},
        };

        string identifiers[] = {
            "ABS", "ASR", "ASSERT", "BOOLEAN", "BYTE", "CHAR", "CHR",
            "DEC", "EXCL", "FLOOR", "FLT", "INC", "INCL", "INTEGER",
            "LEN", "LSL", "NEW", "ODD", "ORD", "PACK", "REAL", "ROR",
            "SET", "UNPK",
        };

        for (auto keyword : keywords)
            symbol_table.insert(keyword.first, keyword.second);

        for (string identifier : identifiers)
            symbol_table.insert(identifier, ClassName::Ident);
    }

    bool isDigit(char c) {
        return c >= '0' && c <= '9';
    }

    bool isHexDigit(char c) {
        return isDigit(c) || (c >= 'A' && c <= 'F');
    }

    bool isLetter(char c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }

    bool isOtherSymbol(char c) {
        return (c >= '!' && c <= '/') || (c >= ':' && c <= '>') || (c >= '[' && c <= ']');
    }

    Token parseString() {
        Token token;
        string value = "";

        src_iter++;

        while (src_iter != src.end() && *src_iter != '"') {
            value += *src_iter;
            src_iter++;
        }

        if (src_iter == src.end()) {
            cerr << "Strings should have quotes in the end" << endl;
            exit(1);
        }

        src_iter++;

        token.class_name = ClassName::Str;
        token.value = value;
        return token;
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

    Token parseOtherSymbol() {
        Token token = Token(ClassName::None, "");
        
        if (*src_iter == '+') {
            token.value = "+";
            token.class_name = ClassName::Add;
        }
        else if (*src_iter == '-') {
            token.value = "-";
            token.class_name = ClassName::Sub;
        }
        else if (*src_iter == '*') {
            token.value = "*";
            token.class_name = ClassName::Mul;
        }
        else if (*src_iter == '/') {
            token.value = "/";
            token.class_name = ClassName::Div;
        }
        else if (*src_iter == '#') {
            token.value = "#";
            token.class_name = ClassName::NoEq;
        }
        else if (*src_iter == '=') {
            token.value = '=';
            token.class_name = ClassName::Eq;
        }
        else if (*src_iter == '<') {
            if (src_iter + 1 != src.end() && *(src_iter + 1) == '=') {
                src_iter++;
                token.value = "<=";
                token.class_name = ClassName::LessOrEq;
            }
            else {
                token.value = "<";
                token.class_name = ClassName::Less;
            }
        }
        else if (*src_iter == '>') {
            if (src_iter + 1 != src.end() && *(src_iter + 1) == '=') {
                src_iter++;
                token.value = ">=";
                token.class_name = ClassName::GreatOrEq;
            }
            else {
                token.value = ">";
                token.class_name = ClassName::Great;
            }
        }
        else if (*src_iter == '~') {
            token.value = "~";
            token.class_name = ClassName::Not;
        }
        else if (*src_iter == '[') {
            token.value = "[";
            token.class_name = ClassName::LefSqBr;
        }
        else if (*src_iter == '(') {
            token.value = "(";
            token.class_name = ClassName::LefBr;
        }
        else if (*src_iter == '{') {
            token.value = "{";
            token.class_name = ClassName::LefFigBr;
        }
        else if (*src_iter == ']') {
            token.value = "]";
            token.class_name = ClassName::RightSqBr;
        }
        else if (*src_iter == ')') {
            token.value = ")";
            token.class_name = ClassName::RightBr;
        }
        else if (*src_iter == '}') {
            token.value = "}";
            token.class_name = ClassName::RightFigBr;
        }
        else if (*src_iter == '&') {
            token.value = "&";
            token.class_name = ClassName::And;
        }
        else if (*src_iter == ':') {
            if (src_iter + 1 != src.end() && *(src_iter + 1) == '=') {
                src_iter++;
                token.value = ":=";
                token.class_name = ClassName::Assign;
            }
            else {
                token.value = ":";
                token.class_name = ClassName::Colon;
            }
        }
        else if (*src_iter == ';') {
            token.value = ";";
            token.class_name = ClassName::Semicolon;
        }

        src_iter++;
        return token;
    }

    Token parseNumber() {
        Token token;
        string value = "";    

        // Skip zeros
        while (src_iter != src.end() && *src_iter == '0') { src_iter++; }

        while (true) {
            if (isDigit(*src_iter)) {
                value += *src_iter;
            }
            else if (*src_iter >= 'A' && *src_iter <= 'F') {
                while (src_iter != src.end() && isHexDigit(*src_iter)) {
                    value += *src_iter;
                    src_iter++;
                }

                if (src_iter == src.end() || (*src_iter != 'H' && *src_iter != 'X')) {
                    cerr << "Hex number should be ended with an 'H' or an 'X' symbol" << endl;
                    exit(1);
                }

                value += *src_iter;
                src_iter++;

                if (value.back() == 'H') {
                    token.class_name = ClassName::IntHex;
                }
                else if (value.back() == 'X') {
                    token.class_name = ClassName::StrHex;
                }
                token.value = value;
                return token;
            }
            else if (*src_iter == 'H') {
                value += *src_iter;
                src_iter++;

                token.class_name = ClassName::IntHex;
                token.value = value;
                return token;
            }
            else if (*src_iter == 'X') {
                value += *src_iter;
                src_iter++;

                token.class_name = ClassName::StrHex;
                token.value = value;
                return token;
            }
            else if (*src_iter == '.') {
                value += *src_iter;
                src_iter++;

                while (src_iter != src.end() && isDigit(*src_iter)) {
                    value += *src_iter;
                    src_iter++;
                }

                if (src_iter != src.end() && *src_iter == 'E') {
                    value += *src_iter;
                    src_iter++;

                    if (src_iter != src.end() && (*src_iter == '+' || *src_iter == '-')) {
                        value += *src_iter;
                        src_iter++;
                    }

                    if (src_iter == src.end() || !isDigit(*src_iter)) {
                        cerr << "An exponent notation should be ended with a number" << endl;
                        exit(1);
                    }

                    while (src_iter != src.end() && isDigit(*src_iter)) {
                        value += *src_iter;
                        src_iter++;
                    }
                }

                token.class_name = ClassName::Real;
                token.value = value;
                return token;
            }
            else {
                break;
            }

            src_iter++;
        }

        token.class_name = ClassName::IntDec;
        token.value = value;
        return token;
    }

    Token parseIdentifier() {
        Token token;
        string value = "";

        while (isLetter(*src_iter) || isDigit(*src_iter)) {
            value += *src_iter;
            src_iter++;
        }

        ClassName class_name = symbol_table.find(value);

        if (class_name == ClassName::None) {
            class_name = ClassName::Ident;
            symbol_table.insert(value, class_name);
        }

        token.class_name = class_name;
        token.value = value;

        return token;
    }
};


int main() {
    string src = "123 1A123H (* 123 *) 0123.E+10 0ABCD123X 228X \"Hey there\" 123 Abba LOLKEK123 A1B2 hello hello 1+2";

    Lexer lexer = Lexer(src);
    Token token = lexer.next();

    while (token.class_name != ClassName::None) {
        cout << "\n";
        cout << "Token " << token.class_name << " " << token.value << endl;
        token = lexer.next();
    }

    return 0;
}
