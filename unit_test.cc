#ifdef TMAIN_KAUTIL_WSTD_FS_RECURSIVE_DIRECTORY_ITERATOR_STATIC

#include "recursive_directory_iterator.h"
#include <stdio.h>

int tmain_kautil_wstd_fs_walk_static(){
    const char *path="C:\\Windows\\System32";
//    while(true)
//    for(auto i = 0; i <100; ++i)
    for(auto & e : kautil::filesystem::recursive_directory_iterator(path)){
        for(auto & ee : kautil::filesystem::recursive_directory_iterator(e.path())){
            printf("%d %s\n",ee.is_directory(),ee.path());fflush(stdout);
        }
        printf("%d %s\n",e.is_directory(),e.path());fflush(stdout);
    }
    return 0;
}


int main(){
    return tmain_kautil_wstd_fs_walk_static();
}


#elif defined(TMAIN_KAUTIL_WSTD_FS_RECURSIVE_DIRECTORY_ITERATOR_SHARED)


#include <stdio.h>
#include "recursive_directory_iterator.h"
#include "sharedlib/sharedlib.h"


namespace kautil {
namespace extern_utils {

template<typename X>
struct iterator{
    explicit iterator(X * x ) :m(x){}
    iterator & operator++(){ (*m).operator++();return *this; }
    X & operator*(){ return *m; }
    bool operator!=(const iterator & l){ return *m != *l.m; }
    iterator begin(){ return iterator{&m->begin()}; }
    iterator end(){ return iterator{&m->end()}; }
private:
    X * m;
};


template<typename X>
struct scope{
    explicit scope(X *x) :m(x){}
    ~scope(){ m->~X(); }
    X * operator &(){return m;}
    X & operator *(){return *m;}
//    operator X&() { return *m; }
private:
    X * m;
};

} //namespace extern_utils {
} //namespace kautil {



int tmain_kautil_wstd_fs_walk_shared(){

    auto dl = __dlopen(PATH_TO_SHARED_LIB,rtld_lazy|rtld_nodelete);
    if(!dl){ fprintf(stderr,"fail to open sharedlib\n");throw false; }
    
    auto recursive_itr_factory = (decltype(kautil_recursive_directory_iterator)*) kautil_dlsym(dl,"kautil_recursive_directory_iterator");
    auto recursive_itr_free = (decltype(kautil_recursive_directory_iterator_free)*) kautil_dlsym(dl,"kautil_recursive_directory_iterator_free");

    if(!recursive_itr_factory){ fprintf(stderr,"fail to find symbol\n");throw false; }
    if(!recursive_itr_free){ fprintf(stderr,"fail to find symbol\n");throw false; }

//    while(true)
//    for(auto i = 0; i < 1000 ; ++i)
    {
        
        {
            auto recurse = recursive_itr_factory(".");
            for(auto & itr = recurse->begin(); itr != recurse->end(); ++itr)printf("%s\n",itr.path());
            recursive_itr_free(recurse);
        }
        
        
        {
            // cause undefined reference. i had thought this is possible.
            // could not fake undefined reference. i tried some compile/linker option 
                //    for(auto & e : *recurse){
                //          printf("%s\n",e.path());fflush(stdout);  
                //    }
            auto sc=kautil::extern_utils::scope(recursive_itr_factory("."));
            for(auto & e : kautil::extern_utils::iterator(&sc)){ printf("%s\n",e.path());fflush(stdout);
            }
        }
    }
    
    
    
    __dlclose(dl);
    
    
    
    return 0;
}

int main(){
    tmain_kautil_wstd_fs_walk_shared();
}


#endif