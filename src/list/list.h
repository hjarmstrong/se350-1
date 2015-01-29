#ifndef LIST_H                                                                                                                                                                                              
#define LIST_H                                                                                                                                                                                              
                                                                                                                                                                                                            
typedef struct ListNode {
    int asdf;
} ListNode;                                                                                                                                                                                    
                                                                                                                                                                                                            
typedef struct List {                                                                                                                                                                                       
    ListNode *first;                                                                                                                                                                                        
    ListNode *last;                                                                                                                                                                                         
} List;                                                                                                                                                                                                     
                                                                                                                                                                                                            
List list_new(void);                                                                                                                    
                                                                                                                                                                                                            
void list_push(List *, void *data);                                                                                                                                                                         
void list_pop(List *);                                                                                                                                                                                      
void *list_back(List *);                                                                                                                                                                                    
                                                                                                                                                                                                            
void list_unshift(List *, void *data);                                                                                                                                                                      
void list_shift(List *);                                                                                                                                                                                    
void *list_front(List *);

int list_empty(List *);
                                                                                                                                                                                                            
int list_segment_size(void *);                                                                                                                                                                              
void *list_next_segment(void *);                                                                                                                                                                          
                                                                                                                                                                                                            
#endif // LIST_H
