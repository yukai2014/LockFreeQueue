CXXFLAGS =	-O2 -g -Wall -fmessage-length=0 -std=c++11

OBJS =		LockFreeQueueTest.o

LIBS =

TARGET =	LockFreeQueue

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)
	

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

LockFreeQueueTest.o : LockFreeQueueTest.cpp Timer.h LockFreeQueue.h Guard.h CompilerFeature.h 
