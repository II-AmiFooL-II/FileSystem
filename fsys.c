#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define block_size 8
#define blocks 131072
#define status_blocks 2048
#define waste_bytes 256
#define entry_size_bytes 64
#define last_block_bytes status_blocks*block_size


long find_place(){
	//printf("entered find place\n");
	FILE *fp;
	char c;
	if((fp = fopen("fsysb.bin","rb+"))==NULL){
		printf("problem in opening the bin while finding place\n");
	}
	long i,j=blocks;
	int n,k;
	for(i=0,k=0;i<j,k<status_blocks;i+=8,k++){
		//printf("k=%d,i=%d,",k,i);
		fseek(fp,waste_bytes+k,SEEK_SET);
		fread(&c,sizeof(c),1,fp);
		n=0;
		while((c>>n)&1U!=0&&n<=7){
			n++;
		}
		//printf("%d",sizeof(c));
		//return;
		//while(c){
		//	printf("%u ",c?1:0);
		//	c >>= 1;
		//}
		//return;
		//for(int i=7;i>=0;i--){
		//	putchar((c&(1<<i))?'1':'0');
		//}
		//printf(",\n");
		if(n<8){
			c |= 1UL << n;
			fseek(fp,waste_bytes+k,SEEK_SET);
			fwrite(&c,sizeof(c),1,fp);
			fclose(fp);
			return i+n;
		}
	}
	fclose(fp);
	return -1;
}
void entrywriter(char *name,long place1,long place2){//here thirty is a magic number carefull use with caution
	//printf("entered entry writer\n");
	FILE *fp = fopen("fsysb.bin","rb+");
	int state = 0;
	int sum=0,i=1,k=entry_size_bytes,entries;
	char buf[1024]={0},c;
	char blank = ' ';
	float temp=place1;

	//------------------------//
	while(temp>=1&&temp!=0){
		temp/=10;
		sum++;
	}
	if(temp==0)
		sum++;
	//---------------------------//
	if(place2==-1){
		state = 1;
	}
	if(state == 0){
		//printf("enterd if\n");
		temp=place2;
		while(temp>=1&&temp!=0){
			temp/=10;
			sum++;
			//printf("ll");
		}
		if(temp==0)
			sum++;
		sum = sum+2+strlen(name);//one for tilde and one for comma
		//printf("before while\n");
		//printf("last block =%d\n",last_block_bytes);
		while(1){
			//printf("while\n");
			fseek(fp,last_block_bytes-i,SEEK_SET);
			fread(&c,sizeof(c),1,fp);
			if(c=='\0'){
				c |= 1UL <<0;
				c |= 1UL <<1;
				c |= 1UL <<2;
				c |= 1UL <<3;
				c |= 1UL <<4;
				c |= 1UL <<5;
				c |= 1UL <<6;
				c |= 1UL <<7;
				fseek(fp,last_block_bytes-i,SEEK_SET);
				fwrite(&c,sizeof(c),1,fp);
				break;
			}
			else
				i++;
		}
		printf("after while i = %d\n",i);
		fseek(fp,i*entry_size_bytes,SEEK_END);
		strcat(buf,name);
		strcat(buf,"~");
		char l[10];
		sprintf(l,"%ld",place1);
		strcat(buf,l);
		strcat(buf,",");
		sprintf(l,"%ld",place2);
		strcat(buf,l);
		if(sizeof(buf)<entry_size_bytes){
			printf("problem in entring entry size over\n");
		}
		printf("%s\n",buf);
		fwrite(buf,sizeof(buf),1,fp);
		for(sum;sum<=entry_size_bytes;sum++){ 
			fwrite(&blank,sizeof(blank),1,fp);
		}
		fseek(fp,0,SEEK_SET);
		//-----------------------------------------------------------------//
		/*
		buf[10]='\0';
		fread(buf,strlen(buf),1,fp);
		i = atoi(buf);
		i++;
		sprintf(buf,%d,i);
		fseek(fp,0,SEEK_SET);
		fwrite(buf,strlen(buf),1,fp);
		*/
		fread(&entries,sizeof(int),1,fp);
		entries++;
		fseek(fp,0,SEEK_SET);
		fwrite(&entries,sizeof(int),1,fp);
	}

	if(state == 1){
		i=1;
		fseek(fp,0,SEEK_SET);
		fread(&entries,sizeof(int),1,fp);
		temp=strlen(name);
		buf[k]='\0';
		while(entries>0){
			fseek(fp,i*k,SEEK_END);
			fread(buf,sizeof(buf),1,fp);
			if(buf[0]==' '){
				printf("no matching file found\n");
				fclose(fp);
				return;
			}
			if((strncmp(buf,name,temp)!=0)){
				entries--;
				continue;
			}
			strcat(buf,",");
			char l[10];
			sprintf(l,"%ld",place1);
			strcat(buf,l);
			fseek(fp,i*k,SEEK_END);
			fwrite(buf,strlen(buf),1,fp);	
		
		}

	}
	fclose(fp);
}

