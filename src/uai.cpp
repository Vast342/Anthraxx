#include "global_includes.h"
#include "lookups.h"
#include "tests.h"
#include "search.h"

int hardBoundDivisor = 2;
int softBoundFractionNumerator = 3;
int softBoundFractionDenominator = 4;
double softBoundMultiplier = 0.6;
int defaultMovesToGo = 20;

Board board("x5o/7/7/7/7/7/o5x x 0 1");

// loads a position, either startpos or a fen string
void loadPosition(const std::vector<std::string>& bits) {
    if(bits[1] == "startpos") {
        board = Board("x5o/7/7/7/7/7/o5x x 0 1");
        for(int i = 3; i < static_cast<int>(bits.size()); i++) {
            board.makeMove(Move(bits[i]));
        }
    } else if(bits[1] == "fen") {
        board = Board(bits[2] + " " + bits[3] + " " + bits[4] + " " + bits[5]);
        for(int i = 6; i < static_cast<int>(bits.size()); i++) {
            board.makeMove(Move(bits[i]));
        }
    } else {
        std::cout << "invalid position command\n";
    }
}

// has the engine identify itself when the GUI says uai
void identify() {
    std::cout << "id name Claritaxx " << Version << '\n';
    std::cout << "id author Vast\n";
    std::cout << "uaiok" << std::endl;
}

// tells the engine to search
void go(const std::vector<std::string> &bits) {
    int time = 0;
    int inc = 0;
    int movestogo = defaultMovesToGo;
    for(int i = 1; i < std::ssize(bits); i+=2) {
        if(bits[i] == "wtime" && board.getColorToMove() == 1) {
            time = std::stoi(bits[i+1]);
        }
        if(bits[i] == "btime" && board.getColorToMove() == 0) {
            time = std::stoi(bits[i+1]);
        }
        if(bits[i] == "winc" && board.getColorToMove() == 1) {
            inc = std::stoi(bits[i+1]);
        }
        if(bits[i] == "binc" && board.getColorToMove() == 0) {
            inc = std::stoi(bits[i+1]);
        }
    }
    Move bestMove;
    // go wtime x btime x
    // the formulas here are former formulas from Stormphrax, so this means that they are adapted to Chess timing, and may not be the best for Ataxx
    const int softBound = softBoundMultiplier * (time / movestogo + inc * softBoundFractionNumerator / softBoundFractionDenominator);
    const int hardBound = time / hardBoundDivisor;
    bestMove = think(board, softBound, hardBound);
    std::cout << "bestmove " << bestMove.toLongAlgebraic() << '\n';
}

// interprets the command
void interpretCommand(const std::string command) {
    const std::vector<std::string> bits = split(command, ' ');

    if(bits.empty()) {
        return;
    } else if(bits[0] == "position") {
        loadPosition(bits);
    } else if(bits[0] == "isready") {
        std::cout << "readyok\n";
    } else if(bits[0] == "uai") {
        identify();
    } else if(bits[0] == "go") {
        go(bits);
    } else if(bits[0] == "perft") {
        runPerftTest(board, std::stoi(bits[1]));    
    } else if(bits[0] == "splitperft") {
        runSplitPerft(board, std::stoi(bits[1]));
    } else if(bits[0] == "makemove") {
        board.makeMove(Move(bits[1]));
    } else if(bits[0] == "uainewgame") {
        // I just added this so it wouldn't say it doesn't support uainewgame, it'll do something eventually  
    } else if(bits[0] == "printstate") {
        board.toString();
    } else if(bits[0] == "getfen") {
        std::cout << board.getFen() << std::endl;  
    } else if(bits[0] == "perftsuite") {
        runPerftSuite();  
    } else {
        std::cout << "invalid or unsupported command\n";
    }
}

int main() {
    std::cout << std::boolalpha;
    std::string command;
    while(true) {
        std::getline(std::cin, command, '\n');
        if(command == "quit") {
            return 0;
        }
        interpretCommand(command);
    }
    return 0;
}
