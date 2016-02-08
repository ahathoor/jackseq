/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Note.cpp
 * Author: Mikko Tamminen
 * 
 * Represents a single MIDI note on an abstract timeline
 * 
 * Created on February 7, 2016, 2:05 AM
 */

#include "Note.h"
#include <iostream>

Note::Note(int channel, int type, int note, int velocity) {
    this->id =  next_id++;
    this->channel = channel;
    this->type = type;
    this->note = note;
    this->velocity = velocity;
}
Note::Note(unsigned char* buffer) {
    this->id =  next_id++;

    this->channel = buffer[0] & 15;
    this->type = buffer[0] & 240;
    this->note = buffer[1];
    this->velocity = buffer[2];
}

int Note::is_noteon() {
    return this->type == 144;
}

uint32_t Note::next_id = 0;

void Note::print_note() {
    std::cout << "A note: " << this->note << std::endl;
}
