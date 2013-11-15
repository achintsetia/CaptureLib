#Written by Achint Setia (05-06-2012)


CXX = g++

INCFLAGS = -I./include/ `pkg-config libavformat --cflags`\
	`pkg-config libavcodec --cflags`\
    `pkg-config opencv --cflags`

CXXFLAGS = -pthread -Wall -g -O6 $(INCFLAGS)

LDFLAGS = -L./lib/ -pthread -lCapture `pkg-config libavformat --libs`\
	`pkg-config libavcodec --libs`\
    `pkg-config opencv --libs`

VPATH = src/
OBJDIR = obj/
BINDIR = bin/
LIBDIR = lib/

SRCS = testCapture.cpp
OBJS = $(SRCS:%.cpp=$(OBJDIR)/%.o)
TRGT = $(BINDIR)/testCapture

LIBSRCS = capture.cpp videoCapture.cpp imageCapture.cpp cameraCapture.cpp
LIBOBJS = $(LIBSRCS:%.cpp=$(OBJDIR)/%.o)
LIBTRGT = $(LIBDIR)/libCapture.a

all : $(LIBTRGT) $(TRGT)

$(LIBTRGT) : $(LIBOBJS)
	echo $(LIBOBJS)
	ar -rs $(LIBTRGT) $^

$(OBJDIR)/%.o : %.cpp
	$(CXX) -c $< $(CXXFLAGS) -o $@

$(TRGT) : $(OBJS) $(LIBTRGT)
	$(CXX) $^ $(LDFLAGS) -o $(TRGT)

clean:
	$(RM) $(OBJS) $(LIBOBJS)

distclean : clean
	$(RM) $(TRGT) $(LIBTRGT)

