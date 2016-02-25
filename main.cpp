/*
  Copyright (C) 2016 Mikko Tamminen

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

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
             if(ch == 'c') nh->sendCommand("clear_notes", 0);
             if(ch == 'p') nh->sendCommand("toggle_passthrough", 0);

             if(ch == 'l') nh->TriggerLearn("seek", 0);
             if(ch == 'u') nh->TriggerUnlearn("seek", 0);
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


        addstr("frame: ");
        addstr(std::to_string(nh->state.internal_frame).c_str());
        addstr("\n");
        addstr("event count: ");
        addstr(std::to_string(nh->event_count()).c_str());
        addstr("\n");
        addstr("learning: ");
        addstr(nh->learning_status().c_str());
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
        addstr(nh->state.pass_through ? "[x]" : "[ ]");
        addstr("PASS_THROUGH");

        addstr("\n");
        addstr("\n");
        addstr("(q) quit (r) Toggle recording (w) toggle wait_for_input");

        addstr("\n");
        addstr("(s) stop/play (a) rewind (c) clear notes (p) toggle passthrough");

        addstr("\n");
        addstr("\n");
        addstr("(l) learn stop/play trigger (u) unlearn stop/play trigger");

        std::this_thread::sleep_for(std::chrono::milliseconds(40));
    }
}
