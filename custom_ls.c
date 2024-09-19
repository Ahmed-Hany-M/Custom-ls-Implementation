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
#include "auxiliary_functions.h"
#define MAX_FILE_NAME_LENGTH (255)
#define MAX_PATHS (10000)
#define printable(ch) (isprint((unsigned char) ch) ? ch : '#')

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
	if (paths[0] == NULL) {
		paths[0] = ".";
        path_count++;
    }

	if (d_flag) {
        for(int j = 0 ; j < path_count ; j++) {
		    struct stat dir_stat;
		    if (lstat(paths[j], &dir_stat) == -1) {
			    perror("lstat failed");
			    exit(EXIT_FAILURE);
		    }

		    if (i_flag) {
			    inode_number_print(paths[j]);
			    printf(" ");
		    }

		    if (l_flag) {
			    file_type_print(paths[j]);
			    file_permissions_print(paths[j]);
			    printf("   ");
			    file_info_links_names_size(paths[j]);
			    time_conversion_epoch(dir_stat.st_mtime);
			    printf("   ");
			    color_mode_print(paths[j], paths[j]);
			    printf("\n");
		    } else {
			    color_mode_print(paths[j], paths[j]);
			    printf("\n");
		    }
        }

		return 0;
	}
    for(int j = 0 ; j < path_count ; j++) {
	    DIR *dirp = opendir(paths[j]);
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
		    /* We know that by default, ls sorts alphabetically. */
		    qsort(entries, entry_count, sizeof(char *), cmpstringp);
		    /* I read that The -t option takes priority over -c or -u flags and is sorted by modification time. */
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
				    paths[j], entries[i]);

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
            if(path_count > 1) {
                printf("%s:\n", paths[j]);
            }
		    printf("total %ld\n", total_blocks / 2);

		    for (int i = 0; i < entry_count; i++) {
			    char file_path[MAX_FILE_NAME_LENGTH + 2];
			    snprintf(file_path, sizeof(file_path), "%s/%s",
				    paths[j], entries[i]);

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
	    	    if(path_count > 1) {
                	printf("%s:\n", paths[j]);
            	    }
		    for (int i = 0; i < entry_count; i++) {
			    char file_path[MAX_FILE_NAME_LENGTH + 2];
			    snprintf(file_path, sizeof(file_path), "%s/%s",
				    paths[j], entries[i]);
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
    }

	return 0;

}
