# Image Editor

This C project is an image editor developed for a college assignment, focusing on providing functionalities for manipulating PPM or PGM images.

## Features

The project supports the following commands:

### LOAD <file>
- Loads the image (PPM or PGM) from the specified file into the program's memory.
- Command output:
  - Loaded <file> - if the operation is successful.
  - Failed to load <file> - if there are difficulties.

### SELECT <x1> <y1> <x2> <y2>
- Restricts the effect of subsequent commands to the specified pixel range.
- Command output:
  - Selected <x1> <y1> <x2> <y2> - if the operation is successful.
  - Invalid set of coordinates - if any of the points are outside the image.
  - No image loaded - if there is no loaded image.

### SELECT ALL
- Resets the selection to the entire image.
- Command output:
  - Selected ALL - if the operation is successful.
  - No image loaded - if there is no loaded image.

### HISTOGRAM <x> <y>
- Displays the histogram of the image.
- Command output:
  - Histogram - if it can be calculated.
  - Black and white image needed - if the image is colored.
  - No image loaded - if there is no loaded image.

### EQUALIZE
- Equalizes the grayscale image according to the specified method.
- Command output:
  - Equalize done - if the operation is successful.
  - Black and white image needed - if the image is colored.
  - No image loaded - if there is no loaded image.

### ROTATE <angle>
- Rotates the current selection by a specified angle.
- Command output:
  - Rotated <angle> - if the operation is successful.
  - The selection must be square - if the selection is incompatible.
  - Unsupported rotation angle - if the rotation angle is not allowed.
  - No image loaded - if there is no loaded image.

### CROP
- Crops the image to the current selection.
- Command output:
  - Image cropped - if the operation is successful.
  - No image loaded - if there is no loaded image.

### APPLY <PARAMETER>
- Applies one of the specified image kernels.
- Command output:
  - APPLY <PARAMETER> done - if the operation is successful.
  - APPLY parameter invalid - if the parameter is not valid.
  - No image loaded - if there is no loaded image.

### SAVE <filename> [ascii]
- Saves the current image to the specified file.
- Command output:
  - Saved <filename> - after saving.
  - No image loaded - if there is no loaded image.

### EXIT
- Exits the program.
