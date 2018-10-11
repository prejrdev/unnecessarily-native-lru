//
// Created by <prejrdev@gmail.com> on 2018-10-11.
//

#ifndef UNNECESSARILY_NATIVE_LRU_POOL_H
#define UNNECESSARILY_NATIVE_LRU_POOL_H
namespace unlru {
    namespace pool{
        template<typename T> class Node;
        template<typename T> class Pool;
        template<typename T> class LinkedList;

        template<typename T> class ValueType{
        private:
            Node<T>* nodePtr;

        public:

            void associate(Node<T>* node, ValueType<T> *vp = nullptr){
                nodePtr = node;
                node->associate(this, node);
            }
        };


        template<typename T> class Node{
        private:
            ValueType<T> *vp;
            Pool<T>* m_pool;
            Node<T> *prev, *next;

        public:

            //the invariant of this class of objects is that each instance exist regardless
            //if it is an actual node with a value type or just some node on the free list.
            //the invariant conditions are: having the reference of a pool for the duration
            //of the object's lifetime.
            Node(Pool<T>* pool){
                if(pool == nullptr){
                    throw std::invalid_argument("pool is not specified");
                }else{
                    m_pool = pool;
                }

                this->vp = nullptr;
                this->prev = nullptr;
                this->next = nullptr;
            }

            ~Node(){

            }


            bool pool(Pool<T>* pool){
                //this would break the invariant because it allows change to m_pool which
                //we want for our class of these objects to be assumed to always be the case as being present.
                //this only changes m_pool if m_pool is specified as nullptr. the invariant for this classs is to assume
                //that any instance of the class, the objects have a reference to a proper pool.

                //this is necessary because the constructor for this class cant be provided with parameters
                //if we are block allocating.
                if(this->m_pool == nullptr){
                    this->m_pool = pool;
                    return true;
                }
                return false;
            }

            const Pool<T>* pool() const{
                return this->m_pool;
            }



            void associate(ValueType<T> *vptr, Node<T>* np = nullptr){
                this->vp = vptr;
                if(np != nullptr && np != this){
                    vptr->associate(this, vptr);
                }
            }
        };

        template<typename T> class Pool{
        public:
            Node<T>* nodes;
            const size_t total;
            size_t used;

        public:

            Pool(const size_t reserved): total(reserved), nodes(new Node<T>[reserved]){
                used = 0;
                for(int i = 0; i < reserved; i++){
                    auto n = &(nodes[i]);
                    n->pool(this);
                }
            }

            ~Pool(){
                delete [] nodes;
            }

            Node<T>* allocate(){
                //if the free list is empty, return nullptr to signal to the caller that
                //we can't provide a new node, and that it should do something to reclaim
                //one
            }

            void free(Node<T>* node){

            }
        };
    }
}
#endif //UNNECESSARILY_NATIVE_LRU_POOL_H
