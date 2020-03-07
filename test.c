#include<stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

int test1(char* str){
	char	*x = NULL,
            *y = NULL;
    char *ptr = strchr(str, ',');

    printf("%s\n", str);

    if(ptr == NULL)
        return -1;
    /*
    printf("%s\n", ptr);
    printf("%d\n", ptr - str);
    printf("%d\n", strlen(str));
    printf("deref %s\n",str);
    */
    int given_size = ptr-str;
    int family_size = strlen(str) - given_size-1;

    if(given_size < 2 || family_size < 2)
        return -1;

    if(*(ptr-1) == ' ')
        return -1;

    if(strchr(ptr+1, ',')!= NULL)
        return -1;

    if(*(ptr+1) == ' '){
        ptr+=1;
        family_size -=1;
    }
    // printf("%d %d %c %c \n", given_size, family_size, str[0], ptr[1]);
    if(('a'<=str[0] && str[0]<= 'z') || ('a'<=ptr[1] && ptr[1]<= 'z'))
        return -1;

    printf("%d %d\n", given_size, family_size);
    x = (char *) malloc(sizeof(char)*(given_size+1));
    y = (char *) malloc(sizeof(char)*(family_size+1));

    memcpy(x, str, given_size);
    memcpy(y, ptr+1, family_size);
    *(x+given_size) = '\0';
    *(y+family_size) = '\0';

    printf("x %s y %s\n", x, y);
    free(x);
    free(y);
    return 0;
}

int main(){
    char* test_data [] = {
        "Smith,John",
        "Smith, John",
        "O'Brien, Patrick Sean",
        "Mahagedara Patabendige,Minosha Mitsuaki Senakasiri",
        "I-Sun, Chen Wang",
        "Clifton-Everest,Charles Edward"
        };

    for (int i = 0; i < sizeof(test_data) / sizeof(char *); i++){
        assert(test1(test_data[i]) == 0);
    }

    char* test_data2 [] = {
        "Jesus",
        "Smith  ,  Harold",
        "Gates, William H., III",
        "A,B C",
        "Smith, john"
        };
    for (int i = 0; i < sizeof(test_data2) / sizeof(char *); i++){
        assert(test1(test_data2[i])==-1);
    }
    printf("done");
    return 0;
}
