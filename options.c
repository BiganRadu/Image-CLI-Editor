#include "options.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

void load(image_t **image, command_t * command){
	if(*image!= NULL){
		free_image(image);
	}
	FILE *image_file = fopen(command->args[1],"r");
	if(!image_file){
		printf("Failed to load %s\n",command->args[1]);
		return;
	}

	char type_specifier[4];
	fgets(type_specifier,4,image_file);
	remove_trailing_newline(type_specifier);
	if(!get_type_of_image(type_specifier)){
		printf("Failed to load %s\n", command->args[1]);
		return;
	}
	(*image) = malloc(sizeof(image_t));
	if(!*image){
		printf("Failed to load %s\n", command->args[1]);
		return;
	}
	(*image)->type = get_type_of_image(type_specifier);
	ignore_comments(image_file);

	int height, width;
	fscanf(image_file,"%d %d",&width,&height);
	ignore_comments(image_file);
	fscanf(image_file,"%d",&(*image)->max_channel);
	ignore_comments(image_file);
	int mult = 1;
	if((*image)->type == P3 || (*image)->type == P6){
		mult = 3;
	}
	(*image)->matrix = allocate_matrix(height ,width * mult);
	if(!(*image)->matrix){
		free(*image);
		*image = NULL;
		printf("Failed to load %s\n", command->args[1]);
		return;
	}

    /*The mult variable is used in the for loops in order to 
    read more values if the  images is color*/
	if((*image)->type < 4){
		for(int  i = 0;i < height; i++){
			for(int  j = 0; j< width * mult; j++){
				fscanf(image_file, "%lf", &(*image)->matrix[i][j]);
			}
		}
	}else{
		unsigned char c;
		fscanf(image_file,"%c",&c);
		for(int  i = 0;i < height; i++){
			for(int  j = 0; j < width * mult; j++){
				fscanf(image_file, "%c", &c);
				(*image)->matrix[i][j] = (double)c;
			}
		}
	}
	fclose(image_file);
	(*image)->height = height;
	(*image)->width = width;
	(*image)->selection.x1 = 0;
	(*image)->selection.y1 = 0;
	(*image)->selection.x2 = width;
	(*image)->selection.y2 = height;
	printf("Loaded %s\n", command->args[1]);
}

void selection(image_t *image, command_t *command){
	if(image == NULL){
		printf("No image loaded\n");
		return;
	}
	int height = image->height;
	int width = image->width;
	if(command->arg_cnt == 2 && !strcmp(command->args[1], "ALL")){
		image->selection.x1 = 0;
		image->selection.x2 = width;
		image->selection.y1 = 0;
		image->selection.y2 = height;
	}else{
		if(command->arg_cnt != 5){
			printf("Invalid command\n");
			return;
		}
        /*If the parameters are not numbers or they are negative,
        the parsing function will return -1*/
		int x1 = str_to_num(command->args[1]);
		int y1 = str_to_num(command->args[2]);
		int x2 = str_to_num(command->args[3]);
		int y2 = str_to_num(command->args[4]);
		if(x1 != -1 && y1 != -1 && x2 != -1 && y2 != -1){
			if(x1 <= width && x2 <= width && y1 <= height && y2 <= height){
				if(x1 > x2 && y1 > y2){
					image->selection.x1 = x2;
					image->selection.x2 = x1;
					image->selection.y1 = y2;
					image->selection.y2 = y1;
					printf("Selected %d %d %d %d\n",x1,y1,x2,y2);
				}else if(x1 < x2 && y1 < y2){
					image->selection.x1 = x1;
					image->selection.x2 = x2;
					image->selection.y1 = y1;
					image->selection.y2 = y2;
					printf("Selected %d %d %d %d\n",x1,y1,x2,y2);
				}else{
					printf("Invalid set of coordinates1\n");
				}
			}else{
				printf("Invalid set of coordinates2\n");
			}
		}else{
			printf("Invalid set of coordinates3\n");
		}
	}
}

