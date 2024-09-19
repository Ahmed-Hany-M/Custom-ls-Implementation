#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <grp.h>
#include <dirent.h>
#include <pwd.h>
#include <getopt.h>
#include <ctype.h>

#define MAX_FILE_NAME_LENGTH (255)
#define MAX_PATHS (10000)
#define printable(ch) (isprint((unsigned char) ch) ? ch : '#')

extern char *optarg;
extern int optind, opterr, optopt;

/* This function given the arguments count and the arguments, will print the permissions for argv[1].*/
void file_permissions_print(char *file_path)
{
	struct stat buf;

	if (lstat(file_path, &buf) < 0) {
		perror("Error in lstat");
		exit(EXIT_FAILURE);
	}

	int mode = buf.st_mode;
	char str[10] = "---------";

	/* Owner Permissions: */
	if (mode & S_IRUSR)
		str[0] = 'r';
	if (mode & S_IWUSR)
		str[1] = 'w';
	str[2] = (mode & S_IXUSR) ? 'x' : '-';
	if (mode & S_ISUID) {
		str[2] = (mode & S_IXUSR) ? 's' : 'S';
	}

	/* Group Permissions: */
	if (mode & S_IRGRP)
		str[3] = 'r';
	if (mode & S_IWGRP)
		str[4] = 'w';
	str[5] = (mode & S_IXGRP) ? 'x' : '-';
	if (mode & S_ISGID) {
		str[5] = (mode & S_IXGRP) ? 's' : 'S';
	}

	/* Others Permissions: */
	if (mode & S_IROTH)
		str[6] = 'r';
	if (mode & S_IWOTH)
		str[7] = 'w';
	str[8] = (mode & S_IXOTH) ? 'x' : '-';
	if (mode & S_ISVTX) {
		str[8] = (mode & S_IXOTH) ? 't' : 'T';
	}

	str[9] = '\0';
	printf("%s ", str);
}

/* This functions print the date, it takes the epoch time */
void time_conversion_epoch(long epoch_time)
{
	/* This is to get the local time. */
	struct tm *timeinfo = localtime((time_t *) & epoch_time);
	if (timeinfo == NULL) {
		perror("Failed to convert time");
		exit(EXIT_FAILURE);
	}

	/* Formatting the output to be similar to that of ls -l. */
	char buffer[100];
	strftime(buffer, sizeof(buffer), "%b %d %H:%M", timeinfo);

	printf("%s", buffer);
}

/* This function will take the file path and prints its type (a letter.) */
void file_type_print(char *file_path)
{
	struct stat buf;
	if (lstat(file_path, &buf) < 0) {
		perror("Error in stat");
		exit(EXIT_FAILURE);
	}

	char type_char;
	/* Directory */
	if ((buf.st_mode & S_IFMT) == S_IFDIR) {
		type_char = 'd';

		/* Regular file */
	} else if ((buf.st_mode & S_IFMT) == S_IFREG) {
		type_char = '-';

		/* Symbolic link */
	} else if ((buf.st_mode & S_IFMT) == S_IFLNK) {
		type_char = 'l';

		/* Character device */
	} else if ((buf.st_mode & S_IFMT) == S_IFCHR) {
		type_char = 'c';

		/* Block device */
	} else if ((buf.st_mode & S_IFBLK) == S_IFBLK) {
		type_char = 'b';

		/* Socket */
	} else if ((buf.st_mode & S_IFSOCK) == S_IFSOCK) {
		type_char = 's';

		/* FIFO */
	} else if ((buf.st_mode & S_IFIFO) == S_IFIFO) {
		type_char = 'p';

		/* I will put '?' for undefined type. */
	} else {
		type_char = '?';
	}

	printf("%c", type_char);
}

