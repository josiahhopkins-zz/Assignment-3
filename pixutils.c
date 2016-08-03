#include "pixutils.h"
#include "lodepng.h"

//private methods
static pixMap *pixMap_init(); //allocate memory for pixMap object, set variables to zero, and return a pointer to the object
static void pixMap_reset();  //free the allocated memoray and set to zero but do not free memory for pixMap
static void pixMap_copy(pixMap *dest,pixMap *source); //copy one pixmap to another
static int pixMap_read(pixMap *p,char *filename); //read in a pixmap from file

static pixMap* pixMap_init(){
	pixMap *toReturn = malloc(sizeof(pixMap));
	toReturn->width = 0; 
	toReturn->height = 0;
	toReturn->pixArray = 0;
	toReturn->image = 0;
 //allocate memory for pixMap object, set variables to zero, and return a pointer to the object
	return toReturn;
}	
pixMap* pixMap_init_filename(char *filename){
	//use pixMap_int to create the pixMap object
	pixMap *p = pixMap_init();
//use pixMap read to fill the fields of the object
	pixMap_read(p, filename);	

	//return the pointer to the new object
	return p;
}

static int pixMap_read(pixMap *p,char *filename){

//read and allocate image, read in width and height using using lodepng_decode32_file
 //example is in lodepng/examples - one liner
  unsigned error = lodepng_decode32_file(&(p->image), &(p->width), &(p->height), filename);
	
	if(error){
		printf("%s", filename);
	}	
	
	p->pixArray = malloc(p->height * sizeof(rgba*));
	p->pixArray[0] = (rgba*) p->image;
	
	//There is an error on the next line which I cannot figure out.
	//p->pixArray[0] = &(p->image);
	for(int i = 1; i < p->height; i++){
		p->pixArray[i] = p->pixArray[i - 1] + p->width;
		//fprintf(stdout, "Going through the loop\n");
	}
//then allocate p->pixArray to hold p->height pointers
	//set p->pixArray[0] to p->image and p->pixArray[i]=p->pixArray[i-1]+p->width
	
	return 0;
	
}	
static void pixMap_copy(pixMap *dest,pixMap *source){
	//if source image is zero then reset dest and copy width and height
	
			
	if(source->image == 0){
		pixMap_reset(dest);
		dest->width = source->width;
		dest->height = source->height;
		
	} else {

		if(dest->height != source->height || dest->width != source->width){
			if(dest->width * dest->height != source->width * source->height){
				if(!(dest->image)){
					dest->image = malloc(source->height * source->width * sizeof(rgba));					
				} else{
					dest->image = realloc((dest->image), source->height * source->width * sizeof(rgba));
				}	
			} 
		}
		
		
		if(dest->height != source->height){
			if(!(dest->pixArray)){
				dest->pixArray = malloc(source->height * sizeof(rgba*));					
			} else{
				dest->pixArray = realloc((dest->pixArray), source->height * sizeof(rgba*));
			}	
			//dest->pixArray = realloc(dest->pixArray, source->height * sizeof(rgba));
		}
	}
	dest->pixArray[0] = dest->image;
	for(int i = 1; i < source->height; i++){
		
		//dest->pixArray[i] = malloc(sizeof((source->width)) * source->width);
		dest->pixArray[i] = dest->pixArray[i - 1] + source->width;
	}
	memcpy(dest->image, source->image, source->height * source->width * sizeof(rgba));
	dest->width = source->width;
	dest->height = source->height;
	//if source image is not zero
	  //if width or height are different
	    //if width*height is different then we need to allocate dest->image
	      //if dest->image is zero use malloc to allocate memory for dest->image
	      //else use realloc to allocate memory dest->image
	    //
	    //if dest->height is different
	      //malloc or realloc dest->pixArray depending on whether dest->pixArray is zero
	    //
	    //even if the height is the same set dest->pixArray[0] to dest->image and 
	    //dest->pixArray[i]=dest->pixArray[i-1]+source->width 
	  //
	  //do a memcpy from source->image to dest->image
	  //set dest->width and dest->height to source values
	//
}

