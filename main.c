#include<stdlib.h>
#include<stdio.h>
#include<strings.h>
#include<time.h>
#include <bcrypt.h>
#define TABLE_SIZE 25
#define STRING_MAX 50
#define MAX 100

int Logged_in = 0;
long int cursor;

//I COULD HAVE USED FSCAN FOR LIKE ALL OF THESE WHICH WOULD HAVE SAVED TIME AND SPACE,
//TOO LATE TO REPLACE EVERYTHING BUT KEEP IN MIND FSCAN

// Node structure declaration
typedef struct Node {
    char* Mail;
    char* hashed_pw;
    struct Node* next;
} Node;

// Array of pointers creation, the hashtable
Node* hashTable[TABLE_SIZE];

// Hashing function
unsigned long hashFunction(char* str) 
{
    unsigned long hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;  // djb2 hash algorithm
    }
    return hash % TABLE_SIZE;  // Return the index
}

//Manual Hasher (Manually reproducing the checkpw )
char* hasher(char*key, char* mail)
{
    char* Mail1 = (char*)malloc(STRING_MAX * sizeof(char));
    char* salt = (char*)malloc(22 * sizeof(char));
    char* keysalt = (char*)malloc(84 * sizeof(char));
    char* key1 = (char*)malloc(61 * sizeof(char));
    char* hashedkey = (char*)malloc(61 * sizeof(char));
    FILE* fp1 = fopen("hashTable.txt", "r");
    char str[136];
    while(!feof(fp1))
    {
        fgets(str, sizeof(str), fp1);
        sscanf(str, "%[^:]:%[^;]", Mail1, keysalt);
        if(strcmp(Mail1, mail) == 0)
        {
            sscanf(keysalt, "%[^:]:%[^;]", key1, salt);
            bcrypt_hashpw(key, salt, hashedkey);
            fclose(fp1);
            return hashedkey;
        }
        else 
        {
            printf("error");
            fclose(fp1);
            return "error";
        } 
    }
    return "idk";
}

//Retrieving the saved hash table
void load_hash(void)
{
    char* str = (char*)malloc(61 * sizeof(char));
    char Mail[STRING_MAX];
    char hashed_pw[61];
    unsigned long c;
    FILE* fp1 = fopen("hashTable.txt", "r");
    while(!feof(fp1))
    {
        c = ftell(fp1);
        fgets(str, STRING_MAX + 65, fp1);
        if(ftell(fp1) == c) break;
        sscanf(str, "%[^:]:%[^:]", Mail, hashed_pw);
        unsigned long index = hashFunction(Mail);
        //Create a new node
        Node* NewNode = (Node*)malloc(sizeof(Node));
        if(NewNode == NULL)
        {
            fprintf(stderr, "Memory allocation failed\n");
            return;
        }

        NewNode->Mail = strdup(Mail);
        NewNode->hashed_pw = strdup(hashed_pw);
        NewNode->next = NULL;

        if(hashTable[index] == NULL)
        {
            hashTable[index] = NewNode;
        }
        else
        {
            NewNode->next = hashTable[index];
            hashTable[index] = NewNode;
        }
        memset(Mail, '\0', STRING_MAX);
        memset(hashed_pw, '\0', 61);
    }
    
}

// Insertion function
void insert_keys(char* Mail, char* key) 
{
    FILE* fp1 = fopen("hashTable.txt", "a");
    unsigned long index = hashFunction(Mail);
    char salt[29];
    char hashed_pw[61];
    // Generate a salt
    if (bcrypt_gensalt(10, salt) != 0) 
    {
        printf("Problem while generating the salt\n");
        return;
    }

    // Hash the password
    if (bcrypt_hashpw(key, salt, hashed_pw) != 0) 
    {
        printf("Problem while hashing the password\n");
        return;
    }

    // Create a new node
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }
    newNode->Mail = strdup(Mail);
    newNode->hashed_pw = strdup(hashed_pw);  // Store the hashed password
    newNode->next = NULL;
    if (hashTable[index] == NULL) 
    {
        hashTable[index] = newNode;
    } 
    else 
    {
        newNode->next = hashTable[index];
        hashTable[index] = newNode;
    }
    fprintf(fp1, "%s:%s:%s;\n", Mail, hashed_pw, salt);
}

