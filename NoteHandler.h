/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   NoteStore.h
 * Author: mothi
 *
 * Created on February 7, 2016, 2:25 AM
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
    void sendCommand(std::string command);
private:
    NoteHandlerState state;
    uint32_t internal_frame = 0;
    int window_size = 0;

    std::map<double, std::vector<Note*>> store;
    std::map<double, std::vector<Note*>> play_queue;
    std::map<std::string, void(NoteHandler::*)()> commands;
    std::vector<std::string> command_queue;

    void rewind() {internal_frame = 0;};
    void seek(int where) {internal_frame = where;};
    void stop() { state.rolling = false;};
    void start() { state.rolling = true;};
};

#endif /* NOTEHANDLER_H */

