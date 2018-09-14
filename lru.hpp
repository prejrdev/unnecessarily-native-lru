//
// Created by <prejrdev@gmail.com> on 2018-08-28.
//

//#define ___DEBUG__
#ifndef UNNECESSARILY_NATIVE_LRU_LRU_H
#define UNNECESSARILY_NATIVE_LRU_LRU_H

#include <cstdio>
#include <chrono>
#include <unordered_map>

namespace lru {
    namespace monotonic {
        struct mono_t {
            unsigned long ticks = 0;
        };

        /**
         *
         * @param mono
         * @return
         */
        inline unsigned long increment(mono_t &mono) {
            mono.ticks = mono.ticks + 1;
            return mono.ticks;
        }

        /**
         * it tells us how many ticks so far, but it doesn't
         * increment the ticking.
         * @param mono
         * @return
         */
        inline unsigned long now(mono_t &mono) {
            return mono.ticks;
        }
    };

    namespace cache {

        struct cache_metadata{
            unsigned int hits;
            unsigned long birth;        //birth in ticks
            unsigned long last_access;   //last access in ticks
        };

        template<typename T>
        class CacheDataWrapper {
            //it needs monotonic implemented to provide tick timing
        private:
            T m_data;
            monotonic::mono_t* mono;
            cache_metadata md;
        public:
            CacheDataWrapper(): m_data(){
                md.hits = 0;
                md.birth = 0;
                md.last_access = md.birth;
            }

            CacheDataWrapper(T data, monotonic::mono_t& time): m_data(data), mono(&time){
                md.hits = 0;
                md.birth = monotonic::now(*mono);
                md.last_access = md.birth;
            }

            T data(T data){
                return this->data(data, *mono);
            }

            T data(T data, monotonic::mono_t& time){
                md.hits = md.hits + 1;
                md.last_access = monotonic::increment(*mono);
                m_data = data;
                return m_data;
            }

            T data(){
                return data(*mono);
            }

            T data(monotonic::mono_t& time){
                md.hits = md.hits + 1;
                md.last_access = monotonic::increment(time);
                return m_data;
            }

            T inspect(){
                return m_data;
            }

            T inspect(T data){
                this->m_data = data;
                return m_data;
            }

            const cache_metadata meta(){return md;}

        };

        template<typename KeyType, typename ValueType> class LRU{
        private:
            std::unordered_map<KeyType, CacheDataWrapper<ValueType>*> map;
            monotonic::mono_t time;
        protected:

        public:

        };
    }

    namespace pool {
        template<typename T>
        class Node;

        template<typename T>
        class Pool;

        template<typename T>
        class LinkedList;

        template<typename T>
        class Node {
        private:
            T m_data;
            Pool<T> *pool;
            Node<T> *m_next, *m_prev;
        public:
            Node() : pool(nullptr), m_next(nullptr), m_prev(nullptr), m_data() {}

            Node(Pool<T> *pool) : pool(pool), m_data(), m_next(nullptr), m_prev(nullptr) {}

            ~Node() {
                //m_data = nullptr;
                pool = nullptr;
                m_next = nullptr;
                m_prev = nullptr;
            }

            bool attach(Pool<T> *pool) {
                if (this->pool == nullptr) {
                    this->pool = pool;
                    return true;
                }
                return false;
            }

            bool isOwnedByPool(Pool<T> *pool) {
                return this->pool == pool;
            };

            void free() {
                if (this->m_next != nullptr || this->m_prev != nullptr) {
                    this->m_prev = nullptr;
                    this->m_next = nullptr;
                }

                if (this->pool != nullptr) {
                    this->pool->free(this);
                }

            }

            inline void next(Node<T> *node) { this->m_next = node; }

            inline void prev(Node<T> *node) { this->m_prev = node; }

            inline Node<T> *prev() { return this->m_prev; }

            inline Node<T> *next() { return this->m_next; }

            inline void data(T data) { this->m_data = data; }

            inline T data() { return this->m_data; }
        };

        template<typename T>
        class Pool {
            /**
             * it contains a block of memory we allocated for a collecton of nodes.
             * it maintains a free list of nodes not allocated, and when asked, it provides
             * a node from the free list. when this happens it removes the node from the free list,
             * and only maintains a reference to the node via ownership of the block of memory it allocated.
             * the node resides in this block, but not in the list. because the node only has 2 referents for
             * next and prev respectively, it cannot have  membership to multiple lists. its membership to a
             * list arises from how it has information that reference symbols which allows procedures to 'continue on'
             * iterating based on that information.
             *
             * the block gets deleted, and each node destructor should be called
             */

            Node<T> *block;

            LinkedList<T> *ll;

            //total block pool size
            unsigned long block_length;

            //size of how many free nodes are left
            unsigned long free_length;

            inline Node<T> *get(unsigned int n) {
                return &(block[n]);
            }

