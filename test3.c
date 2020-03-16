#include <stdio.h>
#include <string.h>
typedef enum {false, true} bool;
void pname_abs_error_internal(char * str){
      printf ("error string \"%s\" into tokens:\n",str);
}
bool
is_upper_alphabet(char c)
{
     if(c >= 'A' && c <= 'Z')
        return true;
    printf("not upper in func %c\n", c);

     return false;
}

bool
is_lower_alphabet(char c)
{
     if(c >= 'a' && c <= 'z')
        return true;
     return false;
}

static bool
is_alphabet(char c)
{
     if( is_lower_alphabet(c) || is_upper_alphabet(c) )
        return true;
     return false;
}

static bool
is_allowed_symbol(char c)
{
     if( c == '-' || c == '\'')
        return true;
     return false;
}

static bool
is_allowed_char(char c)
{
     if( is_allowed_symbol(c) || is_alphabet(c))
        return true;
     return false;
}
int main ()
{
  char str[] ="S''Smith, John";
    bool space = false;
    char *ptr = strchr(str, ',');
    int family_size = ptr-str;
    int given_size = strlen(str) - family_size-1;
    char * pch;
    char* tempstr = malloc(strlen(str)+1);
    char prev;

    if(given_size < 2 || family_size < 2)
        pname_abs_error_internal("less than 2");

    if(*(ptr-1) == ' ')
        pname_abs_error_internal("space before ,");

    if(strchr(ptr+1, ',')!= NULL)
        pname_abs_error_internal("too many ,");

    if(*(ptr+1) == ' '){
        ptr+=1;
        given_size -=1;
        space = true;
    }

    if(str[strlen(str)-1] == ' ')
        pname_abs_error_internal("last char is space");

    strcpy(tempstr, str);
    pch = strtok (tempstr," ,");
    while (pch != NULL)
    {
        if(strlen(pch)<2)
            pname_abs_error_internal("intern size less tha 2");

        if(!is_upper_alphabet(pch[0]))
            pname_abs_error_internal("not upper");

        prev = '\0';
        printf("%s\n", pch);
        for(int i = 1; i < strlen(pch); i++){
                printf("%c\n", pch[i]);
            if(!(is_lower_alphabet(pch[i]) || is_allowed_symbol(pch[i]))){
                if(!(is_allowed_symbol(prev) && is_upper_alphabet(pch[i])))
                    pname_abs_error_internal("not sym or lower");
            }
            prev = pch[i];
        }

        pch = strtok (NULL, " ,");
    }

}
