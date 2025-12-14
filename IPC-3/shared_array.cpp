#include "shared_array.h"

shared_array::shared_array(const std::string& name, size_t size)
    : shm_fd(-1),
      data(nullptr),
      length(size),
      sem(nullptr)
{
    if (size < 1 || size > 1000000000) {
        throw std::invalid_argument("array size out of range");
    }

    shm_name = "/" + name;
    sem_name = "/sem_" + name;

    bytes = length * sizeof(int);

    shm_fd = shm_open(shm_name.c_str(),
                      O_RDWR | O_CREAT | O_EXCL,
                      0666);

    if (shm_fd != -1) {
        if (ftruncate(shm_fd, bytes) == -1) {
            close(shm_fd);
            shm_unlink(shm_name.c_str());
            throw std::runtime_error("ftruncate failed");
        }
    } else {
        shm_fd = shm_open(shm_name.c_str(), O_RDWR, 0666);
        if (shm_fd == -1)
            throw std::runtime_error("shm_open failed");

        struct stat st;
        if (fstat(shm_fd, &st) == -1) {
            close(shm_fd);
            throw std::runtime_error("fstat failed");
        }

        if ((size_t)st.st_size != bytes) {
            close(shm_fd);
            throw std::runtime_error("shared memory size mismatch");
        }
    }

    void* addr = mmap(nullptr, bytes,
                      PROT_READ | PROT_WRITE,
                      MAP_SHARED,
                      shm_fd, 0);

    if (addr == MAP_FAILED) {
        close(shm_fd);
        shm_unlink(shm_name.c_str());
        throw std::runtime_error("mmap failed");
    }

    data = static_cast<int*>(addr);

    sem = sem_open(sem_name.c_str(), O_CREAT, 0666, 1);
    if (sem == SEM_FAILED) {
        munmap(data, bytes);
        close(shm_fd);
        throw std::runtime_error("sem_open failed");
    }
}
//move-constructor
shared_array::shared_array(shared_array&& other) noexcept
    : shm_name(std::move(other.shm_name)),
      sem_name(std::move(other.sem_name)),
      shm_fd(other.shm_fd),
      data(other.data),
      length(other.length),
      bytes(other.bytes),
      sem(other.sem)
{
    other.shm_fd = -1;
    other.data = nullptr;
    other.sem = nullptr;
}
shared_array::~shared_array() {
    if (data)
        munmap(data, bytes);

    if (shm_fd != -1)
        close(shm_fd);

    if (sem)
        sem_close(sem);
}

int& shared_array::operator[](size_t index) {
    if (index >= length)
        throw std::out_of_range("index out of range");
    return data[index];
}

size_t shared_array::size() const {
    return length;
}

sem_t* shared_array::get_semaphore() {
    return sem;
}
//deleting shared memory and semaphore
void shared_array::unlink() {
    shm_unlink(shm_name.c_str());
    sem_unlink(sem_name.c_str());
}
