// DOCUMENTATION
/*
Name: Sidharth R Nath
Student ID: 26018_120
Batch ID: 26018D

Start Date: 08-10-2026
End Date: 19-10-2026

Description:
Image Steganography is a C project that hides secret text data inside a BMP image
using Least Significant Bit (LSB) encoding and retrieves it through decoding.

File : main.c

-->main()
Validates command-line arguments and selects the encoding or decoding operation.
Calls the appropriate validation and processing functions to perform the operation.

-->check_operation_type()
Checks whether the user selected encoding (-e) or decoding (-d).
Returns the corresponding operation type or an unsupported-operation status.

File : encode.c

-->get_image_size_for_bmp()
Reads the width and height of the BMP image from its header.
Calculates the image capacity using width × height × 3.

-->get_file_size()
Determines the size of the secret file using fseek() and ftell().
Rewinds the file pointer to the beginning after calculating its size.

-->read_and_validate_encode_args()
Validates the source BMP image, secret TXT file, and optional output filename.
Stores the validated filenames and the secret file extension in EncodeInfo.

-->open_files()
Opens the source image, secret file, and destination stego image.
Returns failure if any required file cannot be opened.

-->check_capacity()
Calculates the source image capacity and the size of the secret file.
Checks whether the image has sufficient capacity to store the secret data.

-->copy_bmp_header()
Reads the first 54 bytes of the source BMP image.
Writes the same header bytes into the destination stego image.

-->encode_magic_string()
Embeds the magic string "#*" into the least significant bits of image bytes.
This signature helps the decoder identify an image containing hidden data.

-->encode_secret_file_extn_size()
Encodes the length of the secret file extension into 32 image-byte LSBs.
This allows the decoder to determine how many extension characters to extract.

-->encode_secret_file_extn()
Encodes each character of the secret file extension into image-byte LSBs.
Stores the extension information in the destination stego image.

-->encode_secret_file_size()
Encodes the size of the secret file into 32 image-byte LSBs.
This tells the decoder how many bytes of secret data must be recovered.

-->encode_secret_file_data()
Reads the contents of the secret file and processes them byte by byte.
Embeds each data byte into the LSBs of eight image bytes.

-->copy_remaining_img_data()
Copies the remaining source image bytes after the secret data is encoded.
Preserves the rest of the image data in the destination file.

-->encode_byte_to_lsb()
Stores the eight bits of a character in the LSBs of eight image bytes.
Preserves the other bits of each image byte.

-->encode_size_to_lsb()
Stores the 32 bits of an integer in the LSBs of 32 image bytes.
Preserves the remaining bits of the image data.

-->do_encoding()
Coordinates file opening, capacity checking, and BMP header copying.
Calls the encoding functions in sequence and reports success or failure.

File : decode.c

-->read_and_validate_decode_args()
Validates the encoded BMP filename and selects the decoded output filename.
Uses the specified output name or defaults to "encoded.txt".

-->open_outfile()
Opens the encoded image for reading and the decoded file for writing.
Returns failure if either file cannot be opened.

-->decode_magic_string()
Extracts the embedded magic string from the encoded image.
Verifies the extracted characters against the expected signature "#*".

-->decode_byte()
Reconstructs one character from the LSBs of eight image bytes.
Compares the reconstructed character with the expected character.

-->decode_file_extn_size()
Extracts the 32-bit value representing the secret file extension length.
Stores the recovered length in the DecodeInfo structure.

-->decode_file_extn()
Extracts the secret file extension character by character from the image.
Stores the recovered extension in the extension buffer.

-->decode_file_size()
Extracts the 32-bit value representing the original secret file size.
Stores the recovered size in the DecodeInfo structure.

-->decode_data()
Extracts the specified number of secret data bytes from the encoded image.
Stores the recovered bytes in the decoded-data buffer.

-->write_data()
Writes the recovered secret data to the decoded output file.
Uses fwrite() to write the specified number of bytes.

-->do_decoding()
Coordinates file opening, magic string verification, and metadata extraction.
Decodes the secret data and writes it to the output file.

File:common.h
Defines the magic string used to identify hidden data.

File : types.h
Defines data types and enums for status(ssucces and failure) and operation selection(decode or encode).

File : encode.h
Defines EncodeInfo(strcuture) and declares encoding function prototypes.

File : decode.h
Defines DecodeInfo(structures) and declares decoding function prototypes.

##########SAMPLE OUTPUT##########

------------ENCODING-----------------------

./a.out -e beautiful.bmp secret.txt out.bmp
ENCODE OPERATION SELECTED
INFO: Opening required files
INFO: Opened beautiful.bmp
INFO: Opened secret.txt
INFO: Opened out.bmp
INFO: Done
### Encoding Procedure Started ###
INFO:Checking for secret.txt size
INFO: Done. Not Empty
INFO: Checking for beautiful.bmp capacity to handle secret.txt
width = 1024
height = 768
INFO:Found.OK
INFO: Copying Image Header
DONE
INFO: Encoding Magic String Signature
INFO:Done
INFO: Encoding secret.txt File Extenstion size
INFO:Done
INFO: Encoding secret.txt File Extenstion
INFO:Done
INFO:Encoding secret file size
INFO:Done
INFO:Encoding Secret File DATA
INFO:Done
INFO:Copying remainig image data
INFO:Done
## ENCODING DONE SUCCESSFULLY

-----------DECODING---------
DECODE OPERATION SELECTED
### Decoding procedure Started ###
INFO:Opening required files
INFO:Opened out.bmp file
INFO:Decoding Magic string Signature
INFO:Decoding Magic string Signature
INFO:Done
INFO:Decoding file extension size
INFO:Done
INFO:Decoding file extension
INFO:Done
INFO:Done
INFO:Decoding file size
INFO:Done
INFO:Decoding data
INFO:Done
## INFO:Decoding Done succesfully ##
*/

