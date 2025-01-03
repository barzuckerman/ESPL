#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 5
#define BUFFER_SIZE 100

//-----------------------struct-----------------------
typedef struct {
  char debug_mode;
  char file_name[128];
  int unit_size;
  unsigned char mem_buf[10000];
  size_t mem_count;
  // additional fields
  char display_mode;
} state;

typedef struct fun_desc
{
    char *name;
    void (*fun)(state *s);
} FunDesc;

//-----------------------functions-----------------------
void ToggleDebugMode(state* s);
void setFileName(state* s);
void setUnitSize(state* s);
void loadIntoMemory(state* s);
void toggleDisplayMode(state* s);
void fileDisplay(state* s);
void memoryDisplay(state* s);
void saveIntoFile(state* s);
void memoryModify(state* s);
void quit(state* s);

//-----------------------vars-----------------------
static char* hex_formats[] = {"%#hhx\n", "%#hx\n", "No such unit", "%#x\n"};
static char* dec_formats[] = {"%#hhd\n", "%#hd\n", "No such unit", "%#d\n"};
//-----------------------main-----------------------
int main(int argc, char **argv)
{
    char input[MAX_LEN];
    FunDesc arr[] = {{"Toggle Debug Mode", ToggleDebugMode}, {"Set File Name", setFileName},{"Set Unit Size", setUnitSize} ,{"Load Into Memory", loadIntoMemory},{"Toggle Display Mode", toggleDisplayMode},{"File Display", fileDisplay},{"Memory Display", memoryDisplay}, {"Save Into File", saveIntoFile}, {"Memory Modify", memoryModify}, {"Quit", quit}, {NULL, NULL}};
    int bound = sizeof(arr) / sizeof(arr[0]) ;
    state* s = NULL;
    s = malloc(sizeof(state));
    if (s == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
    }
    
    s->debug_mode = 0;
    s->unit_size = 1;
    s->mem_count = 0;
    memset(s->file_name, 0, sizeof(s->file_name));
    s->display_mode = 0; // Default display mode is decimal
    while (!feof(stdin))
    {
        if (s->debug_mode == 1)
        {
            fprintf(stderr, "\nunit size: %d\nfile name: %s\nmem count: %d\n\n", s->unit_size, s->file_name, s->mem_count);
        }
        fprintf(stdout, "Choose action: \n");
        int num_input = 0;
        // printing the menu
        int i = 0;
        while (arr[i].name != NULL)
        {
            fprintf(stdout, "%d) %s\n", i, arr[i].name);
            i++;
        }

        fprintf(stdout, "option number: ");
        if (fgets(input, sizeof(input), stdin) == NULL)
        { // read the number
            quit(s);
            return 0;
        }
        input[strcspn(input, "\n")] = '\0'; // remove the last char of the empty char
        if (sscanf(input, "%d", &num_input) != 1)
        {
            quit(s);
            return 0; // If sscanf fails to parse an integer, exit
        } // convert to int
        if (num_input < 0 || num_input >= bound)
        { // check the number is ok
            printf("not in bounds\n");
        }
        else{
            arr[num_input].fun(s);
            printf("DONE.\n");
        }

        
    }
    return 0;
}

void ToggleDebugMode(state* s)
{
    if (s->debug_mode == 1)
    {
        s->debug_mode = 0;
        printf("Debug flag now off\n");
    }
    else
    {
        s->debug_mode = 1;
    printf("Debug flag now on\n");
    }
}

void setFileName(state* s)
{
    char buffer[BUFFER_SIZE];
    printf("Enter file name: \n");
    if (fgets(buffer, sizeof(buffer), stdin) == NULL)
    {
        quit(s);
    }
    buffer[strcspn(buffer, "\n")] = '\0'; // remove the last char of the empty char
    strcpy(s->file_name, buffer);
    if (s->debug_mode == 1)
    {
        fprintf(stderr, "Debug: file name set to %s\n", s->file_name);
    }

}

