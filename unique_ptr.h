#pragma once

#include <cstddef>
#include <memory>
#include <utility>
#include <tuple>

template <typename T, typename Deleter = std::default_delete<T>>
class UniquePtr {
public:
    // ======================== Constructors ========================
    UniquePtr() noexcept : tuple_(nullptr, Deleter()) {}
    explicit UniquePtr(T* ptr) noexcept : tuple_(ptr, Deleter()) {}
    UniquePtr(T* ptr, const Deleter& deleter) : tuple_(ptr, deleter) {}
    UniquePtr(T* ptr, Deleter&& deleter) : tuple_(ptr, std::move(deleter)) {}

    // =================== No copy ==================================
    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    // =================== Move ====================================
    UniquePtr(UniquePtr&& other) noexcept 
        : tuple_(other.release(), std::move(other.get_deleter())) {}
    
    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this != &other) {
            reset();
            std::get<0>(tuple_) = other.release();
            get_deleter() = std::move(other.get_deleter());
        }
        return *this;
    }

    // =================== Destructor ==============================
    ~UniquePtr() {
        reset();
    }

    // =================== Observers ===============================
    T* get() noexcept { return std::get<0>(tuple_); }
    const T* get() const noexcept { return std::get<0>(tuple_); }
    explicit operator bool() const noexcept { return get() != nullptr; }

    T& operator*() noexcept { return *get(); }
    const T& operator*() const noexcept { return *get(); }

    T* operator->() noexcept { return get(); }
    const T* operator->() const noexcept { return get(); }

    Deleter& get_deleter() noexcept { return std::get<1>(tuple_); }
    const Deleter& get_deleter() const noexcept { return std::get<1>(tuple_); }

    // =================== Modifiers ===============================
    T* release() noexcept {
        T* tmp = get();
        std::get<0>(tuple_) = nullptr;
        return tmp;
    }
    
    void reset(T* ptr = nullptr) noexcept {
        if (get()) {
            get_deleter()(get());
        }
        std::get<0>(tuple_) = ptr;
    }
    
    void swap(UniquePtr& other) noexcept {
        std::swap(tuple_, other.tuple_);
    }

private:
    std::tuple<T*, Deleter> tuple_;
};

// =====================================================================
// Partial specialization for arrays: UniquePtr<T[]>
// =====================================================================

template <typename T, typename Deleter>
class UniquePtr<T[], Deleter> {
public:
    // ======================== Constructors ========================
    UniquePtr() noexcept : tuple_(nullptr, Deleter()) {}
    explicit UniquePtr(T* ptr) noexcept : tuple_(ptr, Deleter()) {}
    UniquePtr(T* ptr, const Deleter& deleter) : tuple_(ptr, deleter) {}
    UniquePtr(T* ptr, Deleter&& deleter) : tuple_(ptr, std::move(deleter)) {}

    // =================== No copy ==================================
    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    // =================== Move ====================================
    UniquePtr(UniquePtr&& other) noexcept
        : tuple_(other.release(), std::move(other.get_deleter())) {}
    
    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this != &other) {
            reset();
            std::get<0>(tuple_) = other.release();
            get_deleter() = std::move(other.get_deleter());
        }
        return *this;
    }

    // =================== Destructor ==============================
    ~UniquePtr() {
        reset();
    }

    // =================== Observers ===============================
    T* get() noexcept { return std::get<0>(tuple_); }
    const T* get() const noexcept { return std::get<0>(tuple_); }
    explicit operator bool() const noexcept { return get() != nullptr; }

    T& operator[](size_t index) noexcept { return get()[index]; }
    const T& operator[](size_t index) const noexcept { return get()[index]; }

    Deleter& get_deleter() noexcept { return std::get<1>(tuple_); }
    const Deleter& get_deleter() const noexcept { return std::get<1>(tuple_); }

    // =================== Modifiers ===============================
    T* release() noexcept {
        T* tmp = get();
        std::get<0>(tuple_) = nullptr;
        return tmp;
    }
    
    void reset(T* ptr = nullptr) noexcept {
        if (get()) {
            get_deleter()(get());
        }
        std::get<0>(tuple_) = ptr;
    }
    
    void swap(UniquePtr& other) noexcept {
        std::swap(tuple_, other.tuple_);
    }

private:
    std::tuple<T*, Deleter> tuple_;
};

// =================== Free function ===============================

template <typename T, typename... Args>
UniquePtr<T> make_unique(Args&&... args) {
    return UniquePtr<T>(new T(std::forward<Args>(args)...));
}
