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

class JackInterface {
public:
    JackInterface(void (*tick_fn)(int frames),
                  void (*note_fn)(Note*, int offset),
                  void (*trig_fn)(Note*,  int offset),
                  void (*play_fn)(void (*)(Note*, int offset))
    );
    void init();
    void shutdown();
private:

};

#endif /* JACKINTERFACE_H */

