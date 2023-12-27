#include "SSE.h"

void main()
{
    char c, fname[20];
    char key[100];
    element item;
    printf("************* Command ************\n");
    printf("R: Read data, S: Search data \n");
    printf("P: Print hash table, Q: Quit \n");
    printf("**********************************\n");
    while (1) {
        printf("\nCommand> ");
        c = _getche();
        c = toupper(c);

        switch (c) {
        case 'R':
            build_filename();
            printf("\nTotal number of documents: %d\n", documents_num);
            printf("Total number of indexed words: %d\n", words_num);
            printf("Total number of comparison: %d\n", comparison_num);
            break;
        case 'S':
            printf("\nEnter keyword: ");
            scanf("%s", key);
            search_comparison = 1;
            item = hash_search(key);
            if (item.duplicate_num > 0) {
                printf("\n------------ Result ------------\n");
                printf("Keyword: %s\n", key);
                printf("Total documents: %d\n\n", item.document_count);

                doc_ptr doc = item.doc_list;
                while (doc != NULL) {
                    printf("<%s> (%s: %d)\n", doc->doc_name, key, doc->cnt);
                    for (int i = 0; i < doc->cnt; i++) {
                        printf("%s\n", doc->sentence[i]);
                    }
                    doc = doc->next;
                }

                printf("\nTotal number of comparison: %d\n", search_comparison);
            }
            else {
                printf("\nKeyword not found.\n");
            }
            break;
        case 'P':
            printf("\n");
            hash_show();
            break;
        case 'Q':
            printf("\n");
            exit(1);
        default:
            break;
        }
    }
}

void build_filename() {
    char fname[15];

    for (int i = 0; i < FILE_NUM; i++) {
        sprintf(fname, "doc%03d.txt", i + 1);
        build_dictionary(fname, i);
    }
    mergeSortAllDocLists();
}

void build_dictionary(char* fname, int fnum) {
    char* key;
    char buffer[100];
    char line[100];
    element item;

    FILE* ifp = fopen(fname, "r");

    if (ifp == NULL) {
        printf("No such file ! \n");
        return;
    }
    documents_num++;

    while (fgets(buffer, sizeof(buffer), ifp)) {
        strcpy(line, buffer);
        key = strtok(buffer, token);

        while (key != NULL) {
            _strlwr(key);
            strcpy(item.key, key);
            item.duplicate_num = 1;
            item.document_count = 1;
            item.doc_list = NULL;
            hash_insert(item, fname, line);
            key = strtok(NULL, token);
        }
    }

    fclose(ifp);
}

void hash_insert(element item, char* doc_name, char* sentence) {
    int i = hash(item.key);
    list_ptr temp, lead, trail = NULL;

    for (lead = hash_table[i]; lead; trail = lead, lead = lead->link) {
        comparison_num++;
        if (strcmp(lead->item.key, item.key) == 0) { //중복
            lead->item.duplicate_num++;
            if (strcmp(lead->item.doc_list->doc_name, doc_name) != 0) { //중복인데 새로 문서 삽입하는 경우
                doc_ptr new_doc = (doc_ptr)malloc(sizeof(doc_node));
                strcpy(new_doc->doc_name, doc_name);
                new_doc->cnt = 1;
                strcpy(new_doc->sentence[0], sentence);
                new_doc->next = lead->item.doc_list;
                lead->item.doc_list = new_doc;
                lead->item.document_count++;
            }
            else { //중복이고 기존 문서인 경우
                doc_ptr doc = lead->item.doc_list;
                while (doc != NULL) {
                    if (strcmp(doc->doc_name, doc_name) == 0) {
                        doc->cnt++;
                        strcpy(doc->sentence[doc->cnt - 1], sentence);
                        break;
                    }
                    doc = doc->next;
                }
            }
            return;
        }
    }

    temp = (list_ptr)malloc(sizeof(list_node));
    temp->item = item;
    temp->link = NULL;

    if (trail) trail->link = temp;
    else hash_table[i] = temp;

    doc_ptr new_doc = (doc_ptr)malloc(sizeof(doc_node));
    strcpy(new_doc->doc_name, doc_name);
    new_doc->cnt = 1;
    strcpy(new_doc->sentence[0], sentence);
    new_doc->next = temp->item.doc_list;
    temp->item.doc_list = new_doc;
    temp->item.document_count = 1;
    words_num++;
}

element hash_search(char* key) {
    int i = hash(key);
    list_ptr temp;

    for (temp = hash_table[i]; temp; temp = temp->link) {
        search_comparison++;
        if (strcmp(temp->item.key, key) == 0) {
            return temp->item;
        }
    }

    element not_found;
    not_found.duplicate_num = 0;
    not_found.document_count = 0;
    return not_found;
}

void hash_show() {
    list_ptr temp;

    for (int i = 0; i < TABLE_SIZE; i++) {
        for (temp = hash_table[i]; temp; temp = temp->link) {
            printf("hash_table[%d] = <%s>, duplicated = %d\n", i, temp->item.key, temp->item.duplicate_num);
        }
    }
}

int hash(char* key) {
	unsigned long hash = 5381;
	int c;

	while ((c = *key++) != '\0') {
		hash = (((hash << 5) + hash) + c) % TABLE_SIZE;
	}
	return hash % TABLE_SIZE;
}

void mergeSortDocList(doc_ptr* head) {
    doc_ptr first, second;
    if (*head == NULL || (*head)->next == NULL) {
        return;
    }

    splitList(*head, &first, &second);
    mergeSortDocList(&first);
    mergeSortDocList(&second);

    *head = mergeDocLists(first, second);
}


void splitList(doc_ptr head, doc_ptr* first, doc_ptr* second) {
    doc_ptr fast, slow;
    slow = head;
    fast = head->next;

    while (fast != NULL) {
        fast = fast->next;
        if (fast != NULL) {
            slow = slow->next;
            fast = fast->next;
        }
    }

    *first = head;
    *second = slow->next;
    slow->next = NULL;
}

doc_ptr mergeDocLists(doc_ptr first, doc_ptr second) {
    doc_ptr result = NULL;

    if (first == NULL)
        return second;
    else if (second == NULL)
        return first;

    if (first->cnt >= second->cnt) {
        result = first;
        result->next = mergeDocLists(first->next, second);
    }
    else {
        result = second;
        result->next = mergeDocLists(first, second->next);
    }

    return result;
}

void mergeSortAllDocLists() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        list_ptr temp = hash_table[i];
        while (temp != NULL) {
            mergeSortDocList(&(temp->item.doc_list));
            temp = temp->link;
        }
    }
}