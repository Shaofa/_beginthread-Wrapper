# _beginthread-Wrapper
A class wrapper of windows thread APIs.Inherit from this calss and overwirte the @run method to implements you business.

##avaliable methods
```cpp
virtual void run() = 0;
bool start(bool suspend=false) throw(ThreadException);
bool terminate() throw(ThreadException);
bool suspend() throw(ThreadExcepttion);
void resume() throw(ThreadException);
void join(int32_t timeout=-1) throw(Exception);
bool isRunning();
bool isSuspended();
bool isStopped();
bool isAlive();
```