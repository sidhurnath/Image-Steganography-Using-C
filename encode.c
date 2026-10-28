#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

uint get_file_size(FILE *fptr)
{
    // Find the size of secret file data
    // return secret file size
    printf("INFO:Checking for secret.txt size\n");
    int file_size;
    if (fptr == NULL)
    {
        return 0;
    }
    fseek(fptr, 0, SEEK_END);
    file_size = ftell(fptr);
    if (file_size == 0)
    {
        printf("INFO:Secret file is empty\n");
        return 0;
    }
    printf("INFO: Done. Not Empty\n");
    rewind(fptr);

    return file_size;
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    // step 1 -> check argv[2] is having .bmp or not
    // yes -> store the file name into encInfo -> src_image_fname = argv[2]
    // no - > return e_failure

    // step 2 -> check argv[3] is having extn (.txt or .sh or .c) there or not
    // yes - > store the file name into encInfo -> secret_fname = argv[3]
    // no -> return e_failure

    // step 3 -> check argv[4] is NULL or not
    // yes -> check argv[4] is having .bmp or not
    // yes -> store the file name into encInfo -> stego_image_fname = argv[4]
    // no - > return e_failure

    // no -> store default name the encInfo -> stego_image_fname = "stego.bmp"

    // return e_success;

    char *check;
    
    if ((check = strstr(argv[2], ".bmp"))!= NULL)
    {
        if (strcmp(check, ".bmp") == 0)
        {
            encInfo->src_image_fname=argv[2];
        }
        else
        {
            return e_failure;
        }
    }
    else
    {
        return e_failure;
    }

    if ((check = strstr(argv[3], ".txt"))!= NULL || 
          (check = strstr(argv[3], ".c"))!= NULL||
          (check = strstr(argv[3], ".sh"))!= NULL)
    {
        if (strcmp(check, ".txt") == 0)
        {
            strcpy(encInfo->extn_secret_file,check);
            encInfo->secret_fname=argv[3];
        }
        else if (strcmp(check, ".c") == 0)
        {
            strcpy(encInfo->extn_secret_file,check);
            encInfo->secret_fname=argv[3];
        }
        else if (strcmp(check, ".sh") == 0)
        {
            strcpy(encInfo->extn_secret_file,check);
            encInfo->secret_fname=argv[3];
        }
        else
        {
            printf("required .c/.txt/.sh file\n");
            return e_failure;
        }
    }
    else
    {
        printf("INFO:Missign secret file\n");
        return e_failure;
    }
    if(argv[4]!=NULL)
    {
     if ((check = strstr(argv[4], ".bmp"))!= NULL)
     {
        if (strcmp(check, ".bmp") == 0)
        {
            encInfo->stego_image_fname=argv[4];
        }
        else
        {
            return e_failure;
        }
     }
     else
     {
        printf(".bmp extension required for output file\n");
        return e_failure;
     }
    }
    else
    {
        printf("INFO: Output File not mentioned. Creating stego.bmp as default\n");
        encInfo->stego_image_fname="stego.bmp"; 
    }

    return e_success;
}

Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

        return e_failure;
    }


    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

        return e_failure;
    }

    // No failure return e_success
    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
    // Magic                 = 2 × 8 = 16
    // Extension size        = 4 × 8 = 32
    // Extension             = 4 × 8 = 32
    // Secret size           = 4 × 8 = 32
    // Secret data           = 9 × 8 = 72

    // step 1 -> encInfo -> image_capacity = call get_image_size_for_bmp(encoInfo -> fptr_src_image)

    // step 2 -> encInfo -> size_secret_file = call get_file_size(encInfo -> fptr_secret)

    // step 3 -> check encInfo -> image_capacity > 16 + 32 + (strlen(encInfo -> extn_secret_file) * 8) + 32 + (encInfo -> size_secret_file * 8)
    // t -> return e_success;
    // f -> return e_failure

    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    printf("INFO: Checking for %s capacity to handle %s\n",encInfo->src_image_fname,encInfo->secret_fname);
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);

    int enc_size = (54 + 16 + 32 + 32 + 32 + (encInfo->size_secret_file * 8));
    if (encInfo->image_capacity < enc_size)
    {
        printf("INFO:Not enough capacity for bmp file\n");
        return e_failure;
    }
   
    return e_success;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    // step 1 -> rewind source file pointer
    rewind(fptr_src_image);
    // declare one character buffer of size 54

    // step 2 -> read 54 bytes from source file fread()

    // step 3 -> write 54 bytes to dest file fwrite()

    // return e_success
    char buff[54];
    printf("INFO: Copying Image Header\n");
    if (fread(buff, 54, 1, fptr_src_image) != 1) 
    {
        return e_failure;
    }
    if(fwrite(buff, 54, 1, fptr_dest_image)!=1)
    {
        return e_failure;
    }
    return e_success;
}
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    // char buffer[8];

    // step 1 -> read 8 bytes from source file

    // step 2 -> call encode_byte_to_lsb(magic_string[i], buffer)

    // step 3 -> write the buffer into dest file

    // repeat this for strlen(magic_string) times from step 1
    int i;
    char buffer[8];
    printf("INFO: Encoding Magic String Signature\n");
    for (i = 0; i < strlen(magic_string); i++)
    {
        if(fread(buffer, sizeof(char), 8, encInfo->fptr_src_image)!=8)
        {
            return e_failure;
        }
        encode_byte_to_lsb(magic_string[i], buffer);
        if(fwrite(buffer, sizeof(char), 8, encInfo->fptr_stego_image)!=8)
        {
            return e_failure;
        }
    }
    return e_success;
}
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    // char buffer[32];

    // step 1 -> read 32 bytes from source file

    // step 2 -> call encode_size_to_lsb(size, buffer)

    // step 3 -> write the buffer into dest file
    char buffer[32];
    printf("INFO: Encoding secret.txt File Extenstion size\n");
    if(fread(buffer, sizeof(int), 8, encInfo->fptr_src_image)!=8)
    {
        return e_failure;
    }
    encode_size_to_lsb(size, buffer);
    if(fwrite(buffer, sizeof(int), 8, encInfo->fptr_stego_image)!=8)
    {
        return e_failure;
    }
    return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    // char buffer[8];
    char buffer[8];
    int i;
    printf("INFO: Encoding secret.txt File Extenstion\n");
    for(i=0;i<strlen(encInfo->extn_secret_file);i++)
    {
    // step 1 -> read 8 bytes from source file
    if(fread(buffer,sizeof(char),8,encInfo->fptr_src_image)!=8)
    {
        return e_failure;
    }

    // step 2 -> call encode_byte_to_lsb(file_extn[i], buffer)
    encode_byte_to_lsb(file_extn[i], buffer);
    // step 3 -> write the buffer into dest file
    if(fwrite(buffer,sizeof(char),8,encInfo->fptr_stego_image)!=8)
    {
        return e_failure;
    }
    // repeat this for strlen(file_extn) times from step 1
    }
    return e_success;

}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    // char buffer[32];
    char buffer[32];
    printf("INFO:Encoding secret file size\n");

    // step 1 -> read 32 bytes from source file

    // step 2 -> call encode_size_to_lsb(file_size, buffer)

    // step 3 -> write the buffer into dest file
    
    if(fread(buffer, sizeof(int), 8, encInfo->fptr_src_image)!=8)
    {
        return e_failure;
    }
    encode_size_to_lsb(file_size, buffer);
    if(fwrite(buffer, sizeof(int), 8, encInfo->fptr_stego_image)!=8)
    {
        return e_failure;
    }
    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    // Move pointer to beginning of secret file

    printf("INFO:Encoding Secret File DATA\n");
    rewind(encInfo->fptr_secret);
    // step 1 -> read secret data from file and store it into encInfo -> secret_data
    
    fread(encInfo->secret_data,encInfo->size_secret_file,1,encInfo->fptr_secret);
    // char buffer[8];
    char buffer[8];
    int i;
    // step 2 -> read 8 bytes from source file
    for(i=0;i<encInfo->size_secret_file;i++)
    {
        if(fread(buffer,sizeof(char),8,encInfo->fptr_src_image)!=8)
        {
            return e_failure;
        }
    // step 3 -> call encode_byte_to_lsb(encInfo -> secret_data[i], buffer)
       encode_byte_to_lsb(encInfo -> secret_data[i], buffer);
    // step 4 -> write the buffer into dest file
       if(fwrite(buffer,sizeof(char),8,encInfo->fptr_stego_image)!=8)
       {
        return e_failure;
       }
    // repeat this for size_secret_file times from step 2
    }
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;
    printf("INFO:Copying remainig image data\n");
    while (fread(&ch, 1, 1, fptr_src) == 1) // Read remaining bytes
    {
        fwrite(&ch, 1, 1, fptr_dest); // Write to destination
    }

    if (ftell(fptr_src) == ftell(fptr_dest)) // Check if both pointers match
    {
        return e_success;
    }
    return e_failure;
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    // Repeat for 8 bits
    
    // Extract LSB from data

    // Clear LSB of image byte

    // Put data bit into image LSB

    // Shift data right
    int i;
    for (i = 0; i < 8; i++)
    {
        image_buffer[i] = image_buffer[i] & (~1);
        image_buffer[i] = (data >> (7 - i) & 1) | image_buffer[i];
    }
    return e_success;
}

