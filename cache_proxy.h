//
// Created by <prejrdev@gmail.com> on 2018-10-08.
//

#ifndef UNNECESSARILY_NATIVE_LRU_CACHE_PROXY_H
#define UNNECESSARILY_NATIVE_LRU_CACHE_PROXY_H

#include "monotonic.h"

namespace unlru{
    namespace proxy{

        struct Metadata_t{
            unsigned long hits = 0;
            unsigned long birth = 0;
            unsigned long last = 0;

            inline void reset(){
                hits = birth = last = 0;
            }

            inline void reset(const unlru::monotonic::MonoTimer& timer){
                hits = 0;
                birth = timer.now();
                last = timer.now();
            }
        };

        /**
         * each instance of TimeAcesssor keeps track of how many times and when the
         * underlying type is accesssed. 
         */
        template <typename Type> class TimedAccessor{
        private:
            Metadata_t md;
            Type* pointer;

        public:
            TimedAccessor(): pointer(nullptr){}

            TimedAccessor(Type* pointer, const unlru::monotonic::MonoTimer& time): pointer(pointer){
                md.birth = time.now();
                md.last = time.now();
                md.hits = 0;
            }

            TimedAccessor(TimedAccessor const &) = delete;
            TimedAccessor& operator=(TimedAccessor& other) = delete;


            TimedAccessor(TimedAccessor&& other) noexcept{
                md.last = other.md.last;
                md.birth = other.md.birth;
                md.hits = other.md.hits;
                other.md.reset();

                pointer = other.pointer;
                other.pointer = nullptr;
            }

            TimedAccessor& operator=(TimedAccessor&& other) noexcept {
                md.last = other.md.last;
                md.birth = other.md.birth;
                md.hits = other.md.hits;
                other.md.reset();

                pointer = other.pointer;
                other.pointer = nullptr;
            }

            ~TimedAccessor(){
                if(pointer != nullptr){
                    delete pointer;
                }
            }


            void Reset(){
                if(pointer != nullptr){
                    delete pointer;
                    pointer = nullptr;
                }
            }

            void Reset(Type* pointer, const unlru::monotonic::MonoTimer& timer){
                if(pointer != this->pointer && pointer != nullptr){
                    if(this->pointer != nullptr) {
                        delete this->pointer;
                        this->pointer = nullptr;
                    }

                    this->pointer = pointer;
                    this->md.reset(timer);
                }

            }

            const bool IsEmpty() const{
                return pointer != nullptr;
            }

            const Type* Inspect() const{
                return pointer;
            }

            /**
             * copy-by-value
             * @param data
             * @return the internal handle after being updated.
             */
            const Type* Inspect(const Type& data){
                if(this->pointer != nullptr){
                    *(this->pointer) = data;
                }
                return this->pointer;
            }

            Type* Access(const unlru::monotonic::MonoTimer &timer){
                md.last = timer.now();
                md.hits += 1;
                return pointer;
            }

            Type* Access(Type* ptr, const unlru::monotonic::MonoTimer &timer){
                if(ptr != nullptr && ptr != pointer){
                    pointer = ptr;
                    md.birth = timer.now();
                    md.last = timer.now();
                    md.hits = 1;
                }
                return pointer;
            }

            inline const Metadata_t& Metadata() const{
                return md;
            }

        };
    };
};

#endif //UNNECESSARILY_NATIVE_LRU_CACHE_PROXY_H
