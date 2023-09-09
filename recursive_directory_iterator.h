#ifndef KAUTIL_GENERAL_FILE_WALK_H
#define KAUTIL_GENERAL_FILE_WALK_H

#include <stdint.h>




namespace kautil{
namespace filesystem{


struct RecursiveDirectoryIteratorInternal;
struct recursive_directory_iterator_reference;
struct RecursiveDirectoryIterator{
    
    using self_type = RecursiveDirectoryIterator;
    RecursiveDirectoryIterator(RecursiveDirectoryIterator const& l);
    virtual RecursiveDirectoryIterator & operator=(RecursiveDirectoryIterator const& l );
    
    virtual self_type & operator++();
    virtual bool operator!=(const self_type & l) const;
    virtual self_type & begin();
    virtual self_type & end();
    
    /// @note i want to prevent reference to call begin / end recursively(inside foreach) and to avoid copying members of __A.  
    virtual recursive_directory_iterator_reference & operator*();
    virtual ~RecursiveDirectoryIterator();
    
    RecursiveDirectoryIterator(const char * path);
    
    virtual bool is_directory();
    virtual const char * path() const;

private:
    RecursiveDirectoryIteratorInternal * m = 0;
};


struct recursive_directory_iterator_reference : private RecursiveDirectoryIterator{ 
    recursive_directory_iterator_reference()=default;
    virtual ~recursive_directory_iterator_reference()=default;
    using RecursiveDirectoryIterator::path; 
    using RecursiveDirectoryIterator::is_directory;
};

RecursiveDirectoryIterator recursive_directory_iterator(const char * p);

} //namespace filesystem{
} //namespace kautil{

extern "C" kautil::filesystem::RecursiveDirectoryIterator * kautil_recursive_directory_iterator(const char * p);
extern "C" void kautil_recursive_directory_iterator_free(kautil::filesystem::RecursiveDirectoryIterator * itr);







#endif