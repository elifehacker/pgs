#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
typedef struct Pname
{
	char *		family;
	char *		given;
} Pname;

typedef struct Test
{
	char *		string;
	char *		string2;

} Test;

Pname * test1(char* str){
	char	*family = NULL,
            *given = NULL;
    char *ptr = strchr(str, ',');
    Pname   *result;
    printf("%s\n", str);

    if(ptr == NULL)
        return NULL;
    /*
    printf("%s\n", ptr);
    printf("%d\n", ptr - str);
    printf("%d\n", strlen(str));
    printf("deref %s\n",str);
    */
    int family_size = ptr-str;
    int given_size = strlen(str) - family_size-1;

    if(given_size < 2 || family_size < 2)
        return NULL;

    if(*(ptr-1) == ' ')
        return NULL;

    if(strchr(ptr+1, ',')!= NULL)
        return NULL;

    if(*(ptr+1) == ' '){
        ptr+=1;
        given_size -=1;
    }
    // printf("%d %d %c %c \n", given_size, family_size, str[0], ptr[1]);
    if(('a'<=str[0] && str[0]<= 'z') || ('a'<=ptr[1] && ptr[1]<= 'z'))
        return NULL;

    printf("%d %d\n", family_size, given_size);
    family = (char *) malloc(sizeof(char)*(family_size+1));
    given = (char *) malloc(sizeof(char)*(given_size+1));

    memcpy(family, str, family_size);
    memcpy(given, ptr+1, given_size);
    *(family+family_size) = '\0';
    *(given+given_size) = '\0';

    printf("x %s y %s\n", family, given);
    result = (Pname *) malloc(sizeof(Pname));
	result->family = family;
	result->given = given;
    return result;
}

void freePname(Pname * pname){
    free(pname->given);
    free(pname->family);
    free(pname);
}

static int
pname_abs_cmp_internal(Pname * a, Pname * b)
{
	int result = strcmp(a->family, b->family);
    if (result != 0)
        return result;

    return strcmp(a->given, b->given);
}

void show(char* a, char* b){
    char* c = (char *) malloc(sizeof(char)*(strlen(a)+strlen(b)+1));
    memcpy(c, a, strlen(a));
    memcpy(c+strlen(a), b, strlen(b));
    *(c+strlen(a)+strlen(b)) = '\0';
    printf("string %s\n", c);
    free(c);
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
        Pname *pname = (Pname *)test1(test_data[i]);
        assert(pname != NULL);
        freePname(pname);
    }

    char* test_data2 [] = {
        "Jesus",
        "Smith  ,  Harold",
        "Gates, William H., III",
        "A,B C",
        "Smith, john"
        };
    for (int i = 0; i < sizeof(test_data2) / sizeof(char *); i++){
        assert(test1(test_data2[i])==NULL);
    }

    Pname *pname1 = test1("Smith,John");
    Pname *pname2 = test1("Smith, John");
    Pname *pname3 = test1("Smith, John David");
    Pname *pname4 = test1("Smith, James");

    assert(pname_abs_cmp_internal(pname1, pname1)==0);
    assert(pname_abs_cmp_internal(pname1, pname2)==0);
    assert(pname_abs_cmp_internal(pname2, pname1)==0);
    assert(pname_abs_cmp_internal(pname2, pname3)!=0);
    assert(pname_abs_cmp_internal(pname2, pname4)!=0);

    freePname(pname1);
    freePname(pname2);
    freePname(pname3);
    freePname(pname4);

    pname1 = test1("Smith,James");
    pname2 = test1("Smith,John");
    pname3 = test1("Smith,John David");
    pname4 = test1("Zimmerman, Trent");

    assert(pname_abs_cmp_internal(pname1, pname2)<=0);
    assert(pname_abs_cmp_internal(pname1, pname3)<=0);
    assert(pname_abs_cmp_internal(pname3, pname2)>0);
    assert(pname_abs_cmp_internal(pname1, pname1)<=0);
    assert(pname_abs_cmp_internal(pname4, pname3)>0);

    show("aa","bb");

    char text[500];
    fgets (text, sizeof(text), stdin);
    Test * tst = malloc(sizeof(*tst) + sizeof(char)* strlen(text));
    memcpy(tst->string, text, strlen(text));
    printf("input was %s", tst->string);

    printf("done");
    return 0;
}
