
// program to add a /n in one line array and Brakets for 128x160 display array [128]1[160] d.lobos

//file name background.c must be in the same folder can be changed.

# define FILENAME "background.txt"

# include <stdio.h>
# include <stdlib.h>

void ins_new_line_and_brackets( char* file){

const char separator=',';
volatile char ch;
int count_separator=0;
int quantity_separator=16;
//are 8 rows x 16 pixels to have 128 pixels
int quantity_newline_in_row=8;
int count=0;
int countbracket=0;

// temporal file to add the new line
const char* temp_file= "temp_file.txt";

//open files

FILE *fileopen, *tempopen; 

fileopen= fopen(file,"r"); 
tempopen= fopen(temp_file,"w");

if (fileopen==NULL | tempopen==NULL){
printf("error open files");
exit(EXIT_FAILURE);
}

printf("counting character %c in file %s...\n",separator,file);

fputc('{',tempopen);
fputc('{',tempopen);

for (;;){

if (feof(fileopen)){
fputc('\n',tempopen);
fputc('}',tempopen);
fputc('}',tempopen);
fputc('\n',tempopen);
fputc('\0',tempopen);

break;

}


ch=  fgetc(fileopen);
printf("Extracting char: %c \n", ch);

if(ch==separator){
count++;
printf("Separator found \n");

}

if(count==quantity_separator){
printf("New line added \n");

countbracket++;


if(countbracket==quantity_newline_in_row){
printf("A row was completed \n");

fputc('}',tempopen);
fputc(',',tempopen);
fputc(' ',tempopen);
fputc('\n',tempopen);
fputc('\n',tempopen);
fputc('{',tempopen);
countbracket=0;

}
else{
fputc(ch,tempopen);
fputc('\n',tempopen);

}

count=0;
}
else{
if (ch!='\0'){
//store readed character into temp file without store end of file
printf("saving char: %c in temp \n ",ch);
fputc(ch,tempopen);
}
}
}

// finished the dump in temp
printf("Finished dumping in temp...about closing and renaming \n");


fclose(fileopen);
fclose(tempopen);

// delete original and rename temporal;

if(remove(file)!=0){
printf("error removing original, try using sudo");
}

if(rename(temp_file,file)!=0){
printf("error renaming modified file, try using sudo");
}

}

int main(){

ins_new_line_and_brackets(FILENAME);

return 0;
}
