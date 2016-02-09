#include <iostream>
#include "Note.h"
#include "NoteHandler.h"
#include "JackEngine.h"

int main(int argc, char *argv[])
{
    NoteHandler *ns = new NoteHandler();
    JackEngine *je = new JackEngine(ns);
    je->init();
    std::cin.ignore();
    return 0;
}