        public:
            Pool(unsigned int size) {
                block = new Node<T>[size];
                Node<T> *elm;

                ll = new LinkedList<T>();

                for (int i = 0; i < size; i++) {
                    elm = get(i);
                    elm->attach(this);
                    ll->add(elm);
                }

                free_length = size;
                block_length = free_length;
            }

            ~Pool() {
                if (block != nullptr) {
                    delete[] block;
                    delete ll;
                    block_length = 0;
                    block = nullptr;
                }
            }


            Node<T> *allocate() {
                if(free_length > 0){
                    Node<T>* n = ll->pop();
                    free_length--;
                    return n;
                }
                return nullptr;
            }


            bool free(Node<T> *node) {
                //we dont free nodes unless they are not associated to any linked list.
                //the check for association to a linked list is via the condition that the node
                //next and prev pointers reference nothing.
                if (node != nullptr && (node->next() == nullptr && node->prev() == nullptr)) {
                    if (node->isOwnedByPool(this)) {
                        ll->add(node);
                        free_length++;
                        return true;
                    }
                }
                return false;
            }
        };

        template<typename T>
        class LinkedList {
        private:
            Node<T> *m_head;
            Node<T> *m_tail;
            unsigned long length;

            struct nodePair_t {
                Node<T> *target;
                Node<T> *other;
            };

            inline const nodePair_t isOr(Node<T> *a, Node<T> *b, Node<T> *target) {
                nodePair_t t;

                t.target = a == target ? a : (b == target ? b : nullptr);
                t.other = (t.target == nullptr) ? nullptr : (t.target == a) ? b : a;
                return t;
            }


#ifdef ___DEBUG__
            inline void _debugnode(const char *nodename, Node<T> *a, bool b = false) {

                if (b) {
                    std::cout << nodename << "" << dataornull(a) << ", ";
                    std::cout << "prev: " << dataornull(a->prev()) << ", ";
                    std::cout << "next: " << dataornull(a->next()) << std::endl;
                } else {
                    std::cout << nodename << "" << dataornull(a) << std::endl;
                }
            }

            inline std::string dataornull(Node<std::string> *a) {
                char buffer[256];
                unsigned long ptrAddr = (unsigned long) a;
                if (a == nullptr) {
                    sprintf(buffer, "node[null]: null");
                } else {
                    sprintf(buffer, "node[%s]: %d", a->data().c_str(), ptrAddr);
                }
                std::string stra = buffer;
                return stra;
            }
#define debugnode(a, b, c)  _debugnode(a, b, c)
#else
#define debugnode(a, b, c)
#define dataornull(node) ""
#endif

            inline void connect(Node<T> *a, Node<T> *prev, Node<T> *next) {
                if (prev != nullptr) {
                    if (a != prev) {
                        a->prev(prev);
                        prev->next(a);
                    } else {
                        auto *p = prev->prev();
                        if (p != nullptr) {
                            debugnode("\t\t\tp ", p, true);
                            p->next(a);

                        }
                        a->prev(p);
                    }
                } else {
                    a->prev(nullptr);
                }


                if (next != nullptr) {
                    if (a != next) {
                        next->prev(a);
                        a->next(next);
                    } else {
                        auto *n = next->next();
                        if (n != nullptr) {
                            debugnode("\t\t\tn ", n, true);
                            n->prev(a);
                        }
                        a->next(n);
                    }
                } else {
                    a->next(nullptr);
                }
                debugnode("\t\tafter ", a, true);
            }

        public:

            LinkedList() : length(0), m_head(nullptr), m_tail(nullptr) {};

            Node<T> *head() { return this->m_head; }

            Node<T> *tail() { return this->m_tail; }

            inline void push(Node<T> *node) { this->add(node); }

            void add(Node<T> *node) {
                if (node != nullptr) {

                    node->next(nullptr);
                    node->prev(nullptr);

                    if (this->m_tail == this->m_head) {
                        if (this->m_tail == nullptr) {
                            //empty list case
                            this->m_head = node;
                            this->m_tail = node;
                        } else if (this->m_head != node) {
                            //case where tail/head exist as a single entry
                            //and we want to append another node
                            node->prev(this->m_head);
                            this->m_head->next(node);
                            this->m_tail = node;
                        }
                    } else {
                        node->prev(this->m_tail);
                        node->next(nullptr);
                        this->m_tail->next(node);
                        this->m_tail = node;
                    }
                }
            }

            Node<T> *pop() {
                if (this->m_tail != nullptr) {
                    if (this->m_tail == this->m_head) {
                        auto p = this->m_tail;
                        this->m_tail = nullptr;
                        this->m_head = nullptr;
                        return p;
                    } else {
                        auto t = this->m_tail;
                        this->m_tail = t->prev();
                        if (this->m_tail != nullptr) {
                            this->m_tail->next(nullptr);
                        }
                        return t;
                    }
                }
                return nullptr;
            }

