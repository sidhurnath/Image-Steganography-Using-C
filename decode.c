#include <stdio.h>
#include "decode.h"
#include "types.h"
#include "common.h"
#include <string.h>

/*FUNCTION DEFINITIONS*/

/*READ AND VALIDATE*/
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decodeinfo)
{
    //check if the encoded image is of .bmp format if not print error(INPUT IMAGE)
    //check argv[2] has  .bmp extension ,if not print error message
    char *check;

    if ((check = strstr(argv[2], ".bmp")) != NULL)
    {
        if (strcmp(check, ".bmp") == 0)
        {
            decodeinfo->encoded_image_fname = argv[2];
        }
        else
        {
            printf("Required .bmp extension\n");
            return e_failure;
        }
    }
    else
    {
        printf("Required .bmp extension\n");
        return e_failure;
    }
    
    //check if argv[3](output file name) is present or not
    //if not give a default name as encoded
    //conctanate the extension after decoding the extension
    if (argv[3] != NULL)
    {
        //if output file name is given copy it into dec_fname later concatanate extension after decoding it
        strcpy(decodeinfo->dec_fname,argv[3]);
        

    }
    else
    {
        //if output file not provided make "encoded" as default late concatanate extension after decoding the extension
        strcpy(decodeinfo->dec_fname,"encoded");
        
    }

    return e_success;
}

/*OPENING REQUIRED FILES*/

//open the encoded image file and the output txt file to write the data
Status open_out_image_file(DecodeInfo *decodeinfo)
{
    printf("INFO:Opening required files\n");
    decodeinfo->fptr_encoded_image = fopen(decodeinfo->encoded_image_fname, "r");
    if (decodeinfo->fptr_encoded_image == NULL)
    {
        perror("Error:");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decodeinfo->encoded_image_fname);
        return e_failure;
    }

    
    return e_success;
}

/*OPENING OUTPUT FILE*/
Status open_out_data_file(DecodeInfo *decodeinfo)
{
    decodeinfo->fptr_dec_file = fopen(decodeinfo->dec_fname, "w");
    if (decodeinfo->fptr_dec_file == NULL)
    {
        perror("Error:");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decodeinfo->dec_fname);
        return e_failure;
    }
    return e_success;
}

/*MAGIC STRING DECODING PART*/
Status decode_magic_string(const char *magic_string, DecodeInfo *decodeinfo)
{
    //create a buffer of size 8
    //read 8bytes after 54 bytes from the encoded image and stor it in buffer
    //repeat it two time for # and *
    int i;
    char decoded_char;
    for (i = 0; i < 2; i++)
    {
        char buffer[8];
        printf("INFO:Decoding Magic string Signature\n");
        if (fread(buffer, sizeof(char), 8, decodeinfo->fptr_encoded_image) != 8)
        {
           
            return e_failure;
        }
        
        //extract the lsb of each 8bytes and decode the data
        decode_char_byte(&decoded_char, buffer);

        if(decoded_char!=magic_string[i])
        {
            
            return e_failure;
        }
    }
    return e_success;
}

/*DECODING byte*/
Status decode_char_byte(char *data, char *image_buffer)
{
    //extract the lsb from each byte from the buffer and ceck whether the data is # or *
    int i;
    char decode = 0;
    for (i = 0; i < 8; i++)
    {
        image_buffer[i] = image_buffer[i] & (1);
        decode = decode << 1;
        decode = decode | image_buffer[i];
    }
    *data=decode;
    return e_success;
}

/*DECODING EXTN SIZE*/
Status decode_file_extn_size(DecodeInfo *decodeinfo)
{
    //create a buffer of 32 bytes
    //read 32bytes from the encoded image
    //extract the lsb from each byte and decode the integer value
    int i;
    char buffer[32];
    printf("INFO:Decoding file extension size\n");
    int decode = 0;
    if(fread(buffer, 32, 1, decodeinfo->fptr_encoded_image)!=1)
    {
        
        return e_failure;
    }
    decode_int_byte(&decode,buffer);
    decodeinfo->dec_extn_size = decode;
    
    return e_success;
}

/*For decoding the int value by taking lsb from each 32 bytes*/
Status decode_int_byte(int *data,char *image_buffer)
{
  int decode=0;
  for (int i = 0; i < 32; i++)
    {
        image_buffer[i] = image_buffer[i] & 1;
        decode = decode << 1;
        decode = decode | image_buffer[i];
    }  
    *data=decode;
    return e_success;

}

Status decode_file_extn(DecodeInfo *decodeinfo)
{
    //create a buffer of 8 bytes
    //read 8 bytes from the image n times where n is the size of extension
    //extract lsb from each byte of 8byte and decode the char
    //decode the extension and store in extension variable
    //conctanate the extension and file name
    char buffer[8];
    printf("INFO:Decoding file extension\n");
    int i;
    int j;
    char data = 0;
    for (i = 0; i < decodeinfo->dec_extn_size; i++)
    {
        data = 0;
        if(fread(buffer, sizeof(char), 8, decodeinfo->fptr_encoded_image)!=8)
        {
           return e_failure;
        }
        decode_char_byte(&data,buffer);
        decodeinfo->extn_dec_file[i] = data;
    }
    decodeinfo->extn_dec_file[i] = '\0';
    strcat(decodeinfo->dec_fname,decodeinfo->extn_dec_file);
    
    return e_success;
}

