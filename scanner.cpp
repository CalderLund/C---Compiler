//
// WLP4 Scanner
//

#include <string>
#include <vector>
#include <set>
#include <cstdint>
#include <ostream>
#include <sstream>
#include <iomanip>
#include <cctype>
#include <algorithm>
#include <iostream>
#include <utility>
#include <set>
#include <array>
#include <climits>


// DEFINITIONS

/*
 * Token class is used to store information about each token read.
 * - getKind: returns the kind of Token
 * - getLexeme: returns the initial string value read in for the Token
 * - getLong: returns the Long version of any ints or hexints read in
 */
class Token {
public:
    // Types of tokens
    enum Kind {
        ID = 0, NUM,
        LPAREN, RPAREN, LBRACE, RBRACE, LBRACK, RBRACK,
        BECOMES, EQ, NE, LT, GT, LE, GE,
        PLUS, MINUS, STAR, SLASH, PCT,
        COMMA, SEMI, AMP,
        WHITESPACE, COMMENT
    };

private:
    Kind kind;
    std::string lexeme;

public:
    Token(Kind kind, std::string lexeme);

    Kind getKind() const;
    const std::string &getLexeme() const;

    int64_t toLong() const;
};



/* An exception class thrown when an error is encountered while scanning.
 */
class ScanningFailure {
    std::string message;

public:
    explicit ScanningFailure(std::string message);

    // Returns the message associated with the exception.
    const std::string &what() const;
};



// IMPLEMENTATION

Token::Token(Token::Kind kind, std::string lexeme):
        kind(kind), lexeme(std::move(lexeme)) {}

Token:: Kind Token::getKind() const { return kind; }
const std::string &Token::getLexeme() const { return lexeme; }

std::ostream &operator<<(std::ostream &out, const Token &tok) {
    switch (tok.getKind()) {
        case Token::ID: {
            const std::string &lexeme = tok.getLexeme();
            if (lexeme == "return")
                out << "RETURN";
            else if (lexeme == "if")
                out << "IF";
            else if (lexeme == "else")
                out << "ELSE";
            else if (lexeme == "while")
                out << "WHILE";
            else if (lexeme == "println")
                out << "PRINTLN";
            else if (lexeme == "wain")
                out << "WAIN";
            else if (lexeme == "new")
                out << "NEW";
            else if (lexeme == "delete")
                out << "DELETE";
            else if (lexeme == "NULL")
                out << "NULL";
            else if (lexeme == "int")
                out << "INT";
            else
                out << "ID";
            break;
        }
        case Token::NUM:        out << "NUM";        break;
        case Token::LPAREN:     out << "LPAREN";     break;
        case Token::RPAREN:     out << "RPAREN";     break;
        case Token::LBRACE:     out << "LBRACE";     break;
        case Token::RBRACE:     out << "RBRACE";     break;
        case Token::LBRACK:     out << "LBRACK";     break;
        case Token::RBRACK:     out << "RBRACK";     break;
        case Token::BECOMES:    out << "BECOMES";    break;
        case Token::EQ:         out << "EQ";         break;
        case Token::NE:         out << "NE";         break;
        case Token::LT:         out << "LT";         break;
        case Token::GT:         out << "GT";         break;
        case Token::LE:         out << "LE";         break;
        case Token::GE:         out << "GE";         break;
        case Token::PLUS:       out << "PLUS";       break;
        case Token::MINUS:      out << "MINUS";      break;
        case Token::STAR:       out << "STAR";       break;
        case Token::SLASH:      out << "SLASH";      break;
        case Token::PCT:        out << "PCT";        break;
        case Token::COMMA:      out << "COMMA";      break;
        case Token::SEMI:       out << "SEMI";       break;
        case Token::AMP:        out << "AMP";        break;
        case Token::WHITESPACE: out << "WHITESPACE"; break;
        case Token::COMMENT:    out << "COMMENT";    break;
    }
    out << " " << tok.getLexeme();

    return out;
}


int64_t Token::toLong() const {
    std::istringstream iss;
    int64_t result;

    if (kind == NUM) {
        iss.str(lexeme);
    } else {
        // This should never happen if the user calls this function correctly
        return 0;
    }

    iss >> result;

    return result;
}


ScanningFailure::ScanningFailure(std::string message):
        message(std::move(message)) {}

const std::string &ScanningFailure::what() const { return message; }


