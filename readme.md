### kautil_recursive_directory_iterator
* prepared this to deal with the exception that occurs when using std::filesystem for windows directory.
* possible to extern via dlopen/LoadLibrary


### example 

* non extern 
```c++
#include "kautil/recursive_directory_iterator/recursive_directory_iterator.h"
#include <stdio.h>

int main(){
    const char *path="C:\\Windows\\System32";
    for(auto & e : kautil::filesystem::recursive_directory_iterator(path)){
        for(auto & ee : kautil::filesystem::recursive_directory_iterator(e.path())){
            printf("%d %s\n",ee.is_directory(),ee.path());fflush(stdout);
        }
        printf("%d %s\n",e.is_directory(),e.path());fflush(stdout);
    }
    return 0;
}
```

* extern 
```c++
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



int main(){

    auto dl = __dlopen(PATH_TO_SHARED_LIB,rtld_lazy|rtld_nodelete);
    if(!dl){ fprintf(stderr,"fail to open sharedlib\n");throw false; }

    auto recursive_itr_factory = (decltype(kautil_recursive_directory_iterator)*) kautil_dlsym(dl,"kautil_recursive_directory_iterator");
    if(!recursive_itr_factory){ fprintf(stderr,"fail to find symbol\n");throw false; }

    auto recursive_itr_free = (decltype(kautil_recursive_directory_iterator_free)*) kautil_dlsym(dl,"kautil_recursive_directory_iterator_free");
    if(!recursive_itr_free){ fprintf(stderr,"fail to find symbol\n");throw false; }

    {
        auto recurse = recursive_itr_factory(".");
        for(auto & itr = recurse->begin(); itr != recurse->end(); ++itr)printf("%s\n",itr.path());
        recursive_itr_free(recurse);
    }


    { // need some helper (kautil::extern_utils::iterator) to implement c++11 iteration. kautil::extern_utils::scope is not necessarily.   
        auto sc=kautil::extern_utils::scope(recursive_itr_factory("."));
        for(auto & e : kautil::extern_utils::iterator(&sc)){ printf("%s\n",e.path());fflush(stdout); }
    }
    
    __dlclose(dl);
    return 0;
}

```

* extern with utils (recursive_directory_iterator_ext.h)
```c++


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


#include "recursive_directory_iterator_ext.h"
#include <stdio.h>

int tmain_kautil_wstd_fs_recursive_directory_iterator_extern_static(){
    auto rdir_itr_auto = kautil_recursive_directory_iterator_extern::kautil_recursive_directory_iterator_extern_auto();
    if(!rdir_itr_auto){ fprintf(stderr,"fail to open share library"); return 1; }
    {
        auto itr = kautil::extern_utils::scope(rdir_itr_auto->initialize("."));
        for(auto & e : kautil::extern_utils::iterator(&itr)){  printf("%s\n",e.path());fflush(stdout); }
    }
    return 0;
}

int main(){
    return tmain_kautil_wstd_fs_recursive_directory_iterator_extern_static();
}

```
