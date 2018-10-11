//
// Created by <prejrdev@gmail.com> on 2018-10-08.
//

#ifndef UNNECESSARILY_NATIVE_LRU_MONOTONIC_H
#define UNNECESSARILY_NATIVE_LRU_MONOTONIC_H
namespace unlru{
    namespace monotonic{
        class MonoTimer{
        private:
            unsigned long t;
        public:
            MonoTimer(): t(0){
            }

            inline unsigned long now() const{
                return t;
            };


            inline const MonoTimer& tick(){
                t = t + 1;
                return *this;
            };
        };


    };
};
#endif //UNNECESSARILY_NATIVE_LRU_MONOTONIC_H
