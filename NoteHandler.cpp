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
#include "JackInterface.h"
#include <map>
#include <vector>
#include <iostream>


std::map<double, std::vector<Note*>> store;
void receive_tick(int nframes);
void receive_note(Note* note, int offset);
void receive_trig(Note* note, int offset);
void play_notes(void(*play_function)(Note*, int));

uint32_t internal_frame = 0;
int window_size = 0;

JackInterface ji = JackInterface(receive_tick, receive_note, receive_trig, play_notes);

NoteHandler::NoteHandler() {
    ji.init();
}

void receive_tick(int nframes) {
    internal_frame += nframes;
    window_size = nframes;
}

void receive_note(Note* note, int offset) {
    store[internal_frame + offset].push_back(note);
}

void receive_trig(Note* note, int offset) {
    std::cout << "triggered" << std::endl;
}

void play_notes(void(*play_function)(Note*, int)) {
    for(auto it = store.lower_bound (internal_frame); it != store.end() && it->first < internal_frame+window_size; it++) {
        for (auto &note : it->second) {
            play_function(note, it->first - internal_frame);
        }
    }
}



