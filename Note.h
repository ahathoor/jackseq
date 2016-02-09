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