char file_type_return(char *file_path)
{
	struct stat buf;
	if (lstat(file_path, &buf) < 0) {
		perror("Error in stat");
		exit(EXIT_FAILURE);
	}

	char type_char;
	/* Directory */
	if ((buf.st_mode & S_IFMT) == S_IFDIR) {
		type_char = 'd';

		/* Regular file */
	} else if ((buf.st_mode & S_IFMT) == S_IFREG) {
		type_char = '-';

		/* Symbolic link */
	} else if ((buf.st_mode & S_IFMT) == S_IFLNK) {
		type_char = 'l';

		/* Character device */
	} else if ((buf.st_mode & S_IFMT) == S_IFCHR) {
		type_char = 'c';

		/* Block device */
	} else if ((buf.st_mode & S_IFBLK) == S_IFBLK) {
		type_char = 'b';

		/* Socket */
	} else if ((buf.st_mode & S_IFSOCK) == S_IFSOCK) {
		type_char = 's';

		/* FIFO */
	} else if ((buf.st_mode & S_IFIFO) == S_IFIFO) {
		type_char = 'p';

		/* I will put '?' for undefined type. */
	} else {
		type_char = '?';
	}

	return type_char;
}

/* This function takes the file name and prints the number of links, user and group names (converted from IDs) and size of the file. */
void file_info_links_names_size(char *fname)
{
	struct stat info;
	int rv = lstat(fname, &info);
	if (rv == -1) {
		perror("stat failed");
		exit(EXIT_FAILURE);
	}

	struct passwd *pw = getpwuid(info.st_uid);
	struct group *gr = getgrgid(info.st_gid);

	printf("%ld  ", info.st_nlink);
	printf("%s  ", pw ? pw->pw_name : "Unknown");
	printf("%s  ", gr ? gr->gr_name : "Unknown");
	printf("%ld  ", info.st_size);
}

/* For sorting alphabetically with qsort() function. */
static int cmpstringp(const void *p1, const void *p2)
{
	return strcmp(*(const char **)p1, *(const char **)p2);
}

/* Just for errors. */
static void usageError(char *progName, char *msg, int opt)
{
	if (msg != NULL && opt != 0)
		fprintf(stderr, "%s (-%c)\n", msg, printable(opt));
	fprintf(stderr, "Usage: %s [-l] [-i] [-u] [-t] [directory]\n",
		progName);
	exit(EXIT_FAILURE);
}

void parseOptions(int arg_count, char **args, int *l_flag, int *a_flag,
		  int *t_flag, int *u_flag, int *c_flag, int *i_flag,
		  int *f_flag, int *d_flag, int *one_flag, char *paths[],
		  int *path_count, int *ignore_sort, int *f_option_first)
{
	// Check for -f as the first option
	for (int i = 1; i < arg_count; i++) {
		if (args[i][0] == '-') {
			// Check if -f is the first option
			if (args[i][1] == 'f'
			    && (i == 1 || args[i - 1][0] != '-')) {
				*f_option_first = 1;
			}
			// Check if -f is present
			if (strchr(args[i], 'f')) {
				*f_flag = 1;
				*ignore_sort = 1;	// Set ignore_sort if -f is anywhere
			}
		}
	}

	int opt;
	/* Setting the options' flags correctly */
	while ((opt = getopt(arg_count, args, "latucifd1")) != -1) {
		switch (opt) {
		case 'l':
			*l_flag = 1;
			break;
		case 'a':
			*a_flag = 1;
			break;
		case 't':
			*t_flag = 1;
			break;
		case 'u':
			*u_flag = 1;
			break;
		case 'c':
			*c_flag = 1;
			break;
		case 'i':
			*i_flag = 1;
			break;
		case 'f':
			*f_flag = 1;
			break;
		case 'd':
			*d_flag = 1;
			break;
		case '1':
			*one_flag = 1;
			break;
		case '?':
			fprintf(stderr, "Unrecognized option\n");
			exit(EXIT_FAILURE);
		default:
			fprintf(stderr, "Unexpected option\n");
			exit(EXIT_FAILURE);
		}
	}

	/* Storing the paths in an array. */
	for (int index = optind; index < arg_count; index++) {
		if (*path_count < MAX_PATHS) {
			paths[*path_count] = args[index];
			(*path_count)++;
		} else {
			fprintf(stderr, "Too many paths specified\n");
			exit(EXIT_FAILURE);
		}
	}
}

/* Checks if the file is executable or not. Returns 0 if not and 1 if it is executable.*/
int is_executable(const char *file_path)
{
	struct stat st;
	if (stat(file_path, &st) != 0)
		return 0;
	return (st.st_mode & S_IXUSR) || (st.st_mode & S_IXGRP)
	    || (st.st_mode & S_IXOTH);
}

