CXXFLAGS =	-O2 -g -Wall -fmessage-length=0 -std=c++11 

OBJS =		LockFreeQueueTest.o

BASE_SRC = Guard.h CompilerFeature.h Timer.h 

LIBS =	-pthread

TARGET =	LockFreeQueue FixSizeLockFreeQueue

#$(TARGET):	$(OBJS)
#	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)
	
LockFreeQueue: LockFreeQueueTest.o
	$(CXX) -o LockFreeQueue LockFreeQueueTest.o $(LIBS)
	
LockFreeQueueTest.o : LockFreeQueueTest.cpp LockFreeQueue.h $(BASE_SRC)


FixSizeLockFreeQueue: FixSizeLockFreeQueueTest.o
	$(CXX) -o FixSizeLockFreeQueue FixSizeLockFreeQueueTest.o $(LIBS)
	
FixSizeLockFreeQueueTest.o : FixSizeLockFreeQueueTest.cpp FixSizeLockFreeQueue.h $(BASE_SRC)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
