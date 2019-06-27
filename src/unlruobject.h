//
// Created by <prejrdev@gmail.com>
//

#ifndef UNNECESSARILY_NATIVE_LRU_UNLRUOBJECT_H
#define UNNECESSARILY_NATIVE_LRU_UNLRUOBJECT_H

#include "lru.h"

namespace unlru {
    namespace addon {
        class unlruobject : public node::ObjectWrap {
        protected:
            std::unique_ptr<lru> ptr_lru;
            size_t reserved, hits;

            explicit unlruobject(size_t reservedSize);

        public:
            ~unlruobject();

            static v8::Persistent<v8::Function> constructor;
            static v8::Persistent<v8::FunctionTemplate> fnTemplate;

            static void Init(v8::Isolate *isolate);

            static void Constructor(const v8::FunctionCallbackInfo<v8::Value> &args);


            static void NewInstance(const v8::FunctionCallbackInfo<v8::Value> &args);

            static void Set(const v8::FunctionCallbackInfo<v8::Value> &args, v8::Isolate *isolate,
                            const v8::Local<v8::Object> &self, unlruobject &obj, unsigned int argc);

            static void Get(const v8::FunctionCallbackInfo<v8::Value> &args, v8::Isolate *isolate,
                            const v8::Local<v8::Object> &self, unlruobject &obj, unsigned int argc);

            static void Evict(const v8::FunctionCallbackInfo<v8::Value> &args, v8::Isolate *isolate,
                              const v8::Local<v8::Object> &self, unlruobject &obj, unsigned int argc);

            static void Metadata(const v8::FunctionCallbackInfo<v8::Value> &args, v8::Isolate *isolate,
                                 const v8::Local<v8::Object> &self, unlruobject &obj, unsigned int argc);

            static void Clear(const v8::FunctionCallbackInfo<v8::Value> &args, v8::Isolate *isolate,
                              const v8::Local<v8::Object> &self, unlruobject &obj, unsigned int argc);

            static void Has(const v8::FunctionCallbackInfo<v8::Value> &args, v8::Isolate *isolate,
                            const v8::Local<v8::Object> &self, unlruobject &obj, unsigned int argc);


            static void Reserved(const v8::PropertyCallbackInfo<v8::Value> &info, v8::Isolate *isolate,
                                 const v8::Local<v8::Object> &self, unlruobject &obj);

            static void Remaining(const v8::PropertyCallbackInfo<v8::Value> &info, v8::Isolate *isolate,
                                  const v8::Local<v8::Object> &self, unlruobject &obj);

            static void Length(const v8::PropertyCallbackInfo<v8::Value> &info, v8::Isolate *isolate,
                               const v8::Local<v8::Object> &self, unlruobject &obj);
        };


        inline static bool isInstance(v8::Isolate *isolate, const v8::Local<v8::Object> &self, unlruobject *&instance) {
            if (unlruobject::fnTemplate.Get(isolate)->HasInstance(self)) {
                instance = node::ObjectWrap::Unwrap<unlruobject>(self);
                return true;
            } else {
                instance = nullptr;
                TypeException(isolate, "cannot access as this is not an UnLRU object");
                return false;
            }
        }

        typedef void (*WrappedFunction)(const v8::FunctionCallbackInfo<v8::Value> &args, v8::Isolate *isolate,
                                        const v8::Local<v8::Object> &self, unlruobject &obj, unsigned int argc);

        typedef void (*WrappedGetAccessor)(const v8::PropertyCallbackInfo<v8::Value> &info, v8::Isolate *isolate,
                                           const v8::Local<v8::Object> &self, unlruobject &obj);

        void noop(v8::Isolate *isolate, const v8::Local<v8::Object> &self, unlruobject &obj);

        template <
            WrappedFunction Op,
            const v8::Persistent<v8::FunctionTemplate> &instanceRef = unlruobject::fnTemplate
        >
        class v8FunctionWrapper {
        public:
            static void Wrapped(const v8::FunctionCallbackInfo<v8::Value> &args) {
                v8::Isolate *isolate = args.GetIsolate();
                v8::Local<v8::Object> self = args.This();
                unsigned int argc = (unsigned) args.Length();
                unlruobject *instance;

                if (instanceRef.Get(isolate)->HasInstance(self)) {
                    instance = node::ObjectWrap::Unwrap<unlruobject>(self);
                    if (instance != nullptr) {
                        Op(args, isolate, self, *instance, argc);
                    } else {
                        TypeException(isolate, "internal reference to the UnLRU object is invalid");
                    }
                } else {
                    TypeException(isolate, "cannot access as this is not an UnLRU object");
                }
            }
        };


        template <
            WrappedGetAccessor Op,
            const v8::Persistent<v8::FunctionTemplate> &instanceRef = unlruobject::fnTemplate
        >
        class v8GetAccessorWrapper {
        public:
            static void Wrapped(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info) {
                v8::Isolate *isolate = info.GetIsolate();
                v8::Local<v8::Object> self = info.This();
                unlruobject *instance;

                if (instanceRef.Get(isolate)->HasInstance(self)) {
                    instance = node::ObjectWrap::Unwrap<unlruobject>(self);
                    if (instance != nullptr) {
                        Op(info, isolate, self, *instance);
                    } else {
                        TypeException(isolate, "internal reference to the UnLRU object is invalid");
                    }
                } else {
                    TypeException(isolate, "cannot access as this is not an UnLRU object");
                }
            }
        };
    };
};


#endif //UNNECESSARILY_NATIVE_LRU_UNLRUOBJECT_H
