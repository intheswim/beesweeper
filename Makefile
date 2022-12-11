#OBJS specifies which files to compile as part of the project
OBJS = ./*.cpp ./ui/*.cpp
BOOST_PO_LIBS=-lboost_program_options
INC=-I/usr/include/SDL2 -I./ui

#CC specifies which compiler we're using 
CC = g++

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
COMPILER_FLAGS = -Wall -Weffc++ -pedantic -O2 -std=c++14

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf $(BOOST_PO_LIBS)

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = mines

PREFIX = /usr/local

#This is the target that compiles our executable
all : $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) $(INC) -o $(OBJ_NAME)

install : all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
#	mkdir -p $(DESTDIR)$(PREFIX)/bin/assets
#	mkdir -p $(DESTDIR)$(PREFIX)/bin/images
#	mkdir -p $(DESTDIR)$(PREFIX)/bin/sounds
	cp mines $(DESTDIR)$(PREFIX)/bin/mines
#	cp -pr assets/* $(DESTDIR)$(PREFIX)/bin/assets
#	cp -pR images/* $(DESTDIR)$(PREFIX)/bin/images
#	cp -pR sounds/* $(DESTDIR)$(PREFIX)/bin/sounds
#	chmod 755 $(DESTDIR)$(PREFIX)/bin/images/counters
#	chmod 755 $(DESTDIR)$(PREFIX)/bin/images/faces
#	chmod 755 $(DESTDIR)$(PREFIX)/bin/images/small_images
#	chmod 755 $(DESTDIR)$(PREFIX)/bin/sounds
#	ls -Rl $(DESTDIR)$(PREFIX)/bin/images
#	ls -Rl $(DESTDIR)$(PREFIX)/bin/sounds

clobber :
	rm -dr $(DESTDIR)$(PREFIX)/bin/assets
	rm -dr $(DESTDIR)$(PREFIX)/bin/images
	rm -dr $(DESTDIR)$(PREFIX)/bin/sounds
	rm -f $(DESTDIR)$(PREFIX)/bin/mines

clean:
	rm -f ./mines