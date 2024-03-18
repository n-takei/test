//#include <stdio.h>
//#include <string.h>
//
//#define MAX_FILENAME_LEN 256
//#define MAX_READ_LEN 4096
//int main(){
//
//	char fileName[MAX_FILENAME_LEN];
//	scanf_s("%s", fileName, MAX_FILENAME_LEN);
//
//	FILE* fp = NULL;
//	errno_t ret = fopen_s(&fp, fileName, "r");
//
//	if (ret != 0) {
//		printf("ファイルオープンエラー");
//		return -1;
//	}
//
//	char line[MAX_READ_LEN], *result;
//	while ((result = fgets(line, MAX_READ_LEN, fp)) != NULL)
//	{
//		printf("%s", line);
//	}
//
//	fclose(fp);
//
//	return 0;
//}