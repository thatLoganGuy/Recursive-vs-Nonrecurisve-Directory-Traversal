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

void monitorFile(char* fileName); 

struct fileNode{ 
	long int fileSize; 
	char* fileName; 
	struct fileNode* next;
};  

struct directNode{ 
	char* directName; 
	struct directNode* next;
};

void addLast(struct directNode** start, char* fileName); 

void addStart(struct fileNode** start, char* fileName);
void bubbleSort(struct fileNode** start, int count);
void printList(struct fileNode* start); 
void freeFileList(struct fileNode* start); 
void freeDirectList(struct directNode* start);
struct fileNode* swap(struct fileNode* a, struct fileNode* b);
long int findSize(char* fileName);

struct fileNode* fileStart = NULL; 
struct directNode* directStart = NULL;
int fileCount = 0; 

enum {FALSE, TRUE};

int main(int argc, char* argv[]){ 
	if(argc == 1){ 
		printf("No arguments passed.\n");
		return 0;
	} 
	char* userPath = argv[1];
	addLast(&directStart, userPath); 
	struct directNode* tmp = directStart; 
	while(tmp != NULL){ 
		DIR* dir;
		struct dirent *dirEntry; 
		char fileName[MAX_FILENAME]; 
		
		dir = opendir(tmp->directName); 
		while(TRUE){ 
			dirEntry = readdir(dir); 
			if(dirEntry == NULL){ 
				break;
			} 
			if(strcmp(dirEntry->d_name, ".") != 0 && strcmp(dirEntry->d_name, "..") != 0){ 
				sprintf(fileName, "%s/%s", tmp->directName, dirEntry->d_name); 
				monitorFile(fileName);
			}
		}		
		tmp = tmp->next;
	}
	bubbleSort(&fileStart, fileCount);  
	printList(fileStart); 
	freeFileList(fileStart); 
	freeDirectList(directStart);
	return 0;
} 
void monitorFile(char* fileName){ 
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
		addLast(&directStart, fileName);
	} 
	else if(S_ISREG(mode)){ 
		fileCount = fileCount + 1;
		addStart(&fileStart, fileName);
	}
}

void addLast(struct directNode** start, char* fileName){ 
	struct directNode *newNode = malloc(sizeof(struct directNode)); 
	newNode->directName = malloc(strlen(fileName) + 1); 
	strcpy(newNode->directName, fileName);
	newNode->next = NULL; 
	if(*start == NULL){ 
		*start = newNode;
	} 
	else{ 
		struct directNode *lastNode = *start; 

		while(lastNode->next != NULL){ 
			lastNode = lastNode->next;
		} 
		lastNode->next = newNode;
	}
} 

void addStart(struct fileNode** start, char* fileName){ 
	struct fileNode* newNode = (struct fileNode*)malloc(sizeof(struct fileNode));
        newNode->fileSize = findSize(fileName); 
	newNode->fileName = malloc(strlen(fileName) + 1);
	strcpy(newNode->fileName, fileName); 
	newNode->next = *start; 
	*start = newNode;
} 

long int findSize(char* fileName){ 
	FILE* fp = fopen(fileName, "r"); 
	fseek(fp, 0L, SEEK_END); 
	long int res = ftell(fp); 
	fclose(fp); 
	return res;
}

void bubbleSort(struct fileNode** start, int count){ 
	struct fileNode** h; 
	int i, j, swapped; 

	for(i = 0; i <= count; i++){ 
		h = start; 
		swapped = 0; 
		for(j = 0; j < count - i - 1; j++){ 
			struct fileNode* p1 = *h; 
			struct fileNode* p2 = p1->next; 
			if(p1->fileSize > p2->fileSize){ 
				*h = swap(p1, p2); 
				swapped = 1;
			} 
			h = &(*h)->next;
		}
		if(swapped==0){ 
			break;
		}
	}
} 

struct fileNode* swap(struct fileNode* a, struct fileNode* b){ 
	struct fileNode* tmp = b->next; 
	b->next = a; 
	a->next = tmp; 
	return b;
} 

void printList(struct fileNode* start){ 
	struct fileNode* temp = start; 
	while(temp != NULL){ 
		printf("%ld\t%s\n", temp->fileSize, temp->fileName); 
		temp = temp->next;
	}
} 

void freeFileList(struct fileNode* start){ 
	struct fileNode* temp = NULL; 
       	while(start){ 
		temp = start->next; 
		free(start->fileName); 
		free(start); 
		start = temp;
	}	

} 

void freeDirectList(struct directNode* start){ 
	struct directNode* temp = NULL; 
	while(start){ 
		temp = start->next; 
		free(start->directName); 
		free(start); 
		start = temp;
	}
}