#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "decode.h"

OperationType check_operation_type(char *);
OperationType operation;
int main(int argc, char *argv[]) // .aout -e <source_file> <secret_data_file> <optional>
{
  EncodeInfo encInfo;
  DecodeInfo decodeinfo;
  // step 1 -> check check_operation_type(argv[1]) is returning e_encode or not
  //  yes -> EncodeInfo encInfo;
  //  check read_and_validate_encode_args(argv, &encInfo) is returning e_success or e_failure
  //  failure -> print error msg and stop
  //  success -> print success msg
  //  check do_encoding(&encInfo) is returning e_success or e_failure
  //  failure -> print error msg and stop
  //  success -> print success msg and stop

  if (argc < 2 || argc > 5) //checks the no of arguments passed enough or not
  {
    printf("For Encoding: ./a.out -e <.bmp_file> <.text_file> [output file]\n");
    printf("For Decoding: ./a.out -d <.bmp_file> [output file]\n");
    return 0;
  }

  operation = check_operation_type(argv[1]);
  if (operation == e_encode)
  {
    if (argc < 4)
    {
      printf(" Encoding: ./a.out -e <.bmp_file> <.text_file> [output file]");
      return 0;
    }
    if (read_and_validate_encode_args(argv, &encInfo) != e_success)
    {
      printf("Validation failed\n");
      return 0;
    }
    if (do_encoding(&encInfo) != e_success)
    {
      printf("Encoding failed\n");
      return 0;
    }
  }
 //else
  // step 2 -> check check_operation_type(argv[1]) is returning e_decode or not
  //  yes -> EncodeInfo encInfo;
  //  check read_and_validate_encode_args(argv, &decodeinfo) is returning e_success or e_failure
  //  failure -> print error msg and stop
  //  success -> print success msg
  //  check do_encoding(&decodeinfo) is returning e_success or e_failure
  //  failure -> print error msg and stop
  //  success -> print success msg and stop
  else if (operation == e_decode)
  {
    if (argc < 3)
    {
      printf("Decoding: ./a.out -d <.bmp_file> [output file]");
      return 0;
    }
    if (read_and_validate_decode_args(argv, &decodeinfo) != e_success)
    {
      printf("Validation failed\n");
      return 0;
    }
    if (do_decoding(&decodeinfo) != e_success)
    {
      printf("Encoding failed\n");
      return 0;
    }
  }
  else
  {
    printf("Invalid operation Selected\n");
    printf("For Encoding: ./a.out -e <.bmp_file> <.text_file> [output file]\n");
    printf("For Decoding: ./a.out -d <.bmp_file> [output file]\n");
  }
  return 0;
}

OperationType check_operation_type(char *symbol)
{
  // step 1 -> check symbol is -e or not
  //  yes -> return e_encode

  // step 2 -> check symbol is -d or not
  // yes -> return e_decode

  // return e_unsupported

  if (strcmp(symbol, "-e") == 0)
  {
    printf("ENCODE OPERATION SELECTED\n");
    return e_encode;
  }

  if (strcmp(symbol, "-d") == 0)
  {
    printf("DECODE OPERATION SELECTED\n");
    return e_decode;
  }
  return e_unsupported;
}
