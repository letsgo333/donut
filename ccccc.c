#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>


int main(int argc, char * argv[]) {
	
	if (argv[1]!=NULL && argv[2] !=NULL ){
		//Create New shellcode file 
		DecodeCreateNewFile(argv[1],argv[2]);
		
		printf("Create new file :%s\n",argv[2]);
		return 0;
	}else if (argv[1] != NULL && argv[2] == NULL){
		//Run shellcode filename:a1
		
		RunCode(DecodeCreateNewFile(argv[1]));
		
		printf("Run shellcode file :%s",argv[1]);
		return 0;
	}else{
		printf("Nothing just break");
		return 0;
	}
	

	return 0;
}


//Create new Shellcode File
void EncodeCreateNewFile(char * oldfile ,char * newfile){
	//read old file
	int keylen, index=0;
    char *source, *dest, *key, fBuffer[1], tBuffer[20], ckey;
    FILE *fSource, *fDest;
    source = oldfile; // 原文件
    dest = newfile;   // 目的文件
    key = "rmvpfr";    // 加密字串
    // 获取key长度
    keylen = strlen(key);
 
    fSource = fopen(source, "rb");
    fDest = fopen(dest, "wb");
 
    while(!feof(fSource)){
        
        fread(fBuffer, 1, 1, fSource);    // 读取1字节
        
        if(!feof(fSource)){
            ckey = key[index%keylen];     // 循环获取key
            *fBuffer = *fBuffer ^ ckey;   // xor encrypt
            fwrite(fBuffer, 1, 1, fDest); // 写入文件
            index ++;
        }
    
    }
 
    fclose(fSource);
    fclose(fDest);
    exit(0);

}

void DecodeCreateNewFile(char * oldfile){
	//decode file
	int keylen, index=0;
    char *source, *key, fBuffer[1], tBuffer[20], ckey;
    FILE *fSource;
    source = oldfile;
    key = "rmvpfr";
    keylen = strlen(key);
    fSource = fopen(source, "rb");
    while(!feof(fSource)){
        fread(fBuffer, 1, 1, fSource);
        if(!feof(fSource)){
            ckey = key[index%keylen];	//get key
            *fBuffer = ckey ^ *fBuffer;	//xor decrypt
            index ++;
        }
    }
    fclose(fSource);
 	return *fBuffer;
}


void RunCode(char * file){
	//Run file load shellcode to RAM
	printf("Run success");
	return 0;
}