// Search & Retrieve function
char* search(char* Mail) 
{
    unsigned long index = hashFunction(Mail);
    Node* current = hashTable[index];
    while (current != NULL) 
    {
        if (strcmp(current->Mail, Mail) == 0) 
        {
            return current->hashed_pw;  // Return the hashed password
        }
        current = current->next;
        
    }
    return "error";  // Return error if key is not found
}

//Acc Creation function
void Acc_Creation()
{
    //Array declaration
    char* key = (char*)malloc(STRING_MAX * sizeof(char));
    char* Name = (char*)malloc(STRING_MAX * sizeof(char));
    char* Nick = (char*)malloc(STRING_MAX * sizeof(char));
    char* Mail = (char*)malloc(STRING_MAX * sizeof(char));
    char* Add = (char*)malloc(STRING_MAX * sizeof(char));
    char* Dom = (char*)malloc(STRING_MAX * sizeof(char));
    char* key2 = (char*)malloc(STRING_MAX * sizeof(char));
    char* Num = (char*)malloc(10 * sizeof(char));
    char* Check = (char*)malloc(STRING_MAX * sizeof(char));
    char* str = (char*)malloc(STRING_MAX * sizeof(char));
    
    //Opening the personal data file
    FILE* fp = fopen("Bankacc.txt","a");
    //Prompting the user to enter the data
    printf("Enter your name : ");
    fgets(str, STRING_MAX, stdin);
    strncpy(Name, str, STRING_MAX);
    Name[strcspn(Name, "\n")] = '\0';
    memset(str, '\0', MAX);
    printf("Enter your nickname : ");
    fgets(str, STRING_MAX, stdin);
    strncpy(Nick, str, STRING_MAX);
    Nick[strcspn(Nick, "\n")] = '\0';
    memset(str, '\0', MAX);
    printf("Enter your e-mail address : ");
    fgets(str, MAX, stdin);
    sscanf(str,"%50[^@]@%50[^.].%50s",Add, Mail, Dom);
    memset(str, '\0', MAX);
    //Valid e-mail verification
    while (!((strcmp(Mail, "gmail") == 0) || (strcmp(Mail, "outlook") == 0) || (strcmp(Mail, "yahoo") == 0)) || 
        !((strcmp(Dom, "com") == 0) || (strcmp(Dom, "org") == 0) || (strcmp(Dom, "ma") == 0) || (strcmp(Dom, "fr") == 0)))
    {
        printf("Invalid mail provider or domain, please try again: ");
        fgets(str, MAX, stdin);
        sscanf(str, "%50[^@]@%50[^.].%50s", Add, Mail, Dom);  
        memset(str, '\0', MAX); 
    }
    //Concatenating the parts of the e-mail address into one string
    strcat(Add, "@");
    strcat(Add, Mail);
    strcat(Add, ".");
    strcat(Add, Dom);
    printf("Enter your phone number : ");
    fgets(str, STRING_MAX, stdin);
    strncpy(Num, str, STRING_MAX - 1);
    Num[strcspn(Num, "\n")] = '\0';
    memset(str, '\0', MAX);
        while(strlen(Num) != 10)
        {
            printf("Invalid phone number, must be 10 digits long\n");
            printf("Please re-enter your phone number : ");
            fgets(str, STRING_MAX, stdin);
            strncpy(Num, str, STRING_MAX);
            Num[strcspn(Num, "\n")] = '\0';
            memset(str, '\0', MAX);
        }
        printf("Create a new password : ");
        fgets(str, STRING_MAX, stdin);
        strncpy(key, str, STRING_MAX);
        key[strcspn(key, "\n")] = '\0';
        char* value = strdup(key);
        memset(str, '\0', MAX);
        //Short passwords verfication
        while(strlen(key) < 8)
        {
            printf("Password too short, must at least contain 8 characters");
            printf("Enter a new password that is at least 8 characters long : ");
            fgets(str, STRING_MAX, stdin);
            strncpy(key, str, STRING_MAX);
            key[strcspn(key, "\n")] = '\0';
            char* value = strdup(key);
            memset(str, '\0', MAX);
        }
        printf("Re-enter your password to verify it : ");
        fgets(str, STRING_MAX, stdin);
        strncpy(key2, str, STRING_MAX);
        key2[strcspn(key2, "\n")] = '\0';
        memset(str, '\0', MAX);
        //First && Second Password matching verification
        while(strcmp(key, key2) != 0)
        {
            printf("The first password doesnt match the second one, please re-enter the second password : ");
            fgets(str, STRING_MAX, stdin);
            strncpy(key2, str, STRING_MAX);
            key2[strcspn(key2, "\n")] = '\0';
            memset(str, '\0', MAX);
        }
        printf("Are all the informations correct ? If not type 'retry' exit the terminal, else just type 'yes' : ");
        fgets(str, STRING_MAX, stdin);
        strncpy(Check, str, STRING_MAX);
        Check[strcspn(Check, "\n")] = '\0';
        memset(str, '\0', MAX);
    //Retry option
    if(strcmp(Check, "retry") == 0) exit(0);
    insert_keys(Add,key);
    //Inputing the data in the txt file
    fprintf(fp, "Name : %42s\n",Name);
    fprintf(fp, "Nickname : %38s\n",Nick);
    fprintf(fp, "E-mail : %40s\n",Add);
    fprintf(fp, "Number : %40s\n",Num);
    fprintf(fp, "Balance :                                       0\n");
    fprintf(fp,"**********                                       \n");
    printf("Congrats on creating a new account !\n");

    //Freeing the memory
    free(Name);
    free(Nick);
    free(Num);
    free(key);
    free(Check);
    free(str);
    free(key2);
    free(Mail);
    free(Dom);
    free(Add);
    //Closing the file
    fclose(fp);
}

