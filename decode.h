#ifndef DECODE_H
#define DECODE_H
#include <stdio.h>
#include "types.h"



typedef struct _DecodeInfo
{
    /* Encoded Image info */
    char *encoded_image_fname;
    FILE *fptr_encoded_image;  
   

    /* Decoded File Info */
    char dec_fname[50];       
    FILE *fptr_dec_file;     
    char extn_dec_file[5];
    int dec_extn_size; 
    char dec_data[100];    
    long size_dec_file;   

} DecodeInfo;


/*Read and validate Encode args*/
Status read_and_validate_decode_args(char *argv[],DecodeInfo *decodeinfo);

/*Perform the decoding*/
Status do_decoding(DecodeInfo *decodeinfo);

/*Get file pointer from encoded input image file*/
Status open_out_image_file(DecodeInfo *decodeinfo);

/*Get file pointer for output file*/
Status open_out_data_file(DecodeInfo *decodeinfo);

/*DECODE MAGIC STRING*/
Status decode_magic_string(const char* magic_string,DecodeInfo *decodeinfo);

/*Decoding the lsb of byte for decoding character*/
Status decode_char_byte(char *data,char *image_buffer);

/*Decoding the lsb of byte for decoding size*/
Status decode_int_byte(int *data,char *image_buffer);

/*Decode txt file extn size*/
Status decode_file_extn_size(DecodeInfo *decodeinfo);

/*Decode txt file extn*/
Status decode_file_extn(DecodeInfo *decodeinfo);

/*Decoding of file size*/
Status decode_file_size(DecodeInfo *decodeinfo);

/*Decoding of data*/
Status decode_data(DecodeInfo *decodeinfo);

/*Writing data into output file*/
Status write_data(DecodeInfo *decodeinfo);
















#endif