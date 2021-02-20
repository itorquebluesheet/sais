#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>

typedef struct customtype {
	int count;
	char** buf;
} customType;

customType listdir(char *dirname, char **buf);
void usage();
int pathtype(char *path);
int filesize(char *path);
int function(char *path);
bool accurate = true;
int dirc, filc = 0;
int main(int argc, char **argv) {
	
	if (argc != 2) {
		printf("Number of argument received: %d\n", argc);
		usage();
		exit(87);
	}
	
	int pathform = pathtype(argv[1]);
	if (pathform == -1) {
		printf("File doesn't even exists.\n");
		exit(7);
	} else if (pathform != 2) {
		printf("File exists but it is not a directory.\n");
		printf("Size of the file:%d\n", filesize(argv[1]));
		exit(0);
	}
		
	int useful = function(argv[1]);
	printf("Total size:%d\n", useful);
	if (accurate == false) {
		printf("Some files are missing. The size is not accuarte!\n");	
	}
	printf("Number of file(s): %d, director(ies) (including the outer directory): %d\n", filc, dirc);
	return 0;
}

void usage() {
	printf("Calculate size of files in a directory\n");
	printf("dirsize <dir>\n");

}

int pathtype(char *path) { // -1 for no such file or directory, 1 for regular, 2 for directory, 3 for symbolic link, 4 for others
	struct stat path_stat;
	if (stat(path, &path_stat)==-1) return -1;
	if (S_ISREG(path_stat.st_mode)) { filc++; return 1; } else
	if (S_ISDIR(path_stat.st_mode)) { dirc++; return 2; }  else
	if (S_ISLNK(path_stat.st_mode)) return 3; else
	return 4;
}

customType listdir(char *dirname, char **buf) {
	DIR *d;
	d = opendir(dirname);
	struct dirent *dir;
	int count = 0;
	int dirlen = strlen(dirname);
	char dname[1024];
	strcpy(dname, dirname);
	strcat(dname, "/");
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..")) {

				count++;
				buf = realloc(buf, count*sizeof(char *));
				buf[count-1] = malloc((strlen(dir->d_name)+dirlen+1)*sizeof(char));
				strcpy(buf[count-1], dname);
				strcat(buf[count-1], dir->d_name);
			}
		}
		closedir(d);
	}
	customType returning;
	returning.count = count;
	returning.buf = buf;

	return returning;
}

int filesize(char *path) {
	struct stat path_stat;
	stat(path, &path_stat);
	return path_stat.st_size;
}

int function(char *path) {
	char **buf = malloc(sizeof(char*));
	buf[0];
	int size;	
	int i;
	int i2;
	int type;
	customType ret;
	ret = listdir(path, buf);
	i2 = ret.count;
	buf = ret.buf;
	for (i=0;i<i2;i++) {
		type = pathtype(buf[i]);
		switch (type) {
			case -1: 
				printf("path %s doesn't exists! Did the file moved?\n", buf[i]);
				accurate = false;
				break;
			case 1: 
				size+=filesize(buf[i]);
				break;	
			case 2: 
				size+=function(buf[i]);
				break;
			case 3:
				printf("not following symbolic link %s\n", buf[i]);
				break;
		}
	}
	return size;
}

