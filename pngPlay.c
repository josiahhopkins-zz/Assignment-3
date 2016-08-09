#include "pixutils.h"

int main(int argc, char *argv[]){
  char *inputfile=0,*outputfile=0;
  float degrees=0, grayFlag=0, bitFlag=0, sortFlag=0;
  
  //write the parser yourself or use a package like https://github.com/skeeto/optparse
  
		//check for flags -i -o -r -g - can be in any order
  //-i is followed by the input png
  //-o is followed the output png
  //-r is followd by the rotation angle in degrees (float) <optional for user>
  //-g is whether th png should be grayed <optional for user>
 int i =  1;
 while(i < argc){
  if(strcmp("-i", argv[i]) == 0){
	inputfile = argv[++i];
  } else if(strcmp("-o", argv[i]) == 0){
	outputfile = argv[++i];
  } else if(strcmp("-r", argv[i]) == 0){
	degrees = atoi(argv[++i]);
  } else if(strcmp("-g", argv[i]) == 0){
	grayFlag = 1;
  } else if(strcmp("-b", argv[i]) == 0){
	bitFlag = 1;
  } else if(strcmp("-s", argv[i]) == 0){
	sortFlag = 1;
  }
  i++;
 }   

  pixMap *p=pixMap_init_filename(inputfile);
  if(degrees)pixMap_rotate(p,degrees);
  if(grayFlag)pixMap_gray(p);
  if(sortFlag){
	  pixMap_sort(p);
	  fprintf(stdout, "You are trying to sort");
  }
  if(bitFlag){
	  
	printf("bmp printing\n");
	  pixMap_write_bmp16(p,outputfile);
  } else{
	pixMap_write(p,outputfile);
  }
  pixMap_destroy(p);
  
  return 0;
}

