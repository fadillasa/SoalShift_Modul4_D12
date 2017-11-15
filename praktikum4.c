#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>

static const char *dirpath = "/home/frandita/Documents";

static int xmp_getattr(const char *path, struct stat *stbuf)
{
  	int res;
	char fpath[1000];
	sprintf(fpath,"%s%s",dirpath,path);
	res = lstat(fpath, stbuf);

	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
  	char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	int res = 0;

	DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;

	dp = opendir(fpath);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		res = (filler(buf, de->d_name, &st, 0));
			if(res!=0) break;
	}

	closedir(dp);
	return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
  	char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	int res = 0;
  	int fd = 0 ;

	if (strstr(fpath,".pdf")>0 || strstr(fpath,".doc")>0 || strstr(fpath,".txt")>0){
		// ---- no1 ----		
		system("zenity --error --text=\"Terjadi kesalahan! File berisi konten berbahaya.\"");

		char c[1000];
		
		if (strstr(fpath,".ditandai")==NULL){
			strcpy(c,fpath);
			strcat(c,".ditandai");
			rename(fpath,c);
		}
		// ---- no1 end ----

		// ---- no2 ----
		if(strstr(fpath,"rahasia")==NULL){
			DIR *dir = opendir("rahasia");
			if (dir == NULL){
				system("mkdir rahasia");
			}
			char mod[] = {"chmod "};
			strcat(mod,dirpath);
			strcat(mod,path);
			strcat(mod," 000");
			//printf("%s\n",mod);
			system(mod);
			char temp[] = {"mv "};
			strcat(temp,c);
			strcat(temp," ");
			strcat(temp,dirpath);
			strcat(temp,"/rahasia");
			strcat(temp,path);
			strcat(temp,".ditandai");
			//printf("%s\n",temp);
			system(temp);
		}
		// ---- no2 end----
		return -1;
	} else {	
		(void) fi;
		fd = open(fpath, O_RDONLY);
		if (fd == -1)
			return -errno;

		res = pread(fd, buf, size, offset);
		if (res == -1)
			res = -errno;

		close(fd);
		return res;
	}
}

static int xmp_rename(const char *from, const char *to)
{
	int res;

	res = rename(from, to);
	if (res == -1)
		return -errno;

	return 0;
}

static struct fuse_operations xmp_oper = {
	.getattr	= xmp_getattr,
	.readdir	= xmp_readdir,
	.read		= xmp_read,
	.rename		= xmp_rename,
};

int main(int argc, char *argv[])
{
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}
