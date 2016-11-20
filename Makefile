CXXFLAGS =	-O2 -g -Wall -fmessage-length=0 -std=c++11 

OBJS =	FixSizeLockFreeQueueTest.o LockFreeQueueTest.o

BASE_SRC = Guard.h CompilerFeature.h Timer.h 

LIBS =	-pthread

TARGET =	LockFreeQueue FixSizeLockFreeQueue

all:	$(TARGET)

	
LockFreeQueue: LockFreeQueueTest.o
	$(CXX) -o LockFreeQueue LockFreeQueueTest.o $(LIBS)
	
LockFreeQueueTest.o : LockFreeQueue.h $(BASE_SRC)


FixSizeLockFreeQueue: FixSizeLockFreeQueueTest.o
	$(CXX) -o FixSizeLockFreeQueue FixSizeLockFreeQueueTest.o $(LIBS)
	
FixSizeLockFreeQueueTest.o : FixSizeLockFreeQueue.h $(BASE_SRC)


clean:
	rm -f $(OBJS) $(TARGET)