#define line 50

void Acc_login()
{
    FILE* fp = fopen("Bankacc.txt","r");
    char* str = (char*)malloc(STRING_MAX * sizeof(char));
    char* Mail = (char*)malloc(STRING_MAX * sizeof(char));
    char* Mail1 = (char*)malloc(STRING_MAX * sizeof(char));
    char* Num = (char*)malloc(STRING_MAX * sizeof(char));
    char* Num1 = (char*)malloc(STRING_MAX * sizeof(char));
    char* key = (char*)malloc(STRING_MAX * sizeof(char));
    int Isverified = 0;
    //Getting the e-mail input
    printf("Welcome back ! Please enter your e-mail address : ");
    fgets(str, STRING_MAX, stdin);
    strncpy(Mail,str, STRING_MAX);
    // Remove newline character from input
    Mail[strcspn(Mail, "\n")] = '\0';
    memset(str, '\0', STRING_MAX);
    //Checking if the e-mail is in the data base
    fseek(fp, 0, SEEK_SET);
    while(Isverified == 0)
    {
        fseek(fp, (2 * line), SEEK_CUR);
        fgets(str, STRING_MAX + 1, fp);
        sscanf(str,"E-mail : %[^\n]",Mail1);
        if(strcmp(Mail,Mail1) == 0)
        {
            Isverified = 1;
            printf("Valid email, ");
            cursor = ftell(fp);
            break;
        }
        if(feof(fp))
        {
            printf("Invalid e-mail\n");
            printf("Would you like to try again ? : (Y/N)");
            fgets(str, 2, stdin);
            str[strcspn(str, "\n")] = '\0';
            if(strcmp(str,"Y") == 0)
            {
             memset(str, '\0', STRING_MAX);
             getchar();
             printf("Please re-enter your e-mail : ");
             fgets(str, STRING_MAX, stdin);
             strncpy(Mail, str, STRING_MAX);
             Mail[strcspn(Mail, "\n")] = '\0';
             fseek(fp, 0, SEEK_SET);
             continue;
            }
            else
            {
                memset(str, '\0', STRING_MAX);
                break;
            }
        }
        fseek(fp, (3 * line),SEEK_CUR);
        memset(Mail1, '\0', STRING_MAX);
    }
    if(Isverified == 1)
    {
        char* Name = (char*)malloc(STRING_MAX * sizeof(char));
        for(int i = 0; i <= 3; i++)
        {
            printf("enter your password : ");
            fgets(str, STRING_MAX, stdin);
            strncpy(key, str, STRING_MAX);
            key[strcspn(key, "\n")] = '\0';
            char* hashed_pw = search(Mail);
            if(bcrypt_checkpw(key, search(Mail)) == 0)
            {
                memset(str, '\0', MAX);
                fseek(fp, (-3) * line, SEEK_CUR);
                fgets(str, STRING_MAX, fp);
                sscanf(str,"Name : %[^\n]",Name);
                memset(str, '\0', MAX);
                printf("You have been logged successfully !\n");
                printf("Welcome back dear %s\n",Name);
                Logged_in = 1;
                break;
            }
            else printf("Wrong Password %d tries left before getting kicked out\n", 3 - i);
        }
    }
    free(str);
    free(Mail);
    free(Mail1);
    free(key);
    free(Num1);
    free(Num);
    fclose(fp);
}

