/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   JackEngine.cpp
 * Author: mothi
 * 
 * Created on February 7, 2016, 2:46 AM
 */

#include "Note.h"
#include "JackEngine.h"
#include <stdio.h>
#include <iostream>
#include <jack/jack.h>
#include <jack/midiport.h>
#include "IJackEngineCallbackProvider.h"

void* outbuf;
void* inbuf;
void* trigbuf;

jack_client_t *client;
jack_port_t *input_port;
jack_port_t *output_port;
jack_port_t *trigger_port;

int process(jack_nframes_t nframes, void* arg);

void play_fn(Note* note, int offset) {
    jack_midi_data_t data[] = {(unsigned char)(note->channel | note->type),(unsigned char)note->note, (unsigned char)note->velocity};
    jack_midi_event_write(outbuf, offset, data, 3);
}

JackEngine::JackEngine(IJackEngineCallbackProvider *callbackProvider) {
    this->callbackProvider = callbackProvider;
}

int Process_Callback(jack_nframes_t x, void* p)
{
    return static_cast<JackEngine*>(p)->process(x);
}

void JackEngine::init() {
    std::cout << "Connecting to Jack..." << std::endl;
    const char *client_name = "JackSeq";
    const char *server_name = NULL;
    jack_options_t options = JackNullOption;
    jack_status_t status;

    /* open a client connection to the JACK server */
    
    client = jack_client_open(client_name, options, &status, server_name);

    if (client == NULL) {
        std::cout << "Jack connection failed :(" << std::endl;
        exit(1);
    }
    if (status & JackNameNotUnique) {
        client_name = jack_get_client_name(client);
    }
    std::cout << "Client name: " << client_name << std::endl;

    jack_set_process_callback(client, Process_Callback, this);
    
    /* create the ports */

    input_port = jack_port_register(client, "MIDI IN",
            JACK_DEFAULT_MIDI_TYPE,
            JackPortIsInput, 0);
    output_port = jack_port_register(client, "MIDI OUT",
            JACK_DEFAULT_MIDI_TYPE,
            JackPortIsOutput, 0);
    trigger_port = jack_port_register(client, "TRIGGER",
            JACK_DEFAULT_MIDI_TYPE,
            JackPortIsInput, 0);

    if ((input_port == NULL) || (output_port == NULL) || (trigger_port == NULL)) {
        std::cout << "No Jack Ports available" << std::endl;
        exit(1);
    }
    
    if (jack_activate(client)) {
        std::cout << "Cannot activate client" << std::endl;
        exit(1);
    }
    std::cout << "Connected!" << client_name << std::endl;
}

void JackEngine::shutdown() {
    jack_client_close(client);
}


int JackEngine::process(jack_nframes_t nframes) {
    this->callbackProvider->JackEngineTickHandler(nframes);

    inbuf = jack_port_get_buffer(input_port, nframes);
    for (uint32_t i = 0; i < jack_midi_get_event_count(inbuf); i++) {
        jack_midi_event_t event;
        jack_midi_event_get(&event, inbuf, i);
        Note *n = new Note(event.buffer);
        this->callbackProvider->JackEngineNoteHandler(n, event.time);
    }

    outbuf = jack_port_get_buffer(output_port, nframes);
    jack_midi_clear_buffer(outbuf);
    this->callbackProvider->JackEnginePlayFunctionHandler(play_fn);

    trigbuf = jack_port_get_buffer(trigger_port, nframes);
    for (uint32_t i = 0; i < jack_midi_get_event_count(trigbuf); i++) {
        jack_midi_event_t event;
        jack_midi_event_get(&event, inbuf, i);
        Note *n = new Note(event.buffer);
        this->callbackProvider->JackEngineTriggerHandler(n, event.time);
    }

    return 0;
}