static void pixMap_reset(pixMap *p){
	//free all the memory in the fields and set everything to zero
	//for(int i = 0; i < p->height; i++){
	//	free(p->pixArray[i]);
	//}
	free(p->pixArray);
	free(p->image);
	p->pixArray = 0;
	p->image = 0;
	p->width = 0; 
	p->height = 0;
}	


void pixMap_destroy(pixMap *p){
	//reset pixMap and free the object p
	pixMap_reset(p);
	free(p);
}


void pixMap_rotate (pixMap *p, float theta){
 //make a new temp blank pixMap structure
 //copy p to temp
 pixMap *temp = pixMap_init();
 pixMap_copy(temp, p);

 
 //set the values in the image to zero using memset - Note that the 3rd argument of memset is 
 memset(temp->image, 0, p->height * p->width * sizeof(rgba)); 
 //the size in BYTES

 //calculate the coordinates ox and oy of the middle of the png graphic
 //calculate the values of sine and cosine used by the rotation formula 
 float ox = p->width / 2;
 float oy = p->height / 2;
 float c = cos(degreesToRadians(theta));
 float s = sin(degreesToRadians(theta)); 
 

	for(int y=0;y<p->height;y++){   //two for loops to loop through each pixel in the original
	 for(int x=0;x<p->width;x++){
	 
	    //calculate the new rotated coordinates rotx roty using the formula from 
	    //http://stackoverflow.com/questions/2259476/rotating-a-point-about-another-point-2d
	    //use the answer from stackoverflowery
	    
	    //However this answer assumes that y is going from the bottom to the top (mathematical convention)
	    //but the pixmap starts at the upper left hand corner and height grows DOWN (scan order)
	    //so use this formula instead where c is cos(degreesToRadians(theta)) 
	    //and s is sin(degreeToRadians(theta))
	        float rotx = c*(x-ox) - s * (oy-y) + ox;
            float roty = -(s*(x-ox) + c * (oy-y) - oy);
	    
			int nrotx = rotx + .5;
			int nroty = roty + .5;
 	   //round the coordinates to the nearest integer in your calculations (add 0.5 and cast to integer)	
	
	    //if rotated coordinates are within the height and width limits
	      //copy the pixel at the old coords to the pixel to the temporary copy using memcpy
	      //i.e. 	memcpy(temp->pixArray[roty]+rotx,p->pixArray[y]+x,sizeof(rgba))
	    //
	    if(nrotx < p->width && nroty < p->height && nrotx >= 0 && nroty >= 0){
			memcpy(temp->pixArray[nroty]+ nrotx,p->pixArray[y]+x,sizeof(rgba));
		}
		
	  //
     }
    }
	//  
	//copy the temp pixMap to the original
	pixMap_copy(p, temp);
	//destroy the temp;
	pixMap_destroy(temp);
	
}	

void pixMap_gray (pixMap *p){
	//for() loop through pixels using two for loops 
	for(int i = 0; i < p->height; i++){
	  //for()
	  for(int j = 0; j < p->height; j++){
		  float ave = (p->pixArray[i][j].r + p->pixArray[i][j].g + p->pixArray[i][j].b) / 3;
		  int a = (int) (ave + .5);
		  
	    //calculate average value of r and g and b values (assign to a float variable)
	    //for example the red value of for the pixel at height i, width j would be p->pixel[i][j].r
	    //round float variable to integer (add 0.5 before casting to integer)
	    //assign the rounded value to r,g and b values
	    p->pixArray[i][j].r = a;
	    p->pixArray[i][j].g = a;
	    p->pixArray[i][j].b = a;
	  //
      }
	}
	//   
}
int pixMap_write(pixMap *p,char *filename){
 //write out to filename using lodepng_encode32_file
 //example is in lodepng/examples - one liner
	unsigned error = lodepng_encode32_file(filename, (p->image), (p->width), (p->height));

	if(error) printf("error %u: %s\n", error, lodepng_error_text(error));
	return 0;
}	 
