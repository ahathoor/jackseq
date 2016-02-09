/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   JackInterface.h
 * Author: mothi
 *
 * Created on February 7, 2016, 2:46 AM
 */

#ifndef JACKINTERFACE_H
#define JACKINTERFACE_H

typedef void (*TickFn)(int nframes);
typedef void (*NoteFn)(Note*, int offset);
typedef void (*TrigFn)(Note*,  int offset);
typedef void (*PlayFn)(void (*)(Note*, int offset));

class JackInterface {
public:
    JackInterface(TickFn tick_fn,
                  void (*note_fn)(Note*, int offset),
                  void (*trig_fn)(Note*,  int offset),
                  void (*play_fn)(void (*)(Note*, int offset))
    );
    void init();
    void shutdown();
private:

};

#endif /* JACKINTERFACE_H */