// Represents a DFA to handle the scanning process.
class AsmDFA {
public:
    enum State {
        // States that are also kinds
        ID = 0,
        NUM,
        LPAREN,
        RPAREN,
        LBRACE,
        RBRACE,
        LBRACK,
        RBRACK,
        BECOMES,
        EQ,
        NE,
        LT,
        GT,
        LE,
        GE,
        PLUS,
        MINUS,
        STAR,
        SLASH,
        PCT,
        COMMA,
        SEMI,
        AMP,
        WHITESPACE,
        COMMENT,

        // States that are not also kinds
        FAIL,
        CANT,
        START,
        EXCLAM,
        ZERO,

        // Hack to let this be used easily in arrays. This should always be the
        // final element in the enum, and should always point to the previous
        // element.

        LARGEST_STATE = ZERO
    };

private:
    /* A set of all accepting states for the DFA.
     * Currently non-accepting states are not actually present anywhere
     * in memory, but a list can be found in the constructor.
     */
    std::set<State> acceptingStates;

    /*
     * The transition function for the DFA, stored as a map.
     */

    std::array<std::array<State, 128>, LARGEST_STATE + 1> transitionFunction;

    /*
     * Converts a state to a kind to allow construction of Tokens from States.
     * Throws an exception if conversion is not possible.
     */
    Token::Kind stateToKind(State s) const {
        switch(s) {
            case ID:         return Token::ID;
            case NUM:        return Token::NUM;
            case LPAREN:     return Token::LPAREN;
            case RPAREN:     return Token::RPAREN;
            case LBRACE:     return Token::LBRACE;
            case RBRACE:     return Token::RBRACE;
            case LBRACK:     return Token::LBRACK;
            case RBRACK:     return Token::RBRACK;
            case BECOMES:    return Token::BECOMES;
            case EQ:         return Token::EQ;
            case NE:         return Token::NE;
            case LT:         return Token::LT;
            case GT:         return Token::GT;
            case LE:         return Token::LE;
            case GE:         return Token::GE;
            case PLUS:       return Token::PLUS;
            case MINUS:      return Token::MINUS;
            case STAR:       return Token::STAR;
            case SLASH:      return Token::SLASH;
            case PCT:        return Token::PCT;
            case COMMA:      return Token::COMMA;
            case SEMI:       return Token::SEMI;
            case AMP:        return Token::AMP;
            case ZERO:       return Token::NUM;
            case WHITESPACE: return Token::WHITESPACE;
            case COMMENT:    return Token::COMMENT;
            case CANT: throw ScanningFailure("ERROR: Lexer error.");
            default: throw ScanningFailure("ERROR: Cannot convert state to kind.");
        }
    }


public:
    /* Tokenizes an input string according to the SMM algorithm.
     * You will learn the SMM algorithm in class around the time of Assignment 6.
     */
    std::vector<Token> simplifiedMaximalMunch(const std::string &input) const {
        std::vector<Token> result;

        State state = start();
        std::string munchedInput;

        // We can't use a range-based for loop effectively here
        // since the iterator doesn't always increment.
        for (std::string::const_iterator inputPosn = input.begin();
             inputPosn != input.end();) {

            State oldState = state;
            state = transition(state, *inputPosn);

            if (!failed(state)) {
                munchedInput += *inputPosn;
                oldState = state;

                ++inputPosn;
            }

            if (inputPosn == input.end() || failed(state)) {
                if (accept(oldState)) {
                    result.push_back(Token(stateToKind(oldState), munchedInput));

                    munchedInput = "";
                    state = start();
                } else {
                    if (failed(state)) {
                        munchedInput += *inputPosn;
                    }
                    throw ScanningFailure("ERROR: Simplified maximal munch failed on input: "
                                          + munchedInput);
                }
            }
        }

        return result;
    }

