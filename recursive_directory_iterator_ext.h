#ifndef KAUTIL_FILESYSTEM_RECURSIVE_DIRECTORY_ITERATOR_EXT_H
#define KAUTIL_FILESYSTEM_RECURSIVE_DIRECTORY_ITERATOR_EXT_H

#include "recursive_directory_iterator.h"

struct kautil_recursive_directory_iterator_extern_internal;
struct kautil_recursive_directory_iterator_extern{
    static kautil_recursive_directory_iterator_extern * kautil_recursive_directory_iterator_extern_auto();
    static kautil_recursive_directory_iterator_extern * kautil_recursive_directory_iterator_extern_initialize(
        void* (*dlopen)(const char * ,int)
        ,void* (*dlsym)(void * ,const char *)
        ,int (*dlclose)(void *)
        ,int option);
    static void kautil_recursive_directory_iterator_extern_free(kautil_recursive_directory_iterator_extern * hdl);
    
    kautil_recursive_directory_iterator_extern();
    ~kautil_recursive_directory_iterator_extern();
    kautil::filesystem::RecursiveDirectoryIterator * initialize(const char * p);
    void free(kautil::filesystem::RecursiveDirectoryIterator *);
private:
    kautil_recursive_directory_iterator_extern_internal  * m =0;
};






#endif