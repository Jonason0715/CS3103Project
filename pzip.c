#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include <sys/mman.h> 
#include <pthread.h>
#include <sys/stat.h> 


int total_threads;
int page_size; 
int num_files;
int total_pages;
int head = 0;
int tail = 0;
int size = 0;
struct buffer buf[10];

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER, filelock=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER, fill = PTHREAD_COND_INITIALIZER;

struct fd{
    int id;
	char* address;
	int size;
};

struct buffer {
    char* address; //Mapping address of the file_number file + page_number page
    int file_number; //File Number
    int page_number; //Page number
    int last_page_size; //Page sized or (size_of_file)%page_size
};

void write(struct buffer b){
    buf[head] = b;
    head = (head + 1) % 10;
    size++;
}

struct buffer get(){
   struct buffer b = buf[tail];
   tail= (tail + 1 )%10;
   size--;
   return b;
}

void* defineAdd(void* arg){

    struct stat st;
    FILE *fp1;
    char* map;
    char** filenames = (char **)arg;
    
    fp1 = fopen(filenames[0],"r");
    map = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fp1, 0); //address

    int pages_in_file = (st.st_size/page_size);
    int last_page_size;
		if(((double)st.st_size/page_size)>pages_in_file){ 
			pages_in_file += 1;
			last_page_size = st.st_size%page_size;
		}
		else{ 
			last_page_size=page_size;
		}
		total_pages+=pages_in_file;

    for(int i = 0;i < total_pages; i++){
        struct buffer buf;

        if(i == total_pages - 1){
            buf.last_page_size = last_page_size;
        }
        
        buf.address = map;
        buf.page_number = i;
        map += page_size;
        write(buf);
    }
    close(fp1);

}

void* compress(){

    
}

int main(int argc, char* argv){

    total_threads = hardware_concurrency();
    num_files = argv - 1;
    page_size = 10000000;
    int pages_per_file = malloc(sizeof(int)*num_files);

    pthread_t pid,cid[total_threads];
	
    pthread_create(&pid, NULL, defineAdd, argv+1 );
    for (int i = 0; i < total_threads; i++) {

        pthread_create(&cid[i], NULL, compress, NULL);
    }




	return 0;
}