    /* Initializes the accepting states for the DFA.
     */
    AsmDFA() {
        acceptingStates = {ID, NUM, LPAREN, RPAREN, LBRACE, RBRACE, LBRACK, RBRACK, BECOMES,
                           EQ, NE, LT, GT, LE, GE, PLUS, MINUS, STAR, SLASH, PCT, COMMA,
                           SEMI, AMP, WHITESPACE, COMMENT, ZERO, CANT};

        // Initialize transitions for the DFA
        for (size_t i = 0; i < transitionFunction.size(); ++i) {
            for (size_t j = 0; j < transitionFunction[0].size(); ++j) {
                transitionFunction[i][j] = FAIL;
            }
        }

        
        // adds these transitions to the DFA
        registerTransition(START, isalpha, ID);
        registerTransition(ID, isalnum, ID);
        registerTransition(START, "0", ZERO);
        registerTransition(START, "123456789", NUM);
        registerTransition(NUM, isdigit, NUM);
        registerTransition(START, "-", MINUS);
        registerTransition(START, "+", PLUS);
        registerTransition(START, "*", STAR);
        registerTransition(START, "&", AMP);
        registerTransition(START, "=", BECOMES);
        registerTransition(BECOMES, "=", EQ);
        registerTransition(START, "!", EXCLAM);
        registerTransition(EXCLAM, "=", NE);
        registerTransition(START, "<", LT);
        registerTransition(START, ">", GT);
        registerTransition(LT, "=", LE);
        registerTransition(GT, "=", GE);
        registerTransition(EQ, "=<>!", CANT);
        registerTransition(NE, "=<>!", CANT);
        registerTransition(LE, "=<>!", CANT);
        registerTransition(GE, "=<>!", CANT);
        registerTransition(LT, "<>!", CANT);
        registerTransition(GT, "<>!", CANT);
        registerTransition(BECOMES, "<>!", CANT);
        registerTransition(START, "/", SLASH);
        registerTransition(START, "%", PCT);
        registerTransition(START, isspace, WHITESPACE);
        registerTransition(START, ",", COMMA);
        registerTransition(START, ";", SEMI);
        registerTransition(SLASH, "/", COMMENT);
        registerTransition(START, "(", LPAREN);
        registerTransition(START, ")", RPAREN);
        registerTransition(START, "{", LBRACE);
        registerTransition(START, "}", RBRACE);
        registerTransition(START, "[", LBRACK);
        registerTransition(START, "]", RBRACK);
        registerTransition(COMMENT, [](int c) -> int { return c != '\n'; },
                           COMMENT);
        registerTransition(WHITESPACE, isspace, WHITESPACE);
        registerTransition(ZERO, isalnum, CANT);
        registerTransition(NUM, isalpha, CANT);
    }

    // Register a transition on all chars in chars
    void registerTransition(State oldState, const std::string &chars,
                            State newState) {
        for (char c : chars) {
            transitionFunction[oldState][c] = newState;
        }
    }

    // Register a transition on all chars matching test
    // For some reason the cctype functions all use ints, hence the function
    // argument type.
    void registerTransition(State oldState, int (*test)(int), State newState) {

        for (int c = 0; c < 128; ++c) {
            if (test(c)) {
                transitionFunction[oldState][c] = newState;
            }
        }
    }

    /* Returns the state corresponding to following a transition
     * from the given starting state on the given character,
     * or a special fail state if the transition does not exist.
     */
    State transition(State state, char nextChar) const {
        return transitionFunction[state][nextChar];
    }

    /* Checks whether the state returned by transition
     * corresponds to failure to transition.
     */
    bool failed(State state) const { return state == FAIL; }

    /* Checks whether the state returned by transition
     * is an accepting state.
     */
    bool accept(State state) const {
        return acceptingStates.count(state) > 0;
    }

    /* Returns the starting state of the DFA
     */
    State start() const { return START; }
};



std::vector<Token> scan(const std::string &input) {
    static AsmDFA theDFA;

    std::vector<Token> tokens = theDFA.simplifiedMaximalMunch(input);

    // We need to:
    // * Throw exceptions for WORD tokens whose lexemes aren't ".word".
    // * Remove WHITESPACE and COMMENT tokens entirely.

    std::vector<Token> newTokens;

    for (auto &token : tokens) {
        if (token.getKind() == Token::NUM) {
            int64_t num = token.toLong();
            if (num > INT_MAX || num < INT_MIN)
                throw ScanningFailure("ERROR: Numeric literal out of range");
            newTokens.push_back(token);
        }
        else if (token.getKind() != Token::WHITESPACE
            && token.getKind() != Token::Kind::COMMENT) {
            newTokens.push_back(token);
        }
    }

    return newTokens;
}


int main() {
    std::string line;

    try {
        std::ostringstream oss;
        while (getline(std::cin, line)) {
            // For example, just print the scanned tokens
            std::vector<Token> tokenLine = scan(line);
            for (auto &token : tokenLine) {
                oss << token << std::endl;
            }
        }
        std::string output = oss.str();
        std::cout << output;
    } catch (ScanningFailure &f) {
        std::cerr << f.what() << std::endl;

        return 1;
    }

    return 0;
}
