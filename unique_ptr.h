#pragma once

#include <cstddef>
#include <memory>
#include <utility>
#include <type_traits>

template <typename T, typename Deleter = std::default_delete<T>>
class UniquePtr : private Deleter {
public:
    // ======================== Constructors ========================
    UniquePtr() noexcept : ptr_(nullptr) {}
    explicit UniquePtr(T* ptr) noexcept : ptr_(ptr) {}
    UniquePtr(T* ptr, const Deleter& deleter) 
        : Deleter(deleter), ptr_(ptr) {}
    UniquePtr(T* ptr, Deleter&& deleter) 
        : Deleter(std::move(deleter)), ptr_(ptr) {}

    // =================== No copy ==================================
    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    // =================== Move ====================================
    UniquePtr(UniquePtr&& other) noexcept
        : Deleter(std::move(other.get_deleter())), ptr_(other.ptr_) {
        other.ptr_ = nullptr;
    }
    
    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this != &other) {
            reset();
            ptr_ = other.ptr_;
            get_deleter() = std::move(other.get_deleter());
            other.ptr_ = nullptr;
        }
        return *this;
    }

    // =================== Destructor ==============================
    ~UniquePtr() {
        reset();
    }

    // =================== Observers ===============================
    T* get() noexcept { return ptr_; }
    const T* get() const noexcept { return ptr_; }
    explicit operator bool() const noexcept { return ptr_ != nullptr; }

    T& operator*() noexcept { return *ptr_; }
    const T& operator*() const noexcept { return *ptr_; }

    T* operator->() noexcept { return ptr_; }
    const T* operator->() const noexcept { return ptr_; }

    Deleter& get_deleter() noexcept { return *this; }
    const Deleter& get_deleter() const noexcept { return *this; }

    // =================== Modifiers ===============================
    T* release() noexcept {
        T* tmp = ptr_;
        ptr_ = nullptr;
        return tmp;
    }
    
    void reset(T* ptr = nullptr) noexcept {
        if (ptr_) {
            get_deleter()(ptr_);
        }
        ptr_ = ptr;
    }
    
    void swap(UniquePtr& other) noexcept {
        std::swap(ptr_, other.ptr_);
        std::swap(get_deleter(), other.get_deleter());
    }

private:
    T* ptr_;
};

// =====================================================================
// Partial specialization for arrays: UniquePtr<T[]>
// =====================================================================

template <typename T, typename Deleter>
class UniquePtr<T[], Deleter> : private Deleter {
public:
    // ======================== Constructors ========================
    UniquePtr() noexcept : ptr_(nullptr) {}
    explicit UniquePtr(T* ptr) noexcept : ptr_(ptr) {}
    UniquePtr(T* ptr, const Deleter& deleter) 
        : Deleter(deleter), ptr_(ptr) {}
    UniquePtr(T* ptr, Deleter&& deleter) 
        : Deleter(std::move(deleter)), ptr_(ptr) {}

    // =================== No copy ==================================
    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    // =================== Move ====================================
    UniquePtr(UniquePtr&& other) noexcept
        : Deleter(std::move(other.get_deleter())), ptr_(other.ptr_) {
        other.ptr_ = nullptr;
    }
    
    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this != &other) {
            reset();
            ptr_ = other.ptr_;
            get_deleter() = std::move(other.get_deleter());
            other.ptr_ = nullptr;
        }
        return *this;
    }

    // =================== Destructor ==============================
    ~UniquePtr() {
        reset();
    }

    // =================== Observers ===============================
    T* get() noexcept { return ptr_; }
    const T* get() const noexcept { return ptr_; }
    explicit operator bool() const noexcept { return ptr_ != nullptr; }

    T& operator[](size_t index) noexcept { return ptr_[index]; }
    const T& operator[](size_t index) const noexcept { return ptr_[index]; }

    Deleter& get_deleter() noexcept { return *this; }
    const Deleter& get_deleter() const noexcept { return *this; }

    // =================== Modifiers ===============================
    T* release() noexcept {
        T* tmp = ptr_;
        ptr_ = nullptr;
        return tmp;
    }
    
    void reset(T* ptr = nullptr) noexcept {
        if (ptr_) {
            get_deleter()(ptr_);
        }
        ptr_ = ptr;
    }
    
    void swap(UniquePtr& other) noexcept {
        std::swap(ptr_, other.ptr_);
        std::swap(get_deleter(), other.get_deleter());
    }

private:
    T* ptr_;
};

// =================== Free function ===============================

template <typename T, typename... Args>
UniquePtr<T> make_unique(Args&&... args) {
    return UniquePtr<T>(new T(std::forward<Args>(args)...));
}
