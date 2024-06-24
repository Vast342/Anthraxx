/*
    Anthraxx
    Copyright (C) 2024 Joseph Pasfield

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "global_includes.h"
#include "lookups.h"
#include "tests.h"
#include "search.h"
#include "tt.h"

TT tt;
Engine engine(&tt);
int threadCount = 1;
Board board("x5o/7/7/7/7/7/o5x x 0 1");

// resets everything
void newGame() {
    //engine.newGame();
    board = Board("x5o/7/7/7/7/7/o5x x 0 1");
    tt.clearTable();
}

void runBench(int depth = 7) {
    int total = 0;
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    for(const std::string &fen : benchPositions) {
        newGame();
        Board board(fen);
        total += engine.benchSearch(board, depth);
    }
    const auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count();
    std::cout << total << " nodes " << std::to_string(int(total / (double(elapsedTime) / 1000))) << " nps" << std::endl;
}

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
    std::cout << "option name Hash type spin default 64 min 1 max 2048" << std::endl;
    std::cout << "option name Threads type spin default 1 min 1 max 2048" << std::endl;
    std::cout << "uaiok" << std::endl;
}

// tells the engine to search
void go(const std::vector<std::string> &bits) {
    int time = 0;
    int inc = 0;
    int movestogo = 20;
    int depth = 0;
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
        if(bits[i] == "depth") {
            depth = std::stoi(bits[i+1]);
        }
    }
    if(depth != 0) {
        engine.fixedDepthSearch(board, depth, true);
    } else if(time != 0) {
        // go wtime x btime x
        // the formulas here are former formulas from Stormphrax, so this means that they are adapted to Chess timing, and may not be the best for Ataxx
        const int softBound = 0.6 * (time / movestogo + inc * 3.0 / 4.0);
        const int hardBound = time / 2;
        engine.think(board, softBound, hardBound, true);
    } else {
        std::cout << "Invalid arguments" << std::endl;
    }
}

// sets options, though currently just the hash size
void setOption(const std::vector<std::string>& bits) {
    std::string name = bits[2];
    if(name == "Hash") {
        int newSizeMB = std::stoi(bits[4]);
        int newSizeB = newSizeMB * 1024 * 1024;
        // this should be 16 bytes
        int entrySizeB = sizeof(Transposition);
        assert(entrySizeB == 16); 
        int newSizeEntries = newSizeB / entrySizeB;
        //std::cout << log2(newSizeEntries);
        tt.resize(newSizeEntries);
    } else if(name == "Threads") {
        threadCount = std::stoi(bits[4]);
        newGame();
    }
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
        newGame();
    } else if(bits[0] == "printstate") {
        board.toString();
    } else if(bits[0] == "getfen") {
        std::cout << board.getFen() << std::endl;  
    } else if(bits[0] == "perftsuite") {
        runPerftSuite();  
    } else if(bits[0] == "setoption") {
        setOption(bits);
    } else if(bits[0] == "bench") {
        if(bits.size() == 1) {
            runBench();
        } else {
            runBench(std::stoi(bits[1]));
        }
    } else {
        std::cout << "invalid or unsupported command\n";
    }
}

int main(int argc, char* argv[]) {
    initializeZobrist();
    newGame();
    std::cout << std::boolalpha;
    if(argc > 1 && std::string(argv[1]) == "bench") {
        runBench();
        return 0;
    }
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
