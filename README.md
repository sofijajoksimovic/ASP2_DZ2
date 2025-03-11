# **ASP2_DZ2 – Algorithms and Data Structures 2: Efficient Data Retrieval from Multiple Database Tables**

## **Project Description**

The project aims to implement a B-tree of order m (where m is at least 3 and at most 10) as an index structure over the `CUSTOMER_ACCOUNT` table. The index is designed for efficient data retrieval and supports operations such as adding, deleting, and searching data from multiple database tables. The project involves working with two tables: `CUSTOMER` and `CUSTOMER_ACCOUNT`, which are located in text files.

### **Key Operations:**

1. **Create and Delete Index**: An index is created over the `CUSTOMER_ACCOUNT` table, and the index can also be deleted.
2. **Print Index**: The index is printed to the standard output, with nodes clearly spaced for better readability.
3. **Search User Data**: The program provides a way to search for user data based on their accounts.
4. **Add New Record to Index**: A new account is added to the index, checking if the primary key already exists in the system.
5. **Delete Record from Index**: A record is deleted from the index, with the program outputting the number of search steps taken.
6. **Delete All Accounts for a User**: Deletes all accounts for a given user, showing the number of steps and records removed.

### **Requirements:**

- Implement the solution in C or C++.
- The program should allow user interaction through a menu that facilitates the selection and execution of operations.
- Data reading and writing is done through the `Customer.txt` and `CustomerAccount*.txt` files.
- The program should include error handling mechanisms and provide appropriate messages in case of failures.

### **Project Objective:**

The goal is to create an efficient data structure that enables fast search and management of data from multiple database tables using a B-tree as the index structure.
