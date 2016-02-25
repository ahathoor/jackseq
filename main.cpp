#include <iostream>
#include "Note.h"
#include "NoteHandler.h"
#include "JackEngine.h"
#include <curses.h>
#include <chrono>
#include <thread>
#include <string>

NoteHandler *nh = new NoteHandler();
JackEngine *je = new JackEngine(nh);
void draw();

int main(int argc, char *argv[])
{

    je->init();
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    std::thread dt(draw);

    int ch;
    nodelay(stdscr, TRUE);
    for (;;) {
         if ((ch = getch()) == ERR) {
             /* user hasn't responded
              ...
             */
         }
         else {
             if(ch == 'q') break;
             if(ch == 'r') nh->sendCommand("toggle_recording", 0);
             if(ch == 'w') nh->sendCommand("toggle_waiting", 0);
             if(ch == 's') nh->sendCommand("toggle_rolling", 0);
             if(ch == 'a') nh->sendCommand("seek", 0);

             if(ch == 'i') nh->TriggerLearn("toggle_rolling", 0);
             if(ch == 'o') nh->TriggerLearn("seek", 0);
             if(ch == 'p') nh->TriggerLearn("toggle_recording", 0);
         }
    }

    endwin();
    return 0;
}

void draw(){
    for(;;) {
        move(0,0);
        addstr(je->getName());
        addstr("\n");
        addstr("=======");
        addstr("\n");
        addstr("\n");

        addstr(std::to_string(nh->state.internal_frame).c_str());
        addstr("\n");
        addstr("\n");

        addstr(nh->state.recording ? "[x]" : "[ ]");
        addstr("RECORDING");

        addstr("\n");
        addstr(nh->state.rolling ? "[x]" : "[ ]");
        addstr("ROLLING");

        addstr("\n");
        addstr(nh->state.wait_for_input ? "[x]" : "[ ]");
        addstr("WAITING_FOR_INPUT");

        addstr("\n");
        addstr("\n");
        addstr("(q) quit (r) Toggle recording (w) toggle wait_for_input");

        addstr("\n");
        addstr("(s) stop/play (a) rewind");

        addstr("\n");
        addstr("\n");
        addstr("(i) MIDI learn stop/play (o) MIDI learn rewind (p) MIDI learn recording");

        std::this_thread::sleep_for(std::chrono::milliseconds(40));
    }
}