void histogram(image_t *image, command_t *command){
	if(!image){
		printf("No image loaded\n");
		return;
	}
	if(image->type == P3 || image->type == P6){
		printf("Black and white image needed\n");
		return;
	}

	int x = str_to_num(command->args[1]);
	int y = str_to_num(command->args[2]);

	if(x != -1 && y != -1){
		if(y >= 2 && y <= 256 && is_pow_of_2(y)){
			int freq[256] = {0};
			for(int i = image->selection.y1; i< image->selection.y2; i++){
				for(int j = image->selection.x1; j< image->selection.x2; j++){
					freq[(int)round(image->matrix[i][j])]++;
				}
			}

			//First the program needs to know the maximum frequency per bin
			int mx = 0;
			for(int i = 0; i < y; i++){
				int freq_per_bin = 0;
				for(int  j = 0; j < 256 / y;j++){
					freq_per_bin += freq[i * 256 / y + j];
				}
				if(freq_per_bin > mx){
					mx = freq_per_bin;
				}
			}

			for(int i = 0; i < y; i++){
				int freq_per_bin = 0;
				for(int  j = 0; j < 256 / y;j++){
					freq_per_bin += freq[i * 256 / y + j];
				}
				int num_of_stars;
				if(mx > 0){
					num_of_stars  = (int)floor((double)freq_per_bin / mx * x);
				}else{
					num_of_stars = 0;
				}
				printf("%d\t|\t",num_of_stars);
				for(int k = 0;k<num_of_stars;k++){
					printf("*");
				}
				printf("\n");
			}
		}else{
			printf("Invalid set of parameters\n");
		}
	}else{
		printf("Invalid set of parameters\n");
	}
}

void equalize(image_t *image){
	if(!image){
		printf("No image loaded\n");
		return;
	}
	if(image->type == P3 || image->type == P6){
		printf("Black and white image needed\n");
		return;
	}

	int freq[256] = {0};
	for(int  i = 0; i < image->height; i++){
		for(int  j =0; j < image->width; j++){
			freq[(int)round(image->matrix[i][j])]++;
		}
	}

	double area = image->height * image->width;
	//The array dp will store on position k the sum of freq[i] where 0<=i<=k
	int dp[256];
	for(int i = 0;i < 256; i++){
		dp[i] = -1;
	}
	for(int i = 0;i<image->height; i++){
		for(int j = 0; j < image->width; j++){
			int aux = (int)round(image->matrix[i][j]);
			aux = top_down_dp(aux,freq,dp);
			image->matrix[i][j] = round(clamp(255.0 / area * aux,0,255));
		}

	}
	printf("Equalization done\n");
}

void rotate(image_t *image, command_t *command){
	if(!image){
		printf("No image loaded\n");
		return;
	}
	int x1 = image->selection.x1;
	int x2 = image->selection.x2;
	int y1 = image->selection.y1;
	int y2 = image->selection.y2;
	if(x2 - x1 != y2 - y1){
		printf("The selection must be square\n");
		return;
	}
	if(command->arg_cnt != 2){
		printf("Unsupported rotation angle\n");
		return;
	}
	int angle = parse_angle(command->args[1]);
	if(angle % 90 != 0 || angle > 360 || angle < -360 || angle == 0){
		printf("Unsupported rotation angle\n");
		return;
	}
	if(angle < 0)
		angle  = 360 - angle;
	switch (angle){
	case 90:
		rotate90(image);
		break;

	case 180:
		rotate180(image);
		break;

	case 270:
		rotate270(image);
		break;

	default:
		//Default means the angle is 360 and nothing should happen
		break;
	}
	printf("Rotated %s\n", command->args[1]);
}

