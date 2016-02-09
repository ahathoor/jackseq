#ifndef IJACKENGINECALLBACKPROVIDER_H
#define IJACKENGINECALLBACKPROVIDER_H

#include "Note.h"

class IJackEngineCallbackProvider {
public:
    virtual ~IJackEngineCallbackProvider() {};
    virtual void JackEngineTickHandler(int nframes) = 0;
    virtual void JackEngineNoteHandler(Note* note, int offset) = 0;
    virtual void JackEngineTriggerHandler(Note* note, int offset) = 0;
    virtual void JackEnginePlayFunctionHandler(void (*play_fn)(Note*, int offset)) = 0;
};

#endif /* IJACKENGINECALLBACKPROVIDER_H */

