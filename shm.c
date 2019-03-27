#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>

const key_t shkey = 314159;
const key_t npkey = 271828;
const int PERMISSION = 0666; //-rw-rw-rw-


void* shmalloc(size_t size, int *shmid_store, key_t key) {
	if ((*shmid_store = shmget(key, size, IPC_CREAT | PERMISSION)) == -1) {
		perror("shmget: shmget failed");
		exit(-1);
	}
	void *ret;
   	if ((ret = shmat(*shmid_store, NULL, 0)) == (void*)-1) {
		perror("shmat: shmat failed");
		exit(-1);
	}
	return ret;
}

void shdelete(void *maddr, int shmid) {
	if (shmdt(maddr) == -1) {
		perror("shmdt: shmdt failed");
		exit(-1);
	}
	if (shmctl(shmid, IPC_RMID, NULL) == -1) {
		perror("shmctl: shmctl failed");
		exit(-1);
	}
}

int main() {
	int size = 1, shmid;
	int *ptr = (int*)shmalloc(sizeof(int) * size, &shmid, shkey);
	for (int i = 0; i < size; ++i) {
		ptr[i] = 0;
	}	

	int np_size = 4096, np_shmid;
	int *np_ptr = (int*)shmalloc(sizeof(int) * np_size, &np_shmid, npkey);	

	int exit = 0;
	while (!exit) {
		char cmd[105];
		scanf("%s", cmd);
		switch (cmd[0]) {
		        case 'p': { // print
				for (int i = 0; i < size; ++i) {
				    printf("%d ", ptr[i]);
				}
				printf("\n");
				break;
			}
			case 'w': { // write pos val
				int pos;
			   	int val;
				scanf("%d%d", &pos, &val);
				if (pos < 0 || pos >= size) {
					printf("Error: out of array range.\n");
				} else {
					ptr[pos] = val;
					for (int i = 0; i < size; ++i) {
						printf("%d ", ptr[i]);
					}
					printf("\n");
				}
				break;
			}
			case 'e': { //exit
				exit = 1;
				break;
			}
		        case 'n': { // print np array
			        int len;
				scanf("%d", &len);
				for (int i = 0; i < len; ++i) {
				    printf("%d ", np_ptr[i]);
				}
				printf("\n");
				break;
			}
			default: {
				printf("Help\n");
				printf("p - print array\n");
				printf("w pos val - change value\n");
				printf("e - exit\n");
			}
		}
	}
	shdelete(ptr, shmid);
	shdelete(np_ptr, np_shmid);
	return 0;
}
