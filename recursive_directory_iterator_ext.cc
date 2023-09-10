

#include "./recursive_directory_iterator_ext.h"
#include "sharedlib/sharedlib.h"


struct kautil_recursive_directory_iterator_extern_internal {
    ~kautil_recursive_directory_iterator_extern_internal(){
        if((!dl+!close) == 0)close(dl);
        delete entity_for_auto;
    }
    bool setup(
        void* (*dlopen)(const char * ,int)
        ,void* (*dlsym)(void * ,const char *)
        ,int (*dlclose)(void *)
        ,int option){
        dl = dlopen(KAUTIL_RECURSIVE_DIRECTORY_ITERATOR_SO,option);
        initialize = (decltype(kautil_recursive_directory_iterator)*) dlsym(dl,"kautil_recursive_directory_iterator");
        free = (decltype(kautil_recursive_directory_iterator_free)*) dlsym(dl,"kautil_recursive_directory_iterator_free");
        close = dlclose;
        return (!dl + !initialize + !free + !close) == 0 ;
    }
    
    decltype(kautil_recursive_directory_iterator)* initialize=0;
    decltype(kautil_recursive_directory_iterator_free)* free=0;
    void * dl=0;
    int (*close)(void *)=0;
    kautil_recursive_directory_iterator_extern * entity_for_auto=0;
    
};

kautil_recursive_directory_iterator_extern::kautil_recursive_directory_iterator_extern() : m(new kautil_recursive_directory_iterator_extern_internal){}
kautil_recursive_directory_iterator_extern::~kautil_recursive_directory_iterator_extern(){ delete m; }
kautil::filesystem::RecursiveDirectoryIterator * kautil_recursive_directory_iterator_extern::initialize(const char * p){ return m->initialize(p); }
void kautil_recursive_directory_iterator_extern::free(kautil::filesystem::RecursiveDirectoryIterator * itr){ m->free(itr); }


kautil_recursive_directory_iterator_extern * kautil_recursive_directory_iterator_extern::kautil_recursive_directory_iterator_extern_initialize(
        void* (*dlopen)(const char * ,int)
        ,void* (*dlsym)(void * ,const char *)
        ,int (*dlclose)(void *)
        ,int option
        ){
    auto res = new kautil_recursive_directory_iterator_extern{};
    if(res->m->setup(dlopen,dlsym,dlclose,option)){
        return res;
    }
    delete res;
    return nullptr;
}
void kautil_recursive_directory_iterator_extern::kautil_recursive_directory_iterator_extern_free(kautil_recursive_directory_iterator_extern * hdl){ delete hdl; }


kautil_recursive_directory_iterator_extern_internal autodel;
kautil_recursive_directory_iterator_extern * kautil_recursive_directory_iterator_extern::kautil_recursive_directory_iterator_extern_auto(){
    auto kInit = [](){ 
        autodel.entity_for_auto = new kautil_recursive_directory_iterator_extern{};
        if(false == autodel.entity_for_auto->m->setup(kautil_dlopen,kautil_dlsym,kautil_dlclose,rtld_nodelete|rtld_lazy)){
            delete autodel.entity_for_auto;
            autodel.entity_for_auto = nullptr;
        }
        return true; 
    }.operator()(); 
    return autodel.entity_for_auto;
}
