#include <memory>

namespace lilac::impl {
    constexpr size_t allocator_size = 0x8000;
    constexpr size_t frame_size = allocator_size / 32;

    static inline uint8_t allocator_pool[allocator_size];
    static inline ptrdiff_t allocator_pointer = 0x0;

    template <typename T>
    class lilac_allocator {
    public:
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;
        typedef T* pointer;
        typedef const T* const_pointer;
        typedef T& reference;
        typedef const T& const_reference;
        typedef T value_type;

        template<typename T1>
        struct rebind {
            typedef lilac_allocator<T1> other;
        };

        lilac_allocator() throw() { }

        lilac_allocator(const lilac_allocator&) throw() { }

        template<typename T1>
        lilac_allocator(const lilac_allocator<T1>&) throw() { }

        ~lilac_allocator() throw() { }

        pointer address(reference x) const { 
            return &x; 
        }

        const_pointer address(const_reference x) const { 
            return &x; 
        }

        pointer allocate(size_type n, const void* hint= 0) { 
            pointer ret = reinterpret_cast<pointer>(allocator_pool + allocator_pointer);
            allocator_pointer += n * sizeof(T);

            // pointer ret = static_cast<pointer>(::operator new(n*sizeof(T)));
            // fprintf(stderr, "Alloc %p bytes (%p).\n", (void*)(n*sizeof(T)), (void*)allocator_pointer);
            return ret;
        }

        void deallocate(pointer p, size_type n) { 
            // fprintf(stderr, "Dealloc %p bytes (%p).\n", (void*)(n*sizeof(T)), (void*)allocator_pointer);
            allocator_pointer -= n * sizeof(T);
            // ::operator delete(p); 
        }

        size_type max_size() const throw() { 
            return size_t(-1) / sizeof(T); 
        }
    };

    template<typename T>
    inline bool operator==(const lilac_allocator<T>&, const lilac_allocator<T>&) { 
        return true; 
    }

    template<typename T>
    inline bool operator!=(const lilac_allocator<T>&, const lilac_allocator<T>&) { 
        return false; 
    }
}
