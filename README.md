# Library-Management-System
Developed an Online Library Management System (OLMS) that not only provides basic functionalities of a library but also ensures data security and concurrency control.
The system employs socket programming to enable multiple clients to access the library database concurrently. Additionally, system calls are utilized extensively for various operations such as process management, file handling, file locking, multithreading, and interprocess communication, ensuring optimal performance and resource utilization.

client.c file simulates the options available to the clients, either as members or as administrators.
server.c file simulates the server that holds access to all the various type of records about the books, members and administrators as well as their cart history etc.

Run server.c first. This creates a half socket and starts listening for connections. 
Run client.c after that. Socket connection will be established and the client and the server can then communicate with each other.

In client.c, you will be given options to either sign-in/log-in as member or sign-in/log-in as administrator. Initially you always have to sign in (equivalent to creating an account) before logging in. Once you create an account, then you are free to log in. In case, while logging in if you forget your password , options have been provided to reset your password using a forgot_password function that just takes in your gmail to verify it is indeed you and allows you to change your password.

For the administrators, in order to ensure that only the authorized personnel are able to sign up or log in as administrators, there is a common key that they need to know and only once they enter that key will they be able to sign up or log in as admins.

Once you log in as a member, you have the opportunity to:
1: Add book to cart.
2: To view cart.
3: To modify cart.
4: To borrow book. (This is like confirming payment for the items in your cart)
5: To return book.
6: To view borrowed books.
7: To view available books.

Once you log in as an admin, you have the oppurtunity to:
1: To view members.
2: To add a book to the library.
3: To remove a book from the library.
4: To modify a book's details.
5: To view books.
6: To view books borrowed by a specific member.
7: To view specific book details.
8: To view admin details.

Note: The member Id, admin Id, book Id are all assigned by the server and in an incremental fashion starting with id=1. I have made use of several text files to act as databases to store information. 
"member.txt" -> holds all the member information(Name, Email, Password, ID)
"admin.txt" -> holds all the admin information(Name, Email, ID)
"book.txt" -> holds all the book information(Name, Author, ID, Number of Copies, Price for borrowing for one day)
"records.txt" -> holds details about members and their corresponding cart information
"borrowed_books.txt" -> holds details about members and the list of books they have borrowed

Whatever you choose to do in "client.c" will be communicated to "server.c" using the socket established and all the actions on the files will be done by the server ensuring client doesn't have direct access to the server. The server is a concurrent server and can support multiple client connections simultaneously.

In order to prevent any race conditions, mutex locks have been applied in the critical sections ensuring that the data is consistent.

  