void infowriter(char *info,long place){
	//printf("entered infowriter\n");
	FILE *fp = fopen("fsysb.bin","a+");
	fseek(fp,status_blocks*block_size+place,SEEK_SET);
	//fprintf(fp,"%s",info);
	fwrite(info,sizeof(info),1,fp);
	fclose(fp);
}

void create_linker(){
	//printf("entered create linker\n");
	int X = 1024*1024-1;
	int i=0;
	FILE *fp = fopen("fsysb.bin","w");
	if(fp==NULL){
		printf("error");
	}
	//printf("fopen crossed \n");
	//fseek(fp,X,SEEK_SET);
	//printf("fseek crossed \n");
	fputc('\0',fp);
	fclose(fp);
	//printf("fclose crossed \n");
	fp = fopen("fsysb.bin","r+");
	//printf("2fopen crossed \n");	
	if(fp==NULL){
		printf("error");
	}
	fseek(fp,waste_bytes,SEEK_SET);
	//printf("2fseek");
	char buf[status_blocks*block_size]={0};
	memset(buf,0,sizeof(buf));

	fwrite(buf,sizeof(buf),1,fp);
	//printf("fread  check\n");
	fseek(fp,0,SEEK_END);
	fwrite(&i,sizeof(i),1,fp);
	//printf("fwrite check \n");
	fclose(fp);
}
void file_create(char *name,char *info){
	//printf("entered file create\n");
	//printf("%s\n%s\n",name,info);
	long place1 = find_place();
	//printf("second call\n");
	long place2 = find_place();
	int j=2;
	//printf("the val of place1 = %ld,place2=%ld\n",place1,place2);
	entrywriter(name,place1,place2);
	float i= ((int)strlen(info));
	i=i/8;
	//printf("i=%f\n",i);
	for(i;i>0;i--,j--){ 
		//printf("IN loop i=%f j=%d  \n",i,j);
		if(j<=0){
			//place1 = find_place();
			entrywriter(name,place1,-1);
		}
		else if(j==1){
			place1=place2;
		}
		infowriter(info,place1);
		info+=8;
	}
	
}
void name_info(char *s){
	//printf("entered name info\n");
	int i=0;
	char name[10];
	while(*s!='"'){
	//	printf("%c\n",s[i]);
		name[i]=*s;
		i++;
		s++;
	}
	name[i-1] = '\0';
	s++;// it starts from first letter of info not the cote
	for(i=0;s[i]!='"';i++){
	}
	s[i]='\0';
	file_create(name,s);
}
void init(){
        FILE *fp;
        if((fp=fopen("fsysb.bin", "r"))==NULL){
		create_linker();
		return;
	}
	fclose(fp);
	/*if((fp = fopen("linker.txt","r"))==NULL){
		create_linker();
		printf("created linker\n");
	}
	fclose(fp);
	fp = fopen("filelink.txt","w+");
	//printf("created files\n");
	fclose(fp);*/
}
int finder(char *name,char *buffer){
	FILE *fp = fopen("fsysb.bin","r");
	int count;
	fread(&count,sizeof(count),1,fp);
	printf("count=%d\n",count);
	while(count>0){
		printf("in whle\n");
		fseek(fp,entry_size_bytes*count,SEEK_END);
		fread(buffer,sizeof(buffer),1,fp);
		printf("%s\n",buffer);
		if(strncmp(name,buffer,strlen(name))==0)
			return 0;
		count--;
	}
	return -1;
}
void view(char *s){
	printf("%s\n",s);
	char buffer[64];
	int status,j,arr[10];
	FILE *fp;
	status = finder(s,buffer);
	fp = fopen("fsysb.bin","r");
	if(status<0){
		printf("no matching file found\n");
	}
	for(int i=0,j=0;buffer[i]!=' ';i++){
		if(buffer[i]=='~'){
			status=1;
		}
		if(status==1){
			if(buffer[i]==','){
				j++;
				continue;
			}
			arr[j]=arr[j]*10+buffer[i]-48;
		}

	}
	for(int i=0;i<j;i++){
		fseek(fp,status_blocks*block_size+arr[i],SEEK_SET);
		fread(buffer+(i+1)*8,8,1,fp);
	}
	printf("\n%s\n",buffer);
}
void process(){
	int state;
	char s[1024];
	printf("fsys is open to exit type exit\n");
	while(1){
		printf("fsys ~>");
		gets(s);
		//printf("%s\n",s);
		if(strncmp(s,"exit",4)==0){
			return;
		}
		else if(strncmp(s,"create",6)==0){
			state = 1;
			//printf("%s\n",s);
			name_info(s+7);
		}
		else if(strncmp(s,"view",4)==0){
			view(s+5);
		}
		else{
			printf("wrong command or please enter again with proper spaces\n");
		}
	}
}
int main(){
	system("clear");
	init();
	//printf("crossed init\n");
	process();
//	cleanup();
}
