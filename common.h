#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/resource.h>
#include<string.h>
#include<stdlib.h>
#include<error.h>
#include<pthread.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<netinet/in.h>
#define MAX_ITEMS 20
#define MAX_CARTS 5
#define PORT 9897 //port on which connection will happen
struct book{
    int bookId; //stores book id
    char name[50]; //stores name of the book
    char author[50];//stores the author name for that book
    int copies;//stores number of books available
    double price;//stores price of one book  for renting per day
};
struct member{
    int memberId;//stores member's user id
    char password[50];//stores member's password
    char name[50];//stores member's name
    char email[50];//stores member's email address
};
struct admin{
    int adminId;//stores admin's ID
    char name[50];//stores admin's name
    char email[50];//stores admin's email
};
struct borrowed_book{
    int memberId;
    int bookId;
    int numberOfCopies;
    int numberOfDays;
};
struct bookIndex{//this structure will store the offset at which book with "bookId" is stored for easier access to details about that book
    int bookId;
    int bookOffset;  
};
struct memberIndex{//this structure will store the offset at which member info with "memberId" is stored for easier access to details about that member
    int memberId;
    int memberOffset;
};
struct  adminIndex{//this structure will store the offset at which admin info with "adminId" is stored for easier access to details about that admin
    int adminId;
    int adminOffset;
};