void setUnitSize(state *s) {
    int size;
    printf("Enter unit size (1, 2, or 4): ");
    char buffer[10];
    if(fgets(buffer, sizeof(buffer), stdin) == NULL) {
        quit(s);
    }
    sscanf(buffer, "%d", &size);
    if (size == 1 || size == 2 || size == 4) {
        s->unit_size = size; // Set the new unit size
        if (s->debug_mode) {
            printf("Debug: set size to %d\n", s->unit_size);
        }
    } else {
        printf("Invalid size; size unchanged.\n");
    }
}

void loadIntoMemory(state* s)
{
    if(strcmp(s->file_name, "") == 0) {
        fprintf(stderr, "Error: file name is empty\n");
        return;
    }
    FILE* file = fopen(s->file_name, "rb");
    if (file == NULL){
        fprintf(stderr, "Error: file didn't open\n");
        return;
    }
    int locationHEX;
    int lengthDEC;

    printf("Please enter <location> <length> ");
    char buffer[10];
    if(fgets(buffer, sizeof(buffer), stdin) == NULL) {
        quit(s);
    }
    if (sscanf(buffer, "%x %d", &locationHEX, &lengthDEC) != 2) {
        printf("Invalid input. Format: <location_hex> <length_decimal>\n");
        fclose(file);
        return;
    }
    if (s->debug_mode) {
        printf("Debug: file name: %s, location: %x, length: %d\n", s->file_name, locationHEX, lengthDEC);
    }


    fseek(file, locationHEX, SEEK_SET);

    size_t bytesRead = fread(s->mem_buf, s->unit_size, lengthDEC, file);
    if (bytesRead == 0)
    {
        printf("No data read from file\n");
        fclose(file);
        return;
    }
    s->mem_count = bytesRead * s->unit_size; // Update mem_count in bytes
    
    printf("Loaded %d units into memory from location 0x%X\n", bytesRead, locationHEX);

    fclose(file);
}

void toggleDisplayMode(state* s)
{
    if (s->display_mode == 1)
    {
        s->display_mode = 0;
        printf("Display flag now off, decimal representation\n");
    }
    else
    {
        s->display_mode = 1;
        printf("Display flag now on, hexadecimal representation\n");
    }
}

void fileDisplay(state* s)
{
    if(strcmp(s->file_name, "") == 0) {
        fprintf(stderr, "Error: file name is empty\n");
        return;
    }
    FILE* file = fopen(s->file_name, "rb");
    if (file == NULL){
        fprintf(stderr, "Error: file didn't open\n");
        return;
    }
    int offsetHex; // addr
    int lengthDEC; // u Number of units to display

    printf("Enter file offset and length ");
    char buffer[10];
    if(fgets(buffer, sizeof(buffer), stdin) == NULL) {
        quit(s);
    }
    if (sscanf(buffer, "%x %d", &offsetHex, &lengthDEC) != 2) {
        printf("Invalid input. Format: <offset_hex> <length_decimal>\n");
        fclose(file);
        return;
    }

    fseek(file, offsetHex, SEEK_SET);
    unsigned char *readBuffer = malloc(lengthDEC * s->unit_size);
    if (!readBuffer)
    {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(file);
        return;
    }

    size_t bytesRead = fread(readBuffer, s->unit_size, lengthDEC, file);
    if (bytesRead == 0)
    {
        printf("No data read from file\n");
        free(readBuffer);
        fclose(file);
        return;
    }

    printf("%s\n=======\n", s->display_mode == 0 ? "Decimal" : "Hexadecimal");
    for (size_t i = 0; i < bytesRead; i++)
    {
        //converted into a proper variable type
        unsigned int value = 0;
        memcpy(&value, readBuffer + i * s->unit_size, s->unit_size);

        if (s->display_mode == 0)
        {
            printf(dec_formats[s->unit_size - 1], value);
        }
        else
        {
            printf(hex_formats[s->unit_size - 1], value);
        }
    }

    free(readBuffer);
    fclose(file);
}

