#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure to represent a book
struct Book {
    char title[100];
    char author[50];
    int id;
    int isAvailable;
    struct Book* next; // Linked list pointer
};

// Structure to represent a student
struct Student {
    char name[50];
    char department[50];
    int id;
    int booksIssued;
};

struct StudentNode {
    struct Student data;
    struct StudentNode* next;
};

#define TABLE_SIZE 100

struct StudentHashTable {
    struct StudentNode* table[TABLE_SIZE];
};

void addBook(struct Book** library, int* bookCount);
void removeBook(struct Book** library, int* bookCount);
void issueBook(struct Book** library, struct StudentHashTable* students, int* bookCount);
void returnBook(struct Book** library, struct StudentHashTable* students, int* bookCount);
void displayAvailableBooks(struct Book* library);
void displayIssuedBooks(struct Book* library);
void saveLibraryToFile(struct Book* library);
void loadLibraryFromFile(struct Book** library, int* bookCount);
void initializeStudentHashTable(struct StudentHashTable* students);
void addStudent(struct StudentHashTable* students, struct Student student);
struct Student* findStudent(struct StudentHashTable* students, int id);

int main() {
    struct Book* library = NULL;
    int bookCount = 0;
    struct StudentHashTable students;
    initializeStudentHashTable(&students);

    // Load existing data from text files (if any)
    loadLibraryFromFile(&library, &bookCount);

    int choice;
    do {
        printf("\nLibrary Management System Menu:\n");
        printf("1. Add Book\n");
        printf("2. Remove Book\n");
        printf("3. Issue Book\n");
        printf("4. Return Book\n");
        printf("5. Display Available Books\n");
        printf("6. Display Issued Books\n");
        printf("7. Save Library to File\n");
        printf("8. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                addBook(&library, &bookCount);
                break;
            case 2:
                removeBook(&library, &bookCount);
                break;
            case 3:
                issueBook(&library, &students, &bookCount);
                break;
            case 4:
                returnBook(&library, &students, &bookCount);
                break;
            case 5:
                displayAvailableBooks(library);
                break;
            case 6:
                displayIssuedBooks(library);
                break;
            case 7:
                saveLibraryToFile(library);
                printf("Library data saved to file.\n");
                break;
            case 8:
                printf("Exiting the program.\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 8);

    return 0;
}

void addBook(struct Book** library, int* bookCount) {
    // Allocate memory for a new book
    struct Book* newBook = (struct Book*)malloc(sizeof(struct Book));
    if (newBook == NULL) {
        printf("Memory allocation failed. Book cannot be added.\n");
        return;
    }

    // Ask user for book details
    printf("Enter Book Title: ");
    scanf("%s", newBook->title);
    printf("Enter Author: ");
    scanf("%s", newBook->author);

    // Assign ID to the new book and update availability status
    newBook->id = *bookCount + 1; // Assign a unique ID to the book
    newBook->isAvailable = 1;     // Set availability status to '1' (available)

    // Insert the new book at the beginning of the linked list
    newBook->next = *library; // Point the new book to the current library
    *library = newBook;       // Update the library head to the new book

    // Increment the total book count
    (*bookCount)++;

    printf("Book added successfully.\n");
}

void removeBook(struct Book** library, int* bookCount) {
    int bookId;
    printf("Enter Book ID to remove: ");
    scanf("%d", &bookId);

    struct Book* current = *library; // Start at the beginning of the linked list
    struct Book* prev = NULL; // Initialize a pointer to track the previous node

    while (current != NULL) { // Traverse the linked list
        if (current->id == bookId) { // Check if the current book matches the given ID
            if (prev == NULL) { // If the book to remove is the first in the list
                *library = current->next; // Update the library head to skip the current book
            } else {
                prev->next = current->next; // Adjust pointers to bypass the current book
            }
            free(current); // Deallocate memory for the removed book
            (*bookCount)--; // Decrement the book count
            printf("Book removed successfully.\n");
            return; // Exit the function after removing the book
        }
        prev = current; // Update the 'prev' pointer to the current node
        current = current->next; // Move to the next book in the list
    }

    printf("Book not found.\n"); // If the book with the specified ID is not found
}


void issueBook(struct Book** library, struct StudentHashTable* students, int* bookCount) {
    int bookId, studentId;
    printf("Enter Book ID to issue: ");
    scanf("%d", &bookId);
    printf("Enter Student ID: ");
    scanf("%d", &studentId);

    struct Student* student = findStudent(students, studentId);
    if (student == NULL) {
        printf("Student not found. Adding the student to issue the book...\n");

        // Create a new student since the entered ID doesn't exist
        struct Student newStudent;
        newStudent.id = studentId;

        printf("Enter Student Name: ");
        scanf("%s", newStudent.name);
        printf("Enter Student Department: ");
        scanf("%s", newStudent.department);
        newStudent.booksIssued = 1; // Issue the book to the new student

        // Add the new student to the hash table
        addStudent(students, newStudent);
        student = findStudent(students, studentId); // Retrieve the added student
    }

    // Now that we have the student information, proceed to issue the book
    struct Book* current = *library;
    while (current != NULL) {
        if (current->id == bookId) {
            if (current->isAvailable) {
                current->isAvailable = 0;
                student->booksIssued++;
                printf("Book issued to student %s.\n", student->name);
                return;
            } else {
                printf("Book is already issued.\n");
                return;
            }
        }
        current = current->next;
    }

    printf("Book not found.\n");
}



void returnBook(struct Book** library, struct StudentHashTable* students, int* bookCount) {
    int bookId, studentId;
    printf("Enter Book ID to return: ");
    scanf("%d", &bookId);
    printf("Enter Student ID: ");
    scanf("%d", &studentId);

    struct Student* student = findStudent(students, studentId);
    if (student == NULL) {
        printf("Student not found.\n");
        return;
    }

    struct Book* current = *library;
    while (current != NULL) {
        if (current->id == bookId) {
            if (!(current->isAvailable)) {
                current->isAvailable = 1;
                if (student->booksIssued > 0) {
                    student->booksIssued--;
                }
                printf("Book returned by student %s.\n", student->name);
                return;
            } else {
                printf("Book is not issued.\n");
                return;
            }
        }
        current = current->next;
    }
    printf("Book not found.\n");
}

void displayAvailableBooks(struct Book* library) {
    printf("Available Books:\n");
    struct Book* current = library;
    while (current != NULL) {
        if (current->isAvailable) {
            printf("ID: %d, Title: %s, Author: %s\n", current->id, current->title, current->author);
        }
        current = current->next;
    }
}

void displayIssuedBooks(struct Book* library) {
    printf("Issued Books:\n");
    struct Book* current = library;
    while (current != NULL) {
        if (!(current->isAvailable)) {
            printf("ID: %d, Title: %s, Author: %s\n", current->id, current->title, current->author);
        }
        current = current->next;
    }
}

void saveLibraryToFile(struct Book* library) {
    FILE* file = fopen("library.txt", "w");
    if (file == NULL) {
        printf("Error opening file for saving library data.\n");
        return;
    }

    struct Book* current = library;
    while (current != NULL) {
        fprintf(file, "%d,%s,%s,%d\n", current->id, current->title, current->author, current->isAvailable);
        current = current->next;
    }

    fclose(file);
}

void loadLibraryFromFile(struct Book** library, int* bookCount) {
    FILE* file = fopen("library.txt", "r");
    if (file == NULL) {
        return;  // No data file exists yet.
    }

    while (!feof(file)) {
        struct Book* newBook = (struct Book*)malloc(sizeof(struct Book));
        if (newBook == NULL) {
            printf("Memory allocation failed while loading library data.\n");
            fclose(file);
            return;
        }

        fscanf(file, "%d,%[^,],%[^,],%d\n", &newBook->id, newBook->title, newBook->author, &newBook->isAvailable);
        newBook->next = *library;
        *library = newBook;
        (*bookCount)++;
    }

    fclose(file);
}

void initializeStudentHashTable(struct StudentHashTable* students) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        students->table[i] = NULL;
    }
}

void addStudent(struct StudentHashTable* students, struct Student student) {
    int index = student.id % TABLE_SIZE;

    struct StudentNode* newNode = (struct StudentNode*)malloc(sizeof(struct StudentNode));
    if (newNode == NULL) {
        printf("Memory allocation failed. Student cannot be added.\n");
        return;
    }

    newNode->data = student;
    newNode->next = students->table[index];
    students->table[index] = newNode;

    printf("Student added successfully.\n");
}

struct Student* findStudent(struct StudentHashTable* students, int id) {
    int index = id % TABLE_SIZE;

    struct StudentNode* current = students->table[index];
    while (current != NULL) {
        if (current->data.id == id) {
            return &current->data;
        }
        current = current->next;
    }
    return NULL;
}