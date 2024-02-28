#include "utils.h"
#include <string.h>
#include <stdlib.h>

void remove_trailing_newline(char * str){
	if(strlen(str) > 0 && str[strlen(str)-1]=='\n'){
		str[strlen(str)-1] = '\0';
	}
}
/*This is my own implementation of atoi for positive numbers that doesn't
have a unpredictable behaviour if there are other characters
than numbers in the string and returns -1 in such scenario*/
int str_to_num(char * str){
	if(strlen(str) > 10){
		return -1;
	}
	int num = 0;
	for(size_t i = 0;i<strlen(str);i++){
		if((str[i]-'0') < 10 && (str[i] - '0') >= 0){
			num = num * 10 + (str[i] - '0');
		}else{
			printf("aici");
			return -1;
		}
	}

	/*If the number represented in the string is too large,
	the int will overflow and the number will get negative*/
	if(num < 0){
		return -1;
	}
	return num;
}

void swap(double * a, double *b){
	double aux = *a;
	*a = *b;
	*b = aux;
}

int is_pow_of_2(int x){
	if ((x & (x-1)) == 0) {
		return 1;
	}
	return 0;
}

double clamp(double x, double minim, double maxim){
	if(x < minim)
		return minim;
	if(x > maxim){
		return maxim;
	}
	return x;
}

command_t * read_command(){
	command_t * command = malloc(sizeof(command_t));
	if(!command){
		printf("Memory allocation error\n");
		return NULL;
	}

	char *input = malloc(256);
	if(!input){
		free(command);
		printf("Memory allocation error\n");
		return NULL;
	}
	fgets(input,256,stdin);
	remove_trailing_newline(input);

	/*The number of arguments that will be created from the 
	input string is being counted in order to allocate 
	memory for the arguments precisely.*/
	int count = 1;
	for(size_t i = 1;i<strlen(input);i++){
		if(input[i]==' ' && input[i-1]!=' '){
			count++;
		}
	}
	//If there is no input, the command is invalid.
	if(strlen(input) > 0){
		command->arg_cnt = count;
		command->args = malloc(count * sizeof(char * ));
		if(!command->args){
			free(input);
			free(command);
			printf("Memory allocation error\n");
			return NULL;
		}

		//The input string gets separated into multiple arguments
		char *aux= strtok(input," ");
		int  i = 0;
		while(aux !=NULL){
			command->args[i] = malloc(strlen(aux));
			if(!command->args[i]){
				for(int j = 0; j < i; j++){
					free(command->args[i]);
				}
				free(command->args);
				free(command);
				free(input);
				printf("Memory allocation error\n");
				return NULL;
			}
			strcpy(command->args[i],aux);
			i++;
			aux = strtok(NULL," ");
		}
	}else{
		free(input);
		free(command);
		printf("Invalid command\n");
		return NULL;
	}
	free(input);
	return command;
}

void free_command(command_t * command){
	if(command->args!=NULL){
		for(int  i = 0;i < command->arg_cnt;i++){
			free(command->args[i]);
		}
		free(command->args);
	}
	free(command);
}

double ** allocate_matrix(int height, int width){
	double **mat = malloc(sizeof(double *) * height);
	if(!mat){
		return NULL;
	}
	for(int  i =0;i<height;i++){
		mat[i] = malloc(sizeof(double *) * width);
		if(!mat[i]){
			for(int j = 0; j < i; j++){
				free(mat[i]);
			}
			free(mat);
			return NULL;
		}
	}
	return mat;
}

void free_matrix(double ***mat, int height){
	for(int  i = 0;i<height;i++){
		free((*mat)[i]);
	}
	free(*mat);
	*mat = NULL;
}

void free_image(image_t ** image){
	free_matrix(&(*image)->matrix,(*image)->height);
	free(*image);
	*image = NULL;
}

int get_type_of_image(char * type_specifier){
	if(!strcmp(type_specifier,"P2")){
		return 2;
	}else if(!strcmp(type_specifier,"P3")){
		return 3;
	}else if(!strcmp(type_specifier,"P5")){
		return 5;
	}else if(!strcmp(type_specifier,"P6")){
		return 6;
	}
	return 0;
}

void ignore_comments(FILE * image_file){
	char first_on_line;
	fscanf(image_file,"%c",&first_on_line);
	while(first_on_line=='#'){
		while(fgetc(image_file)!='\n');
		fscanf(image_file,"%c",&first_on_line);
	}
	fseek(image_file,-1,SEEK_CUR);
}

/*This function is used to parse the angle given as a parameter 
of the command rotate. It works for positive & negative numbers 
and returns -1 in case of any characters present in the string 
that cannot be fiited into the number.*/
int parse_angle(char * angle){
	int negative = 0, num = 0, start = 0;
	if(angle[0] == '-'){
		negative = 1;
		start = 1;
	}
	for(size_t  i = start; i < strlen(angle); i++) {
		if(angle[i] - '0' < 10 &&  angle[i] -'0' >= 0){
			num = num * 10 + (angle[i] - '0');
		} else {
			return -1;
		}
	}
	if(negative){
		return num * (-1);
	}
	return num;
}

int top_down_dp(int k, int *freq, int * dp){
	if(k == 0){
		return freq[0];
	}
	if(dp[k] != -1){
		return dp[k];
	}else{
		return dp[k] = freq[k] + top_down_dp(k-1, freq, dp);
	}
}