int main(void)
{
    //Bank selection and welcome screen
    char* str = (char*)malloc(2 * sizeof(char));
    char* str1 = (char*)malloc(STRING_MAX * sizeof(char));
    int choice;
    char choice1[2];
    load_hash();
    //Prompt the use for choice
    printf("Hello User, welcome to the Bbank\n");
    printf("Type a number from 1-3 for any of the following actions :\n");
    printf("-Create a new account :       1\n-Login into your account :    2\n-Exit :                       3\n");
    printf("Action : ");
    fgets(str, sizeof(str), stdin);
    sscanf(str, "%d", &choice);
    memset(str, '\0', 2);
    //Switch compute the choice
    switch(choice)
    {
        case 1: Acc_Creation();
                printf("Would you like to login ?(Y/N) : ");
                fgets(str, sizeof(str), stdin);
                sscanf(str, "%s", choice1);
                if(strcmp(choice1, "Y") == 0) Acc_login();
                else exit(0);
        break;
        case 2: Acc_login();
        break;
        case 3: exit(0);
        break;
    }
    //Logged-in options
    FILE* fp = fopen("Bankacc.txt","r+");
    while(Logged_in == 1)
    {
        char* name = (char*)malloc(STRING_MAX * sizeof(char));
        char* nick = (char*)malloc(STRING_MAX * sizeof(char));
        char* num = (char*)malloc(STRING_MAX * sizeof(char));
        char* mail = (char*)malloc(STRING_MAX * sizeof(char));
        int balance;
        fseek(fp, cursor + line, SEEK_SET);
        fgets(str1, STRING_MAX + 1, fp);
        sscanf(str1, "Balance : %d", &balance);
        memset(str1, '\0', STRING_MAX);
        fseek(fp, (-3) * line, SEEK_CUR);
        fgets(str1, STRING_MAX + 1,fp);
        sscanf(str1, "E-mail : %[^\n]", mail);
        fseek(fp, 2 * line, SEEK_CUR);
        memset(str1, '\0', STRING_MAX);
        //Prompting the use for choice
        printf("What action do you want to undertake ? (1-4)\n");
        printf("-Check balance :\t\t-Send money :\t\t-Check account information :\t\t-Exit :\t\t\n");
        printf("        1\t                     2\t                              3\t                           4\n");
        printf("Action : ");
        fgets(str, sizeof(str), stdin);
        sscanf(str, "%d", &choice);
        memset(str, '\0', 2);
        //Computing the choice
        switch(choice)
        {
            case 1 : printf("Your balance is : %d\n", balance);
            break;
            case 2 : 
                {
                     char* mail1 = (char*)malloc(STRING_MAX * sizeof(char));
                     char* mail2 = (char*)malloc(STRING_MAX * sizeof(char));
                     printf("Please enter the e-mail of the receiver :");
                     fgets(mail1, STRING_MAX, stdin);
                     mail1[strcspn(mail1, "\n")] = '\0';
                     fseek(fp, 0, SEEK_SET);
                     while(1)
                     {
                         fseek(fp, 2 * line, SEEK_CUR);
                         fgets(str1, STRING_MAX + 1, fp);
                         sscanf(str1, "E-mail : %[^\n]", mail2);
                         memset(str1, '\0', STRING_MAX);
                         if(strcmp(mail1, mail2) == 0)
                         {
                            //Check if the email is not ours
                            if(strcmp(mail1, mail) == 0)
                            {
                                printf("You cannot send money to yourself, please try with someone else\n");
                                break;
                            }
                             int balance1;
                             int balance2;
                             fseek(fp, line, SEEK_CUR);
                             fgets(str1, STRING_MAX + 1, fp);
                             sscanf(str1, "Balance : %d", &balance2);
                             printf("Valid recipient\n");
                             printf("How much money do you wanna send ?\n");
                             printf("Amount : ");
                             fgets(str1, STRING_MAX, stdin);
                             //Check if the amount is valid
                             if(sscanf(str1, "%d", &balance1) != 1)
                             {
                                printf("Invalid amount, please try again\n");
                                break;
                             }
                             memset(str1, '\0', STRING_MAX);
                             if(balance >= balance1)
                             {
                                 printf("Balance on your account is sufficient\n");
                                 printf("Do you confirm the transaction ? (Y/N)\n");
                                 printf("Response : ");
                                 fgets(str, sizeof(str), stdin);
                                 sscanf(str, "%s", choice1);
                                 memset(str, '\0', 2);
                                 if(strcmp(choice1, "Y") == 0)
                                 {
                                     balance -= balance1;
                                     balance2 += balance1;
                                     fseek(fp, -line, SEEK_CUR);
                                     fprintf(fp, "Balance :                                       0\n");
                                     fseek(fp, -line, SEEK_CUR);
                                     fprintf(fp, "Balance : %39d", balance2);
                                     fseek(fp, cursor + line, SEEK_SET);
                                     fprintf(fp, "Balance :                                       0\n");
                                     fseek(fp, -line, SEEK_CUR);
                                     fprintf(fp, "Balance : %39d", balance);
                                     printf("Operation successful\n");
                                     break;
                                 }
                                 else
                                 {
                                     printf("Farewell\n");
                                     break;
                                 }
                             }
                             else
                             {
                                 printf("Balance on your account is insufficient\n");
                                 break;
                             }
                         }
                         fseek(fp, 3 * line, SEEK_CUR);
                         if(fgets(str1, STRING_MAX + 1, fp) == NULL)
                         {
                            printf("Invalid recipient\n");
                            break;
                         }
                         fseek(fp, -line, SEEK_CUR);
                     }
                }
            break;
            case 3 : fseek(fp, -250, SEEK_CUR);
                     fgets(str1, STRING_MAX + 1, fp);
                     sscanf(str1,"Name : %[^\n]",name);
                     memset(str1, '\0', STRING_MAX);
                     fgets(str1, STRING_MAX + 1, fp);
                     sscanf(str1, "Nickname : %[^\n]",nick);
                     memset(str1, '\0', STRING_MAX);
                     fgets(str1, STRING_MAX + 1, fp);
                     sscanf(str1, "E-mail : %[^\n]",mail);
                     memset(str1, '\0', STRING_MAX);
                     fgets(str1, STRING_MAX + 1, fp);
                     sscanf(str1, "Number : %[^\n]",num);
                     memset(str1, '\0', STRING_MAX);
                     printf("\t\t\tPersonal info : \t\t\t\n");
                     printf("Name :     %50s\n", name);
                     printf("Nickname : %50s\n", nick);
                     printf("Number :   %50s\n", num);
                     printf("E-mail :   %50s\n", mail);
            break;
            case 4 : printf("Goodbye !");
                     Logged_in = 0;
            break;
        }
        
    }
}