Status encode_size_to_lsb(int size, char *imageBuffer)
{
    // write logic to encode the int
    int i;
    for (i = 0; i < 32; i++)
    {
        imageBuffer[i] = imageBuffer[i] & (~1);
        imageBuffer[i] = (size >> (31 - i) & 1) | imageBuffer[i];
    }
    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{
    // step 1 -> check open_files(encInfo) is returning e_success or not
    // no -> print error return e_failure
    // yes -> print success and goto next step

    if (open_files(encInfo) == e_success)
    {
        printf("INFO: Opening required files\n");
        printf("INFO: Opened %s\n", encInfo->src_image_fname);
        printf("INFO: Opened %s\n", encInfo->secret_fname);
        printf("INFO: Opened %s\n", encInfo->stego_image_fname);
        printf("INFO: Done\n");
    }
    else
    {
        printf("INFO:Failed opening required files");
        return e_failure;
    }

    // step 2 -> check check_capacity(encInfo) is returning e_success or not
    // no -> print error return e_failure
    // yes -> print success and goto next step
    printf("### Encoding Procedure Started ###\n");
    if (check_capacity(encInfo) == e_success)
    {
         printf("INFO:Found.OK\n");
    }
    else
    {
        return e_failure;
    }
    

    // step 3 -> call copy_bmp_header(encInfo -> fptr_src_image, encInfo -> fptr_stego_image)is returning e_success or not
    // no -> print error return e_failure
    // yes -> print success and goto next step

    if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        printf("DONE\n");
    }
    else
    {
        printf("INFO:Failed copying header\n");
        return e_failure;
    }

    // step 4 -> call encode_magic_string(MAGIC_STRING, encInfo)

    if(encode_magic_string(MAGIC_STRING,encInfo)==e_success)
    {
      printf("INFO:Done\n");
    }
    else
    {
        printf("INFO:Magic string enoding failed\n");
        return e_failure;
    }

    // step 5 -> call encode_secret_file_extn_size(strlen(encInfo -> extn_secret_file), encInfo)
    if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file),encInfo)==e_success)
    {
      printf("INFO:Done\n");

    }
    else
    {
        printf("INFO:Failed extension size encoding\n");
        return e_failure;
    }
    // step 6 -> call encode_secret_file_extn(encInfo -> extn_secret_file, encInfo)
    if(encode_secret_file_extn(encInfo -> extn_secret_file, encInfo)==e_success)
    {
        printf("INFO:Done\n");
    }
    else
    {
        printf("INFO:Failed extension format encoding");
        return e_failure;
    }

    // step 7 -> call encode_secret_file_size(encInfo ->  size_secret_file, encInfo)
    if(encode_secret_file_size(encInfo -> size_secret_file, encInfo)==e_success)
    {
        printf("INFO:Done\n");
    }
    else
    {
        printf("INFO:Failed file size encoding\n");
        return e_failure;
    }

    // step 8 -> call encode_secret_file_data(encInfo)
    if(encode_secret_file_data(encInfo)==e_success)
    {
        printf("INFO:Done\n");
    }
    else
    {
        printf("INFO:Failed file data encoding\n");
        return e_failure;
    }

    // step 9 -> copy_remaining_img_data(encInfo -> fptr_src_image, encInfo -> fptr_stego_image)is returning e_success or not
    // no -> print error return e_failure
    // yes -> print success
    if((copy_remaining_img_data(encInfo -> fptr_src_image, encInfo -> fptr_stego_image))==e_success)
    {
        printf("INFO:Done\n");
    }
    else
    {
     printf("INFO:Coping of remaining byte failed:\n");
       return e_failure;
    }
    printf("## ENCODING DONE SUCCESSFULLY\n");
    return e_success;
    // return e_success
}