/*Rotation in place by 90 degrees works by reversing the order of the rows
and then swapping the elements over the main diagonal with the ones below it*/
void rotate90(image_t *image){
	int x1 = image->selection.x1;
	int x2 = image->selection.x2;
	int y1 = image->selection.y1;
	int y2 = image->selection.y2;
	double **mat = image->matrix;
	int height = y2 - y1;
	int width = x2 - x1;

	for(int i = 0; i < height / 2; i++){
		for(int j = 0; j < width; j++){
			if(image->type == P2 || image->type == P5){
				swap(&mat[i][j], &mat[y1 + height -1 -i][j]);
			}else{
				swap(&mat[y1 + i][x1 * 3 + j * 3], &mat[y1 + height -1 -i][x1 * 3 + j * 3]);
				swap(&mat[y1 + i][x1 * 3 + j * 3 + 1], &mat[y1 + height -1 -i][x1 * 3 + j * 3 + 1]);
				swap(&mat[y1 + i][x1 * 3 + j * 3 + 2], &mat[y1 + height -1 -i][x1 * 3 + j * 3 + 2]);
			}
		}
	}

	for(int i = 0;i < height; i++){
		for(int  j = 0; j < i ;j++){
			if(image->type == P2 || image->type == P5){
				swap(&mat[y1 + i][x1 + j], &mat[y1 + j][x1 + i]);
			}else{
				swap(&mat[y1 + i][x1 * 3 + j * 3], &mat[y1 + j][x1 * 3 + i * 3]);
				swap(&mat[y1 + i][x1 * 3 + j * 3 + 1], &mat[y1 + j][x1 * 3 + i * 3 + 1]);
				swap(&mat[y1 + i][x1 * 3 + j * 3 + 2], &mat[y1 + j][x1 * 3 + i * 3 + 2]);
			}
		}
	}
}

/*Ratation in place 180 degrees works by reversing the order of the rows
and also reversing the order of the elemeents on each row*/
void rotate180(image_t *image){
	int x1 = image->selection.x1;
	int x2 = image->selection.x2;
	int y1 = image->selection.y1;
	int y2 = image->selection.y2;
	double **mat = image->matrix;
	int height = y2 - y1;
	int width = x2 - x1;

	for(int i = 0; i < height / 2; i++){
		for(int j = 0; j < width; j++){
			if(image->type == P2 || image->type == P5){
				swap(&mat[y1 + i][x1 + j], &mat[y1 + height - i -1][x1 + width - j - 1]);
			}else{
				swap(&mat[y1 + i][(x1 + j) * 3], &mat[y1 + height - i -1][(x1 + width - j - 1) * 3]);
				swap(&mat[y1 + i][(x1 + j) * 3 + 1], &mat[y1 + height - i -1][(x1 + width - j - 1) * 3 + 1]);
				swap(&mat[y1 + i][(x1 + j) * 3 + 2], &mat[y1 + height - i -1][(x1 + width - j - 1) * 3 + 2]);
			}
		}
	}

	//If the height of the selection is an odd number the line in the middle needs to be reversed
	if(height % 2 == 1){
		for(int  i = 0;i < width / 2; i++){
			if(image->type == P2 || image->type == P5){
				swap(&mat[y1 + height / 2][x1 + i], &mat[y1 + height / 2][x1 + width - i - 1]);
			}else{
				swap(&mat[y1 + height / 2][(x1 + i) * 3], &mat[y1 + height / 2][(x1 + width - i - 1) * 3]);
				swap(&mat[y1 + height / 2][(x1 + i) * 3 + 1], &mat[y1 + height / 2][(x1 + width - i - 1) * 3 + 1]);
				swap(&mat[y1 + height / 2][(x1 + i) * 3 + 2], &mat[y1 + height / 2][(x1 + width - i - 1) * 3 + 2]);
			}
		}
	}

}

