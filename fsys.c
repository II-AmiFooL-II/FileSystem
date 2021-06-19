#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>

#define block_size 8
#define blocks 131072
#define status_blocks blocks/(block_size*8)
#define waste_bytes status_blocks/8
#define entry_size_bytes 64
#define last_status_index status_blocks*block_size



int find_place(){
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
int list_files(char **queue){
	FILE *fp;
	int n;
	fp = fopen("fsysb.bin", "rb");
	fread(&n,sizeof(n),1,fp);
	if(n==0){
		//printf("Filesystem is empty To create files use create command\n");
		return -1;
	}
	int i=1,j=1,k=0;
	char c,buff[64];
	memset(buff,0,sizeof(buff));
	while(j<=n){
		fseek(fp,status_blocks*block_size-i,SEEK_SET);
		fread(&c,sizeof(c),1,fp);
		if(c=='\0'){
			i++;
			continue;
		}
		fseek(fp,blocks*block_size-(i*entry_size_bytes),SEEK_SET);
		fread(buff,sizeof(buff),1,fp);
		while(buff[k]!=':'){
			queue[j-1][k]=buff[k];	
			k++;
		}
		queue[j-1][k]='\0';
		//printf("%d)%s\n",j,str);
		k=0;
		j++;
		i++;
	}
	return n;
}

void entrywriter(char *name, int place){
	FILE *fp = fopen("fsysb.bin","rb+");
	int n,i,j,k;
	char buf[64]={0},*buf1,c;
	fread(&n,sizeof(n),1,fp);
	i=n;
	k=1;
	//printf("n=%d,place=%d\n",n,place);
	buf1=buf;
	while(n>0){
		fseek(fp,status_blocks*block_size-k,SEEK_SET);
		fread(&c,sizeof(c),1,fp);
		if(c=='\0'){
			k++;
			continue;
		}
		fseek(fp,(blocks*block_size)-((k)*entry_size_bytes),SEEK_SET);
		fread(buf,sizeof(buf),1,fp);
		//printf("buf=%s\n",buf);
		if(strncmp(buf,name,strlen(name))==0){
		//	printf("buffer after if=%s\n",buf);
			while(*buf1!='\0'){
				buf1++;
			}
			*buf1=',';
			buf1++;
			//strncat(buf1,",",1);
			char l[10];
			sprintf(l,"%ld",place);
			strncpy(buf1,l,strlen(l));
			buf1+=strlen(l);
			*buf1='\0';
			//strcat(buf,"\0");
			//printf("buf=%s,size=%d\n",buf,strlen(buf));
		//	fseek(fp,(blocks*block_size)-((n)*entry_size_bytes),SEEK_SET);
		//	fwrite(buf,strlen(buf),1,fp);
		//	fseek(fp,(blocks*block_size)-((n)*entry_size_bytes),SEEK_SET);
		//	memset(buf,0,sizeof(buf));
		//	fread(buf,sizeof(buf),1,fp);
		//	printf("read buf=%s\n",buf);
		//	fclose(fp);
			//printf("n=%d\n",n);
			n=k;
			break;
		}
		else{
			n--;
			k++;
		}
	}
	//printf("after while\n");
	if(n<1){
		n=1;
		//printf("i=%d,n=%d\n",i,n);
		memset(buf,0,sizeof(buf));
		char c;
		buf1=buf;
		while(1){
			if(n>(blocks*block_size)){
				printf("file is full delete some files\n");
				return;
			}
			fseek(fp,last_status_index - (n),SEEK_SET);
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
				fseek(fp,last_status_index - n,SEEK_SET);
				fwrite(&c,sizeof(c),1,fp);
				break;
			}
			else{
				//printf("filed to find place file is full delete some files\n");
				n++;
			}
		}
		//printf("after while i = %d\n",i);
		//int t=fseek(fp,(blocks*block_size)-((i)*entry_size_bytes),SEEK_SET);
		//printf("fseek=%d\n",t);

		strncpy(buf1,name,strlen(name));
		buf1+=strlen(name);
		assert(buf1-buf<64);
		*buf1=':';
		buf1++;
		//strcat(buf,":");
		char l[10];
		sprintf(l,"%d",n);
		strncpy(buf1,l,strlen(l));
		//strcat(buf,l);
		buf1+=strlen(l);
		*buf1='~';
		buf1++;
		//strcat(buf,"~");
		sprintf(l,"%ld",place);
		strncpy(buf1,l,strlen(l));
		buf1+=strlen(l);
		*buf1='\0';
		buf1++;
		assert(buf1-buf<64);
		//strcat(buf,l);
		//fseek(fp,(blocks*block_size)-((i)*entry_size_bytes),SEEK_SET);
		//fwrite(buf,sizeof(buf),1,fp);
		//fseek(fp,0,SEEK_SET);
		//fread(&n,sizeof(n),1,fp);
		i++;
		fseek(fp,0,SEEK_SET);
		fwrite(&i,sizeof(i),1,fp);
	}

	//printf("buf=%s\n",buf);
	fseek(fp,(blocks*block_size)-((n)*entry_size_bytes),SEEK_SET);
	fwrite(buf,strlen(buf)+1,1,fp);//to write /0 delimiter
	fseek(fp,(blocks*block_size)-((n)*entry_size_bytes),SEEK_SET);
	memset(buf,0,sizeof(buf));
	fread(buf,64,1,fp);
	//printf("llll=%s\n",buf);
	fclose(fp);
}