            void remove(Node<T> *node = nullptr) {
                if (node != nullptr) {
                    if (node == this->m_head) {
                        if (node == this->m_tail) {
                            //head and tail is the same
                            m_head = nullptr;
                            m_tail = nullptr;
                        } else {
                            auto a = this->m_head, an = a->next();

                            if(an != nullptr){
                                an->prev(nullptr);
                                this->m_head = an;
                            }

                            a->prev(nullptr);
                            a->next(nullptr);
                        }
                    } else if (node == this->m_tail) {
                        auto a = this->m_tail, ap = a->prev();

                        if(ap != nullptr){
                            ap->next(nullptr);
                            m_tail = ap;
                        }

                        a->prev(nullptr);
                        a->next(nullptr);
                    } else {
                        auto an = node->next(), ap = node->prev();

                        if(an != nullptr){
                            an->prev(ap);
                        }

                        if(ap != nullptr){
                            ap->next(an);
                        }

                        node->prev(nullptr);
                        node->next(nullptr);
                    }
                }
            }

            void top(Node<T> *node) {
                Node<T> *ap, *an, *bp, *bn;
                //guard
                if (node != nullptr && node != this->m_head) {

                    if (this->m_head != this->m_tail) {
                        if (node == this->m_tail) {
                            //special case for tail
                            ap = m_tail->prev();

                            m_tail->next(m_head);
                            m_head->prev(m_tail);

                            (ap != nullptr) ? ap->next(nullptr) : (void) nullptr;
                            m_tail->prev(nullptr);
                            m_head = m_tail;

                            m_tail = ap;
                        } else {
                            ap = node->prev();
                            an = node->next();

                            if (an != nullptr) {
                                an->prev(ap);
                            }

                            if (ap != nullptr) {
                                ap->next(an);
                            }

                            m_head->prev(node);
                            node->prev(nullptr);
                            node->next(m_head);
                            m_head = node;
                        }
                    }else{
                        if(m_head == nullptr){
                            node->prev(nullptr);
                            node->next(nullptr);
                            m_head = node;
                            m_tail = node;
                        }else{
                            //accomodate
                            m_head = node;
                            m_head->prev(nullptr);
                            m_head->next(m_tail);
                            m_tail->prev(m_head);
                        }
                    }
                }
            }

            void swap(Node<T> *a, Node<T> *b) {
                //cond: excludes cases where a and b are same elements or when a and b both point to a node thats both
                //the m_tail and m_head
                if ((a != b) && (a != nullptr && b != nullptr)) {
                    auto ap = a->prev(), an = a->next(), bp = b->prev(), bn = b->next();

                    //changing the next/prev references changes what that node is referencing to.
                    //the node has reference that identifies its predecessor and next entry
                    //but to connect that node back into b place, the nodes that references b
                    //needs to reference a instead

                    //swap functionality:

                    //first-case: adjacent detection
                    if (ap == b || an == b || bp == a || bn == a) {

                        if (an == b && bp == a) {
                            //a follows b case
                            //a = [0] {p: 3, n: 4}
                            //b = [4] {p: 0, n: 1}
                            //a -- b

                            //it needs to become b -- a
                            //a = [0] {p: b, n: 1}
                            //b = [4] {p: 3, n: a}

                            a->prev(b);
                            b->next(a);

                            a->next(bn);
                            if (bn != nullptr) {
                                bn->prev(a);
                            }


                            b->prev(ap);
                            if (ap != nullptr) {
                                ap->next(b);
                            }

                        } else if (ap == b && bn == a) {
                            //a = [4] {p: 0, n: 1},
                            //b = [0] {p: 3, n: 4}
                            //b -- a

                            //a -- b
                            //a = [4]{p: 3/bp, n: b}
                            //b = [0]{p: a, n: 1/an}

                            a->prev(bp);
                            if (bp != nullptr) {
                                bp->next(a);
                            }
                            a->next(b);

                            b->prev(a);
                            b->next(an);
                            if (an != nullptr) {
                                an->prev(b);
                            }
                        }

                    } else {
                        connect(a, bp, bn);
                        connect(b, ap, an);
                    }

                    auto ptrHeadPair = isOr(a, b, this->m_head);
                    auto ptrTailPair = isOr(a, b, this->m_tail);

                    //relabeling functionality
                    if (ptrHeadPair.target == ptrTailPair.target) {
                        //the semantic for swap now becomes a semantic for appending to a list
                        auto head = ptrHeadPair.target, other = ptrHeadPair.other;
                        if (other != nullptr) {
                            if (a == head) {
                                //reassign what is m_tail
                                this->m_tail = b;
                            } else {
                                this->m_head = a;
                            }
                        }
                    } else {
                        if (ptrHeadPair.target != nullptr) {
                            this->m_head = ptrHeadPair.other;
                        }

                        if (ptrTailPair.target != nullptr) {
                            this->m_tail = ptrTailPair.other;
                        }
                    }

                }

                return;
            }

            inline void clear() {
                while (this->m_head != nullptr) {
                    this->pop();
                }
            }
        };



    };
}
#endif //UNNECESSARILY_NATIVE_LRU_LRU_H