/*Rotation in place by 180 degrees works by reversing the order of the columns
and then swapping the elements over the main diagonal with the ones below it*/
void rotate270(image_t *image){
	int x1 = image->selection.x1;
	int x2 = image->selection.x2;
	int y1 = image->selection.y1;
	int y2 = image->selection.y2;
	double **mat = image->matrix;
	int height = y2 - y1;
	int width = x2 - x1;

	for(int i = 0; i < width / 2; i++){
		for(int  j = 0;j < height; j++){
			if(image->type == P2 || image->type == P5){
				swap(&mat[j + y1][i + x1], &mat[j + y1][x1 + width - 1 - i]);
			}else{
				swap(&mat[j + y1][(i + x1) * 3], &mat[j + y1][(x1 + width - 1 - i) * 3]);
				swap(&mat[j + y1][(i + x1) * 3 + 1], &mat[j + y1][(x1 + width - 1 - i) * 3 + 1]);
				swap(&mat[j + y1][(i + x1) * 3 + 2], &mat[j + y1][(x1 + width - 1 - i) * 3 + 2]);
			}
		}
	}

	for(int i = 0;i < height; i++){
		for(int  j = 0; j < i ;j++){
			if(image->type == P2 || image->type == P5){
				swap(&mat[y1 + i][x1 + j], &mat[y1 + j][x1 + i]);
			}else{
				swap(&mat[y1 + i][x1 * 3 + j * 3], &mat[y1 + j][x1 * 3 + i * 3]);
				swap(&mat[y1 + i][x1 * 3 + j * 3 + 1], &mat[y1 + j][x1 * 3 + i * 3 + 1]);
				swap(&mat[y1 + i][x1 * 3 + j * 3 + 2], &mat[y1 + j][x1 * 3 + i * 3 + 2]);
			}
		}
	}
}

void crop(image_t *image){
	if(image == NULL){
		printf("No image loaded\n");
		return;
	}

    //If the image is color, mult will be set to 3
	int mult = 1;
	if(image->type == P3 || image->type == P6){
		mult = 3;
	}
	int new_width = image->selection.x2 - image->selection.x1;
	int new_height = image->selection.y2 -  image->selection.y1;
	double **new_mat = allocate_matrix(new_height, new_width * mult);
	if(!new_mat){
		printf("Memory allocation error\n");
		return;
	}

	for(int  i = 0;i< new_height;i++){
		for(int j = 0; j < new_width* mult; j++){
			new_mat[i][j] = image->matrix[i + image->selection.y1][j + image->selection.x1 * mult];
		}
	}

	free_matrix(&image->matrix, image->height);
	image->matrix = new_mat;
	image->height = new_height;
	image->width = new_width;
	image->selection.x1 = 0;
	image->selection.x2 = new_width;
	image->selection.y1 = 0;
	image->selection.y2 = new_height;
	printf("Image cropped\n");
}

void apply_menu(image_t *image, command_t *command){
	if(!image){
		printf("No image loaded\n");
		return;
	}

	if(image->type == P2 || image->type == P5){
		printf("Easy, Charlie Chaplin\n");
		return;
	}
    
	if(command->arg_cnt == 2){
		if(!strcmp(command->args[1],"EDGE")){
            int kernel[][3]={{-1,-1,-1},{-1,8,-1},{-1,-1,-1}};
			apply(image,kernel,1, command->args[1]);
		}else if(!strcmp(command->args[1],"SHARPEN")){
            int kernel[][3]={{0,-1,0},{-1,5,-1},{0,-1,-0}};
			apply(image,kernel,1, command->args[1]);
		}else if(!strcmp(command->args[1],"BLUR")){
            int kernel[][3]={{1,1,1},{1,1,1},{1,1,1}};
			apply(image,kernel,1.0/9.0, command->args[1]);
		}else if(!strcmp(command->args[1],"GAUSSIAN_BLUR")){
            int kernel[][3]={{1,2,1},{2,4,2},{1,2,1}};
			apply(image,kernel,1.0/16.0, command->args[1]);
		}else {
			printf("APPLY parameter invalid\n");
		}
	}else{
		printf("APPLY parameter invalid\n");
	}
}

