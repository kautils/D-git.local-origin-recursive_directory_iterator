#ifndef KAUTIL_FILESYSTEM_RECURSIVE_DIRECTORY_ITERATOR_EXT_H
#define KAUTIL_FILESYSTEM_RECURSIVE_DIRECTORY_ITERATOR_EXT_H

#include "recursive_directory_iterator.h"

namespace kautil{
    
    struct recursive_directory_iterator_extern_internal;
    struct recursive_directory_iterator_extern{
        using self_type = recursive_directory_iterator_extern;
        static self_type * auto_dl();
        static self_type * load_dl(
            void* (*dlopen)(const char * ,int)
            ,void* (*dlsym)(void * ,const char *)
            ,int (*dlclose)(void *)
            ,int option);
        static void close_dl(self_type * hdl);
        recursive_directory_iterator_extern();
        ~recursive_directory_iterator_extern();
        kautil::filesystem::RecursiveDirectoryIterator * initialize(const char * p);
        void free(kautil::filesystem::RecursiveDirectoryIterator *);
        
    private:
        recursive_directory_iterator_extern_internal  * m =0;
    };
    
}







#endif