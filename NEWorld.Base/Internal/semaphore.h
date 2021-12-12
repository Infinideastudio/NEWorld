#pragma once

#if __has_include(<mach/semaphore.h>)
#include <mach/semaphore.h>
#include <mach/mach_init.h>
#include <mach/task.h>

class semaphore {
public:
    explicit semaphore() noexcept
            :handle(New()) { }

    ~semaphore() { Release(handle); }

    void wait() noexcept {
        while (semaphore_wait(handle)!=KERN_SUCCESS) { }
    }

    void signal() noexcept { semaphore_signal(handle); }
private:
    static semaphore_t New() noexcept {
        semaphore_t ret;
        semaphore_create(mach_task_self(), &ret, SYNC_POLICY_FIFO, 0);
        return ret;
    }

    static void Release(semaphore_t sem) noexcept {
        semaphore_destroy(mach_task_self(), sem);
    }

    semaphore_t handle;
};

#elif __has_include(<Windows.h>)
#include "internal/system.h"

class semaphore {
public:
    semaphore() noexcept
            : handle(CreateSemaphore(nullptr, 0, MAXLONG, nullptr)) { }

    ~semaphore() noexcept { CloseHandle(handle); }

    void wait() noexcept { WaitForSingleObject(handle, INFINITE); }

    void signal() noexcept {
        LONG last;
        ReleaseSemaphore(handle, 1, &last);
    }

private:
    HANDLE handle;
};

#elif __has_include(<semaphore.h>)
#include <semaphore.h>
class semaphore {
public:
    semaphore() noexcept { sem_init(&_semaphore, 0, 0); }

    ~semaphore() noexcept { sem_destroy(&_semaphore); }

    void wait() noexcept { sem_wait(&_semaphore); }

    void signal() noexcept { sem_post(&_semaphore); }
private:
    sem_t _semaphore;
};
#else
class semaphore {
public:
	semaphore() noexcept {}

	~semaphore() noexcept {}

	void wait() noexcept {}

	void signal() noexcept {}
};
# error "No Adaquate semaphore Supported to be adapted from"
#endif