void memoryDisplay(state* s)
{
    int addressHex; // addr
    int lengthDEC; // u Number of units to display

    printf("Enter address and length ");
    char buffer[10];
    if(fgets(buffer, sizeof(buffer), stdin) == NULL) {
        quit(s);
    }
    if (sscanf(buffer, "%x %d", &addressHex, &lengthDEC) != 2) {
        printf("Invalid input. Format: <address_hex> <length_decimal>\n");
        return;
    }

    printf("%s\n=======\n", s->display_mode == 0 ? "Decimal" : "Hexadecimal");
    for (int i = 0; i < lengthDEC; i++)
    {
        //converted into a proper variable type
        unsigned int value = 0;
        // Calculate the current address in the memory buffer
        int currentAddress = addressHex + (i * s->unit_size);
        memcpy(&value, s->mem_buf + currentAddress, s->unit_size);

        if (s->display_mode == 0)
        {
            printf(dec_formats[s->unit_size - 1], value);
        }
        else
        {
            printf(hex_formats[s->unit_size - 1], value);
        }
    }
}

void saveIntoFile(state* s)
{
    if(strcmp(s->file_name, "") == 0) {
        fprintf(stderr, "Error: file name is empty\n");
        return;
    }
    FILE* file = fopen(s->file_name, "r+");
    if (file == NULL){
        fprintf(stderr, "Error: file didn't open\n");
        return;
    }

    unsigned int sourceAddress;
    unsigned int targetLocation;
    int length;

    printf("Please enter <source-address> <target-location> <length> ");
    char buffer[100];
    if(fgets(buffer, sizeof(buffer), stdin) == NULL) {
        quit(s);
    }
    if (sscanf(buffer, "%x %x %d", &sourceAddress, &targetLocation, &length) != 3) {
        printf("Invalid input. Format: <source-address> <target-location> <length>\n");
        fclose(file);
        return;
    }
    fseek(file, 0, SEEK_END);
        if (targetLocation > ftell(file))
        {
            fprintf(stderr, "Target location is greater than file size\n");
            fclose(file);
            return;
        }

    if (s->debug_mode)
        fprintf(stderr, "Debug: source address: %#X, target location: %#X, length: %d\n", sourceAddress, targetLocation, length);

    fseek(file, targetLocation, SEEK_SET);
    fwrite(sourceAddress ? (void *)sourceAddress : s->mem_buf, s->unit_size, length, file);
    

    fclose(file);
}

void memoryModify(state* s)
{
    unsigned int locationHex, valueHex;
    printf("Please enter <location> <val> ");
    char buffer[100];
    if(fgets(buffer, sizeof(buffer), stdin) == NULL) {
        quit(s);
    }
    if (sscanf(buffer, "%x %x", &locationHex, &valueHex) != 2) {
        printf("Invalid input. Format: <location> <val>\n");
        return;
    }
    if (s->debug_mode)
        fprintf(stderr, "Debug: location: %#X, value: %#X\n", locationHex, valueHex);
    
    // Check that location is within bounds
    if (locationHex >= sizeof(s->mem_buf)) {
        fprintf(stderr, "Error: Location is out of bounds of the memory buffer.\n");
        return;
    }

    // Check that we do not exceed memory bounds based on unit size
    if ((locationHex + s->unit_size) > sizeof(s->mem_buf)) {
        fprintf(stderr, "Error: Writing extends past the end of the memory buffer.\n");
        return;
    }
    memcpy(s->mem_buf + locationHex, &valueHex, s->unit_size);
}

void quit(state* s)
{
    if (s->debug_mode == 1)
    {
        fprintf(stderr, "quitting\n");
    }
    free(s);
    exit(0);
}