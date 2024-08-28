CC = g++
CFLAGS = -Wall 
TARGET = main
OBJECTS = main.o zenity_dialog.o 
INCLUDEDIR1 = ./
INCLUDEDIR2 = /usr/include/lua5.3/
LIBS = -lsfml-system -lsfml-audio -lsfml-graphics -lsfml-window -llua5.3

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) -I$(INCLUDEDIR1) -I$(INCLUDEDIR2) $(OBJECTS) $(LIBS) -o $(TARGET)
main.o: main.cc 
	$(CC) -I$(INCLUDEDIR1) -I$(INCLUDEDIR2)  $(CFLAGS) -c main.cc 
zenity_dialog.o: zenity_dialog.cc
	$(CC) $(CFLAGS) -c zenity_dialog.cc  
clean: $(TARGET) *.o
	rm *.o $(TARGET)

