#include <Windows.h>
#include <process.h>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include <exception>


class ThreadException: public std::runtime_error
{
public:
    ThreadException(const std::string message):runtime_error(message){}
};

#define EXCEPTION_MODIFIER  throw(ThreadException)
class Thread
{
private:
    enum THREAD_STATUS{RUNNING, SUSPENDED, STOPPED};

public:
    Thread(){}
    Thread(const std::string threadName) EXCEPTION_MODIFIER;
    ~Thread(){}
    virtual void run() = 0;
    bool start(bool suspend=false) EXCEPTION_MODIFIER;
    void join(int32_t timeout=-1) EXCEPTION_MODIFIER;
    void resume() EXCEPTION_MODIFIER;
    void suspend() EXCEPTION_MODIFIER;
    bool terminate() EXCEPTION_MODIFIER;

public:
    bool isRunning() { return (RUNNING==status); }
    bool isSuspended() { return (SUSPENDED==status); }
    bool isStopped() { return (STOPPED==status); }
    bool isAlive() { return (STOPPED!=status); }
    THREAD_STATUS getStatus() { return status; };
private:
    void setStatus(THREAD_STATUS status) { this->status = status; }
    static unsigned int WINAPI staticThreadFunc(void* arg);

protected:
    HANDLE  handle;
    std::string threadName;
    uint32_t threadId;
    volatile THREAD_STATUS status;
};

Thread::Thread(const std::string threadName/*="noNamed"*/) : threadName(threadName), threadId(0),status(STOPPED)
{
}

bool Thread::start(bool suspend/*=false*/)
{
    handle = (HANDLE)_beginthreadex(NULL, 0, staticThreadFunc,this, (suspend?CREATE_SUSPENDED:0), &threadId);
    if (NULL == handle)
        throw ThreadException("_beginthreadex(): create thread error!");
    setStatus(suspend ? SUSPENDED : RUNNING);
    return isAlive();
}

void Thread::join(int32_t timeout /*=-1*/)
{
    if(handle) {
        WaitForSingleObject(handle, timeout==-1? INFINITE : timeout);
    }
    else {
        throw ThreadException("join(): thread has not been create!");
    }
}

void Thread::resume()
{
    if(handle && isSuspended()) {
        ResumeThread(handle);
        setStatus(RUNNING);
    }
    else 
        throw ThreadException("resume(): thread is not suspened!");
}

void Thread::suspend()
{
    if(handle && isRunning()) {
        SuspendThread(handle);
        setStatus(SUSPENDED);
    }
    else
        throw ThreadException("suspend(): thread is not running!");
}

bool Thread::terminate()
{
    if(handle && isAlive())
    {
        if( TerminateThread(handle, 0) )
        {
            CloseHandle(handle);
            handle = NULL;
            setStatus(STOPPED);
            return true;
        }
    }
    else 
        throw ThreadException("terminate(): thread is not alive!");
    return false;
}

/*static*/ unsigned int Thread::staticThreadFunc(void *arg)
{
    Thread* thread = (Thread*)arg;
    thread->run();
    return 0;
}

