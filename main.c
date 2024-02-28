#include "options.h"
#include <string.h>


int main(){
	command_t *command;
	int running = 1;
	image_t *image = NULL;
	while(running){
		command = read_command();
		if(command !=NULL){
			if(!strcmp(command->args[0],"LOAD")){
				if(command->arg_cnt == 2){
					load(&image,command);
				}else{
					printf("Invalid command\n");
				}
			}else if(!strcmp(command->args[0],"SELECT")){
				if(command->arg_cnt == 2 || command->arg_cnt == 5){
					selection(image,command);
				}else{
					printf("Invalid command\n");
				}
			}else if(!strcmp(command->args[0],"ROTATE")){
				rotate(image,command);
			}else if(!strcmp(command->args[0],"EQUALIZE")){
				if(command->arg_cnt == 1){
					equalize(image);
				}else{
					printf("Invalid command\n");
				}
			}else if(!strcmp(command->args[0],"CROP")){
				if(command->arg_cnt == 1){
					crop(image);
				}else{
					printf("Invalid command\n");
				}
			}else if(!strcmp(command->args[0],"APPLY")){
				apply_menu(image,command);
			}else if(!strcmp(command->args[0],"SAVE")){
				if(command->arg_cnt >= 2 && command->arg_cnt <=3){
					save(image,command);
				}else{
					printf("Invalid command\n");
				}
			}else if(!strcmp(command->args[0],"HISTOGRAM")){
				if(command->arg_cnt == 3){
					histogram(image,command);
				}else{
					printf("Invalid set of parameters\n");
				}
			}else if(!strcmp(command->args[0],"EXIT")){
				if(!image){
					printf("No image loaded\n");
				}else{
					running = 0;
					free_image(&image);
				}
			}else{
				printf("Invalid command\n");
			}
			free_command(command);
		}
	}
	return 0;
}