/* Colors the output. */
void color_mode_print(char *file_path, char *file_name)
{
	char file_type = file_type_return(file_path);
	if (file_type == 'd') {
		printf("\033[34m%s\033[0m", file_name);

	} else if (file_type == '-') {
		if (is_executable(file_path)) {
			printf("\033[32m%s\033[0m", file_name);
		} else {
			printf("\033[0m%s\033[0m", file_name);
		}

	} else if (file_type == 'l') {
		printf("\033[36m%s\033[0m", file_name);

	} else if (file_type == 'c') {
		printf("\033[33m%s\033[0m", file_name);

	} else if (file_type == 'b') {
		printf("\033[33m%s\033[0m", file_name);

	} else if (file_type == 's') {
		printf("\033[35m%s\033[0m", file_name);

	} else if (file_type == 'p') {
		printf("\033[35m%s\033[0m", file_name);

	} else if (file_type == '?') {
		printf("\033[37m%s\033[0m", file_name);

	}
	return;
}

int compare_modification_time(const void *a, const void *b)
{
	struct stat stat_a, stat_b;

	if (stat(*(const char **)a, &stat_a) != 0) {
		//perror("Error getting stat for file A");
		return 0;	// Treat as equal if error occurs
	}
	if (stat(*(const char **)b, &stat_b) != 0) {
		//perror("Error getting stat for file B");
		return 0;	// Treat as equal if error occurs
	}

	return (stat_a.st_mtime > stat_b.st_mtime) ? -1 : (stat_a.st_mtime <
							   stat_b.
							   st_mtime) ? 1 : 0;
}

int compare_change_time(const void *a, const void *b)
{
	struct stat stat_a, stat_b;

	if (stat(*(const char **)a, &stat_a) != 0) {
		//perror("Error getting stat for file A");
		return 1;	// Consider file A "greater" if there's an error
	}
	if (stat(*(const char **)b, &stat_b) != 0) {
		//perror("Error getting stat for file B");
		return -1;	// Consider file B "greater" if there's an error
	}
	// Sort from newest to oldest
	if (stat_a.st_ctime > stat_b.st_ctime) {
		return -1;	// A is newer than B
	} else if (stat_a.st_ctime < stat_b.st_ctime) {
		return 1;	// A is older than B
	} else {
		return 0;	// They are equal
	}
}

int compare_access_time(const void *a, const void *b)
{
	struct stat stat_a, stat_b;

	if (stat(*(const char **)a, &stat_a) != 0) {
		// Handle error for file A
		return 1;	// Consider file A "greater" if there's an error
	}
	if (stat(*(const char **)b, &stat_b) != 0) {
		// Handle error for file B
		return -1;	// Consider file B "greater" if there's an error
	}
	// Sort from newest to oldest based on access time
	if (stat_a.st_atime > stat_b.st_atime) {
		return -1;	// A has more recent access time than B
	} else if (stat_a.st_atime < stat_b.st_atime) {
		return 1;	// A has older access time than B
	} else {
		return 0;	// They have the same access time
	}
}

/* Gets inode number. */
void inode_number_print(char *file_path)
{
	struct stat file_stat;
	if (lstat(file_path, &file_stat) == -1) {
		perror("lstat failed");
		return;
	}

	printf("%lu", file_stat.st_ino);
}

