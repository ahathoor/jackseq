/*
  Copyright (C) 2016 Mikko Tamminen

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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
const char *client_name;
const char *server_name;

int process(jack_nframes_t nframes, void* arg);

const char* JackEngine::getName() {return client_name;}

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
    client_name = "JackSeq";
    server_name = NULL;
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


    inbuf = jack_port_get_buffer(input_port, nframes);
    for (uint32_t i = 0; i < jack_midi_get_event_count(inbuf); i++) {
        jack_midi_event_t event;
        jack_midi_event_get(&event, inbuf, i);
        Note *n = new Note(event.buffer);
        this->callbackProvider->JackEngineNoteHandler(n, event.time);
    }

    trigbuf = jack_port_get_buffer(trigger_port, nframes);
    for (uint32_t i = 0; i < jack_midi_get_event_count(trigbuf); i++) {
        jack_midi_event_t event;
        jack_midi_event_get(&event, trigbuf, i);
        Note *n = new Note(event.buffer);
        this->callbackProvider->JackEngineTriggerHandler(n, event.time);
    }

    this->callbackProvider->JackEngineTickHandler(nframes);

    outbuf = jack_port_get_buffer(output_port, nframes);
    jack_midi_clear_buffer(outbuf);
    this->callbackProvider->JackEnginePlayFunctionHandler(play_fn);


    return 0;
}
