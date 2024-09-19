#ifndef AUXILIARY_FUNCTIONS_H
#define AUXILIARY_FUNCTIONS_H

void file_permissions_print(char *file_path);

void time_conversion_epoch(long epoch_time);

void file_type_print(char *file_path);

char file_type_return(char *file_path);

void file_info_links_names_size(char *fname);

int cmpstringp(const void *p1, const void *p2);

void usageError(char *progName, char *msg, int opt);

void parseOptions(int arg_count, char **args, int *l_flag, int *a_flag,
		  int *t_flag, int *u_flag, int *c_flag, int *i_flag,
		  int *f_flag, int *d_flag, int *one_flag, char *paths[],
		  int *path_count, int *ignore_sort, int *f_option_first);
		  
		  
int is_executable(const char *file_path);

void color_mode_print(char *file_path, char *file_name);

int compare_modification_time(const void *a, const void *b);

int compare_change_time(const void *a, const void *b);

int compare_access_time(const void *a, const void *b);

void inode_number_print(char *file_path);



#endif
