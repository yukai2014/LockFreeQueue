# LockFreeQueue
a demo of lock-free queue

User Manual:

1. If it's certain that there is only one producer, please use SingleProducerQueue class
2. If there are more than one producer and only one producer take up the most push operations, please use FixSizeLockFreeQueue class
3. If there are more than one producer that all push data frequently, please use std::queue class with spinlock