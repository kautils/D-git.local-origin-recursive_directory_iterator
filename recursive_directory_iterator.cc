
#include "./recursive_directory_iterator.h"
#include "dirent.h"
#include <stdio.h>
#include <cstdlib>
#include "sys/stat.h"
#include <string>
#include <vector>

#include "c11_string_allocator/c11_string_allocator.h"



namespace kautil{
namespace filesystem{


struct RecursiveDirectoryIterator;
struct RecursiveDirectoryIteratorInternal{
    
    struct st_level{
        DIR *dir =0; 
        uint32_t pos =0;
        struct dirent * dp=0;
    };
    
    std::vector<st_level*> lev;
    std::string path;
    std::string path_cur;
    int state = 0; // 0 : beg , 2 : end 
    int level = 0;
    bool root = true;
    std::string * membuffer = 0;
    

    void release_lev();
    void cpy(RecursiveDirectoryIteratorInternal * l) const; 
    bool is_dir(const char *dir);
    void set_dir(const char * path);
    void reset();
    bool next();
};


void RecursiveDirectoryIteratorInternal::release_lev(){
    for(auto l : lev){
        if(l->dir)closedir(l->dir);
        delete l;

    } 
    lev.resize(0);
}
    
void RecursiveDirectoryIteratorInternal::cpy(RecursiveDirectoryIteratorInternal * l) const {
    *l = *this;
    l->root = true;
    l->membuffer = nullptr;
    l->lev.resize(0);
}

bool RecursiveDirectoryIteratorInternal::is_dir(const char *dir){
    struct stat st;
    stat(dir,&st);
    return S_ISDIR(st.st_mode);
}

void RecursiveDirectoryIteratorInternal::reset(){
    path_cur =path;
    state = 0;
    level = 0;
}

void RecursiveDirectoryIteratorInternal::set_dir(const char * p){
    path = p;
    path_cur = p;
    if(level >= lev.size()){
        lev.emplace_back(new RecursiveDirectoryIteratorInternal::st_level{});
    }
    auto cur = lev.at(level);
    if(cur->dir) closedir(cur->dir);
    cur->dir = opendir(path_cur.data());
    cur->pos = path_cur.size();
}




//
//
recursive_directory_iterator_reference & RecursiveDirectoryIterator::operator*(){ return (recursive_directory_iterator_reference &)*this; }
////RecursiveDirectoryIterator::self_type & RecursiveDirectoryIterator::operator*(){ return *this; }
//
//
//void /*RecursiveDirectoryIterator::*/fake_copy_constructor(RecursiveDirectoryIterator & r,RecursiveDirectoryIterator const& l){
//    r.m = new RecursiveDirectoryIteratorInternal;
//    l.m->cpy(r.m);
//    r.m->set_dir(r.m->path.data());
//}

RecursiveDirectoryIterator::RecursiveDirectoryIterator(RecursiveDirectoryIterator const& l) {
    m = new RecursiveDirectoryIteratorInternal;
    l.m->cpy(m);
    m->set_dir(m->path.data());
}

RecursiveDirectoryIterator & RecursiveDirectoryIterator::operator=(RecursiveDirectoryIterator const& l ){
    m = new RecursiveDirectoryIteratorInternal;
    l.m->cpy(m);
    m->set_dir(m->path.data());
    return *this;
}

RecursiveDirectoryIterator::RecursiveDirectoryIterator(const char * path) : m(new RecursiveDirectoryIteratorInternal){ m->path = path ;m->path_cur = path; }
RecursiveDirectoryIterator::~RecursiveDirectoryIterator(){ 
    if(m && m->root){
        if(m->membuffer){
            auto arr_len =m->membuffer->size() / sizeof(uintptr_t);
            auto arr = reinterpret_cast<RecursiveDirectoryIterator**>(m->membuffer->data());
            for(auto i = 0; i < arr_len; ++i){
                arr[i]->m->release_lev();
                delete arr[i]->m;
                arr[i]->m =nullptr;
                delete arr[i];
            }
            delete m->membuffer;
            m->membuffer = nullptr; 
        }
        m->release_lev();
    }
    delete m; 
}

bool RecursiveDirectoryIterator::is_directory(){ return m->is_dir(m->path_cur.data()); }

const char * RecursiveDirectoryIterator::path() const { return m->path_cur.data(); }

bool RecursiveDirectoryIteratorInternal::next(){


    state = 1;
    if(path_cur.empty()) return 0;
    if(level >= lev.size()){
        lev.emplace_back(new RecursiveDirectoryIteratorInternal::st_level{});
    }
    auto cur = lev.at(level);
    auto dp = readdir(cur->dir);

    if(dp){
        if(is_dir(path_cur.data())){
            do{
                if(0 != (!strcmp(dp->d_name,".")+!strcmp(dp->d_name,"..")))continue;
                break;
            }while ( (dp=readdir(cur->dir)) );
        }
    }


    if(dp){

        { 
            auto new_len = cur->pos+dp->d_namlen+1;
            if(cur->pos < new_len) path_cur.resize(new_len);
            else path_cur[new_len]=0;
            path_cur[cur->pos] = '/';
            strcpy(&path_cur[cur->pos+1],dp->d_name);
        }


        if(is_dir(path_cur.data())){
            level++;
            set_dir(path_cur.data());
        }

        return 1;
    }

    if(level){
        --level;
        return next();
    }else{
        state = 2;
        return 0;
    }
}

RecursiveDirectoryIterator::self_type & RecursiveDirectoryIterator::operator++(){ m->next();return *this; }
bool RecursiveDirectoryIterator::operator!=(const self_type & l) const { return m->state != l.m->state; }

RecursiveDirectoryIterator::self_type & RecursiveDirectoryIterator::begin(){
    auto res = new RecursiveDirectoryIterator(m->path.data()); 
    if(!m->membuffer) m->membuffer = new std::string{};
    c11_string_register(m->membuffer, c11_string_current_pos(m->membuffer),res);
    m->reset();
    m->cpy(res->m);
    res->m->membuffer = nullptr;
    res->m->root = false;
    res->m->state = 0;
    res->m->set_dir(res->m->path.data());
    return *res;
}

RecursiveDirectoryIterator::self_type & RecursiveDirectoryIterator::end(){
    auto res = new RecursiveDirectoryIterator(m->path.data()); 
    if(!m->membuffer) m->membuffer = new std::string{}; 
    c11_string_register(m->membuffer, c11_string_current_pos(m->membuffer),res);
    //m->cpy(res->m); // not need necesswarily
    res->m->membuffer = nullptr;
    res->m->root = false;
    res->m->state = 2;
    return *res;
}

extern "C" kautil::filesystem::RecursiveDirectoryIterator * recursive_directory_iterator(const char * p){ return new kautil::filesystem::RecursiveDirectoryIterator{p}; }
extern "C" void recursive_directory_iterator_free(RecursiveDirectoryIterator * itr){ delete itr; }

} //namespace filesystem{
} //namespace kautil{


