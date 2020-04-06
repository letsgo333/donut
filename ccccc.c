##include <stdio.h>
#include <stdlib.h>

int main(int argc, char * argv[]) {
	
	if (argv[1]!=NULL && argv[2] !=NULL ){
		//Create New shellcode file 
		
		printf("Create new file :%s\n",argv[2]);
		return 0;
	}else if (argv[1] != NULL && argv[2] == NULL){
		//Run shellcode filename:a1
		
		printf("Run shellcode file :%s",argv[1]);
		return 0;
	}else{
		printf("Nothing just break");
		return 0;
	}
	

//	int a = 64;
//	int b = 1169;
//	int c = a^b;
	//1169
//	printf("a^b = %d",c);
	
	return 0;
}


//Create new Shellcode File
void CreateNewFile(char * oldfile ,char * newfile){
	//read old file
	int key = 249685;
	FILE * oldf,newf;
	oldf = fopen(oldfile,"rb");
	newf = fopen(newfile,"wb");
	unsigned char buf[MAXLEN];
	if(oldf == NULL || newf == NULL){
		exit(1);
	}
	
	int rc;
	//here need oldf stream ^ key
	while((rc=fread(buf,sizeof(unsigned char),MAXLEN,oldf))!=0){
		fwrite(buf,sizeof(unsigned char),rc,newf);
	}
	fclose(oldf);
	fclose(newf);
	return 0;
}


void RunCode(char * file){
	//Run file
	
	
}


void decodefile(char * file){
	//here need newf key ^ newf stream
	
	
}









