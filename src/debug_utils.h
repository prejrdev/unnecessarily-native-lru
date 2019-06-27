//
// Created by <prejrdev@gmail.com>
//

#ifndef UNNECESSARILY_NATIVE_LRU_DEBUG_UTILS_H
#define UNNECESSARILY_NATIVE_LRU_DEBUG_UTILS_H

#ifndef __DEBUG__
#define MARK_HERE
#define DEBUG(format, ...)
#else
#define MARK_HERE printf("%s:%d\n", __PRETTY_FUNCTION__, __LINE__)
#define DEBUG(format, ...) \
    printf("%s:%d: " format "\n", __PRETTY_FUNCTION__, __LINE__, ##__VA_ARGS__)
#endif



#endif //UNNECESSARILY_NATIVE_LRU_DEBUG_UTILS_H