void infowriter(char *info,int place){
	//printf("entered infowriter\n");
	//printf("place=%d\n\n",place);
	char buf[8];
//	memset(buf,0,sizeof(buf));
	int i=0;
	//while(i<block_size&&info[i]!='\0'){
	//	buf[i]=info[i];
	//}
	for(i=0;i<block_size&&info[i]!='\0';i++){
		buf[i]=info[i];
	}
	if(i<8){
		buf[i]='\0';
		i++;
	}
	//printf("before info=%s,len=%d\n",buf,strlen(buf));
	FILE *fp = fopen("fsysb.bin","rb+");
	fseek(fp,(status_blocks+place)*block_size,SEEK_SET);
	fwrite(buf,i,1,fp);
	char o[8];
	memset(o,0,sizeof(o));
	fseek(fp,(status_blocks+place)*block_size,SEEK_SET);
	fread(o,i,1,fp);
	//printf("after info=%s,%d,%d\n\n",o,sizeof(o),strlen(o));
	fclose(fp);
}

void create_linker(){
	//printf("entered create linker\n");
	int X = 1024*1024-1;
	int i=0;
	FILE *fp = fopen("fsysb.bin","wb");
	if(fp==NULL){
		printf("error");
	}
	//printf("fopen crossed \n");
	//fseek(fp,X,SEEK_SET);
	//printf("fseek crossed \n");
	fputc('\0',fp);
	fclose(fp);
	//printf("fclose crossed \n");
	fp = fopen("fsysb.bin","rb+");
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
	int place1 = find_place();
	//printf("second call\n");
	int place2 = find_place();
	int j=2;
	//printf("the val of place1 = %ld,place2=%ld\n",place1,place2);
	entrywriter(name,place1);
	entrywriter(name,place2);
	float i= ((int)strlen(info))+1;
	i=i/8;
	//printf("i=%f\n",i);
	for(i;i>0;i--,j--){ 
		//printf("IN loop i=%f j=%d  \n",i,j);
		if(j<=0){
			place1 = find_place();
			entrywriter(name,place1);
		}
		else if(j<2){
			place1=place2;
		}
		infowriter(info,place1);
		info+=block_size;
	}
	printf("created\n");
}
int name_info(char *name,char *info){
	//printf("entered name, info\n");
	int i;
	char **queue;
	queue = (char **)malloc(10*sizeof(char *));
	for(i =0;i<10;i++){
		*(queue + i) = (char *)calloc(100,sizeof(char));
	}
	i = list_files(queue);
	for(i;i>0;i--){
		if(strcmp(*(queue+i),name)==0){
			return -2;
		}
	}
	if(info[0]!='"' || info[strlen(info)-1]!='"'){
		return -1;
	}
	info[strlen(info)-1]='\0';
	info++;
	//printf("\nname=%s,info=%s\n",name,info);
	file_create(name,info);
	return 0;
}

