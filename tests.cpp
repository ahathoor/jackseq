#include "NoteHandler.h"
#include "Note.h"
#include <iostream>
#include <fstream>
#include <functional>

void p(std::string t) {
    std::cout << t;
}

void print(Note* n, int offset) {
    std::cout << n->to_string() << std::endl;
}

int tick_test() {
    bool passed = false;
    NoteHandler *nh = new NoteHandler();
    nh->state.rolling = true;
    nh->JackEngineTickHandler(100);
    nh->JackEnginePostTickHandler();
    passed = (nh->state.internal_frame == 100);
    nh->JackEngineTickHandler(9999);
    nh->JackEnginePostTickHandler();
    passed = passed && (nh->state.internal_frame == 100 + 9999);
    nh->state.rolling = false;
    nh->JackEngineTickHandler(777);
    nh->JackEnginePostTickHandler();
    passed = passed && (nh->state.internal_frame == 100 + 9999);
    return passed;
}

int save_and_load_note() {
    NoteHandler *nh = new NoteHandler();
    Note *n = new Note(1,2,3,4);
    bool passed;
    std::string tmpfilename = std::tmpnam(nullptr);
    nh->JackEngineNoteHandler(n,0);
    nh->JackEngineTickHandler(100);
    nh->Save(tmpfilename);
    nh->Open(tmpfilename);
    nh->sendCommand("seek", 0);
    nh->JackEngineTickHandler(100);
    nh->JackEnginePlayFunctionHandler( [&] (Note* n, int o) -> void {
            passed = (o==0 && n->channel == 1 && n->type == 2 && n->note == 3 && n->velocity == 4);
    });
    std::remove(tmpfilename.c_str());
    return passed;
}

int add_one_and_play() {
    NoteHandler *nh = new NoteHandler();
    Note *n = new Note(1,2,3,4);
    bool passed;
    nh->JackEngineNoteHandler(n,0);
    nh->JackEngineTickHandler(100);
    std::function<void(Note*,int)> play_fn = [&](Note* n, int o ) -> void { passed = ( o==0 && n->channel == 1 && n->type == 2 && n->note == 3 && n->velocity == 4 ); };
    nh->JackEnginePlayFunctionHandler( play_fn );
    return passed;
}

int add_several_and_play() {
    NoteHandler *nh = new NoteHandler();
    Note *n0 = new Note(1,2,3,4);
    Note *n1 = new Note(11,12,13,14);
    Note *n2 = new Note(21,22,23,24);
    Note *bogus = new Note(0,0,0,0);
    bool passed = false;
    int round = 0;
    //add notes in nonlinear order
    nh->JackEngineNoteHandler(n0,2);
    nh->JackEngineNoteHandler(n1,99);
    nh->JackEngineNoteHandler(n2,3);
    //these should not play
    nh->JackEngineNoteHandler(bogus,100);
    nh->JackEngineNoteHandler(bogus,999999);
    nh->JackEngineNoteHandler(bogus,-1);

    nh->JackEngineTickHandler(100);
    std::function<void(Note*,int)> play_fn = [&](Note* n, int o ) -> void {
        switch(round) {
            case 0: passed = (o==2 && n->channel == 1 && n->type == 2 && n->note == 3 && n->velocity == 4);
                    round++;
                    break;
            case 1: passed = (o==3 && n->channel == 21 && n->type == 22 && n->note == 23 && n->velocity == 24) && passed;
                    round++;
                    break;
            case 2: passed = (o==99 && n->channel == 11 && n->type == 12 && n->note == 13 && n->velocity == 14) && passed;
                    round++;
                    break;
            default:
                    passed = false;

        }
    };
    nh->JackEnginePlayFunctionHandler( play_fn );
    return passed;
}

int runTest(int (test_fn)(), std::string description) {
    std::cout << description <<  " ";
    if (test_fn()) {
        std::cout << "\033[1;32m" << "OK" << "\033[0m" << std::endl;
        return 1;
    }
    else {
        std::cout << "\033[1;31m" << "FAILED" << "\033[0m" << std::endl;
        return 0;
    }
}

int main(int argc, char *argv[])
{

    std::cout << "\033[1m" << "Running tests" << "\033[0m" << std::endl;


    if(
            runTest(add_one_and_play, "Add a note and play immediately") &&
            runTest(add_several_and_play, "Add several notes and play immediately") &&
            runTest(save_and_load_note, "Save and load note to file") &&
            runTest(tick_test, "Tick the nh forwards and see that it keeps the right time")
    )
    {
        std::cout << "\033[1;32m" << "ALL TESTS PASSED" << "\033[0m" << std::endl;
        return 0;
    } else {
        std::cout << "\033[1;31m" << "TESTS FAILED" << "\033[0m" << std::endl;
        return -1;
    }
}
