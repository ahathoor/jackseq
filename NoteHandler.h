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
    bool rolling = true;
} NoteHandlerState;

class NoteHandler : public IJackEngineCallbackProvider{
public:
    NoteHandler();
    ~NoteHandler() {};
    void JackEngineTickHandler(int nframes);
    void JackEngineNoteHandler(Note*, int offset);
    void JackEngineTriggerHandler(Note*, int offset);
    void JackEnginePlayFunctionHandler(void (*play_fn)(Note*, int offset));
    void sendCommand(std::string command, int arg);
private:
    NoteHandlerState state;
    double internal_frame = 0;
    int window_size = 0;

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

    void rewind(double arg) {internal_frame = 0;};
    void seek(double where) {internal_frame = where;};
    void stop(double arg) { state.rolling = false;};
    void start(double arg) { state.rolling = true;};
};

#endif /* NOTEHANDLER_H */

