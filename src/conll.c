#include "conll.h"

conll_file_t create_CoNLLFile(const char* conll_basedir, int conll_section, const char* conll_file ){
    //sprintf(fullpath, "%s/%02d/%s", dir, section, entry->d_name);
    
    
    conll_file_t file = (conll_file_t)malloc(sizeof(struct conll_file_t));
    
    check(file != NULL, "Allocation error in CoNLL file");
    
    
    file->section_dir = (char*)malloc(sizeof(char) * ( strlen(conll_basedir) + 1 + 2));
    check(file->section_dir != NULL, "Allocation error in section directory");
    
    file->section = conll_section;
    sprintf(file->section_dir, "%s/%02d", conll_basedir, file->section);
    
    file->file = strdup(conll_file);
    
    file->fullpath = (char*)malloc(sizeof(char) * ( strlen(file->section_dir) + 1 + strlen(file->file)));
    check(file->fullpath != NULL, "Allocation error in CoNLL full path");
    
    sprintf(file->fullpath, "%s/%s", file->section_dir, file->file);
    
    return file;
    
error:
        exit(1);
}



