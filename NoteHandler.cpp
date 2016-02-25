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
    commands["stop"] = &NoteHandler::stop;
    commands["start"] = &NoteHandler::start;
    commands["seek"] = &NoteHandler::seek;
    commands["toggle_recording"] = &NoteHandler::toggle_recording;
    commands["toggle_rolling"] = &NoteHandler::toggle_rolling;
    commands["toggle_waiting"] = &NoteHandler::toggle_waiting;
    commands["toggle_passthrough"] = &NoteHandler::toggle_passthrough;
    commands["clear_notes"] = &NoteHandler::clear_notes;
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
    if(trigger_learning.first == command && trigger_learning.second == arg)
        trigger_learning.first = "";
}

void NoteHandler::JackEngineTickHandler(int nframes) {
    //execute and discard all queued commands
    for(auto cmd_pair : command_queue) {
        CALL_MEMBER_FN(*this, commands[cmd_pair.first])(cmd_pair.second);
        command_queue.clear();
    }

    if(!state.rolling) return;
    window_size = nframes;

    //add the proper notes from the notestore to the play queue
    for(auto it = store.lower_bound (state.internal_frame); it != store.end() && it->first < state.internal_frame+window_size; it++) {
        for (auto &note : it->second) {
            play_queue[it->first].push_back(note);
        }
    }
}

void NoteHandler::JackEngineNoteHandler(Note *note, int offset) {
    //If we are waiting for input, start rolling
    if(state.wait_for_input) {
        state.rolling = true;
        state.wait_for_input = false;
    }
    //put the incoming notes into the store if recording,
    //and straight to the playback queue if passthrough is enabled
    double timeForNote = state.internal_frame + offset;
    if (state.rolling && state.recording)
        store[timeForNote].push_back(note);
    else if (state.pass_through)
        play_queue[timeForNote].push_back(note);
}

void NoteHandler::JackEngineTriggerHandler(Note *note, int offset) {
    //remove the trigger that has been marked for unlearning, if present
    std::string a = trigger_unlearning.first;
    double b = trigger_unlearning.second;
    triggers.erase(std::remove_if(triggers.begin(), triggers.end(),
                           [a,b](trigger t) { return (t.command == a && t.arg == b); }), triggers.end());
    trigger_unlearning.first = "";
    //add all the triggered commands to the command queue
    for(trigger t : triggers) {
        if(t.note.channel == note->channel &&
                t.note.note == note->note &&
                t.note.type == note->type)
            this->sendCommand(t.command,t.arg);
    }
    //learn a new trigger, if there is a new trigger to learn
    if(commands.find(trigger_learning.first) != commands.end()) {
        trigger t = {trigger_learning.first, trigger_learning.second, Note(*note)};
        triggers.push_back(t);
        trigger_learning.first = "";
    }
}

void NoteHandler::JackEnginePlayFunctionHandler(void(*play_fn)(Note*, int)) {
    for(auto it = play_queue.begin(); it != play_queue.end(); it++) {
        for (auto &note : it->second) {
            play_fn(note, it->first - state.internal_frame);
        }
    }
    play_queue.clear();

    if(state.rolling)
        state.internal_frame += window_size;
}
