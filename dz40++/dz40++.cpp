#include <iostream>

template<typename T>
class MyUniquePtr {
private:
    T* ptr;

public:
    // Конструктор
    explicit MyUniquePtr(T* p = nullptr) : ptr(p) {}

    MyUniquePtr(const MyUniquePtr&) = delete;
    MyUniquePtr& operator=(const MyUniquePtr&) = delete;

    MyUniquePtr(MyUniquePtr&& other) noexcept : ptr(other.ptr) {
        other.ptr = nullptr;
    }

    MyUniquePtr& operator=(MyUniquePtr&& other) noexcept {
        if (this != &other) {
            delete ptr;
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

    ~MyUniquePtr() {
        delete ptr;
    }

    T& operator*() const { return *ptr; }
    T* operator->() const { return ptr; }

    T* get() const { return ptr; }

    T* release() {
        T* oldPtr = ptr;
        ptr = nullptr;
        return oldPtr;
    }

    void reset(T* p = nullptr) {
        delete ptr;
        ptr = p;
    }
};

template<typename T>
class MySharedPtr {
private:
    T* ptr;
    int* refCount;

    void release() {
        if (ptr && --(*refCount) == 0) {
            delete ptr;
            delete refCount;
        }
    }

public:
    explicit MySharedPtr(T* p = nullptr) : ptr(p), refCount(new int(1)) {}

    MySharedPtr(const MySharedPtr& other) : ptr(other.ptr), refCount(other.refCount) {
        ++(*refCount);
    }

    MySharedPtr& operator=(const MySharedPtr& other) {
        if (this != &other) {
            release();
            ptr = other.ptr;
            refCount = other.refCount;
            ++(*refCount);
        }
        return *this;
    }

    MySharedPtr(MySharedPtr&& other) noexcept : ptr(other.ptr), refCount(other.refCount) {
        other.ptr = nullptr;
        other.refCount = nullptr;
    }

    MySharedPtr& operator=(MySharedPtr&& other) noexcept {
        if (this != &other) {
            release();
            ptr = other.ptr;
            refCount = other.refCount;
            other.ptr = nullptr;
            other.refCount = nullptr;
        }
        return *this;
    }

    ~MySharedPtr() {
        release();
    }

    T& operator*() const { return *ptr; }
    T* operator->() const { return ptr; }

    T* get() const { return ptr; }

    int use_count() const { return *refCount; }
};

class Test {
public:
    void sayHello() const {
        std::cout << "Hello from Test!\n";
    }
};

int main() {
    std::cout << "Testing MyUniquePtr:\n";
    MyUniquePtr<Test> uniquePtr1(new Test());
    uniquePtr1->sayHello();

    MyUniquePtr<Test> uniquePtr2 = std::move(uniquePtr1);
    if (!uniquePtr1.get())
        std::cout << "uniquePtr1 is now null\n";
    uniquePtr2->sayHello();

    std::cout << "\nTesting MySharedPtr:\n";
    MySharedPtr<Test> sharedPtr1(new Test());
    std::cout << "sharedPtr1 use count: " << sharedPtr1.use_count() << "\n";

    {
        MySharedPtr<Test> sharedPtr2 = sharedPtr1;
        std::cout << "sharedPtr1 use count after copy: " << sharedPtr1.use_count() << "\n";
        sharedPtr2->sayHello();
    }
    std::cout << "sharedPtr1 use count after sharedPtr2 goes out of scope: " << sharedPtr1.use_count() << "\n";
}
