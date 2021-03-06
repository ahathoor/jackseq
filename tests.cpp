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

int save_and_load_trigger() {
    bool passed;
    NoteHandler *nh = new NoteHandler();
    nh->TriggerLearn("seek", 123);
    nh->JackEngineTriggerHandler(new Note(1,2,3,4), 0);
    std::string tmpfilename = std::tmpnam(nullptr);
    nh->Save(tmpfilename);
    delete nh;

    NoteHandler *nh2 = new NoteHandler();
    nh2->Open(tmpfilename);
    nh2->JackEngineTriggerHandler(new Note(1,2,3,4), 0);
    nh2->JackEngineTickHandler(0);
    passed = nh2->current_frame() == 123;
    std::remove(tmpfilename.c_str());
    delete nh2;
    return passed;
}

int state_changing_commands_test() {
    bool passed = false;
    NoteHandler *nh = new NoteHandler();
    nh->sendCommand("set_rolling", 1);
    nh->sendCommand("set_recording", 1);
    nh->sendCommand("set_waiting", 1);
    nh->sendCommand("set_passthrough", 1);
    nh->JackEngineTickHandler(0);
    passed = (nh->isRolling() && nh->isRecording() && nh->isWaitingForInput() && nh->isPassingThrough());
    nh->sendCommand("set_rolling", 0);
    nh->sendCommand("set_recording", 0);
    nh->sendCommand("set_waiting", 0);
    nh->sendCommand("set_passthrough", 0);
    nh->JackEngineTickHandler(0);
    passed = passed && (!nh->isRolling() && !nh->isRecording() && !nh->isWaitingForInput() && !nh->isPassingThrough());
    return passed;
}

int tick_test() {
    bool passed = false;
    NoteHandler *nh = new NoteHandler();
    nh->sendCommand("set_rolling", 1);
    nh->JackEngineTickHandler(100);
    nh->JackEnginePostTickHandler();
    passed = (nh->current_frame() == 100);
    nh->JackEngineTickHandler(9999);
    nh->JackEnginePostTickHandler();
    passed = passed && (nh->current_frame() == 100 + 9999);
    nh->sendCommand("set_rolling", 0);
    nh->JackEngineTickHandler(777);
    nh->JackEnginePostTickHandler();
    passed = passed && (nh->current_frame() == 100 + 9999);
    return passed;
}

int save_and_load_note() {
    NoteHandler *nh = new NoteHandler();
    Note *n = new Note(1,2,3,4);
    bool passed;
    std::string tmpfilename = std::tmpnam(nullptr);
    nh->sendCommand("set_rolling", 1);
    nh->JackEngineNoteHandler(n,0);
    nh->JackEngineTickHandler(100);
    nh->Save(tmpfilename);
    nh->Open(tmpfilename);
    nh->sendCommand("seek", 0);
    nh->JackEngineTickHandler(100);
    nh->JackEnginePlayFunctionHandler( [&] (Note* n, int o) -> void {
            passed = (o==0 && n->channel == 1 && n->type == 2 && n->note == 3 && n->velocity == 4);
    });
    NoteHandler *nh2 = new NoteHandler();
    nh2->sendCommand("set_rolling", 1);
    nh2->Open(tmpfilename);
    nh2->JackEngineTickHandler(100);
    nh2->JackEnginePlayFunctionHandler( [&] (Note* n, int o) -> void {
            passed = passed && (o==0 && n->channel == 1 && n->type == 2 && n->note == 3 && n->velocity == 4);
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
            runTest(save_and_load_trigger, "Save and load a trigger to file") &&
            runTest(tick_test, "Tick the nh forwards and see that it keeps the right time") &&
            runTest(state_changing_commands_test, "Test the commands that affect the player state")
    )
    {
        std::cout << "\033[1;32m" << "ALL TESTS PASSED" << "\033[0m" << std::endl;
        return 0;
    } else {
        std::cout << "\033[1;31m" << "TESTS FAILED" << "\033[0m" << std::endl;
        return -1;
    }
}
