
// C program for variable length members in
// structures in GCC
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
// A structure of type student
typedef struct student
{
    int stud_id;
    int name_len;

    // This is used to store size of flexible
    // character array stud_name[]
    int size;

    // Flexible Array Member(FAM)
    // variable length array must be last
    // member of structure
    char stud_name[];
}Student;

typedef struct pair
{
    Student one;
    Student two;
}Pair;

// Memory allocation and initialisation of structure
struct student *createStudent(struct student *s,
                              int id, char a[])
{
    // Allocating memory according to user provided
    // array of characters
    s =
        malloc( sizeof(*s) + sizeof(char) * strlen(a));

    s->stud_id = id;
    s->name_len = strlen(a);
    strcpy(s->stud_name, a);

    // Assigning size according to size of stud_name
    // which is a copy of user provided array a[].
    s->size =
        (sizeof(*s) + sizeof(char) * strlen(s->stud_name));

    return s;
}

// Print student details
void printStudent(struct student *s)
{
    printf("Student_id : %d\n"
           "Stud_Name : %s\n"
           "Name_Length: %d\n"
           "Allocated_Struct_size: %d\n\n",
           s->stud_id, s->stud_name, s->name_len,
           s->size);

    // Value of Allocated_Struct_size is in bytes here
}

// Driver Code
int main()
{
    Student *s1 = createStudent(s1, 523, "Cherry");
    Student *s2 = createStudent(s2, 535, "Sanjayulsha");

    Pair *p = (Pair *) malloc(sizeof(Pair));
    p->one = *s1;
    p->two = *s2;

    s1->stud_name[1]='\0';
    printStudent(s1);
    printStudent(s2);

    // Size in struct student
    printf("Size of Struct student: %lu\n",
                    sizeof(struct student));

    // Size in struct pointer
    printf("Size of Struct pointer: %lu",
                              sizeof(s1));

    return 0;
}
