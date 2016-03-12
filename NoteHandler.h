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

#ifndef NOTEHANDLER_H
#define NOTEHANDLER_H

#include "Note.h"
#include "IJackEngineCallbackProvider.h"
#include <string>
#include <map>
#include <vector>

typedef struct {
    bool pass_through = true;
    bool recording = true;
    bool rolling = false;
    bool wait_for_input = true;
    double internal_frame = 0;
} NoteHandlerState;

class NoteHandler : public IJackEngineCallbackProvider{
public:
    NoteHandler();
    ~NoteHandler() {};
    void JackEngineTickHandler(int nframes);
    void JackEngineNoteHandler(Note*, int offset);
    void JackEngineTriggerHandler(Note*, int offset);
    void JackEnginePlayFunctionHandler(std::function<void(Note*,int)> play_fn);
    void JackEnginePostTickHandler();
    void sendCommand(std::string command, double arg);
    void TriggerLearn(std::string command, double arg);
    void TriggerUnlearn(std::string command, double arg);

    std::string learning_status() { return (trigger_learning.first.length() > 0) ? (trigger_learning.first + " " + std::to_string(trigger_learning.second)) : "none" ;}

    void Save(std::string filename);
    void Open(std::string filename);
    bool isRolling() { return state.rolling; }
    bool isRecording() { return state.recording; }
    bool isPassingThrough() { return state.pass_through; }
    bool isWaitingForInput() { return state.wait_for_input; }
    double current_frame() { return state.internal_frame; }
    int event_count() { return store.size();}

private:
    int tick_size = 0;
    NoteHandlerState state;

    struct trigger{
        std::string command;
        double arg;
        Note note;
    };

    std::map<double, std::vector<Note*>> store;
    std::map<double, std::vector<Note*>> play_queue;
    std::map<std::string, void(NoteHandler::*)(double arg)> commands;
    std::vector<std::pair<std::string, double>> command_queue;
    std::vector<trigger> triggers;
    std::pair<std::string, double> trigger_learning;
    std::pair<std::string, double> trigger_unlearning;

    void seek(double where) {state.internal_frame = where;}
    void stop(double arg) { state.rolling = false;}
    void start(double arg) { state.rolling = true;}
    void set_recording(double arg) { state.recording = (arg != 0);}
    void set_rolling(double arg) { state.rolling = (arg != 0);}
    void set_waiting(double arg) { state.wait_for_input = (arg != 0);}
    void set_passthrough(double arg) { state.pass_through = (arg != 0);}
    void clear_notes(double arg) { store.clear() ;}
};

#endif /* NOTEHANDLER_H */

