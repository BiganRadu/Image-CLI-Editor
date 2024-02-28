#ifndef _UTILS_H
#define _UTILS_H
#include <stdio.h>

typedef enum{
	P2=2,P3=3,P5=5,P6=6
} type_t;

typedef struct {
	int arg_cnt;
	char  **args;
} command_t;

typedef struct {
	int x1,x2,y1,y2;
} selection_t;

typedef struct {
	type_t type;
	int height, width, max_channel;
	double **matrix;
	selection_t selection;
} image_t;

void remove_trailing_newline(char *str);

int str_to_num(char * str);

void swap(double * a, double *b);

int is_pow_of_2(int x);

double clamp(double x, double minim, double maxim);

command_t * read_command();

void free_command(command_t * command);

double ** allocate_matrix(int height, int width);

void free_matrix(double ***mat, int height);

void free_image(image_t ** image);

int get_type_of_image(char * type_specifier);

void ignore_comments(FILE * image_file);

int parse_angle(char * angle);

int top_down_dp(int k, int *freq, int * dp);

#endif