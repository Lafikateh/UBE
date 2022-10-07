// Standard library headers
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// MSI Executable data location offsets, tested files:
// 0x1A3090: E7721V24.exe, E7721VB6.exe
// 0x1A30A8: E7740V23.exe, E7680vC3.exe
// 0x1A30B4: E7695V30.exe, E7696V20.exe
const unsigned int MSI_Location_Offset[3] = { 0x1A3090, 0x1A30A8, 0x1A30B4 };

// Entry point of the program
int main(int Count, char** Arguments)
{
	// Check if any arguments were provided
	if(Count > 1)
	{
		// Determine the output path based on argument count
		char Output_Path[256];
		if(Count > 2)
		{
			// Copy the provided output path
			strncpy(Output_Path, Arguments[2], sizeof(Output_Path));
		}
		else
		{
			// Copy the input path and append an extension to it
			strncpy(Output_Path, Arguments[1], sizeof(Output_Path) - 5);
			strncat(Output_Path, ".rom", 5);
		}
		
		// Try to open the input file
		FILE* Input_Handle = fopen(Arguments[1], "rb");
		if(Input_Handle != NULL)
		{
			// Get the file size
			fseek(Input_Handle, 0, SEEK_END);
			unsigned int Full_Size = ftell(Input_Handle);
			fseek(Input_Handle, 0, SEEK_SET);
			
			// Check if the file size is a multiple of 128 KB
			if(Full_Size % 131072 == 0)
			{
				printf("The file appears to have already been extracted!\n");
			}
			else
			{
				// Allocate a buffer and read the input file into it
				unsigned char* Input_Buffer = (unsigned char*)(malloc(Full_Size));
				fread(Input_Buffer, 1, Full_Size, Input_Handle);
				
				// Determine the image data offset
				unsigned int Offset = 0;
				if((Full_Size - 2048) % 131072 == 0)
				{
					Offset = 2048;
					printf("Extracting ASUS capsule...\n");
				}
				else if((Full_Size - 4096) % 131072 == 0)
				{
					Offset = 4096;
					printf("Extracting ASROCK capsule...\n");
				}
				else
				{
					for(unsigned int Index = 0; Index < 3; Index++)
					{
						const unsigned int Start = *(unsigned int*)(&Input_Buffer[MSI_Location_Offset[Index]]);
						const unsigned int Length = Full_Size - Start;
						
						if(Length % 1048576 == 0)
						{
							Offset = Start;
							printf("Extracting MSI executable...\n");
							break;
						}
					}
				}
				
				// Check if an offset was found
				if(Offset != 0)
				{
					// Open the output file
					FILE* Output_Handle = fopen(Output_Path, "wb");
					
					// Write the image data to the file
					fwrite(&Input_Buffer[Offset], 1, Full_Size - Offset, Output_Handle);
					
					// Close the output file
					fclose(Output_Handle);
				}
				else
				{
					printf("Unknown format detected!\n");
				}
				
				// Free the input file buffer
				free(Input_Buffer);
			}
			
			// Close the input file
			fclose(Input_Handle);
		}
	}
	else
	{
		printf("No arguments were provided!\n");
	}
	
	return 0;
}