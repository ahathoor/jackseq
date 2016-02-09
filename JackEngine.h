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

#ifndef JACKENGINE_H
#define JACKENGINE_H

#include "IJackEngineCallbackProvider.h"
#include <jack/jack.h>

class JackEngine {
public:
    JackEngine(IJackEngineCallbackProvider *callbackProvider);
    int process(jack_nframes_t nframes);
    void init();
    void shutdown();
private:
    IJackEngineCallbackProvider *callbackProvider;
};

#endif /* JACKENGINE_H */

