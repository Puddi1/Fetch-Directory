// Packages
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Constants
#define root "/"
#define currentDir "."
#define prevDir ".."
#define binDir "bin"
#define gitDir ".git"

// Structs
struct dirElements {
  int currentArrayDirectoryMaxElements;
  int currentArrayDirectoryElements;
  int currentArrayFileMaxElements;
  int currentArrayFileElements;
  const char **arrayDirectoriesPtr;
  const char **arrayFilesPtr;
};

/*
  concat concatenates two referenced strings,
  returns concatenated string or exit
*/
char *concat(const char *s1, const char *s2) {
  // Create pointer to string with enough space for both one and null term.
  char *result = malloc(strlen(s1) + strlen(s2) + 1); // null term

  // If malloc fails exit with error
  if (result == NULL) {
    fprintf(stderr, "Fatal: failed to concatenate strings: %s and %s", s1, s2);

    exit(0);
  }

  // Copy first and second string in newly created pointer
  strcpy(result, s1);
  strcat(result, s2);
  return result;
}

/*
  increaseArraySize increases the size of a dynamic char pointer to pointers
  array
*/
const char **increaseArraySize(const char **arrayPtr,
                               int *currentMaxElementsPtr, int newMaxElements) {
  // Store
  int totalNewMaxElements = *currentMaxElementsPtr + newMaxElements;

  // Allocate space for enough elements
  const char **newSizeArray = malloc(totalNewMaxElements * sizeof(char *));

  if (newSizeArray == NULL) {
    fprintf(stderr, "Fatal: failed to allocate new array space");

    exit(0);
  }

  // Copy old array values into new array
  void *pointerDestination =
      memcpy((void *)newSizeArray, (const void *)arrayPtr,
             *currentMaxElementsPtr * sizeof(char *));

  // Update struct array pointer, max elements and free old array
  free((void *)arrayPtr);
  *currentMaxElementsPtr = totalNewMaxElements;

  return newSizeArray;
}

/*
  readDirAddFile recursively reads the passed in directory and adds each file to
  the dirElements struct passed as a reference.
*/
void readDirAddFile(const char *dirPath, struct dirElements *dirElementsStr) {
  // Fetch files and add their local dir path to array
  DIR *dir = opendir(dirPath);
  if (dir == NULL) {
    return;
  }

  // Read current directory
  struct dirent *entity = readdir(dir);

  // Create and allocate a string path to the current dir
  const char *pathToDir = concat(dirPath, (const char *)&root);

  // Iterate through all directory elements
  while (entity != NULL) {
    // File
    if (entity->d_type == DT_REG) {
      // Add file path to array
      const char *filePath = concat(pathToDir, (const char *)&entity->d_name);

      // Check if there is enough space for new file element, if not increase
      // size
      if (dirElementsStr->currentArrayFileMaxElements ==
          dirElementsStr->currentArrayFileElements) {
        dirElementsStr->arrayFilesPtr =
            increaseArraySize(dirElementsStr->arrayFilesPtr,
                              &dirElementsStr->currentArrayFileMaxElements, 15);
      }

      // Allocate Element
      dirElementsStr->arrayFilesPtr[dirElementsStr->currentArrayFileElements] =
          filePath;
      dirElementsStr->currentArrayFileElements += 1;
    }

    // Directory
    if (entity->d_type == DT_DIR && strcmp(entity->d_name, currentDir) != 0 &&
        strcmp(entity->d_name, prevDir) != 0 &&
        strcmp(entity->d_name, binDir) != 0 &&
        strcmp(entity->d_name, gitDir) != 0) {
      // Generate path
      const char *newDir = concat(pathToDir, (const char *)&entity->d_name);

      // Check if there is enough space for new directory element, if not
      // increase size
      if (dirElementsStr->currentArrayDirectoryMaxElements ==
          dirElementsStr->currentArrayDirectoryElements) {
        dirElementsStr->arrayDirectoriesPtr = increaseArraySize(
            dirElementsStr->arrayDirectoriesPtr,
            &dirElementsStr->currentArrayDirectoryMaxElements, 15);
      }

      // Add path string to struct
      dirElementsStr
          ->arrayDirectoriesPtr[dirElementsStr->currentArrayDirectoryElements] =
          newDir;
      dirElementsStr->currentArrayDirectoryElements += 1;

      // Recurse new path
      readDirAddFile(newDir, dirElementsStr);
    }

    entity = readdir(dir);
  };

  // Close read and free path to dir string
  closedir(dir);
  free((void *)pathToDir);

  return;
}

/*
  fetchFiles runs through the whole directory and stores all files' path
  into an array of strings pointers.
*/
void fetchFiles(struct dirElements *dirElementsStr) {
  // Recursively read from dir root
  readDirAddFile((const char *)&currentDir, dirElementsStr);
  return;
}

/*
  main execution
*/
int main() {
  // Track execution
  clock_t begin = clock();
  // printf() displays the string inside quotation
  printf("Scanning Directory files and paths\n\n");

  // Init filesStuct
  struct dirElements dirElementsStr = {
      .currentArrayDirectoryMaxElements = 15,
      .currentArrayDirectoryElements = 0,
      .currentArrayFileMaxElements = 15,
      .currentArrayFileElements = 0,
      .arrayFilesPtr = malloc(sizeof(char *) * 15),
      .arrayDirectoriesPtr = malloc(sizeof(char *) * 15)};

  // Fetch
  fetchFiles(&dirElementsStr);

  // Log and free found elements
  // Paths
  for (int i = 0; i < dirElementsStr.currentArrayDirectoryElements; ++i) {
    printf("Path Element number %i Found: %s\n", i,
           dirElementsStr.arrayDirectoriesPtr[i]);
    free((void *)dirElementsStr.arrayDirectoriesPtr[i]);
  };
  free((void *)dirElementsStr.arrayDirectoriesPtr);

  printf("\n");

  // Files
  for (int i = 0; i < dirElementsStr.currentArrayFileElements; ++i) {
    printf("File Element number %i Found: %s\n", i,
           dirElementsStr.arrayFilesPtr[i]);
    free((void *)dirElementsStr.arrayFilesPtr[i]);
  };
  free((void *)dirElementsStr.arrayFilesPtr);

  // Calculate execution Time
  clock_t end = clock();
  double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  printf("\nScript took: %fs\n", time_spent);
  // End
  return 0;
}

