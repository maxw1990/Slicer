#include "ActiveObject.h"
#include <assert.h>
#include <format>


#ifndef DEBUG
#endif

ActiveObject::ActiveObject() : thread(&ActiveObject::run, this),
    running(true),
    active(false),
    threadID(std::hash<std::thread::id>{}(thread.get_id())) 
    {
    }

ActiveObject::~ActiveObject(){
    try
    {
        if(running)
        {
            // This instance of this thread class is delete before the
            // corresponding thread function has finished.
            // This is an exceptional situation and should not happen.
            assert(0);

            // Terminate thxread
            // In the destructor, this would not call a potential derived Terminate()
            // Die Methode darf nicht vom Thread selber aufgerufen werden. Stattdessen sollte
            // man die Thread-Methode verlassen, wodurch sich der Thread sauber beendet.
            assert(std::this_thread::get_id() != thread.get_id());

            if(thread.native_handle())
            {
                // Falls der Thread noch nicht beendet ist, so wird er abgeschossen.
                if(running)
                {
                    pthread_cancel(thread.native_handle()) == 0 ? true : false;
                }

                // thread not allowed to be joinable
                thread.join();
            }
        }
        else if(thread.joinable())
        {
            // Only join if the thread is joinable because the std::thread may already
            // have been joined (indirectly) in a subclass
            thread.join();
        }
    }
    catch(...)
    {
        assert(0);
        // At the end of the program, it can happen that the thread has already been cleaned up.
        // The handle in m_thread.native_handle() is then no longer valid. The
        // API thread methods can throw exceptions due to the invalid handle.
        // These exceptions must be caught, and the destruction of the thread can be skipped,
        // because the operating system has already taken care of it anyway.
    }
};

// send message
void ActiveObject::send(std::unique_ptr<Message> msg) {
    std::lock_guard<std::mutex> lock(mutex);
    active = true;
    messages.push(std::move(msg));
    condition.notify_one();
}

// wait and stop of execution of the object
void ActiveObject::WaitAndStop() {
    std::unique_lock<std::mutex> lock(mutex);
    #ifndef NDEBUG
    printf("Object Waiting to stop...%i\n", getThreadID());
    #endif

    condition.wait(lock, [this]{ return messages.empty();});
    running = false;
    lock.unlock();
    #ifndef NDEBUG
    printf("Object executing stop, ID: %i\n", this->getThreadID());
    #endif
    condition.notify_one();
    if(thread.joinable()){
        thread.join();
    }
}

bool ActiveObject::isActive() const {
    return active;
}

int ActiveObject::getThreadID() const{
    return threadID;
}

class TaskHelper
{
public:
    /*=============================================================================*/
    /**
        Calling function and is doing exception handling.

        @param bCondition	expression called
        @param obj		    active object
        @param function		function
        @return				succes state
        @author				M. Werner
    */
    /*=============================================================================*/
        template<typename FunctionType>
    static bool tryAndCatch(bool bCondition,ActiveObject& obj, FunctionType function)
    {
        try
        {
            return bCondition ? static_cast<bool>(function()) : false;
        }
        catch(...)
        {
            std::runtime_error e(std::format("Unknown exception in thread {}",obj.getThreadID()));
            //throw e;
            assert(0);
        }
        return false;
    }
};

void ActiveObject::run() {
    while (running) {
        std::unique_lock<std::mutex> lock(mutex);
        #ifndef NDEBUG
        printf("Object Waiting...%i\n", getThreadID());
        #endif
        condition.wait(lock, [this] { return !messages.empty() || !running; });
        #ifndef NDEBUG
        printf("Running: %s, ID: %i\n", running? "true":"false", getThreadID());
        #endif

        if (!running) break;

        std::unique_ptr<Message> msg = std::move(messages.front());
        messages.pop();
        lock.unlock();
        // Nachricht verarbeiten
        TaskHelper::tryAndCatch(running,
        *this,
        [&msg](){
            return msg->execute();
        });

        lock.lock();
        if(messages.empty()){
            active = false;
            condition.notify_one();
        }
        lock.unlock();
    }
}