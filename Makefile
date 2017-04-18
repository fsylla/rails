
# _____________________________________________________________________________
#
# file:         Makefile
# created:      Fri Jan 20 18:50:57 CET 2017
# by:           frank
#
# description:	build rails targets
# _____________________________________________________________________________
#


CC      = cc
CXX     = g++

CFLAGS  = -MD -g
LDFLAGS = -lglut -lGL -lGLU -lSDL -lSDL_image

TARGETS	= nodemap_draw nodemap_client nodemap_server
TESTS	= nodemap_test
OBJ	= destseq.o node.o nodemap.o rail.o train.o
OBJ_GL	= matrix.o video.o

DEP	= $(OBJ:.o=.d) $(OBJ_GL:.o=.d)


all:				$(TARGETS) $(TESTS)


nodemap_draw.o:                 nodemap_draw.cpp
	$(CXX) -c `pkg-config --cflags gtk+-3.0` -o $@ $<


%.o:				%.c
	$(CC) -c $(CFLAGS) -o $@ $<

%.o:				%.cpp
	$(CXX) -c $(CFLAGS) -o $@ $<


nodemap_draw:                   nodemap_draw.o $(OBJ)
	$(CXX) -o $@ $@.o $(OBJ) `pkg-config --libs gtk+-3.0`


nodemap_client:			nodemap_client.o $(OBJ) $(OBJ_GL) socketio.o
	$(CXX) -o $@ $@.o $(OBJ) $(OBJ_GL) socketio.o $(LDFLAGS) -lm


nodemap_server:			nodemap_server.o $(OBJ) socketio.o
	$(CXX) -o $@ $@.o $(OBJ) socketio.o


nodemap_test:			nodemap_test.o $(OBJ)
	$(CXX) -o $@ $@.o $(OBJ)


-include $(DEP)


clean:
	rm -f $(TARGETS) $(TESTS) *.o *.d

