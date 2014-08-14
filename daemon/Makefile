#########################################################################
#																		#
# CPUSaver (C)2013, Cristian-Valeriu Soare      						#
# author: Cristian-Valeriu Soare 										#
# e-mail: soare_cristian16@yahoo.com 									#
# supervisor: Conf. Dr. Eng. Florin Pop									#
# Politehnica University of Bucharest, 									#
# Faculty of Automatic Control and Computer Science						#
# 																		#
# This file is part of CPUSaver.										#
# 																		#
# CPUSaver is free software: you can redistribute it and/or modify		#
# it under the terms of the GNU General Public License as published by	#
# the Free Software Foundation, either version 3 of the License, or 	#
# (at your option) any later version.									#
# 																		#
# CPUSaver is distributed in the hope that it will be useful,			#
# but WITHOUT ANY WARRANTY; without even the implied warranty of		#
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 		#
# GNU General Public License for more details.							#
# 																		#
# You should have received a copy of the GNU General Public License 	#
# along with CPUSaver.  If not, see <http:#www.gnu.org/licenses/>.		#
#																		#
#########################################################################

CC = g++
CFLAGS = -c -Wall -m32
LDFLAGS = -lX11 -lpthread
EXE = cpusaver
OBJ = main.o window.o process.o pmanager.o pdelayer.o special.o cfgread.o
OBJLST = $(patsubst %,$(OBJDIR)/%,$(OBJ))
OBJDIR = obj

build: mkobjdir $(OBJLST)
	g++ $(OBJLST) -o $(EXE) $(LDFLAGS) -m32

mkobjdir:
	mkdir -p obj

$(OBJDIR)/main.o: main.cpp
	$(CC) main.cpp $(CFLAGS) -o $(OBJDIR)/main.o

$(OBJDIR)/window.o: window.cpp
	$(CC) window.cpp $(CFLAGS) -o $(OBJDIR)/window.o

$(OBJDIR)/process.o: process.cpp
	$(CC) process.cpp $(CFLAGS) -o $(OBJDIR)/process.o

$(OBJDIR)/pmanager.o: pmanager.cpp
	$(CC) pmanager.cpp $(CFLAGS) -o $(OBJDIR)/pmanager.o

$(OBJDIR)/pdelayer.o: pdelayer.cpp
	$(CC) pdelayer.cpp $(CFLAGS) -o $(OBJDIR)/pdelayer.o

$(OBJDIR)/special.o: special.cpp
	$(CC) special.cpp $(CFLAGS) -o $(OBJDIR)/special.o

$(OBJDIR)/cfgread.o: cfgread.cpp
	$(CC) cfgread.cpp $(CFLAGS) -o $(OBJDIR)/cfgread.o

clean:
	rm -f *~ *.o $(EXE) $(OBJDIR)/*

install:
	cp $(EXE) /usr/local/bin/$(EXE) && chmod a+x /usr/local/bin/$(EXE)
	# echo "cpusaver" >> ~/.xinitrc

uninstall:
	rm /usr/local/bin/$(EXE)
	# sed '/cpusaver/d' ~/.xinitrc > tempf && mv tempf ~/.xinitrc

run:
	./$(EXE)
v:
	valgrind ./$(EXE)
	
