/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   NoteStore.cpp
 * Author: mothi
 * 
 * Created on February 7, 2016, 2:25 AM
 */
#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))

#include "NoteHandler.h"
#include <iostream>

void receive_tick(int nframes);
void receive_note(Note* note, int offset);
void receive_trig(Note* note, int offset);
void play_notes(void(*play_function)(Note*, int));
uint32_t internal_frame = 0;
int window_size = 0;

NoteHandler::NoteHandler() {
    state.pass_through = true;
    state.recording = true;
    state.rolling = true;

    commands["rewind"] = &NoteHandler::rewind;
    commands["stop"] = &NoteHandler::stop;
}

void NoteHandler::sendCommand(std::string command) {
    if(commands.find(command) != commands.end())
        command_queue.push_back(command);
}

void NoteHandler::JackEngineTickHandler(int nframes) {
    for(auto cmd_name : command_queue) {
        CALL_MEMBER_FN(*this, commands[cmd_name])();
        command_queue.clear();
    }

    if(!state.rolling) return;
    internal_frame += nframes;
    window_size = nframes;

    for(auto it = store.lower_bound (internal_frame); it != store.end() && it->first < internal_frame+window_size; it++) {
        for (auto &note : it->second) {
            play_queue[it->first].push_back(note);
        }
    }

    if(internal_frame > 48000) {
        sendCommand("rewind");
    }
}

void NoteHandler::JackEngineNoteHandler(Note *note, int offset) {
    double timeForNote = internal_frame + offset;
    if (state.recording)
        store[timeForNote].push_back(note);
    if (state.pass_through)
        play_queue[timeForNote].push_back(note);
}

void NoteHandler::JackEngineTriggerHandler(Note *note, int offset) {
    std::cout << "triggered" << std::endl;
}

void NoteHandler::JackEnginePlayFunctionHandler(void(*play_fn)(Note*, int)) {
    for(auto it = play_queue.begin(); it != play_queue.end(); it++) {
        for (auto &note : it->second) {
            play_fn(note, it->first - internal_frame);
        }
    }
    play_queue.clear();
}



