/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Note.h
 * Author: mothi
 *
 * Created on February 7, 2016, 2:05 AM
 */

#ifndef NOTE_H
#define NOTE_H

#include <stdint.h>

class Note {
public:
    Note(int channel, int type, int note, int velocity);
    Note(unsigned char* event);
    int is_noteon();
    void print_note();
    int id;
    int channel;
    int type;
    int note;
    int velocity;
private:
    static uint32_t next_id;
};

#endif /* NOTE_H */

