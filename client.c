#include"common.h"
void display_customer_options(){ //function to display possible actions to the customer once he is logged in
    printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
    printf("Enter 1: Add book to cart\n\n");
    printf("Enter 2: To view cart\n\n");
    printf("Enter 3: To modify cart\n\n");
    printf("Enter 4: To borrow book\n\n");
    printf("Enter 5: To return book\n\n");
    printf("Enter 6: To view borrowed books\n\n");
    printf("Enter 7: To view available books\n\n");
    printf("Enter anything else: To log out\n\n");
    printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
}
void display_admin_options(){//function to display possible options to the admin
    printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
    printf("Enter 1: To view members.\n\n");
    printf("Enter 2: To add a book to the library.\n\n");
    printf("Enter 3: To remove a book from the library.\n\n");
    printf("Enter 4: To modify a book's details.\n\n");
    printf("Enter 5: To view books.\n\n");
    printf("Enter 6: To view books borrowed by a specific member.\n\n");
    printf("Enter 7: To view specific book details.\n\n");
    printf("Enter 8: To view admin details.\n\n");
    printf("Enter anything else: To log out\n\n");
    printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
}
void add_book_to_cart(int sockfd,int id){//certain number of copies of a certain book are added to a customer's cart
    printf("Enter the ID of the book you want to borrow.\n\n");
    int bookId;
    scanf("%d",&bookId);//take in book id
    write(sockfd,&bookId,sizeof(int));
    int ans;
    read(sockfd,&ans,sizeof(int));
    if(ans==-1){//this means invalid book id given
        printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
        printf("Entered Book Id is invalid.\n\n");
        printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
        return;
    }
    int copies;
    char name[50],author[50];
    double price;
    //read book details from server
    read(sockfd,&copies,sizeof(int));
    read(sockfd,name,50);
    read(sockfd,author,50);
    read(sockfd,&price,sizeof(double));
    if(copies<=0){
        printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
        printf("Sorry!\n\n");
        if(copies==0)
            printf("%s\t\t by %s\t is out of stock\n\n",name,author);
        else
            printf("%s\t\t by %s\t has been removed from the library\n\n",name,author);
        printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
        return;
    }
    printf("Enter the number of copies you want to borrow.\n\n");
    int numberOfCopies;
    scanf("%d",&numberOfCopies);//read the number of copies user wants to borrow
    while(numberOfCopies<=0 || numberOfCopies>copies){
        if(numberOfCopies<=0){
            printf("Please enter a positive quantity\n\n");
        }
        else{
            printf("Only %d copies available. Cannot borrow %d copies\n\n",copies,numberOfCopies);
            printf("Enter a valid quantity\n\n");
        }
        scanf("%d",&numberOfCopies);
    }
    write(sockfd,&numberOfCopies,sizeof(int));//write number of copies user wants to borrow to the server
    printf("Enter the number of days you want to borrow the book for.\n\n");
    int days;
    scanf("%d",&days);
    write(sockfd,&days,sizeof(int));
    printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
    printf("%d copies of the book: %s for %d days has been added to user cart!\n\n",numberOfCopies,name,days);
    printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
}
void view_cart(int sockfd, int id){//function to view what is present in a user's cart
   int count;
    read(sockfd,&count,sizeof(int));//count holds number of unique books present in the user's cart
    if(count==0){
        printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
        printf("No item is present in the customer's cart.\n\n");
        printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
        return;
    }
    else{
        struct borrowed_book book;
        printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
        printf("Contents of the cart:\n\n");
        while(count>0){
            read(sockfd,&book,sizeof(struct borrowed_book));
            printf("Book Id: %d and Number of Copies:%d for %d Days\n\n",book.bookId,book.numberOfCopies,book.numberOfDays);
            count--;
        }
        printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
    }
}
void modify_cart(int sockfd,int id){//function to modify details of books in the user's cart
    printf("Enter book id of the book whose number of copies or days you want to update\n\n");
    int bookId;
    scanf("%d",&bookId);
    write(sockfd,&bookId,sizeof(int));
    int ans;
    read(sockfd,&ans,sizeof(int));//read the answer returned by the server
    if(ans<0){
        printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
        printf("The book having the Book Id: %d is not present in your cart.\n\nIn order to modify it, you must add it first.\n\n",bookId);
        printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
        return;
    }
    else{
        printf("Enter 1: To modify number of days\n\nEnter 2: To modify number of copies\n\n");
        int choose;
        scanf("%d",&choose);
        while(choose!=1 && choose!=2){
            printf("You have entered an invalid choice.\n\n");
            printf("Enter 1: To modify number of days\n\nEnter 2: To modify number of copies\n\n");
            scanf("%d",&choose);
        }
        write(sockfd,&choose,sizeof(int));
        if(choose==1){//means user wants to modify the number of days he wants to borrow it for
            int days;
            read(sockfd,&days,sizeof(int));
            printf("Currently, the book is scheduled to be borrowed for %d days\n\n",days);
            printf("Enter the new number of days you want borrow the book for\n\n");
            int newdays;
            scanf("%d",&newdays);
            write(sockfd,&newdays,sizeof(int));
            return;
        }
        int numberOfCopies;
        read(sockfd,&numberOfCopies,sizeof(int));
        printf("Enter the new number of copies\n\n");
        int new;
        scanf("%d",&new);
        int cur;
        read(sockfd,&cur,sizeof(int));
        while(new>cur+numberOfCopies){
            printf("Only %d copies left.\n\nYou can increment at maximum that much.\n\nEnter the new number of copies\n\n",cur);
            scanf("%d",&new);
        }
       if(new<=0){
            new=0;
       } 
       write(sockfd,&new,sizeof(int));
    }
    printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
    printf("Cart modified!\n\n");
    printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");


}
void borrow_book(int sockfd,int id){//processes all of the books that the customer has in his cart and asks for payment
    int numberOfCartItems;
    read(sockfd,&numberOfCartItems,sizeof(int));
    if(numberOfCartItems==0){
        printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
        printf("No books in the cart. Cannot borrow!\n\n");
        printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
        return;
    }
    printf("%d number of different books are there in the cart\n\n",numberOfCartItems);
    view_cart(sockfd,id);//this will display the cart
    double price;
    read(sockfd,&price,sizeof(double));
    printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
    printf("The total bill is: %.2lf\n\n",price);
    printf("Please pay the amount by entering it.\n\n");
    printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
    double amount;
    scanf("%lf",&amount);
    while(amount-price!=0){
        printf("The total bill is: %.2lf\n\n",price);
        printf("Please pay the amount by entering it.\n\n");
         scanf("%lf",&amount);
    }
    printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
    printf("Payment Successful!!\n\n");
    printf("Thank you for learning with us!!\n\n");
    printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");

}
void view_borrowed(int sockfd,int id){//function to display books borrowed by a customer and not yet returned
    int count;
    read(sockfd,&count,sizeof(int));//read number of books borrowed
    if(count==0)
    {
        printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
        printf("No books borrowed!\n\n");
        printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
        return;
    }
    int start=0;
    printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
    printf("%d unique books borrowed\n\n",count);
    while(start<count){
        struct borrowed_book book;
        read(sockfd,&book,sizeof(struct borrowed_book));
        printf("%d Copies of Book with Book Id: %d has been borrowed by Customer with Customer Id: %d for %d Days\n\n",book.numberOfCopies,book.bookId,book.memberId,book.numberOfDays);
        start++;
    }
    printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
}
void return_book(int sockfd,int id){//customer returns the books he borrowed
    int count;
    read(sockfd,&count,sizeof(int));
    if(count==0){
        printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
        printf("No books borrowed. Nothing to return.\n\n");
        printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
        return;
    }
    printf("The books that the client had borrowed were:\n\n");
    view_borrowed(sockfd,id);
    printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
    printf("Client has successfully returned all those books!\n\n");
    printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
}
void view_books(int sockfd){//function for the customer to view the books in the library
    int count;
    read(sockfd,&count,sizeof(int));
    if(count==0){
        printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
        printf("No books available.\n\n");
        printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
    }
    else{
        printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
        printf("Books currently present:\n\n");
        while(count>0){
            int id,copies;
            double price;
            char name[50],author[50];
            read(sockfd,&id,sizeof(int));
            read(sockfd,name,50);
            read(sockfd,author,50);
            read(sockfd,&copies,sizeof(int));
            read(sockfd,&price,sizeof(double));
            count--;
            printf("Book Id: %d\t",id);
            printf("Name: %s\t",name);
            printf("Author: %s\t",author);
            printf("Copies Available: %d\t",copies);
            printf("Price: %.2f\n\n",price);
        }
        printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
    }
}
void view_members(int sockfd){//function to view the members enrolled as customers of the library
    int count;
    read(sockfd,&count,sizeof(int));
    if(count==0){
        printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
        printf("No members part of the library.\n\n");
        printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
    }
    else{
        printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
        printf("Members currently present:\n\n");
        while(count>0){
            int id;
            char name[50],email[50];
            read(sockfd,&id,sizeof(int));
            read(sockfd,name,50);
            read(sockfd,email,50);
            count--;
            printf("Member Id: %d\t",id);
            printf("Name: %s\t",name);
            printf("Email: %s\n\n",email);
        }
        printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
    }
}
void view_admins(int sockfd){//function to view all admins of the library
    int count;
    read(sockfd,&count,sizeof(int));
    if(count==0){
        printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
        printf("No admins part of the library.\n\n");
        printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
    }
    else{
        printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
        printf("Admins currently present:\n\n");
        while(count>0){
            int id;
            char name[50],email[50];
            read(sockfd,&id,sizeof(int));
            read(sockfd,name,50);
            read(sockfd,email,50);
            count--;
            printf("Admin Id: %d\t",id);
            printf("Name: %s\t",name);
            printf("Email: %s\n\n",email);
        }
        printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
    }
}
void add_book(int sockfd){//function to add a book to the library

    printf("Book Id will be assigned by the server\n\n");
    int id;//will have book id
    read(sockfd,&id,sizeof(int));//reads the Id assigned by the server
    printf("Book Id:%d\n\n",id);
    printf("Enter other book details.\n\n");
    char name[50]; //stores name of the book
    char author[50];//stores the author name for that book
    int copies;//stores number of books available
    double price;//stores price of one book  for renting per day
    printf("Enter name of the book: ");
    scanf(" %[^\n]", name);
    printf("Enter name of the author: ");
    scanf(" %[^\n]", author);
    getchar(); // Consume the newline character
    printf("Enter number of copies of the book to be added: ");
    scanf("%d",&copies);
    printf("Enter the price of the book for borrowing for one day: ");
    scanf("%lf",&price);
    write(sockfd,name,sizeof(name));
    write(sockfd,author,sizeof(author));
    write(sockfd,&copies,sizeof(int));
    write(sockfd,&price,sizeof(double));
    printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
    printf("Book has been successfully added!\n\n");
    printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
 
}
void remove_book(int sockfd){//function to remove a book from the library
    int id;
    printf("Enter the Id of the book you want to completely remove from the library.\n\n");
    scanf("%d",&id);
    write(sockfd,&id,sizeof(int));
    int ans;
    read(sockfd,&ans,sizeof(int));
    if(ans==-1){
        printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
        printf("The book doesn't exist. Cannot remove!\n\n");
        printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
        return;
    }
    read(sockfd,&ans,sizeof(int));
    if(ans<0){
        printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
        printf("Book was deleted some point before. Cannot delete a book that has already been removed\n\n");
        printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
        return;
    }
    printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
    printf("Book with Book Id: %d successfully removed!!\n\n",id);
    printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
}
void modify_book(int sockfd){//function to modify book details
    printf("Enter the ID of the book whose quantity or price you want to modify\n\n");
    int id;
    scanf("%d",&id);
    write(sockfd,&id,sizeof(int));
    int ans;
    read(sockfd,&ans,sizeof(int));
    if(ans<0){
        printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
        printf("You have entered an Invalid Book Id\n\n");
        printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
        return;
    }
    int copies;
    char name[50],author[50];
    double price;
    read(sockfd,&copies,sizeof(int));
    read(sockfd,name,50);
    read(sockfd,author,50);
    read(sockfd,&price,sizeof(double));
    if(copies==-1){
        printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
        printf("The book :%s\tby %s\thas been deleted from the library.\n\nYou need to separately add it.\n\n",name,author);
        printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
        return;
    }
    printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
    printf("Book details currently:\n\n");
    printf("Book Name: %s\t\tAuthor :%s\tCopies Available :%d\tPrice :%.2lf\n\n",name,author,copies,price);
    printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
    printf("Enter 1: To modify price\nEnter 2: To modify number of copies\n");
    int ch;
    scanf("%d",&ch);
    while(ch!=1 && ch!=2){
        printf("What you entered is invalid.\n");
        printf("Enter 1: To modify price\nEnter 2: To modify number of copies\n");
        scanf("%d",&ch);
    }
    write(sockfd,&ch,sizeof(int)); //write admin's choice to the server
    if(ch==1){//modify price of the book
    printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
        printf("Old price of the book: %.2f\n",price);
        printf("You have chosen to modify the price of the book.\n\nPlease enter the new price.\n\n");
        scanf("%lf",&price);
        write(sockfd,&price,sizeof(double));
        printf("Price of the book set to :%.2f\n\n",price);
        printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
    }
    else{//admin wants to modify the quantity
    printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
        printf("Number of copies currently: %d\n",copies);
        printf("You have chosen to modify the number of copies of the book.\n\nPlease enter the new number of copies.\n\n");
        scanf("%d",&copies);
        write(sockfd,&copies,sizeof(int));
        printf("New number of copies of the book set to :%d\n\n",copies);
        printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
    }

}
void view_books_borrowed(int sockfd){//function to find books borrowed by a member
     printf("Enter the Member Id of the member whose borrowed books details you want to find out about.\n\n");
     int id;
     scanf("%d",&id);
     write(sockfd,&id,sizeof(int));//write member id to the server
     int ans;
     read(sockfd,&ans,sizeof(int));
     if(ans<0){
        printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
        printf("Member with Id: %d doesn't exist\n\n",id);
        printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
        return;
     }
     view_borrowed(sockfd,id);
}
void view_specific_book(int sockfd){//function to view specific book details
    int id;
    printf("Enter the ID of the book whose details you want to find out about\n\n");
    scanf("%d",&id);
    write(sockfd,&id,sizeof(int));//write id to the server
    int ans;
    read(sockfd,&ans,sizeof(int));//read reply
    if(ans==-1){
        printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
        printf("Invalid Book Id\n\n");
        printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
        return;
    }
    read(sockfd,&ans,sizeof(int));
    char name[50],author[50];
    double price;
    int copies;
    read(sockfd,name,50);
    read(sockfd,author,50);
    read(sockfd,&price,sizeof(double));
    if(ans==-1){
        printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
        printf("Book has been deleted at some point!\n\n");
        printf("It's specifications are as follows:\n\n");
        printf("Name: %s\tAuthor Name: %s\tPrice:%.2lf\n\n",name,author,price);
        printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
        return;
    }
    read(sockfd,&copies,sizeof(int));
    printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
    printf("Book details:\n\n");
    printf("Name: %s\tAuthor Name: %s\tPrice:%.2lf\tCopies: %d\n\n",name,author,price,copies);
    printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
}
void forgotPassword(int sockfd){//forgot password portal. Helps in resetting the password
    printf("Welcome to forgot password portal.\n\n");
    int ans=-1;
    while(ans<0)
    {
        printf("Enter your Email Id for verification.\n\n");//keep scanning email till user inputs a valid one
        char email[50];
        scanf("%s",email);
        write(sockfd,email,sizeof(email));
        read(sockfd,&ans,sizeof(int));
        if(ans<0){
            printf("Entered email doesn't exist.\n");
        }

    }
    char password[50];
    printf("Set new password.\n\n");
    scanf("%s",password);
    while(1){//confirm password
        printf("Enter password again to confirm.\n\n");
        char confirm[50];
        scanf("%s",confirm);
        if(strcmp(password,confirm)!=0){
            printf("Password doesn't match.\n\n");
        }
        else
            {
                printf("Password successfully reset!\n\n");
                break;
            }
    }
    write(sockfd,password,sizeof(password));//write the new password to the server
}
void signup_as_user(int sockfd){//function to sign up as user
    printf("Member Id will be assigned by the server.\n\n");
    int id;
    read(sockfd,&id,sizeof(int));//reads the Id assigned by the server
    printf("Member Id:%d\n\n",id);
    printf("Enter name.\n\n");
    char name[50];
    scanf(" %[^\n]", name);
    printf("Enter email.\n\n");
    char email[50];
    scanf("%s",email);
    char password[50];
    printf("Set password.\n\n");
    scanf("%s",password);
    while(1){//confirm password
        printf("Enter password again to confirm.\n\n");
        char confirm[50];
        scanf("%s",confirm);
        if(strcmp(password,confirm)!=0){
            printf("Password doesn't match.\n\n");
        }
        else
            break;
    }
    //write these three details to the server
    write(sockfd,name,sizeof(name));
    write(sockfd,email,sizeof(email));
    write(sockfd,password,sizeof(password));
    printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
    printf("Sign Up Successful!\n\n");
    printf("Your member Id has been sent to your email account.\n\n");
    printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
}
void signup_as_admin(int sockfd){//function to sign up as administrator
    printf("Authenticate yourself before signing up by entering the secret key!\n\n");
    char password[50];
    int sup=-1;
    while(sup==-1){
        printf("Enter 1 : To quit\n\nEnter 2 : To try verification\n\n");
        int ch;
        scanf("%d",&ch);
        while(ch!=1 && ch!=2){
            printf("Enter 1 : To quit\n\nEnter 2 : To try verification\n\n");
            scanf("%d",&ch);
        }
        write(sockfd,&ch,sizeof(int));
        if(ch==1){//means user doesn't want to validate himself 
            return;
        }
        else{
            printf("Enter the secret key!\n\n");
            scanf("%s",password);
            write(sockfd,password,sizeof(password));
            int reply;
            read(sockfd,&reply,sizeof(int));
            if(reply==-1){
                printf("You are not verified for sign up as password entered is incorrect.\n\n");
                continue;
            }
            else{
                printf("You are eligible for sign up!!\n\n");
                sup=1;
                break;
            }
        }
    }
    printf("Admin Id will be assigned by the server.\n\n");
    int id;
    read(sockfd,&id,sizeof(int));//reads the Id assigned by the server
    printf("Admin Id:%d\n\n",id);
    printf("Enter name.\n\n");
    char name[50];
    scanf(" %[^\n]", name);
    printf("Enter email.\n\n");
    char email[50];
    scanf("%s",email);
    //write these  details to the server
    write(sockfd,name,sizeof(name));
    write(sockfd,email,sizeof(email));
    printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
    printf("Sign Up Successful!\n\n");
    printf("Your admin Id has been sent to your email account.\n\n");
    printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
}
void login_as_user(int sockfd){//function to login as user
    printf("Enter User Id.\n\n");
    int id;
    scanf("%d",&id);
    write(sockfd,&id,sizeof(int));//send id to the user
    int reply;
    read(sockfd,&reply,sizeof(int));//read reply from server
    while(reply<0){//if user id entered doesn't exist, it takes you to the sign up portal
        printf("The ID you entered doesn't exist. Kindly sign up first.\n\n");
        printf("Taking you to the sign in portal....");
        signup_as_user(sockfd);
        printf("Enter User Id.\n\n");
        scanf("%d",&id);
        write(sockfd,&id,sizeof(int));//send id to the server to validate
        read(sockfd,&reply,sizeof(int));//read reply from server
    }
    reply=-1;
    int choice=1;
    while(reply<0)
    {
        printf("Enter password.\n\n");//validates your password
        char password[50];
        scanf("%s",password);
        write(sockfd,password,50);
        read(sockfd,&reply,sizeof(int));//read reply from server
        if(reply<0){//in case of incorrect password it asks you to try again or reset password by clicking on forgot password
            printf("Incorrect Password.\n\n");
            printf("Enter 1 to try again. 2 to reset password.\n\n");
            scanf("%d",&choice);
            if(choice==1)
                {
                    write(sockfd,&choice,sizeof(int));
                    continue;
                }
            else if(choice==2)
                {
                    write(sockfd,&choice,sizeof(int));
                    forgotPassword(sockfd);
                }
            else{
                while(1){
                    printf("Enter 1 to try again. 2 to reset password.\n\n");
                    scanf("%d",&choice);
                    if(choice==1)
                       {
                        write(sockfd,&choice,sizeof(int));
                         break;
                       }
                    else if(choice==2)
                    {
                        write(sockfd,&choice,sizeof(int));
                        forgotPassword(sockfd);
                        break;
                    }

                }
            }

        }
        else{//means password match
            printf("Processing...\n");
            sleep(2);
        }
    }
    char name[50];
    read(sockfd,name,50);
    printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
    printf("Log in successful.\n\n");
    printf("Logged in as:%s\n\n",name);
    printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
    sleep(2);
    while(1){
        display_customer_options();
        int choice;//take in input from the customer
        scanf("%d",&choice);//depending on the choice, call the respective function
        write(sockfd,&choice,sizeof(int));//send choice to server
        if(choice==1){
            add_book_to_cart(sockfd,id);
        }
        else if(choice==2){
            view_cart(sockfd,id);
        }
        else if(choice==3){
            modify_cart(sockfd,id);
        }
        else if(choice==4){
            borrow_book(sockfd,id);
        }
        else if(choice==5){
            return_book(sockfd,id);
        }
        else if(choice==6){
            view_borrowed(sockfd,id);
        }
        else if(choice==7){
            view_books(sockfd);
        }
        else{
            printf("Logging out....\n\n");
            sleep(2);
            break;
        }

    }
}
void login_as_admin(int sockfd){//function to login as administrator
    printf("Enter admin Id.\n\n");
    int id;
    scanf("%d",&id);
    write(sockfd,&id,sizeof(int));//send id to the server
    int reply;
    read(sockfd,&reply,sizeof(int));//read reply from server
    while(reply<0){//means invalid admin id entered
        printf("The ID you entered doesn't exist. Kindly sign up first.\n\n");
        printf("Taking you to the sign in portal....");//takes you to sign up portal
        signup_as_admin(sockfd);
        printf("Enter admin Id.\n\n");
        scanf("%d",&id);
        write(sockfd,&id,sizeof(int));//send id to the server to validate
        read(sockfd,&reply,sizeof(int));//read reply from server
    }
    reply=-1;
    while(reply<0)
    {
        printf("Enter password.\n\n");
        char password[50];
        scanf("%s",password);
        write(sockfd,password,sizeof(password));
        read(sockfd,&reply,sizeof(int));//read reply from server
         if(reply<0){//there's only one password for admins, they can't have reset password option
            printf("Incorrect Password.\n\n");
        }
    }
    printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
    printf("Log in successful!\n\n");
    char name[50];
    read(sockfd,name,50);
    printf("Logged in as:%s\n\n",name);
    printf("-------------------------------------------------------------------------------------------------------------------------------------\n\n");
    sleep(2);
    while(1){
        display_admin_options();
        int choice;
        scanf("%d",&choice);//take in choice from admin and call the respective function
        write(sockfd,&choice,sizeof(int));
        if(choice==1){
            view_members(sockfd);
        }
        else if(choice==2){
            add_book(sockfd);
        }
        else if(choice==3){
            remove_book(sockfd);
        }
        else if(choice==4){
            modify_book(sockfd);
        }
        else if(choice==5){
            view_books(sockfd);
        }
        else if(choice==6){
            view_books_borrowed(sockfd);
        }
        else if(choice==7){
            view_specific_book(sockfd);
        }
        else if(choice==8){
            view_admins(sockfd);
        }
        else{
            printf("Logging out....\n\n");
            sleep(2);
            break;
        }

    }

}
int main(){
    printf("Waiting to connect to server....\n\n");
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);//creating a socket(half),TCP connection
    if (sockfd == -1){
        perror("Error in getting a socket ");
        return -1;
    }
    struct sockaddr_in server;
    server.sin_family=AF_INET;
    server.sin_addr.s_addr=INADDR_ANY;
    server.sin_port=htons(PORT);
    if(connect(sockfd,(struct sockaddr *)&server,sizeof(struct sockaddr_in))==-1){//connecting the server and the client
        perror("Error in connecting to the server");
        return -1;
    }
    printf("Server connection established.\n\n");
    while(1){
        //take in user choice and call the respective function
        printf("Enter 1: sign up as user\n\nEnter 2: sign up as admin\n\nEnter 3: login as user\n\nEnter 4: login as admin\n\nAnything else to exit.\n\n");
        int ch;//choice
        scanf("%d",&ch);
        write(sockfd,&ch,sizeof(int));
        int ans;
        if(ch==1){
            signup_as_user(sockfd);
        }
        else if(ch==2){
            signup_as_admin(sockfd);
        }
        else if(ch==3){
            login_as_user(sockfd);
        }
        else if(ch==4){
            login_as_admin(sockfd);
        }
        else{
            //this means user decided to end the connection with the server
            printf("Exiting portal.....\n\n");
            close(sockfd);//therefore close the socket fd 
            sleep(2);
            printf("Exited.\n\n");
            exit(0);
        }
    }
}