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
};

#endif /* NOTEHANDLER_H */

