#include "common.h" //header file containing all the definitions
#define PASSWORD "admin1234" //this if for all admins. They have to sign up or login using this password
pthread_mutex_t mutex1,mutex2,mutex3,mutex4;//mutexes
void setlock(int fd, struct flock *lock){//put a read lock on the entire file with file descriptor "fd"
    lock->l_len = 0;
    lock->l_type = F_RDLCK;
    lock->l_start = 0;
    lock->l_whence = SEEK_SET;
    fcntl(fd, F_SETLKW, lock);
}
void setwritelock(int fd, struct flock *lock){//put a write lock on the entire file with file descriptor "fd"
    lock->l_len = 0;
    lock->l_type = F_WRLCK;
    lock->l_start = 0;
    lock->l_whence = SEEK_SET;
    fcntl(fd, F_SETLKW, lock);
}
void setrecordwritelock(int fd, struct flock *lock,int offset,int size){//put a write lock on a record
    lock->l_len = size;
    lock->l_type = F_WRLCK;
    lock->l_start = offset;
    lock->l_whence = SEEK_SET;
    fcntl(fd, F_SETLKW, lock);
}
void setrecordreadlock(int fd, struct flock *lock,int offset,int size){//put a read lock on a record
    lock->l_len = size;
    lock->l_type = F_RDLCK;
    lock->l_start = offset;
    lock->l_whence = SEEK_SET;
    fcntl(fd, F_SETLKW, lock);
}
void unlockrecordlock(int fd, struct flock *lock,int offset,int size){//put a write lock on a record
    lock->l_len = size;
    lock->l_type = F_UNLCK;
    lock->l_start = offset;
    lock->l_whence = SEEK_SET;
    fcntl(fd, F_SETLKW, lock);
}
void unlock(int fd,struct flock *lock){//unlock the lock on a file with file descriptor "fd"
    lock->l_type=F_UNLCK;
    fcntl(fd, F_SETLKW,lock);
}
int find_smallest_member_id(){//function to find the smallest member id that will be assigned to the customer
    int fd=open("member.txt",O_RDWR|O_CREAT,0776);
    lseek(fd,0,SEEK_SET);
    struct flock lock;
    setlock(fd,&lock);//put a lock
    int id=0;
     struct member members;
    while(read(fd,&members,sizeof(struct member))>0){
        id++;
    }
    id++;
    unlock(fd,&lock);
    close(fd);
    return id; //this will return smallest id available for assigning to member
}
int find_smallest_admin_id(){//find smallest admin id that can be assigned to a given administrator uniquely used to identify the admin
    int fd=open("admin.txt",O_RDWR|O_CREAT,0776);
    lseek(fd,0,SEEK_SET);
    struct flock lock;
    setlock(fd,&lock);//put a lock
    int id=0;
     struct admin admin;
    while(read(fd,&admin,sizeof(struct admin))>0){
        id++;
    }
    id++;
    unlock(fd,&lock);
    close(fd);
    return id; //this will return smallest id available for assigning to admin
}
int find_smallest_book_id(){//find smallest book id that can be assigned to a book to uniquely identify it
    int fd=open("book.txt",O_RDWR|O_CREAT,0776);
    lseek(fd,0,SEEK_SET);
    struct flock lock;
    setlock(fd,&lock);//put a lock
    int id=0;
     struct book book;
    while(read(fd,&book,sizeof(struct book))>0){
        id++;
    }
    id++;
    unlock(fd,&lock);
    close(fd);
    return id; //this will return smallest id available for assigning to a book
}
int getMemberOffset(int memberId, int fd){ //get member offset from memberIndex file
    if (memberId < 0){ //invalid memberID
        return -1;
    }
    struct flock lock;
    setlock(fd,&lock);//put a read lock on the file
    struct memberIndex index;
    while (read(fd, &index, sizeof(struct memberIndex))){
        if (index.memberId== memberId){
            unlock(fd, &lock);
            return index.memberOffset;
        }
    }
    unlock(fd, &lock);
    return -1;
}
int getAdminOffset(int adminId, int fd){ //get admin offset from adminIndex file
    if (adminId < 0){ //invalid adminID
        return -1;
    }
    struct flock lock;
    setlock(fd,&lock);//put a read lock on the file
    struct adminIndex index;
    while (read(fd, &index, sizeof(struct adminIndex))){
        if (index.adminId== adminId){
            unlock(fd, &lock);
            return index.adminOffset;
        }
    }
    unlock(fd, &lock);
    return -1;
}
int getBookOffset(int bookId, int fd){ //get book offset from bookIndex file
    if (bookId < 0){ //invalid bookID
        return -1;
    }
    struct flock lock;
    setlock(fd,&lock);//put a read lock on the file
    struct bookIndex index;
    while (read(fd, &index, sizeof(struct bookIndex))){
        if (index.bookId== bookId ){
            unlock(fd, &lock);
            return index.bookOffset;
        }
    }
    unlock(fd, &lock);
    return -1;
}
int findRecordOffset(int fd,int memberId,int bookId){//get offset of that record in the records.txt file
    if(bookId<0 || memberId<0){//invalid book id or member id
        return -1;
    }
    int offset=0;
    struct flock lock;
    setlock(fd,&lock);//put a read lock on the file
    lseek(fd,0,SEEK_SET);
    struct borrowed_book book;
    while (read(fd, &book, sizeof(struct borrowed_book))){
        if(book.bookId==bookId && memberId==book.memberId && book.numberOfCopies>0){//means the item exists in the customer's cart
            int fd1=open("bookIndex.txt",O_RDWR,0776);
            int bookOffset=getBookOffset(bookId,fd1);
            close(fd1);
            int fd2=open("book.txt",O_RDWR,0776);
            struct flock lock1;
            setrecordreadlock(fd2,&lock1,bookOffset,sizeof(struct book));//put a read lock on that record
            struct book cur_book;
            lseek(fd2,bookOffset,SEEK_SET);
            read(fd2,&cur_book,sizeof(struct book));
            if(cur_book.copies!=-1)//means book has not been deleted
            {
                return offset;
            }
            unlockrecordlock(fd2,&lock1,bookOffset,sizeof(struct book));//unlock the lock you had put on that book record
        }
        offset+=sizeof(struct borrowed_book);
    }
    unlock(fd, &lock);
    return -1;
}
void printMember(struct member members){//helper function
    printf("Name:%s\n",members.name);
    printf("Email:%s\n",members.email);
    printf("Password:%s\n",members.password);
    printf("Id:%d\n",members.memberId);
}
void add_book_to_cart(int new_fd,int id){//function to add a book to a customer's cart
    //add to cart has to be done by one user at a time otherwise it will lead to inconsistency
    pthread_mutex_lock(&mutex1);//put mutex lock
    int bookId;
    read(new_fd,&bookId,sizeof(int));
    int fd=open("bookIndex.txt",O_RDWR,0776);
    int offset=getBookOffset(bookId,fd);
    close(fd);
    write(new_fd,&offset,sizeof(int));
    if(offset==-1){//means invalid book Id
        pthread_mutex_unlock(&mutex1);  
        return;
    }
    fd=open("book.txt",O_RDWR,0776);
    struct book book;
    struct flock lock;
    //put a record lock
    setrecordreadlock(fd,&lock,offset,sizeof(struct book));
    lseek(fd,offset,SEEK_SET);
    read(fd,&book,sizeof(struct book));//read book details
    unlockrecordlock(fd,&lock,offset,sizeof(struct book));//unlock the lock 
    //send book details to the admin
    write(new_fd,&book.copies,sizeof(int));
    write(new_fd,book.name,sizeof(book.name));
    write(new_fd,book.author,sizeof(book.author));
    write(new_fd,&book.price,sizeof(double));
    if(book.copies<=0)//means either book has been deleted or is out of stock. So, close the fd and return
     {  
        pthread_mutex_unlock(&mutex1);
        close(fd);
        return;
     }
     int numberOfCopies;
     read(new_fd,&numberOfCopies,sizeof(int));
     int days;
     read(new_fd,&days,sizeof(int));
     struct borrowed_book borrowed_book;
     borrowed_book.memberId=id;
     borrowed_book.bookId=bookId;
     borrowed_book.numberOfCopies=numberOfCopies;
     borrowed_book.numberOfDays=days;
     int fd1=open("records.txt",O_CREAT|O_RDWR,0776);//open records.txt file and add this struct to it
     struct flock lock1;
     setwritelock(fd1,&lock1);//put a write lock before writing to the records.txt file
     lseek(fd1,0,SEEK_END);
     write(fd1,&borrowed_book,sizeof(struct borrowed_book));
     unlock(fd1,&lock1);//unlock the lock after writing
     book.copies=book.copies-numberOfCopies;//update the number of copies available
     setrecordwritelock(fd,&lock,offset,sizeof(struct book));//put record write lock
     lseek(fd,offset,SEEK_SET);
     write(fd,&book,sizeof(struct book));//write the new updated book details
     unlockrecordlock(fd,&lock,offset,sizeof(struct book));//unlock the lock on that record
     close(fd);
     pthread_mutex_unlock(&mutex1);
     return;
    
}
void view_cart(int new_fd,int id){//function to view books in the customer's cart
    int fd=open("records.txt",O_RDWR,0776);
    int count=0;
    struct borrowed_book books[MAX_ITEMS];
    struct borrowed_book borrowed_book;
    struct flock lock,lock1;
    setlock(fd,&lock);//put a lock on records.txt file
    lseek(fd,0,SEEK_SET);
    while(read(fd,&borrowed_book,sizeof(struct borrowed_book))!=0){
        int memberId=borrowed_book.memberId;
        if(memberId==id)
        {   int bookId=borrowed_book.bookId;
            int fd1=open("bookIndex.txt",O_CREAT|O_RDWR,0776);
            int offset=getBookOffset(bookId,fd1);
            close(fd1);
            int fd2=open("book.txt",O_RDWR,0776);
            lseek(fd2,offset,SEEK_SET);
            struct book book;
            setrecordreadlock(fd2,&lock1,offset,sizeof(struct book));//put read lock on that book record
            read(fd2,&book,sizeof(struct book));//read the book
            if(book.copies!=-1 && borrowed_book.numberOfCopies>0){//means book has not been deleted from the library
                books[count++]=borrowed_book;
            }
            unlockrecordlock(fd2,&lock1,offset,sizeof(struct book));//remove the lock once you have read the book
            close(fd2);
        }
    }
    unlock(fd,&lock);
    close(fd);
    write(new_fd,&count,sizeof(int));
    if(count==0)
        return;
    else{
        int start=0;
        while(start<count){//send whatever books are present in the customer's cart
            write(new_fd,&books[start],sizeof(struct borrowed_book));
            start++;
        }
    }
}
void modify_cart(int new_fd,int id){//function to modify customer cart details
    int bookId;
    read(new_fd,&bookId,sizeof(int));//read the bookId
    int fd=open("records.txt",O_RDWR,0776);//open the records file
    int offset=findRecordOffset(fd,id,bookId);
    write(new_fd,&offset,sizeof(int));
    if(offset==-1){//this means that particular book is not present in user's cart
        return;
    }
    lseek(fd,offset,SEEK_SET);
    struct borrowed_book borrowed_book;
    struct flock lock;
    setrecordreadlock(fd,&lock,offset,sizeof(struct borrowed_book));
    read(fd,&borrowed_book,sizeof(struct borrowed_book));
    unlockrecordlock(fd,&lock,offset,sizeof(struct borrowed_book));
    int choose;
    read(new_fd,&choose,sizeof(int));//read user choice whether he wants to modify copies or days
    if(choose==1){//means he wants to modify the number of days
        write(new_fd,&borrowed_book.numberOfDays,sizeof(int));//send the number of days currently scheduled
        int newdays;
        read(new_fd,&newdays,sizeof(int));
        borrowed_book.numberOfDays=newdays;//update days
        //update the records.txt file by writing
        setrecordwritelock(fd,&lock,offset,sizeof(struct borrowed_book));
        lseek(fd,offset,SEEK_SET);
        write(fd,&borrowed_book,sizeof(struct borrowed_book));
        unlockrecordlock(fd,&lock,offset,sizeof(struct borrowed_book));
        close(fd);
        return;
    }
    write(new_fd,&borrowed_book.numberOfCopies,sizeof(int));//send the number of copies in the cart
    int fd1=open("bookIndex.txt",O_RDWR,0776);
    int bookOffset=getBookOffset(bookId,fd1);
    close(fd1);
    fd1=open("book.txt",O_RDWR,0776);
    setrecordreadlock(fd1,&lock,bookOffset,sizeof(struct book));//put a record read lock 
    lseek(fd1,bookOffset,SEEK_SET);
    struct book book;
    read(fd1,&book,sizeof(struct book));
    unlockrecordlock(fd1,&lock,bookOffset,sizeof(struct book));
    write(new_fd,&book.copies,sizeof(int));//write the current available copies of the book
    int new;
    read(new_fd,&new,sizeof(int));
    //update the number of books in the cart
    setrecordwritelock(fd,&lock,offset,sizeof(struct borrowed_book));
    int oldCopies=borrowed_book.numberOfCopies;
    borrowed_book.numberOfCopies=new;
    lseek(fd,offset,SEEK_SET);
    write(fd,&borrowed_book,sizeof(struct borrowed_book));
    unlockrecordlock(fd,&lock,offset,sizeof(struct borrowed_book));
    //update the number of books overall as well
    int new_copies=book.copies-(new-oldCopies);
    setrecordwritelock(fd1,&lock,bookOffset,sizeof(struct book));//put a record read lock 
    lseek(fd1,bookOffset,SEEK_SET);
    book.copies=new_copies;//update count
    write(fd1,&book,sizeof(struct book));
    unlockrecordlock(fd1,&lock,bookOffset,sizeof(struct book));
    close(fd1);
    close(fd);
}
int numberOfCartItems(int new_fd,int id){//helper function to count the number of items present in the customer's cart
    int fd=open("records.txt",O_RDWR,0776);
    struct flock lock,lock1;
    setlock(fd,&lock);
    lseek(fd,0,SEEK_SET);
    int count=0;
    struct borrowed_book borrowed_book;
    while(read(fd,&borrowed_book,sizeof(struct borrowed_book))!=0){
        if(borrowed_book.memberId==id){
            if(borrowed_book.numberOfCopies>0){//means it is in the cart
                int bookId=borrowed_book.bookId;
                int fd1=open("bookIndex.txt",O_CREAT|O_RDWR,0776);
                int offset=getBookOffset(bookId,fd1);
                close(fd1);
                int fd2=open("book.txt",O_RDWR,0776);
                lseek(fd2,offset,SEEK_SET);
                struct book book;
                setrecordreadlock(fd2,&lock1,offset,sizeof(struct book));//put read lock on that book record
                read(fd2,&book,sizeof(struct book));//read the book
                if(book.copies!=-1){//means book has not been deleted from the library
                    count++;
                }
                unlockrecordlock(fd2,&lock1,offset,sizeof(struct book));//remove the lock once you have read the book
                close(fd2);
            }
        }
    }
    close(fd);
    return count;
}
void borrow_book(int new_fd,int id){//used to compute the total price of all items in the customer's cart
    pthread_mutex_lock(&mutex1);
    int count=numberOfCartItems(new_fd,id);//gives number of items in the cart
    write(new_fd,&count,sizeof(int));//write the number of items to the user
    if(count==0)
     { pthread_mutex_unlock(&mutex1);  
        return;
     }
    view_cart(new_fd,id);//to display the items in the cart
    //to compute price
    double price=0.0;
    int fd=open("records.txt",O_RDWR,0776);
    int fd4=open("records.txt",O_RDWR,0776);
    int fd3=open("borrowed_books.txt",O_RDWR,0776);
    struct flock lock,lock1,lock2,lock3;
    int recordOffset=0;
    setlock(fd,&lock);
    lseek(fd,0,SEEK_SET);
    struct borrowed_book borrowed_book;
    while(read(fd,&borrowed_book,sizeof(struct borrowed_book))!=0){
        if(borrowed_book.memberId==id){
            if(borrowed_book.numberOfCopies>0){//means it is in the cart
                int bookId=borrowed_book.bookId;
                int days=borrowed_book.numberOfDays;
                int fd1=open("bookIndex.txt",O_CREAT|O_RDWR,0776);
                int offset=getBookOffset(bookId,fd1);
                close(fd1);
                int fd2=open("book.txt",O_RDWR,0776);
                lseek(fd2,offset,SEEK_SET);
                struct book book;
                setrecordreadlock(fd2,&lock1,offset,sizeof(struct book));//put read lock on that book record
                read(fd2,&book,sizeof(struct book));//read the book
                if(book.copies!=-1){//means book has not been deleted from the library
                    double amount=book.price;//price for borrowing for 1 day
                    price+=amount*days*borrowed_book.numberOfCopies;//compute total like this
                    setwritelock(fd3,&lock2);
                    lseek(fd3,0,SEEK_END);
                    write(fd3,&borrowed_book,sizeof(struct borrowed_book));//write to the borrowed books file
                    unlock(fd3,&lock2);
                    // setwritelock(fd4,&lock2);
                    borrowed_book.numberOfCopies=0;//update number of copies to 0 to indicate it isn't in the cart anymore
                    lseek(fd4,recordOffset,SEEK_SET);
                    write(fd4,&borrowed_book,sizeof(struct borrowed_book));//update number of copies to 0 to indicate that the book has been borrowed and is no longer in the cart
                    // unlock(fd4,&lock2);
                }
                unlockrecordlock(fd2,&lock1,offset,sizeof(struct book));//remove the lock once you have read the book
                close(fd2);
            }
        }
        recordOffset+=sizeof(struct borrowed_book);
    }
    close(fd);
    write(new_fd,&price,sizeof(double));//send bill to the client
    pthread_mutex_unlock(&mutex1);

}


