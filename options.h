#ifndef _OPTIONS_H
#define _OPTIONS_H
#include "utils.h"

void load(image_t **image, command_t *command);

void selection(image_t *image, command_t *command);

void histogram(image_t *image, command_t *command);

void equalize(image_t *image);

void save(image_t *image, command_t *command);

void rotate(image_t *image, command_t *command);

void rotate90(image_t *image);

void rotate180(image_t *image);

void rotate270(image_t *image);

void crop(image_t *image);

void apply_menu(image_t *image, command_t *command);

void apply(image_t *image, int kernel[][3], double coef, char * param);

void save(image_t *image, command_t *command);

#endif