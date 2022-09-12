#include <stdio.h> 
#include <string.h> 
#include <ctype.h> 
#include <fcntl.h> 
#include <unistd.h> 
#include <sys/syscall.h> 
#include <dirent.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <time.h> 
#include <stdlib.h>
#define MAX_FILENAME 256

void monitorFile(char *fileName); 
void processDirectory(char *dirName);

struct Node { 
	long int fileSize; 
	char* fileName; 
	struct Node* next;
};

void insertAtTheBegin(struct Node **start_ref, char* fileName);
void bubbleSort(struct Node **start, int count); 
void printList(struct Node *start); 
void freeList(struct Node *start);
struct Node* swap(struct Node *a, struct Node *b);
long int findSize(char* fileName);

enum {FALSE, TRUE};

struct Node *start = NULL; 
int fileCount = 0;

int main(int argc, char *argv[]){
	if(argc == 1){ 
		printf("No arguments passed.\n");
		return 0;
	}
	char* userPath = argv[1];
	processDirectory(userPath);
	bubbleSort(&start, fileCount);
	printList(start);
	freeList(start);
	return(0);
}

void monitorFile(char *fileName){ 
	struct stat statBuf; 
	mode_t mode; 
	int result; 

	result = lstat(fileName, &statBuf);
	if(result == -1){ 
		fprintf(stderr, "Cannot stat %s \n", fileName); 
		return; 
	}
        mode = statBuf.st_mode; 
 	if(S_ISDIR(mode)){ 
		processDirectory(fileName);
	}         
	else if(S_ISREG(mode)){ 
		fileCount = fileCount + 1;
		insertAtTheBegin(&start, fileName);
	}
} 

void processDirectory(char *dirName){  
	DIR *dir; 
	struct dirent *dirEntry; 
	char fileName[MAX_FILENAME]; 

	dir=opendir(dirName); 
	while(TRUE){
		dirEntry = readdir(dir);
		if(dirEntry == NULL){
		       	break;
		}	
		if(strcmp(dirEntry->d_name, ".") != 0 && strcmp(dirEntry->d_name, "..") != 0){ 
			sprintf(fileName, "%s/%s", dirName, dirEntry->d_name); 
			monitorFile(fileName); 
		}
	}
}

void insertAtTheBegin(struct Node **start_ref, char* fileName){ 
	struct Node *ptr1 = (struct Node*)malloc(sizeof(struct Node)); 
	ptr1->fileSize = findSize(fileName);
	ptr1->fileName = malloc(strlen(fileName) + 1); 
	strcpy(ptr1->fileName, fileName);
	ptr1->next = *start_ref; 
	*start_ref = ptr1; 
}

long int findSize(char* fileName){
	FILE* fp = fopen(fileName, "r"); 
	fseek(fp, 0L, SEEK_END); 
	long int res = ftell(fp); 
	fclose(fp); 
	return res;
} 

void bubbleSort(struct Node **start, int count){ 
	struct Node** h;
	int i, j, swapped; 

	for(i = 0; i <= count; i++){
		h = start;
		swapped = 0; 
		for(j = 0; j < count - i - 1; j++){
			struct Node* p1 = *h; 
			struct Node* p2 = p1->next; 
			if(p1->fileSize > p2->fileSize){ 
				*h = swap(p1, p2); 
				swapped = 1;
			} 

			h = &(*h)->next;
		} 

		if(swapped == 0){ 
			break;
		}
	}
}

struct Node* swap(struct Node *a, struct Node *b){ 
	struct Node* tmp = b-> next; 
	b->next = a; 
	a->next = tmp; 
	return b;
}

void printList(struct Node *start){ 
	struct Node *temp = start;  
	while(temp != NULL){ 
		printf("%ld\t%s\n", temp->fileSize, temp->fileName); 
		temp = temp->next;
	}	
} 

void freeList(struct Node *start){ 
	struct Node* tmp = NULL;
	while(start){ 
		tmp = start->next; 
		free(start->fileName); 
		free(start); 
		start = tmp;
	}	

}
