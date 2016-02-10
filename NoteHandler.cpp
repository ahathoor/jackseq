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

#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))

#include "NoteHandler.h"
#include <iostream>
#include <algorithm>

NoteHandler::NoteHandler() {
    state.pass_through = true;
    state.recording = true;
    state.rolling = true;

    commands["rewind"] = &NoteHandler::rewind;
    commands["stop"] = &NoteHandler::stop;
    commands["start"] = &NoteHandler::start;
    commands["seek"] = &NoteHandler::seek;

    TriggerLearn("seek", 0);
}

void NoteHandler::sendCommand(std::string command, double arg) {
    if(commands.find(command) != commands.end())
        command_queue.push_back(std::make_pair(command,arg));
}

void NoteHandler::TriggerLearn(std::string command, double arg) {
    if(commands.find(command) != commands.end())
        trigger_learning = std::make_pair(command, arg);
}

void NoteHandler::TriggerUnlearn(std::string command, double arg) {
    if(commands.find(command) != commands.end())
        trigger_unlearning = std::make_pair(command, arg);
}

void NoteHandler::JackEngineTickHandler(int nframes) {
    for(auto cmd_pair : command_queue) {
        CALL_MEMBER_FN(*this, commands[cmd_pair.first])(cmd_pair.second);
        command_queue.clear();
    }
    if((int)internal_frame % 48000 < 2)
        std::cout << internal_frame/48000 << "s" << std::endl;

    if(!state.rolling) return;
    internal_frame += nframes;
    window_size = nframes;

    for(auto it = store.lower_bound (internal_frame); it != store.end() && it->first < internal_frame+window_size; it++) {
        for (auto &note : it->second) {
            play_queue[it->first].push_back(note);
        }
    }
}

void NoteHandler::JackEngineNoteHandler(Note *note, int offset) {
    if(state.wait_for_input) {
        state.rolling = true;
        state.wait_for_input = false;
    }
    double timeForNote = internal_frame + offset;
    if (state.rolling && state.recording)
        store[timeForNote].push_back(note);
    if (state.pass_through)
        play_queue[timeForNote].push_back(note);
}

void NoteHandler::JackEngineTriggerHandler(Note *note, int offset) {
    std::string a = trigger_unlearning.first;
    double b = trigger_unlearning.second;
    triggers.erase(std::remove_if(triggers.begin(), triggers.end(),
                           [a,b](trigger t) { return (t.command == a && t.arg == b); }), triggers.end());

    for(trigger t : triggers) {
        if(t.note.channel == note->channel &&
                t.note.note == note->note &&
                t.note.type == note->type)
            this->sendCommand(t.command,t.arg);
    }

    if(commands.find(trigger_learning.first) != commands.end()) {
        trigger t = {trigger_learning.first, trigger_learning.second, Note(*note)};
        triggers.push_back(t);
        trigger_learning.first = "";
    }
}

void NoteHandler::JackEnginePlayFunctionHandler(void(*play_fn)(Note*, int)) {
    for(auto it = play_queue.begin(); it != play_queue.end(); it++) {
        for (auto &note : it->second) {
            play_fn(note, it->first - internal_frame);
        }
    }
    play_queue.clear();
}



