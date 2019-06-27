//
// Created by <prejrdev@gmail.com> on 2018-10-23.
//

#ifndef UNNECESSARILY_NATIVE_LRU_V8_EXCEPTIONS_H
#define UNNECESSARILY_NATIVE_LRU_V8_EXCEPTIONS_H

namespace unlru {

    //TypeError
    inline void TypeException(v8::Isolate *isolate, const char *message) {
        isolate->ThrowException(
                v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, message))
        );
    }

    //SyntaxError
    inline void SyntaxException(v8::Isolate *isolate, const char *message) {
        isolate->ThrowException(
                v8::Exception::SyntaxError(v8::String::NewFromUtf8(isolate, message))
        );
    }

    //ReferenceError: when there is an error with a reference (nullptr)
    inline void ReferenceException(v8::Isolate *isolate, const char *message) {
        isolate->ThrowException(
                v8::Exception::ReferenceError(v8::String::NewFromUtf8(isolate, message))
        );
    }
}

#endif //UNNECESSARILY_NATIVE_LRU_V8_EXCEPTIONS_H
