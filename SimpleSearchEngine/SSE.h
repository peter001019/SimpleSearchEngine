#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define FILE_NUM 100
#define sentence_NUM 500
#define TABLE_SIZE 24593
#define boolean int
#define true 1
#define false 0

// Hash table
typedef struct {
    char key[100];
    int duplicate_num;
    int document_count;
    struct doc_node* doc_list;
} element;

typedef struct doc_node* doc_ptr;
typedef struct doc_node {
    char doc_name[20];
    int cnt;
    char sentence[sentence_NUM][500];
    doc_ptr next;
} doc_node;

typedef struct list* list_ptr;
typedef struct list {
    element item;
    list_ptr link;
} list_node;

list_ptr hash_table[TABLE_SIZE];

int comparison_num;
int search_comparison;

int documents_num;
int words_num;
char token[] = "0123456789\n\t \"\',.?[]():!-_;/";

void build_filename();
void build_dictionary(char* fname, int fnum);
void hash_insert(element item, char* doc_name, char* sentence);
element hash_search(char* key);
void hash_show();
int hash(char* key);
doc_ptr mergeDocLists(doc_ptr first, doc_ptr second);
void splitList(doc_ptr head, doc_ptr* first, doc_ptr* second);
void mergeSortDocList(doc_ptr* head);
void mergeSortAllDocLists();