#include <iostream>
#include <string>

using namespace std;


class Lexer {
    string source;
    string::iterator source_iter;

    string token;

public:
    Lexer(string source) {
        this->source = source;
        this->source_iter = source.begin();
    }

    string next() {
        token = "";

        while (source_iter != source.end()) {
            if (*source_iter >= '0' && *source_iter <= '9') {
            }
            source_iter++;
        }

        return token;
    }
};


int main() {
    string source = "";

    Lexer lexer = Lexer(source);

    string token;
    while ((token = lexer.next()) != "") {
        cout << "Token " << token << endl;
    }

    return 0;
}
