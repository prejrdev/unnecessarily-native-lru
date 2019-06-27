//
// Created by <prejrdev@gmail.com>
//

#ifndef UNNECESSARILY_NATIVE_LRU_METADATAOBJECT_H
#define UNNECESSARILY_NATIVE_LRU_METADATAOBJECT_H


#include "unlru/metadata.h"

namespace unlru {
    namespace addon {
        class MetadataObject : public node::ObjectWrap {
        public:

            //these guys stay alive for the duration that the module is loaded.
            static v8::Persistent<v8::Function> constructor;
            static v8::Persistent<v8::FunctionTemplate> funcTemplate;

            //initializes the constructor and object template
            static void Init(v8::Isolate *isolate);

            static void ObjectConstructor(const v8::FunctionCallbackInfo<v8::Value> &args);

            static void NewInstance(const v8::FunctionCallbackInfo<v8::Value> &args);


            /**
             * this should be called by the LRU instance if it needs to create a MetadataObject instance initalized
             * to a specific set of values.
             * @param isolate
             * @param metadata
             * @return
             */
            static v8::Local<v8::Object>
            NewFromMetadata(v8::Isolate *isolate, const unlru::proxy::Metadata_t &metadata);

        };

    };
};

#endif //UNNECESSARILY_NATIVE_LRU_METADATAOBJECT_H
