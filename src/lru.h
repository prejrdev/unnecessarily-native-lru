//
// Created by <prejrdev@gmail.com>
//

#ifndef UNNECESSARILY_NATIVE_LRU_LRU_H
#define UNNECESSARILY_NATIVE_LRU_LRU_H

#include "unlru/metadata.h"
#include "unlru/proxy.h"
#include "unlru.h"

namespace unlru{
    namespace addon{
        /**
         * takes ownership of a v8::Persistent<v8::Value> pointer and handles its clean up
         */
        class PersistentValue{
        private:
            v8::Persistent<v8::Value>* ptr;
        public:
            PersistentValue():ptr(nullptr){};
            PersistentValue(v8::Persistent<v8::Value>* ptr): ptr(ptr){};

            //copy constructor
            PersistentValue(PersistentValue const& ref) = delete;

            inline v8::Persistent<v8::Value>* get(){return ptr;};


            ~PersistentValue(){
                DEBUG("bye from PersistentValue %p\n", this);
                free();
            }

            inline void free(){
                if(this->ptr != nullptr){
                    ptr->SetWeak();
                    ptr->Reset();
                    DEBUG("free called on %p\n", ptr);
                    delete ptr;
                    ptr = nullptr;
                }
            }

        };

        using PersistentProxy = unlru::proxy::TimedAccessor<PersistentValue>;

       
        class lru {
        private:
            unlru::monotonic::MonoTimer timer;

            std::unique_ptr<PersistentProxy> owner_persistent;
            size_t _reserved;
            int __test_counter = 0;
        public:

            lru(size_t reservedSize);
            ~lru();

            /**
             * evicts the entry specified by key
             * @param key
             * @return the object being evicted from the lru
             */
            std::unique_ptr<PersistentValue> evict(const std::string& key);

            /**
             * evicts the least recently used entry as indexed by our doubly-linked list
             * and returns a unique_ptr which has ownership of the object.
             * @return the evicted object
             */
            std::unique_ptr<PersistentValue> purge();


            v8::Persistent<v8::Value>* get(const std::string& key);

            v8::Persistent<v8::Value>* set(const std::string& key, v8::Persistent<v8::Value> *value);

            proxy::Metadata_t metadata(const std::string& key, bool &isValid);

            //emptys the cache.
            void clear();

            inline int reserved(){
                return (int) _reserved;
            };

            inline int length(){
                return 10;
            };

            inline int remaining(){
                return reserved() - length();
            };
        };
    }
};


#endif //UNNECESSARILY_NATIVE_LRU_LRU_H
