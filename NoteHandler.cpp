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

#include "NoteHandler.h"
#include <map>
#include <vector>
#include <iostream>
#include <string>

NoteHandlerState state;

void receive_tick(int nframes);
void receive_note(Note* note, int offset);
void receive_trig(Note* note, int offset);
void play_notes(void(*play_function)(Note*, int));

uint32_t internal_frame = 0;
int window_size = 0;

std::map<double, std::vector<Note*>> store;
std::map<double, std::vector<Note*>> play_queue;
std::map<std::string, void(*)()> commands;
std::vector<std::string> command_queue;

void rewind() { internal_frame = 0; }
void stop() { state.rolling = false; }
void start() { state.rolling = true; }

NoteHandler::NoteHandler() {
    state.pass_through = true;
    state.recording = true;
    state.rolling = true;

    commands["rewind"] = rewind;
    commands["stop"] = stop;
    commands["start"] = start;
}

void sendCommand(std::string command) {
    if(commands.find(command) != commands.end())
        command_queue.push_back(command);
}

void NoteHandler::JackEngineTickHandler(int nframes) {
    for(auto cmd_name : command_queue) {
        commands[cmd_name]();
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