void apply(image_t *image, int kernel[][3], double coef, char * param){
	int modify_height = image->selection.y2 - image->selection.y1;
	int modify_width = image->selection.x2 - image->selection.x1;
	double **new_mat = allocate_matrix(modify_height, modify_width * 3);
	if(!new_mat){
		printf("Memory allocation error\n");
		return;
	}
	//Fill the edges first
	for(int  i = 0;i< modify_height; i+=modify_height-1){
		for(int j=0;j<modify_width * 3;j++){
			new_mat[i][j] = image->matrix[i + image->selection.y1][j + image->selection.x1 * 3];
		}
	}
	for(int  i = 0;i< modify_width * 3; i+=modify_width*3-3){
		for(int j=0;j<modify_height;j++){
			new_mat[j][i] = image->matrix[j + image->selection.y1][i+image->selection.x1 * 3];
			new_mat[j][i + 1] = image->matrix[j + image->selection.y1][i+image->selection.x1 * 3 + 1];
			new_mat[j][i + 2] = image->matrix[j + image->selection.y1][i+image->selection.x1 * 3 + 2];
		}
	}
	for(int  i = 1;i<modify_height - 1;i++){
		for(int  j = 3;j < (modify_width - 1) * 3;j+=3){
			double sumr=0.0;
			double sumg = 0.0;
			double sumb =0.0;
			for(int  p = 0; p < 3;p++){
				for(int q = 0;q < 3;q++){
					sumr+= image->matrix[i + image->selection.y1 + p - 1][j + (image->selection.x1  + q - 1)* 3] * kernel[p][q];
					sumg+= image->matrix[i + image->selection.y1 + p - 1][j + (image->selection.x1  + q - 1)* 3 + 1]* kernel[p][q];
					sumb+= image->matrix[i + image->selection.y1 + p - 1][j + (image->selection.x1  + q - 1)* 3 + 2] * kernel[p][q];
				}
			}
            sumr *= coef;
            sumg *= coef;
            sumb *= coef;
			new_mat[i][j] = clamp(sumr,0,255);
			new_mat[i][j+1] = clamp(sumg,0,255);
			new_mat[i][j+2] = clamp(sumb,0,255);
		}
	}
	for(int  i = 0;i < modify_height;i++){
		for(int  j =0;j<modify_width*3;j++){
			image->matrix[i + image->selection.y1][j + image->selection.x1 * 3] = new_mat[i][j];
		}
	}
	free_matrix(&new_mat,modify_height);
	printf("APPLY %s done\n", param);
}

void save(image_t *image, command_t *command){
	if(!image){
		printf("No image loaded\n");
	}
	FILE *save_file = fopen(command->args[1],"w");
	int binary = 1;
	if(command->arg_cnt == 3 && !strcmp(command->args[2],"ascii")){
		binary = 0;
	}
	if(!binary){
		if(image->type % 3 != 0){
			fprintf(save_file,"P%d\n",image->type % 3);
		}else{
			fprintf(save_file,"P%d\n",3);
		}
	}else{
		if(image->type % 3 != 0){
			fprintf(save_file,"P%d\n",(image->type % 3) + 3);
		}else{
			fprintf(save_file,"P%d\n",6);
		}
	}
	fprintf(save_file,"%d %d\n",image->width, image->height);
	fprintf(save_file,"%d\n",image->max_channel);
	int mult = 1;
	if(image->type == P3 || image->type == P6){
		mult = 3;
	}
	if(!binary){
		for(int  i = 0; i < image->height;i++){
			for(int j = 0; j < image->width * mult; j++){
				fprintf(save_file,"%d ",(int)round(image->matrix[i][j]));
			}
			fprintf(save_file,"\n");
		}
	}else{
		for(int  i = 0; i < image->height;i++){
			for(int j = 0; j < image->width * mult; j++){
				fprintf(save_file,"%c",(unsigned char)round(image->matrix[i][j]));
			}
		}
	}
	fclose(save_file);
	printf("Saved %s\n", command->args[1]);
}