int numberOfBooksBorrowed(int new_fd,int id){//counts number of books currently borrowed by a particular customer
    int fd=open("borrowed_books.txt",O_RDWR,0776);
    struct flock lock;
    setlock(fd,&lock);
    lseek(fd,0,SEEK_SET);
    int count=0;
    struct borrowed_book borrowed_book;
    while(read(fd,&borrowed_book,sizeof(struct borrowed_book))!=0){
        if(borrowed_book.memberId==id && borrowed_book.numberOfCopies>0){
            count++;
        }
    }
    unlock(fd,&lock);
    close(fd);
    return count;
}
void view_borrowed(int new_fd,int id){//function to view the borrowed books that have not yet been returned to the library
    int count=numberOfBooksBorrowed(new_fd,id);//holds number of books currently borrowed by user with id "id"
    write(new_fd,&count,sizeof(int));
    if(count==0){
        return;
    }
    int start=0;
    int fd=open("borrowed_books.txt",O_RDWR,0776);
    struct flock lock;
    setlock(fd,&lock);
    lseek(fd,0,SEEK_SET);
    struct borrowed_book books[count];
    struct borrowed_book borrowed_book;
    while(read(fd,&borrowed_book,sizeof(struct borrowed_book))!=0){
        if(borrowed_book.memberId==id && borrowed_book.numberOfCopies>0){//means match found
            books[start++]=borrowed_book;//add that detail to books array
        }
    }
    unlock(fd,&lock);
    start=0;
    while(start<count){
        write(new_fd,&books[start++],sizeof(struct borrowed_book));//write list of borrowed books to the client
    }
    close(fd);
    

}
void return_book(int new_fd,int id){//function to implement return of borrowed books by the customer
    pthread_mutex_lock(&mutex1);
    int count=numberOfBooksBorrowed(new_fd,id);
    write(new_fd,&count,sizeof(int));//write number of books borrowed to the client
    if(count==0)//means nothing borrowed
     {  pthread_mutex_unlock(&mutex1); 
        return;
     }
    view_borrowed(new_fd,id);
    int start=0;
    int recordOffset=0;
    int fd=open("borrowed_books.txt",O_RDWR,0776);
    int fd1=open("borrowed_books.txt",O_RDWR,0776);
    struct flock lock,lock1;
    setlock(fd,&lock);
    lseek(fd,0,SEEK_SET);
    struct borrowed_book borrowed_book;
    while(read(fd,&borrowed_book,sizeof(struct borrowed_book))!=0){
        if(borrowed_book.memberId==id && borrowed_book.numberOfCopies>0){//means it still hasn't been returned
                int bookId=borrowed_book.bookId;
                int copies=borrowed_book.numberOfCopies;
                int fd2=open("bookIndex.txt",O_CREAT|O_RDWR,0776);
                int offset=getBookOffset(bookId,fd2);
                close(fd2);
                int fd3=open("book.txt",O_RDWR,0776);
                lseek(fd3,offset,SEEK_SET);
                struct book book;
                setrecordreadlock(fd3,&lock1,offset,sizeof(struct book));//put read lock on that book record
                read(fd3,&book,sizeof(struct book));//read the book
                unlockrecordlock(fd3,&lock1,offset,sizeof(struct book));//unlock the read lock
                if(book.copies!=-1){//means book has not been deleted
                    lseek(fd3,offset,SEEK_SET);
                    book.copies+=copies;//increment the number of copies as user is returning
                    setrecordwritelock(fd3,&lock1,offset,sizeof(struct book));
                    write(fd3,&book,sizeof(struct book));//since user is returning ,number of copies must increase
                    unlockrecordlock(fd3,&lock1,offset,sizeof(struct book));
                }
                lseek(fd1,recordOffset,SEEK_SET);
                borrowed_book.numberOfCopies=0;//to indicate that user has returned that book
                write(fd1,&borrowed_book,sizeof(struct borrowed_book));
                close(fd3);
        }
        recordOffset+=sizeof(struct borrowed_book);
    }
    unlock(fd,&lock);
    close(fd);
    close(fd1);
   pthread_mutex_unlock(&mutex1);
    
}
void view_books(int new_fd){//to view all the books in the library
        int fd=open("book.txt",O_RDWR,0774);
        struct flock lock;
        setlock(fd,&lock);//set a read lock
        struct book book;
        int count=0;
        while(read(fd,&book,sizeof(struct book))!=0){
            if(book.copies>0)//means book is not been removed or not totally borrowed
            { 
                count++;
            }
        } 
        write(new_fd,&count,sizeof(int));//send number of available books
        lseek(fd,0,SEEK_SET);
         while(read(fd,&book,sizeof(struct book))!=0){
            if(book.copies>0)//means book is not been removed or not totally borrowed
            { 
                   
                write(new_fd,&book.bookId,sizeof(int));
                write(new_fd,book.name,sizeof(book.name));
                write(new_fd,book.author,sizeof(book.author));
                write(new_fd,&book.copies,sizeof(int));
                write(new_fd,&book.price,sizeof(double));
            }
        }
        unlock(fd,&lock);
        close(fd);
}
void add_book(int new_fd){//function to add a book to the library by the administrator
        struct book books;
         //only one admin can add a book at a time otherwise both books can get assigned the same id
        pthread_mutex_lock(&mutex2);
        books.bookId=find_smallest_book_id();//this will give smallest id available to assign
       
        write(new_fd,&books.bookId,sizeof(int));
        char buf[50];
        read(new_fd,buf,sizeof(buf));
        strcpy(books.name,buf);
        read(new_fd,buf,sizeof(buf));
        strcpy(books.author,buf);
        int copies;
        double price;
        read(new_fd,&copies,sizeof(int));
        read(new_fd,&price,sizeof(double));
        books.copies=copies;
        books.price=price;
        //you have the details of the book
        //now you have to enter book details into book.txt and bookOffset.txt
        int fd=open("book.txt",O_RDWR,0774);
        int fd1=open("bookIndex.txt",O_RDWR|O_CREAT,0774);
        struct flock lock,lock1;
        setwritelock(fd,&lock);
        setwritelock(fd1,&lock1);
        int offset=lseek(fd,0,SEEK_END);
        lseek(fd1,0,SEEK_END);
        struct bookIndex ind;
        ind.bookId=books.bookId;
        ind.bookOffset=offset;
        write(fd1,&ind,sizeof(struct bookIndex));
        unlock(fd1,&lock1);//remove write lock
        write(fd,&books,sizeof(struct book));
        unlock(fd,&lock);
        close(fd);
        close(fd1);
         pthread_mutex_unlock(&mutex2);
        
}
void remove_book(int new_fd){//function to remove a book from the library by the administrator
    //only one admin can remove a book at a time
    pthread_mutex_lock(&mutex1);
    int id;
    read(new_fd,&id,sizeof(int));//read the id of the book the user wants to remove
    int fd=open("bookIndex.txt",O_RDWR,0776);
    int offset=getBookOffset(id,fd);
    close(fd);
    write(new_fd,&offset,sizeof(int));
    if(offset==-1)
      { pthread_mutex_unlock(&mutex1); 
        return;
      }
    fd=open("book.txt",O_RDWR,0776);//open "book.txt" in read write mode 
    //since you are removing it, we are going to set the number of copies available to -1
    lseek(fd,offset,SEEK_SET);
    struct flock lock;
    struct book book;
    setrecordreadlock(fd,&lock,offset,sizeof(struct book));
    read(fd,&book,sizeof(struct book));
    unlockrecordlock(fd,&lock,offset,sizeof(struct book));
    write(new_fd,&book.copies,sizeof(int));
    if(book.copies==-1){
        pthread_mutex_unlock(&mutex1);
        return;//means book was already deleted
    }
    setrecordwritelock(fd,&lock,offset,sizeof(struct book));
    lseek(fd,offset,SEEK_SET);
    book.copies=-1;//set copies to -1 to indicate it won't be available any longer
    write(fd,&book,sizeof(struct book));
    unlockrecordlock(fd,&lock,offset,sizeof(struct book));//remove record write lock
    close(fd);
    pthread_mutex_unlock(&mutex1); 
}
void modify_book(int new_fd){//function to modify a book's details by the adminstrator
    pthread_mutex_lock(&mutex1);
    int id;
    read(new_fd,&id,sizeof(int));//read book id from client
    //try to find the book with that id
    int fd=open("bookIndex.txt",O_RDWR,0776);
    int offset=getBookOffset(id,fd);
    close(fd);
    fd=open("book.txt",O_RDWR,0776);
    write(new_fd,&offset,sizeof(int));//write the book offset to the client
    if(offset==-1){//means book doesn't exist
        close(fd);
        pthread_mutex_unlock(&mutex1);
        return;
    }
    struct book book;
    struct flock lock;
    //put a record lock
    lock.l_type = F_RDLCK;  // read lock
    lock.l_start = offset;
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(struct book);
    fcntl(fd, F_SETLKW, &lock);
    lseek(fd,offset,SEEK_SET);
    read(fd,&book,sizeof(struct book));//send book details to the admin
    write(new_fd,&book.copies,sizeof(int));
    write(new_fd,book.name,sizeof(book.name));
    write(new_fd,book.author,sizeof(book.author));
    write(new_fd,&book.price,sizeof(double));
    if(book.copies==-1)
     {  
        lock.l_type=F_UNLCK;
        fcntl(fd,F_SETLKW,&lock);//unlock the lock and close the fd and return
        close(fd);
        pthread_mutex_unlock(&mutex1);
        return;
     }
    int ch;
    read(new_fd,&ch,sizeof(int));
    if(ch==1){//means admin wants to modify the price
        double price;
        read(new_fd,&price,sizeof(double));
        lock.l_type=F_UNLCK;
        fcntl(fd,F_SETLKW,&lock);//unlock the read lock and then put a write lock
        lock.l_type=F_WRLCK;
        fcntl(fd,F_SETLKW,&lock);
        book.price=price;//update the price
        lseek(fd,offset,SEEK_SET);
        write(fd,&book,sizeof(struct book));//write new book details
        lock.l_type=F_UNLCK;
        fcntl(fd,F_SETLKW,&lock);
    }
    else{//means admin wants to modify the quantity
        int copies;
        read(new_fd,&copies,sizeof(int));
        lock.l_type=F_UNLCK;
        fcntl(fd,F_SETLKW,&lock);//unlock the read lock and then put a write lock
        lock.l_type=F_WRLCK;
        fcntl(fd,F_SETLKW,&lock);
        book.copies=copies;//update the copies
        lseek(fd,offset,SEEK_SET);
        write(fd,&book,sizeof(struct book));//write new book details
        lock.l_type=F_UNLCK;
        fcntl(fd,F_SETLKW,&lock);
    }
    close(fd);
    pthread_mutex_unlock(&mutex1);

}
void view_members(int new_fd){//function to view the list of members enrolled in the library website
        int fd=open("member.txt",O_RDWR,0774);
        struct flock lock;
        setlock(fd,&lock);//set a read lock
        struct member member;
        int count=0;
        while(read(fd,&member,sizeof(struct member))!=0){
            if(member.memberId>0)//means member still exists
            { 
                count++;
            }
        } 
        write(new_fd,&count,sizeof(int));//send number of  members
        lseek(fd,0,SEEK_SET);
         while(read(fd,&member,sizeof(struct member))!=0){
            if(member.memberId>0)
            {    
                write(new_fd,&member.memberId,sizeof(int));
                write(new_fd,member.name,sizeof(member.name));
                write(new_fd,member.email,sizeof(member.email));
            }
        }
        unlock(fd,&lock);
        close(fd);

}
void view_admins(int new_fd){
    int fd=open("admin.txt",O_RDWR,0774);
        struct flock lock;
        setlock(fd,&lock);//set a read lock
        struct admin admin;
        int count=0;
        while(read(fd,&admin,sizeof(struct admin))!=0){
            if(admin.adminId>0)//means admin still exists
            { 
                count++;
            }
        } 
        write(new_fd,&count,sizeof(int));//send number of  admins
        lseek(fd,0,SEEK_SET);
         while(read(fd,&admin,sizeof(struct admin))!=0){
            if(admin.adminId>0)
            {    
                write(new_fd,&admin.adminId,sizeof(int));
                write(new_fd,admin.name,sizeof(admin.name));
                write(new_fd,admin.email,sizeof(admin.email));
            }
        }
        unlock(fd,&lock);
        close(fd);
}
void view_specific_book(int new_fd){//function to view the details of a specific book
    int id;
    read(new_fd,&id,sizeof(int));
    int fd=open("bookIndex.txt",O_RDWR,0776);//open index file
    int offset=getBookOffset(id,fd);//get offset
    write(new_fd,&offset,sizeof(int));
    if(offset==-1)//invalid book id given
        return;
    close(fd);
    fd=open("book.txt",O_RDWR,0776);
    struct flock lock;
    struct book book;
    setrecordreadlock(fd,&lock,offset,sizeof(struct book));
    lseek(fd,offset,SEEK_SET);
    read(fd,&book,sizeof(struct book));
    unlockrecordlock(fd,&lock,offset,sizeof(struct book));
    write(new_fd,&book.copies,sizeof(int));
    write(new_fd,book.name,sizeof(book.name));
    write(new_fd,book.author,sizeof(book.author));
    write(new_fd,&book.price,sizeof(double));
    if(book.copies==-1){
        return;//indicates book has been deleted
    }
    //write details
    write(new_fd,&book.copies,sizeof(int));
    close(fd);
}
void view_books_borrowed(int new_fd){//function to view books borrowed by a member
   int id;
   read(new_fd,&id,sizeof(int));
   //check if id is valid
   int fd=open("memberIndex.txt",O_RDWR,0776);
   int offset=getMemberOffset(id,fd);
   write(new_fd,&offset,sizeof(int));
   if(offset<0)//it means member with that id doesn't exist
        return;
    view_borrowed(new_fd,id);
}
void resetPassword(int new_fd,struct member *update){//function to reset password
    struct flock lock;
    int fd=open("member.txt",O_RDWR|O_CREAT,0744);
    int fd1=open("memberIndex.txt",O_RDWR|O_CREAT,0744);
    setlock(fd,&lock);//put a read lock on the file
    struct member members;
    struct member change;
    int ans=-1;
    while(ans<0){
        lseek(fd,0,SEEK_SET);
        char email[50];
        read(new_fd,email,50); //read email input by user
        while (read(fd, &members, sizeof(struct member)))//try to match it with email data in our file
         {   
            if (strcmp(members.email,email)==0){//match found
                strcpy(change.name,members.name);
                strcpy(change.password,members.password);
                change.memberId=members.memberId;
                strcpy(change.email,members.email);
                ans=1;
                break;
            }
        }
        write(new_fd,&ans,sizeof(int));
    }
    unlock(fd,&lock);
    char password[50];
    lseek(fd,0,SEEK_SET);
    lseek(fd1,0,SEEK_SET);
        read(new_fd,password,sizeof(password));
        setwritelock(fd,&lock); //put write lock as you are updating the file
        strcpy(change.password,password);//update the password
        strcpy(update->password,password);
        int offset=getMemberOffset(change.memberId,fd1);
        lseek(fd,offset,SEEK_SET);
        write(fd,&change,sizeof(struct member));
        close(fd1);
        unlock(fd,&lock);
        close(fd);
}
void memberSignUp(int new_fd){//function for member sign up
        pthread_mutex_lock(&mutex3);//sign in can be conflicting operations hence the lock
        struct member members;
        members.memberId=find_smallest_member_id();//this will give smallest id available to assign
        write(new_fd,&members.memberId,sizeof(int));//send the assigned id to the client
        char buf[50];
        read(new_fd,buf,sizeof(buf));
        strcpy(members.name,buf);
         read(new_fd,buf,sizeof(buf));
        strcpy(members.email,buf);
         read(new_fd,buf,sizeof(buf));
        strcpy(members.password,buf);
        //you have the details of the member
        //now you have to enter member details into member.txt and memberOffset.txt
        int fd=open("member.txt",O_RDWR,0774);
        int fd1=open("memberIndex.txt",O_RDWR|O_CREAT,0774);
        struct flock lock,lock1;
        setwritelock(fd,&lock);
        setwritelock(fd1,&lock1);
        int offset=lseek(fd,0,SEEK_END);
        lseek(fd1,0,SEEK_END);
        struct memberIndex ind;
        ind.memberId=members.memberId;
        ind.memberOffset=offset;
        write(fd1,&ind,sizeof(struct memberIndex));
        unlock(fd1,&lock1);//remove write lock
        write(fd,&members,sizeof(struct member));
        unlock(fd,&lock);
        close(fd);
        close(fd1);
        pthread_mutex_unlock(&mutex3);
}
void adminSignUp(int new_fd){//function for administrator sign up
        //admin has to know the secret key before signup
        pthread_mutex_lock(&mutex4);
        int ans=-1;
        int ch;
        char password[50];
        int reply=-1;
        while(ans==-1){
            read(new_fd,&ch,sizeof(int));
            if(ch==1){//means user doesn't want to validate himself
                pthread_mutex_unlock(&mutex4);
                return;
            }
            read(new_fd,password,50);
            if(strcmp(PASSWORD,password)==0)
               { 
                    ans=1; 
                    reply=1;
               }
            write(new_fd,&reply,sizeof(int));
        }
        struct admin admin;
        admin.adminId=find_smallest_admin_id();//this will give smallest id available to assign
        write(new_fd,&admin.adminId,sizeof(int));//send the assigned id to the admin
        char buf[50];
        read(new_fd,buf,sizeof(buf));
        strcpy(admin.name,buf);
         read(new_fd,buf,sizeof(buf));
        strcpy(admin.email,buf);
        //you have the details of the admin
        //now you have to enter admin details into admin.txt and adminOffset.txt
        int fd=open("admin.txt",O_RDWR,0774);
        int fd1=open("adminIndex.txt",O_RDWR|O_CREAT,0774);
        struct flock lock,lock1;
        setwritelock(fd,&lock);
        setwritelock(fd1,&lock1);
        int offset=lseek(fd,0,SEEK_END);
        lseek(fd1,0,SEEK_END);
        struct adminIndex ind;
        ind.adminId=admin.adminId;
        ind.adminOffset=offset;
        write(fd1,&ind,sizeof(struct adminIndex));
        unlock(fd1,&lock1);//remove write lock
        write(fd,&admin,sizeof(struct admin));
        unlock(fd,&lock);
        close(fd);
        close(fd1);
        pthread_mutex_unlock(&mutex4);

}
void memberLogIn(int new_fd){//function used for member log in
        int id;
        read(new_fd,&id,sizeof(int));
        int fd=open("memberIndex.txt",O_RDWR|O_CREAT,0774);
        int ans=getMemberOffset(id,fd);
        write(new_fd,&ans,sizeof(int));
        close(fd);
        while(ans<0){//this means such a user doesn't exist
            memberSignUp(new_fd);//user needs to sign up first
            read(new_fd,&id,sizeof(int));
            fd=open("memberIndex.txt",O_RDWR|O_CREAT,0774);
            ans=getMemberOffset(id,fd);
            write(new_fd,&ans,sizeof(int));
            close(fd);
        }

    
        struct flock lock;
        struct member members;
        fd=open("member.txt",O_RDWR,0774);
        lseek(fd,ans,SEEK_SET);
        setlock(fd,&lock);
        read(fd,&members,sizeof(struct member));
        unlock(fd,&lock);
        close(fd);
        int c=-1;
        int choice=1;
        while(c<0)
        {
            char password[50];
            read(new_fd,password,50);
            if(strcmp(members.password,password)==0){//means match
                c=1;
                write(new_fd,&c,sizeof(int));
                break;
            }
            else{//means password doesn't match
                write(new_fd,&c,sizeof(int));
                read(new_fd,&choice,sizeof(int));
                if(choice==1){
                    continue;
                }
                else if(choice==2){
                    resetPassword(new_fd,&members);
                }
            }
        }
        write(new_fd,members.name,sizeof(members.name));//send member name to client
        while(1){
            int choice;
            read(new_fd,&choice,sizeof(int));
            if(choice==1){
                add_book_to_cart(new_fd,id);
            }
            else if(choice==2){
                view_cart(new_fd,id);
            }
            else if(choice==3){
                modify_cart(new_fd,id);
            }
             else if(choice==4){
                borrow_book(new_fd,id);
            }
            else if(choice==5){
                return_book(new_fd,id);
            }
            else if(choice==6){
                view_borrowed(new_fd,id);
            }
            else if(choice==7){
                view_books(new_fd);
            }
            else{
                break;
            }
        }
}
void adminLogIn(int new_fd){//function used for adminstrator log in
        int id;
        read(new_fd,&id,sizeof(int));
        int fd=open("adminIndex.txt",O_RDWR|O_CREAT,0774);
        int ans=getAdminOffset(id,fd);
          write(new_fd,&ans,sizeof(int));
        close(fd);
        while(ans<0){//this means such an admin doesn't exist
            adminSignUp(new_fd);//admin needs to sign up first
            read(new_fd,&id,sizeof(int));
            fd=open("adminIndex.txt",O_RDWR|O_CREAT,0774);
            ans=getAdminOffset(id,fd);
                      write(new_fd,&ans,sizeof(int));
            close(fd);
        }
        char password[50];
        struct flock lock;
        struct admin admin;
        fd=open("admin.txt",O_RDWR,0774);
        lseek(fd,ans,SEEK_SET);
        setlock(fd,&lock);
        read(fd,&admin,sizeof(struct admin));
        unlock(fd,&lock);
        close(fd);
        int ok=-1;
        while(ok<0)
        {
            read(new_fd,password,50);
            if(strcmp(PASSWORD,password)==0){//means match
            ok=1;
            write(new_fd,&ok,sizeof(int));
            }
            else{//means no match
            ok=-1;
            write(new_fd,&ok,sizeof(int));
            }
        }
        write(new_fd,admin.name,sizeof(admin.name));//send admin name to client
        while(1){
            int choice;
            read(new_fd,&choice,sizeof(int));
             if(choice==1){
                view_members(new_fd);
            }
            else if(choice==2){
                add_book(new_fd);
            }
            else if(choice==3){
                remove_book(new_fd);
            }
            else if(choice==4){
                modify_book(new_fd);
            }
            else if(choice==5){
                view_books(new_fd);
            }
            else if(choice==6){
                view_books_borrowed(new_fd);
            }
            else if(choice==7){
                view_specific_book(new_fd);
            }
            else if(choice==8){
                view_admins(new_fd);
            }
            else{
                break;
            }
        }

}
void * communicate(void *fd){//function executed by the thread to support parallelism
    int new_fd=*((int *)fd);
    printf("Connection with client successful.\n\n");
    int user; //this will store user input 1 or 2 or 3 or 4(for sign up and login as user and admin)
    while(1)
   { 
    read(new_fd, &user, sizeof(int));
    if(user==1){//member sign up
        memberSignUp(new_fd);
    }
    else if(user==2){//admin sign up
        adminSignUp(new_fd);
    }
    else if(user==3){//login as member verification
        memberLogIn(new_fd);
    }
    else if(user==4){//login as admin verification
        adminLogIn(new_fd);
    }
    else 
        pthread_exit(0);//else exit
   }
}
int main(){
    printf("Setting up the server.....\n\n");
    //initialize the mutexes
    if(pthread_mutex_init(&mutex1,NULL)!=0){//mutex1 is for the functions add_book_to_cart,borrow_book,return_book,remove_book,modify_book as these functions can't be accessed simultaneously as it might create inconsistency
        printf("Mutex initialization failed.\n");
        return -1;
    }
    if(pthread_mutex_init(&mutex2,NULL)!=0){//mutex2 is for add_book to prevent inconsistencies
        printf("Mutex initialization failed.\n");
        return -1;
    }
    if(pthread_mutex_init(&mutex3,NULL)!=0){//to prevent simultaneous member sign up
        printf("Mutex initialization failed.\n");
        return -1;
    }
    if(pthread_mutex_init(&mutex4,NULL)!=0){//to prevent simultaneous admin sign up
        printf("Mutex initialization failed.\n");
        return -1;
    }

    int records_fd = open("records.txt", O_RDWR | O_CREAT, 0776);//this file will contain members and their corresponding cart info
    int member_fd = open("member.txt", O_RDWR | O_CREAT, 0776);//this file will contain all member info
    int books_fd = open("book.txt", O_RDWR | O_CREAT, 0776);//this file will contain all book info
    int admin_fd=open("admin.txt",O_RDWR|O_CREAT,0776);//this file will contain all admin info
    int borrowed_books=open("borrowed_books.txt",O_RDWR|O_CREAT,0776);//this file will contain members and their corresponding borrowed books information
    close(records_fd);
    close(member_fd);
    close(books_fd);
    close(admin_fd);
    close(borrowed_books);
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);//creating a socket(half),TCP connection
    if (sockfd == -1){
        perror("Error in getting a socket ");
        return -1;
    }
    struct sockaddr_in server,client;
    server.sin_family=AF_INET;
    server.sin_addr.s_addr=INADDR_ANY;
    server.sin_port=htons(PORT); //specifying port number and ip address of server
    if (bind(sockfd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1){//associating port number and ip address with that socket
        perror("Error in binding");
        return -1;
    }
    if (listen(sockfd, 5) == -1){//listening on that port number and socket. At maximum, 5 client connections possible
        perror("Error in listen() ");
        return -1;
    }
    printf("Server successfully set-up.\n\n");
    int size=sizeof(struct sockaddr);
    int new_fd;
    while(1){
         new_fd=accept(sockfd, (struct sockaddr *)&client, &size);//establishing full socket connection. Socket is uniquely identified by 5 tuple(local Ip,local host,remote Ip,remote host, protocol)
        if (new_fd == -1){
             perror("Error in establishing a connection.\n\n");
            return -1;
        }
        pthread_t thread;//create a thread
        int x=pthread_create(&thread,NULL,communicate,(void *)&new_fd);//and let it do its function which is to communicate
        //multithreading helps us achieve concurrency and better performance.
         
    }
    close(new_fd);
    close(sockfd);
}