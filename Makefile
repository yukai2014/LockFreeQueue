CXXFLAGS =	-O2 -g -Wall -fmessage-length=0 -std=c++11 

OBJS =	FixSizeLockFreeQueueTest.o FixSizeLockFreeQueuePerfTest.o LockFreeQueueTest.o

BASE_SRC =	Guard.h CompilerFeature.h Timer.h 

LIBS =	-pthread

TARGET =	FixSizeLockFreeQueueTest FixSizeLockFreeQueuePerfTest
# TARGET += LockFreeQueue

CC = g++

# $(warning $(CXX))

all:	$(TARGET)


FixSizeLockFreeQueuePerfTest: FixSizeLockFreeQueuePerfTest.o
	$(CXX) -o FixSizeLockFreeQueuePerfTest FixSizeLockFreeQueuePerfTest.o $(LIBS)

FixSizeLockFreeQueuePerfTest.o : FixSizeLockFreeQueue.h $(BASE_SRC)


LockFreeQueue: LockFreeQueueTest.o
	$(CXX) -o LockFreeQueue LockFreeQueueTest.o $(LIBS)

LockFreeQueueTest.o : LockFreeQueue.h $(BASE_SRC)


FixSizeLockFreeQueue: FixSizeLockFreeQueueTest.o
	$(CXX) -o FixSizeLockFreeQueue FixSizeLockFreeQueueTest.o $(LIBS)

FixSizeLockFreeQueueTest.o : FixSizeLockFreeQueue.h $(BASE_SRC)


clean:
	rm -f $(OBJS) $(TARGET)
