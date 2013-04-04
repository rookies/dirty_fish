# makefile
# 
# Copyright (C) 2010 Robert Knauer <robert AT fishmixx.de>
# 
# This file is part of dirty_fish.
# 
# dirty_fish is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published
# by the Free Software Foundation; version 3 only.
# 
# dirty_fish is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with dirty_fish. If not, see <http://www.gnu.org/licenses/>.
# 
# Usage: make dirty_fish

### OPTIONS
## Object-Files:
OBJ = build/main.o build/config.o build/logging.o build/threads.o build/xmpp.o build/functions.o
## Libs:
LIBS = loudmouth-1.0 libxml-2.0 libsoup-2.4
## Compiler-Flags:
CFLAGS = -c `pkg-config $(LIBS) --cflags`
## Linker-Flags:
LFLAGS = `pkg-config $(LIBS) --libs` -lpthread

### TARGETS
## link all:
dirty_fish: $(OBJ)
	gcc -o $@ $(OBJ) $(LFLAGS)
## compile main.c:
build/main.o : src/main.c
	$(CC) src/main.c -o build/main.o $(CFLAGS)
## compile config.c:
build/config.o : src/config.c
	$(CC) src/config.c -o build/config.o $(CFLAGS)
## compile logging.c:
build/logging.o : src/logging.c
	$(CC) src/logging.c -o build/logging.o $(CFLAGS)
## compile threads.c:
build/threads.o : src/threads.c
	$(CC) src/threads.c -o build/threads.o $(CFLAGS)
## compile xmpp.c:
build/xmpp.o : src/xmpp.c
	$(CC) src/xmpp.c -o build/xmpp.o $(CFLAGS)
## compile functions.c:
build/functions.o : src/functions.c
	$(CC) src/functions.c -o build/functions.o $(CFLAGS)
## remove end-target and object-files:
clean :
	rm $(OBJ) $(TARGET)