/*DECODING FILE size*/
Status decode_file_size(DecodeInfo *decodeinfo)
{
    //create a buffer of 32 bytes
    //read 32bytes from the encoded image
    //extract the lsb from each byte and decode the integer value
    int i;
    char buffer[32];
    printf("INFO:Decoding file size\n");
    int decode = 0;
    if(fread(buffer, 32, 1, decodeinfo->fptr_encoded_image)!=1)
    {
        return e_failure;
    }
    decode_int_byte(&decode,buffer);
    decodeinfo->size_dec_file = decode;
    return e_success;
}

/*DECODING DATA*/
Status decode_data(DecodeInfo *decodeinfo)
{
    //create a buffer of 8 bytes
    //read 8 bytes from the image n times where n is the size of file
    //extract lsb from each byte of 8byte and decode the character

    char buffer[8];
    printf("INFO:Decoding data\n");
    int i;
    int j;
    char data = 0;
    for (i = 0; i < decodeinfo->size_dec_file; i++)
    {
        data = 0;
        if(fread(buffer, sizeof(char), 8, decodeinfo->fptr_encoded_image)!=8)
        {
            return e_failure;
        }

        decode_char_byte(&data,buffer);
        decodeinfo->dec_data[i] = data;
    }
    //terminate the end with a null character to indicate the end
    decodeinfo->dec_data[i] = '\0';
    return e_success;
}

Status write_data(DecodeInfo *decodeinfo)
{
    //write the decoded data into output file
    fwrite(decodeinfo->dec_data, decodeinfo->size_dec_file, 1, decodeinfo->fptr_dec_file);
    return e_success;
}

/*MAIN DECODING*/
Status do_decoding(DecodeInfo *decodeinfo)
{

    //STEP 1:OPEN THE RQUIRED FILES
    //open encoded image(INPUT IMAGE) and the output text file in Write mode

    //STEP 2:MOVE THE FILE POINTER  54 BYTES to skip the header part of bmp image
    //skip the first 54bytes image header data

    //STEP 3:DECODE THE MAGIC STRING decode the lsb of 1st 16bytes after 54 bytes
    //check whether the encoded image contain # and * at the begining

    //STEP 4:DECODE FILE EXTENSION SIZE by decoding the lsb of 32 bytes
    //decode the file extension size for decoding the file extension

    //STEP 5:DECODE FILE EXTENSION using the file extension size obtained in previous step
    //decode the file extension

    // STEP 6:OPEN OUTPUT DATA FILE

    //STEP 7:DECODE THE FILE SIZE
    //decode the file size for decoding the data

    //STEP 8:DECODE THE DATA USING 
    // decode the data in the encoded image
    
    //STEP 8:WRITE THE DATA INTO OUTPUT FILE


    printf("### Decoding procedure Started ###\n");
    /*CHECK IF ENCODED FILES ARE OPENED OR NOT*/
    if (open_out_image_file(decodeinfo) == e_success)
    {
        printf("INFO:Opened %s file\n",decodeinfo->encoded_image_fname);
    }
    else
    {
        printf("INFO:Failed opening encoded input file\n");
        return e_failure;
    }

    /*MOVE FILE POINTER OF ENCODED IMAGE TO 54 th position*/

    fseek(decodeinfo->fptr_encoded_image, 54, SEEK_SET);

    /*DECODE MAGIC STRING*/
    if (decode_magic_string(MAGIC_STRING, decodeinfo) == e_success)
    {
        printf("INFO:Done\n");
    }
    else
    {
       printf("INFO:Decoding of magic string failed\n");
        return e_failure;
    }

    /*Decode file extension size*/
    if (decode_file_extn_size(decodeinfo) == e_success)
    {
        printf("INFO:Done\n");
    }
    else
    {
        printf("INFO:Failed decoding extension size\n");
        return e_failure;
    }

    /*Decode file extn*/

    if (decode_file_extn(decodeinfo) == e_success)
    {
        printf("INFO:Done\n");
    }
    else
    {
        printf("INFO:Failed decoding extension\n");
        return e_failure;
    }
     
    /*open output data file*/
    if(open_out_data_file(decodeinfo)==e_success)
    {
        printf("INFO:Done\n");
    }
    else
    {
        printf("INFO:Failed opening output file\n");
        return e_failure;
    }

    /*Decode file size*/
    if (decode_file_size(decodeinfo) == e_success)
    {
        printf("INFO:Done\n");
    }
    else
    {
        printf("INFO:File size decode failed\n");
        return e_failure;
    }

    /*Decode data*/
    if (decode_data(decodeinfo) == e_success)
    {
        printf("INFO:Done\n");
    }
    else
    {
        printf("INFO:Data encoding failed\n");
        return e_failure;
    }

    if (write_data(decodeinfo) == e_success)
    {
        printf("## INFO:Decoding Done succesfully ##\n");
    }
    else
    {
        printf("INFO :Failed decoding data\n");
        return e_failure;
    }
    
    return e_success;
}
