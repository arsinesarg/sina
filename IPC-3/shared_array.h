#ifndef SHARED_ARRAY_H
#define SHARED_ARRAY_H

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>

#include <string>
#include <stdexcept>

class shared_array {
private:
    std::string shm_name;
    std::string sem_name;

    int shm_fd;
    int* data;
    size_t length;
    size_t bytes;

    sem_t* sem;

public:
    shared_array(const std::string& name, size_t size);

    shared_array(const shared_array&) = delete;
    shared_array& operator=(const shared_array&) = delete;
    
//move-constructor
    shared_array(shared_array&& other) noexcept;

    ~shared_array();

    int& operator[](size_t index);
    size_t size() const;

    sem_t* get_semaphore();

    void unlink();
};

#endif