int finder(char *name,char *buffer){
	FILE *fp = fopen("fsysb.bin","rb+");
	int count,i=1,j=1;
	char buff[64],c;
	memset(buff,0,sizeof(buff));
	fread(&count,sizeof(count),1,fp);
	//printf("count=%dand size of buf =%d\n",count,sizeof(buff));
	while(j<=count){
		fseek(fp,status_blocks*block_size-i,SEEK_SET);
		fread(&c,sizeof(c),1,fp);
		if(c=='\0'){
			i++;
			continue;
		}
		//printf("in whle\n");
		fseek(fp,blocks*block_size-(i*entry_size_bytes),SEEK_SET);
		fread(buff,sizeof(buff),1,fp);
		//printf("len is %d\nbuff=%s\n",strlen(buff),buff);
		if(strncmp(name,buff,strlen(name))==0){
			strcpy(buffer,buff);
			return i;
		}
		j++;
		i++;
	}
	return -1;
}
int place_extracter(char *buffer,int *arr){
	int i,j,status=0;
	for(i=0,j=0;buffer[i]!='\0';i++){
		if(buffer[i]=='~'){
			status=1;
			continue;
		}
		if(status==1){
			if(buffer[i]==','){
				j++;
				continue;
			}
			arr[j]=arr[j]*10+buffer[i]-48;
		}
	}
	return j;

}
void view(char *s){
	char buffer[64]={0};
	int j,i,arr[10],status=0;
	FILE *fp;
	memset(arr,0,sizeof(arr));
	if(finder(s,buffer)<0){
		printf("no matching file found\n");
		return;
	}
	fp = fopen("fsysb.bin","rb");
	j = place_extracter(buffer,arr);
	memset(buffer,0,sizeof(buffer));
	for(int i=0;i<=j;i++){
		fseek(fp,(status_blocks+arr[i])*block_size,SEEK_SET);
		fread(&buffer[i*block_size],block_size,1,fp);
	}
	printf("%s\n",buffer);
}
void delete(char *s){
	char buffer[64],c;
	int position,no_of_files,arr[10],j;
	position = finder(s,buffer);
	if(position<0){
		printf("no matching filefound\n");
		return;
	}
	FILE *fp=fopen("fsysb.bin","rb+");

	fseek(fp,0,SEEK_SET);
	fread(&no_of_files,sizeof(no_of_files),1,fp);
	no_of_files--;
	fseek(fp,0,SEEK_SET);
	fwrite(&no_of_files,sizeof(no_of_files),1,fp);

	fseek(fp,last_status_index-position,SEEK_SET);
	fread(&c,sizeof(c),1,fp);
	c='\0';
	fseek(fp,last_status_index-position,SEEK_SET);
	fwrite(&c,sizeof(c),1,fp);
	
	memset(arr,0,sizeof(arr));
	j=place_extracter(buffer,arr);
	int byte_no,bit_no;
	for(j;j>=0;j--){
		byte_no=arr[j]/block_size;
		bit_no=arr[j]%block_size;
		fseek(fp,waste_bytes+byte_no,SEEK_SET);
		fread(&c,sizeof(c),1,fp);
		c &= ~(1<<bit_no);
		fseek(fp,waste_bytes+byte_no,SEEK_SET);
		fwrite(&c,sizeof(c),1,fp);
	}
	fclose(fp);
	printf("deleted\n");
}

void help(){
	printf("For creating a File use create command and the format is\n");
	printf("create <file_name> \"<file_content>\"\n\n");
	printf("For deleting a File use the delete command\n");
	printf("delete <file_name>\n\n");
	printf("For viewing a File use view command\n");
	printf("view <file_name>\n\n");
	printf("To view all File created use ls sommand\n");
	printf("Note that all keywords are case sensitive use only small letters\n");
}

void ls(){
	int i,k=1;
	char **queue;
	queue = (char **)malloc(10*sizeof(char *));
	for(i =0;i<10;i++){
		*(queue + i) = (char *)calloc(100,sizeof(char));
	}
	i = list_files(queue);
	if(i<0){
		printf("No Files in the Filesysten to create use create command\n");
		return;
	}
	printf("The Files are:\n");
	for(i;i>0;i--,k++){
		printf("%d)%s\n",k,queue[i-1]);
	}	
}

void init(){
        FILE *fp;
        if((fp=fopen("fsysb.bin", "rb"))==NULL){
		create_linker();
		return;
	}
	fclose(fp);
}

int comparer(char str[],char** queue,int n){
	int i,k,j;//i is usde for str indexing and k is used for queue indexing
	i=0;
	k=-1;
	while(str[i]!='\0'){
		if(str[i]==' '){
			while(str[i]==' '&&str[i]!='\0'){
				i++;
			}
			if(str[i]=='\0'){
				break;
			}
			k++;
			j=0;
		}
		if(k==-1){
			k++;
			j=0;
		}
		queue[k][j]=str[i];
		j++;
		i++;
	}
	return k;
}
void process(){
	int i;
	char s[1024],**queue;
	queue = (char **)malloc(10*sizeof(char *));
	for(i =0;i<10;i++)
		*(queue + i) = (char *)calloc(100,sizeof(char));
	printf("fsys is open to exit type exit and for help use help\n");
	while(1){
		for(i=0;i<10;i++)
			memset(*(queue+i),0,100);
		printf("fsys ~>");
		gets(s);
		i=comparer(s,queue,100);
		if(strcmp(queue[0],"exit")==0){
			return;
		}
		else if(strcmp(queue[0],"create")==0){
			i=name_info(queue[1],queue[2]);
			if(i==-1){
				printf("please enter the command in right format to use right format use help\n");
			}
			if(i==-2){
				printf("There exist a file with similar name change name or delete older one\n");
			}
		}
		else if(strcmp(queue[0],"view")==0){
			view(queue[1]);
		}
		else if(strcmp(queue[0],"delete")==0){
			delete(queue[1]);
		}
		else if(strcmp(queue[0],"help")==0){
			help();
		}
		else if(strcmp(queue[0],"ls")==0){
			ls();
		}
		else{
			printf("wrong command or please enter again with proper spaces\n");
		}
	}
}
int main(){
	system("clear");
	init();
	process();
}
