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
