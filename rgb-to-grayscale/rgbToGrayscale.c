#include <stdio.h>
#include <stdlib.h>

struct BITMAP_header{
    char signature[2];   // This shoud be equal to BM  
    unsigned int  size; // size is non negative and is of 4 bytes thus using unsigned int
    int reserved;  // Some reserved memory 
    unsigned int image_offset; // offset from where image starts in a file
};

struct DIB_header{
    unsigned int header_size;
    int width;
    int height;
    unsigned short color_pane;
    unsigned short bits_per_pixel;
    unsigned int compression;
    unsigned int image_size;
};

struct RGB{
    __uint8_t blue;
    __uint8_t green;
    __uint8_t red;
};
struct Image{
    int height;
    int width;
    struct RGB **rgb;
    int bytes_to_read;
};

struct Image readImage(FILE * fp,int height,int width){
    struct Image image;
    image.height=height;
    image.width=width;
    image.rgb=(struct RGB **)malloc(height*sizeof(void *));
    // calcultaing bytes_to_read after accounting for padding  (Wiki)
    // 24 is bits per pixel already checked before during opening file
    int bytes_to_read=((24*width+31)/32)*4;
    int num_rgb_needed=bytes_to_read/sizeof(struct RGB);
    if(bytes_to_read % sizeof(struct RGB)) num_rgb_needed++;

    image.bytes_to_read=bytes_to_read;

    for(int i=0;i<height;i++){
        image.rgb[i]=(struct RGB *)malloc(sizeof(struct RGB)*num_rgb_needed);
        fread(image.rgb[i],bytes_to_read,1,fp);
        // converting rgb to grascale
        for(int j=0;j<width;j++){
        __uint8_t red= image.rgb[i][j].red;
        __uint8_t blue= image.rgb[i][j].blue;
        __uint8_t green= image.rgb[i][j].green;
        __uint8_t gray=(__uint8_t)(0.299*red +0.587*green +0.114*blue);
        image.rgb[i][j].red=image.rgb[i][j].blue= image.rgb[i][j].green =gray;
        }
    }
   
    
    return image;

}


void freeImage(struct Image image){
    for(int i=0;i<image.height;i++){
        free(image.rgb[i]);
    }
    free(image.rgb);
}

void writeImage(char * buffer,int buffer_size, struct Image* image){
    char name[100];
    printf("Enter name of output file: ");
    scanf("%s",&name);
    // creating and opening a new file
    FILE * fp = fopen(name,"wb");
    fwrite(buffer,buffer_size,1,fp);
  
    int height= image->height;
    
    for(int i=0;i<height;i++){
        struct RGB * pointer_to_row= (image->rgb)[i];
        fwrite(pointer_to_row,image->bytes_to_read,1,fp);                                      
    }
    fclose(fp);
}

void openBmpFile(){
    char name[100];
    printf("Enter name of input file: ");
    scanf("%s",&name);
    FILE *fp= fopen(name,"rb");
    if(fp==NULL){
        printf("Unable to open file\n");
        return ;
    }
    struct BITMAP_header header;
    struct DIB_header dib_header;

    // reading bitmap header
    fread(header.signature,2,1,fp);

    fread(&header.size,3*(sizeof(int)),1,fp);
    if(header.signature[0]!='B' || header.signature[1]!='M'){
        printf("Not a bmp file\n");
        fclose(fp);
        return ;
    }

    // reading dib header
    fread(&dib_header,sizeof(struct DIB_header),1,fp);
   

    if(dib_header.header_size!=124 || dib_header.compression!=0 ||dib_header.bits_per_pixel!=24){
        
        printf("This type of bmp file is not supported\n");
        fclose(fp);
        return ;
    }

    // moving file pointer to beginning of image offset
    fseek(fp,header.image_offset,SEEK_SET);

    // reading image data
    struct Image image=readImage(fp,dib_header.height,dib_header.width);
    
    // Now we need to copy all the headers as it it in output file
    fseek(fp,0,SEEK_SET);
    char * buffer =(char *)malloc(header.image_offset);
    fread(buffer,header.image_offset,1,fp);

    // Now we have all the reqired data from source image so we can now close it
    fclose(fp);

    // Now writing data in a new image file
    writeImage(buffer,header.image_offset,&image);
    free(buffer);
    freeImage(image);
}

int main(){
    printf("-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-\n");
    printf("RGB Image to Grayscale converter\n\n");
    printf("This program only works for Bitmap images(.bmp files)\n");
    printf("Make sure the file you want to convert is in same folder as of this c file\n");
    openBmpFile();
    printf("-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-\n");
    return 0;
}

