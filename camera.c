#include <unistd.h>
#include <stdio.h>
#include <time.h>

#define NAME_LENGTH 30
#define CODE_LENGTH (NAME_LENGTH+70)

int main(void){
	char name[NAME_LENGTH];
	char code[CODE_LENGTH];
	int num = 0;
	FILE * TEST;

	char buff[1024];
	getcwd(buff, 1024);

	struct tm *t;
	time_t timer;
	
	timer = time(NULL);
	t = localtime(&timer);
	
	sprintf(name, "%d%d%d_%d_%d_%d.jpg", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, num++);
	TEST = fopen(name, "r");
	while(TEST != NULL){
		fclose(TEST);
		sprintf(name, "%d%d%d_%d_%d_%d.jpg", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, num++);
		TEST = fopen(name, "r");
	}

	sprintf(code, "raspistill -w 600 -h 600 -o %s", name);
	system(code);
	
	sprintf(code, "mv %s /home/yeong-oo/study/python/static/img/%s", name, name);
	system(code);
	
	return 0;
}
