#pragma once
 
#include <cstddef>
#include <memory>
#include <utility>
#include <type_traits>
 
template <typename T, typename Deleter = std::default_delete<T>>
class UniquePtr : private Deleter {  // ← наследование вместо члена
public:
    // ======================== Constructors ========================
    UniquePtr() : ptr_(nullptr) {}
    explicit UniquePtr(T* ptr) : ptr_(ptr) {}
    UniquePtr(T* ptr, const Deleter& deleter) 
        : Deleter(deleter), ptr_(ptr) {}
    UniquePtr(T* ptr, Deleter&& deleter) 
        : Deleter(std::move(deleter)), ptr_(ptr) {}
 
    // =================== No copy ==================================
    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;
 
    // =================== Move ====================================
    UniquePtr(UniquePtr&& other) 
        : Deleter(std::move(other.get_deleter())), ptr_(other.release()) {}
    
    UniquePtr& operator=(UniquePtr&& other) {
        if (this != &other) {
            reset(other.release());
            get_deleter() = std::move(other.get_deleter());
        }
        return *this;
    }
 
    // =================== Destructor ==============================
    ~UniquePtr() {
        reset();
    }
 
    // =================== Observers ===============================
    T* get() { return ptr_; }
    const T* get() const { return ptr_; }
    explicit operator bool() const { return ptr_ != nullptr; }
 
    T& operator*() { return *ptr_; }
    const T& operator*() const { return *ptr_; }
 
    T* operator->() { return ptr_; }
    const T* operator->() const { return ptr_; }
 
    Deleter& get_deleter() { return *this; }
    const Deleter& get_deleter() const { return *this; }
 
    // =================== Modifiers ===============================
    T* release() {
        T* tmp = ptr_;
        ptr_ = nullptr;
        return tmp;
    }
    
    void reset(T* ptr = nullptr) {
        if (ptr_) get_deleter()(ptr_);
        ptr_ = ptr;
    }
    
    void swap(UniquePtr& other) {
        std::swap(ptr_, other.ptr_);
        std::swap(get_deleter(), other.get_deleter());
    }
 
private:
    T* ptr_ = nullptr;
};
 
// =====================================================================
// Partial specialization for arrays: UniquePtr<T[]>
// =====================================================================
 
template <typename T, typename Deleter>
class UniquePtr<T[], Deleter> : private Deleter {
public:
    // ======================== Constructors ========================
    UniquePtr() : ptr_(nullptr) {}
    explicit UniquePtr(T* ptr) : ptr_(ptr) {}
    UniquePtr(T* ptr, const Deleter& deleter) 
        : Deleter(deleter), ptr_(ptr) {}
    UniquePtr(T* ptr, Deleter&& deleter) 
        : Deleter(std::move(deleter)), ptr_(ptr) {}
 
    // =================== No copy ==================================
    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;
 
    // =================== Move ====================================
    UniquePtr(UniquePtr&& other) 
        : Deleter(std::move(other.get_deleter())), ptr_(other.release()) {}
    
    UniquePtr& operator=(UniquePtr&& other) {
        if (this != &other) {
            reset(other.release());
            get_deleter() = std::move(other.get_deleter());
        }
        return *this;
    }
 
    // =================== Destructor ==============================
    ~UniquePtr() {
        reset();
    }
 
    // =================== Observers ===============================
    T* get() { return ptr_; }
    const T* get() const { return ptr_; }
    explicit operator bool() const { return ptr_ != nullptr; }
 
    T& operator[](size_t index) { return ptr_[index]; }
    const T& operator[](size_t index) const { return ptr_[index]; }
 
    Deleter& get_deleter() { return *this; }
    const Deleter& get_deleter() const { return *this; }
 
    // =================== Modifiers ===============================
    T* release() {
        T* tmp = ptr_;
        ptr_ = nullptr;
        return tmp;
    }
    
    void reset(T* ptr = nullptr) {
        if (ptr_) get_deleter()(ptr_);
        ptr_ = ptr;
    }
    
    void swap(UniquePtr& other) {
        std::swap(ptr_, other.ptr_);
        std::swap(get_deleter(), other.get_deleter());
    }
 
private:
    T* ptr_ = nullptr;
};
 
// =================== Free function ===============================
 
template <typename T, typename... Args>
UniquePtr<T> make_unique(Args&&... args) {
    return UniquePtr<T>(new T(std::forward<Args>(args)...));
}