int main(int argc, char **argv)
{
	int l_flag = 0, a_flag = 0, t_flag = 0, u_flag = 0, c_flag = 0;
	int i_flag = 0, f_flag = 0, d_flag = 0, one_flag = 0;
	int ignore_sort = 0, f_option_first = 0;
	char *paths[MAX_PATHS];
	int path_count = 0;

	parseOptions(argc, argv, &l_flag, &a_flag, &t_flag, &u_flag, &c_flag,
		     &i_flag, &f_flag, &d_flag, &one_flag, paths, &path_count,
		     &ignore_sort, &f_option_first);
	if (paths[0] == NULL)
		paths[0] = ".";

	if (d_flag) {
		// When -d is set, print info about the directory itself
		struct stat dir_stat;
		if (lstat(paths[0], &dir_stat) == -1) {
			perror("lstat failed");
			exit(EXIT_FAILURE);
		}

		if (i_flag) {
			inode_number_print(paths[0]);
			printf(" ");
		}

		if (l_flag) {
			// Print long format information for the directory
			file_type_print(paths[0]);
			file_permissions_print(paths[0]);
			printf("   ");
			file_info_links_names_size(paths[0]);
			time_conversion_epoch(dir_stat.st_mtime);
			printf("   ");
			color_mode_print(paths[0], paths[0]);
			printf("\n");
		} else {
			// Print the directory name with appropriate coloring
			color_mode_print(paths[0], paths[0]);
			printf("\n");
		}

		return 0;	// Exit after printing directory info
	}

	DIR *dirp = opendir(paths[0]);
	if (dirp == NULL) {
		perror("opendir failed.");
		exit(EXIT_FAILURE);
	}

	struct dirent *d_entry;
	char *entries[MAX_PATHS];
	int entry_count = 0;

	while ((d_entry = readdir(dirp)) != NULL) {
		if (!a_flag && !f_flag && d_entry->d_name[0] == '.')
			continue;
		if (entry_count < MAX_PATHS) {
			entries[entry_count] = strdup(d_entry->d_name);
			entry_count++;
		} else {
			fprintf(stderr, "Too many entries specified\n");
			exit(EXIT_FAILURE);
		}
	}

	closedir(dirp);

	if (ignore_sort == 0) {
		/* By default, sort alphabetically. */
		qsort(entries, entry_count, sizeof(char *), cmpstringp);
		/* The -t option takes priority over -c or -u flags and is sorted by modification time. */
		if (t_flag) {
			qsort(entries, entry_count, sizeof(char *),
			      compare_modification_time);
		}
		/* -c option has priority over -u option. */
		if (!t_flag && c_flag) {
			qsort(entries, entry_count, sizeof(char *),
			      compare_change_time);
		}
		/* -u option has the least priority. */
		if (!t_flag && !c_flag && u_flag) {
			qsort(entries, entry_count, sizeof(char *),
			      compare_access_time);
		}
	}

	if (ignore_sort == 1) {
		if (f_option_first != 1) {
			l_flag = 0;
		}
	}

	if (l_flag) {
		long total_blocks = 0;

		for (int i = 0; i < entry_count; i++) {
			char file_path[MAX_FILE_NAME_LENGTH + 2];
			snprintf(file_path, sizeof(file_path), "%s/%s",
				 paths[0], entries[i]);

			struct stat file_stat;
			if (lstat(file_path, &file_stat) == -1) {
				perror("lstat failed");
				continue;
			}

			if (S_ISREG(file_stat.st_mode)
			    || S_ISDIR(file_stat.st_mode)) {
				total_blocks += file_stat.st_blocks;
			}
		}

		printf("total %ld\n", total_blocks / 2);

		for (int i = 0; i < entry_count; i++) {
			char file_path[MAX_FILE_NAME_LENGTH + 2];
			snprintf(file_path, sizeof(file_path), "%s/%s",
				 paths[0], entries[i]);

			struct stat file_stat;
			if (lstat(file_path, &file_stat) == -1) {
				perror("lstat failed");
				continue;
			}
			if (i_flag) {
				inode_number_print(file_path);
				printf(" ");
			}

			file_type_print(file_path);
			file_permissions_print(file_path);
			printf("   ");
			file_info_links_names_size(file_path);
			time_conversion_epoch(file_stat.st_mtime);
			printf("   ");
			color_mode_print(file_path, entries[i]);
			printf("\n");
			free(entries[i]);
		}
	} else {
		for (int i = 0; i < entry_count; i++) {
			char file_path[MAX_FILE_NAME_LENGTH + 2];
			snprintf(file_path, sizeof(file_path), "%s/%s",
				 paths[0], entries[i]);
			if (i_flag) {
				inode_number_print(file_path);
				printf(" ");
			}
			color_mode_print(file_path, entries[i]);
			if (one_flag) {
				printf("\n");
			} else {
				if (i < entry_count - 1) {
					printf("  ");
				}
			}
			free(entries[i]);
		}
		if (!one_flag) {
			printf("\n");
		}
	}

	return 0;
}
