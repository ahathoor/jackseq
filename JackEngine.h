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

