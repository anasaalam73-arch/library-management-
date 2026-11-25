
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"

#define FILE_NAME "books.dat"
#define TEMP_FILE "temp.dat"

typedef struct {
    int id;
    char name[100];
    char author[100];
    int quantity;
} Book;

typedef struct {
    int id;
    char name[100];
    char course[100];
    char phone[20];
} Student;

typedef struct {
    int studentID;
    int bookID;
    char issueDate[20];
    int isReturned;
} Issue;

void addStudent();
void viewStudents();
void searchStudent();
void updateStudent();
void deleteStudent();
void issueBook();
void returnBook();
void viewIssueRecords();
int studentExists(int id);

void clearInput() {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);
}

void readText(char text[], int size) {
    if (fgets(text, size, stdin) != NULL)
        text[strcspn(text, "\n")] = '\0';
}

int bookExists(int id) {
    FILE *fp = fopen(FILE_NAME, "rb");
    Book b;

    if (fp == NULL) return 0;

    while (fread(&b, sizeof(Book), 1, fp) == 1) {
        if (b.id == id) {
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

void addBook() {
    Book b;

    printf("\nEnter Book ID: ");
    if (scanf("%d", &b.id) != 1) {
        printf("Invalid ID!\n");
        clearInput();
        return;
    }
    clearInput();

    if (bookExists(b.id)) {
        printf("Book ID already exists!\n");
        return;
    }

    printf("Enter Book Name: ");
    readText(b.name, sizeof(b.name));

    printf("Enter Author Name: ");
    readText(b.author, sizeof(b.author));

    printf("Enter Quantity: ");
    if (scanf("%d", &b.quantity) != 1 || b.quantity < 0) {
        printf("Invalid quantity!\n");
        clearInput();
        return;
    }
    clearInput();

    FILE *fp = fopen(FILE_NAME, "ab");
    if (fp == NULL) {
        printf("File opening error!\n");
        return;
    }

    if (fwrite(&b, sizeof(Book), 1, fp) == 1)
        printf("\nBook added successfully!\n");
    else
        printf("\nError saving book!\n");

    fclose(fp);
}

void viewBooks() {
    FILE *fp = fopen(FILE_NAME, "rb");
    Book b;
    int count = 0;

    printf("\n========== ALL BOOKS ==========\n");

    if (fp == NULL) {
        printf("No books found.\n");
        return;
    }

    printf("\n%-10s %-25s %-25s %-10s\n",
           "ID", "BOOK NAME", "AUTHOR", "QUANTITY");

    while (fread(&b, sizeof(Book), 1, fp) == 1) {
        printf("%-10d %-25s %-25s %-10d\n",
               b.id, b.name, b.author, b.quantity);
        count++;
    }

    if (count == 0) printf("No books found.\n");

    fclose(fp);
}

void searchBook() {
    FILE *fp = fopen(FILE_NAME, "rb");
    Book b;
    int id, found = 0;

    printf("\nEnter Book ID to search: ");

    if (scanf("%d", &id) != 1) {
        printf("Invalid ID!\n");
        clearInput();
        if (fp != NULL) fclose(fp);
        return;
    }
    clearInput();

    if (fp == NULL) {
        printf("No books available.\n");
        return;
    }

    while (fread(&b, sizeof(Book), 1, fp) == 1) {
        if (b.id == id) {
            printf("\nBook Found!\n");
            printf("ID: %d\nName: %s\nAuthor: %s\nQuantity: %d\n",
                   b.id, b.name, b.author, b.quantity);
            found = 1;
            break;
        }
    }

    if (!found) printf("Book not found!\n");
    fclose(fp);
}

void updateBook() {
    FILE *fp = fopen(FILE_NAME, "rb+");
    Book b;
    int id, found = 0;

    printf("\nEnter Book ID to update: ");

    if (scanf("%d", &id) != 1) {
        printf("Invalid ID!\n");
        clearInput();
        if (fp != NULL) fclose(fp);
        return;
    }
    clearInput();

    if (fp == NULL) {
        printf("No books available.\n");
        return;
    }

    while (fread(&b, sizeof(Book), 1, fp) == 1) {
        if (b.id == id) {
            printf("\nCurrent Book: %s\n", b.name);

            printf("Enter New Book Name: ");
            readText(b.name, sizeof(b.name));

            printf("Enter New Author Name: ");
            readText(b.author, sizeof(b.author));

            printf("Enter New Quantity: ");
            if (scanf("%d", &b.quantity) != 1 || b.quantity < 0) {
                printf("Invalid quantity!\n");
                clearInput();
                fclose(fp);
                return;
            }
            clearInput();

            fseek(fp, -(long)sizeof(Book), SEEK_CUR);

            if (fwrite(&b, sizeof(Book), 1, fp) == 1)
                printf("\nBook updated successfully!\n");
            else
                printf("\nError updating book!\n");

            found = 1;
            break;
        }
    }

    if (!found) printf("Book not found!\n");
    fclose(fp);
}

void deleteBook() {
    FILE *fp = fopen(FILE_NAME, "rb");
    FILE *temp = fopen(TEMP_FILE, "wb");
    Book b;
    int id, found = 0, error = 0;

    printf("\nEnter Book ID to delete: ");

    if (scanf("%d", &id) != 1) {
        printf("Invalid ID!\n");
        clearInput();
        if (fp != NULL) fclose(fp);
        if (temp != NULL) fclose(temp);
        remove(TEMP_FILE);
        return;
    }
    clearInput();

    if (fp == NULL || temp == NULL) {
        printf("File opening error!\n");
        if (fp != NULL) fclose(fp);
        if (temp != NULL) fclose(temp);
        remove(TEMP_FILE);
        return;
    }

    while (fread(&b, sizeof(Book), 1, fp) == 1) {
        if (b.id == id) {
            found = 1;
            continue;
        }

        if (fwrite(&b, sizeof(Book), 1, temp) != 1) {
            error = 1;
            break;
        }
    }

    if (ferror(fp)) error = 1;

    fclose(fp);
    fclose(temp);

    if (error) {
        printf("File processing error!\n");
        remove(TEMP_FILE);
        return;
    }

    if (!found) {
        printf("Book not found!\n");
        remove(TEMP_FILE);
        return;
    }

    if (remove(FILE_NAME) != 0) {
        printf("Could not replace book file!\n");
        remove(TEMP_FILE);
        return;
    }

    if (rename(TEMP_FILE, FILE_NAME) != 0) {
        printf("File replacement failed!\n");
        return;
    }

    printf("\nBook deleted successfully!\n");
}

void dashboard() {
    int choice;

    do {
        system("cls");

        printf(CYAN BOLD);
        printf("\n");
        printf("  =============================================\n");
        printf("              SMART LIBRARY SYSTEM             \n");
        printf("               ADMIN DASHBOARD                 \n");
        printf("  =============================================\n");
        printf(RESET);

        printf(BLUE BOLD);
        printf("\n                 MAIN MENU                     \n");
        printf(RESET);

        printf(CYAN);
        printf("\n  ---------------------------------------------\n");
        printf(RESET);

        printf(GREEN "   [01] " RESET "Add New Book\n");
        printf(GREEN "   [02] " RESET "View All Books\n");
        printf(GREEN "   [03] " RESET "Search Book\n");
        printf(GREEN "   [04] " RESET "Update Book\n");
        printf(GREEN "   [05] " RESET "Delete Book\n");

        printf(CYAN "  ---------------------------------------------\n" RESET);

        printf(GREEN "   [06] " RESET "Add Student\n");
        printf(GREEN "   [07] " RESET "View Students\n");
        printf(GREEN "   [08] " RESET "Search Student\n");
        printf(GREEN "   [09] " RESET "Update Student\n");
        printf(GREEN "   [10] " RESET "Delete Student\n");

        printf(CYAN "  ---------------------------------------------\n" RESET);

        printf(GREEN "   [11] " RESET "Issue Book\n");
        printf(GREEN "   [12] " RESET "Return Book\n");
        printf(GREEN "   [13] " RESET "Fine Details\n");

        printf(RED "   [14] Logout\n" RESET);

        printf(CYAN);
        printf("\n  =============================================\n");
        printf(RESET);

        printf(YELLOW BOLD "\n   Enter your choice: " RESET);

        if (scanf("%d", &choice) != 1) {
            printf(RED "\n   Enter a valid number!\n" RESET);
            clearInput();
            continue;
        }

        clearInput();

        switch (choice) {
            case 1: addBook(); break;
            case 2: viewBooks(); break;
            case 3: searchBook(); break;
            case 4: updateBook(); break;
            case 5: deleteBook(); break;
            case 6: addStudent(); break;
            case 7: viewStudents(); break;
            case 8: searchStudent(); break;
            case 9: updateStudent(); break;
            case 10: deleteStudent(); break;
            case 11: issueBook(); break;
            case 12: returnBook(); break;

            case 13: viewIssueRecords(); break;
            
            case 14:
                printf(GREEN "\n   Logging out...\n" RESET);
                break;

            default:
                printf(RED "\n   Invalid choice!\n" RESET);
        }

        if (choice != 14) {
            printf("\n   Press Enter to continue...");
            getchar();
        }

    } while (choice != 14);
}


int main() {
    char username[30], password[30];

    system("cls");

    printf("\n");
    printf(CYAN BOLD);
    printf("  ========================================\n");
    printf("          SMART LIBRARY SYSTEM            \n");
    printf("           MANAGEMENT PORTAL              \n");
    printf("  ========================================\n");
    printf(RESET);

    printf(BLUE);
    printf("\n              ADMIN LOGIN                 \n");
    printf(RESET);

    printf(CYAN);
    printf("\n  ----------------------------------------\n");
    printf(RESET);

    printf(YELLOW "\n  Enter Username: " RESET);
    scanf("%29s", username);

    printf(YELLOW "  Enter Password: " RESET);
    scanf("%29s", password);

    if (strcmp(username, "admin") == 0 &&
        strcmp(password, "admin123") == 0) {

        printf(GREEN BOLD "\n  Login Successful!" RESET);
        printf(CYAN "\n  Welcome, Admin!\n" RESET);

        dashboard();

    } else {
        printf(RED BOLD "\n  Access Denied! Wrong Login Details.\n" RESET);
    }

    return 0;
}



void addStudent() {
    Student s;
    FILE *fp;
    Student temp;
    int found = 0;

    printf("\n===== ADD NEW STUDENT =====");

    printf("\nEnter Student ID: ");
    scanf("%d", &s.id);
    clearInput();

    fp = fopen("students.dat", "ab+");

    if (fp == NULL) {
        printf("\nFile could not be opened!");
        return;
    }

    rewind(fp);

    while (fread(&temp, sizeof(Student), 1, fp)) {
        if (temp.id == s.id) {
            found = 1;
            break;
        }
    }

    if (found) {
        printf("\nStudent ID already exists!");
        fclose(fp);
        return;
    }

    printf("Enter Student Name: ");
    readText(s.name, sizeof(s.name));

    printf("Enter Course/Class: ");
    readText(s.course, sizeof(s.course));

    printf("Enter Mobile Number: ");
    readText(s.phone, sizeof(s.phone));

    fseek(fp, 0, SEEK_END);
    fwrite(&s, sizeof(Student), 1, fp);

    fclose(fp);

    printf("\nStudent Added Successfully!");
}

 
    
void viewStudents()
{
    Student s;
    FILE *fp;

    fp = fopen("students.dat", "rb");

    if (fp == NULL) {
        printf("\nNo student records found!");
        return;
    }

    printf("\n========== ALL STUDENTS ==========");

    while (fread(&s, sizeof(Student), 1, fp)) {
        printf("\n\nStudent ID: %d", s.id);
        printf("\nName: %s", s.name);
        printf("\nCourse: %s", s.course);
        printf("\nMobile: %s", s.phone);
        printf("\n--------------------------------");
    }

    fclose(fp);
}

void searchStudent()
{
    Student s;
    FILE *fp;
    int searchID;
    int found = 0;

    printf("\n===== SEARCH STUDENT =====");
    printf("\nEnter Student ID: ");
    scanf("%d", &searchID);
    clearInput();

    fp = fopen("students.dat", "rb");

    if (fp == NULL) {
        printf("\nNo student records found!");
        return;
    }

    while (fread(&s, sizeof(Student), 1, fp) == 1) {
        if (s.id == searchID) {
            printf("\nStudent Found!");
            printf("\nStudent ID: %d", s.id);
            printf("\nName: %s", s.name);
            printf("\nCourse: %s", s.course);
            printf("\nMobile: %s", s.phone);
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("\nStudent not found!");
    }

    fclose(fp);
}

void updateStudent()
{
    Student s;
    FILE *fp;
    int searchID;
    int found = 0;

    printf("\n===== UPDATE STUDENT =====");
    printf("\nEnter Student ID: ");
    scanf("%d", &searchID);
    clearInput();

    fp = fopen("students.dat", "rb+");

    if (fp == NULL) {
        printf("\nNo student records found!");
        return;
    }

    while (fread(&s, sizeof(Student), 1, fp) == 1) {
        if (s.id == searchID) {
            found = 1;

            printf("\nStudent Found!");
            printf("\nCurrent Name: %s", s.name);
            printf("\nEnter New Name: ");
            readText(s.name, sizeof(s.name));

            printf("Enter New Course: ");
            readText(s.course, sizeof(s.course));

            printf("Enter New Mobile: ");
            readText(s.phone, sizeof(s.phone));

            fseek(fp, -(long)sizeof(Student), SEEK_CUR);
            fwrite(&s, sizeof(Student), 1, fp);

            printf("\nStudent Updated Successfully!");
            break;
        }
    }

    if (!found) {
        printf("\nStudent not found!");
    }

    fclose(fp);
}

void deleteStudent()
{
    Student s;
    FILE *fp, *temp;
    int searchID;
    int found = 0;
    char confirm = 'N';

    printf("\n===== DELETE STUDENT =====");
    printf("\nEnter Student ID: ");
    scanf("%d", &searchID);
    clearInput();

    fp = fopen("students.dat", "rb");

    if (fp == NULL) {
        printf("\nNo student records found!");
        return;
    }

    temp = fopen("temp.dat", "wb");

    if (temp == NULL) {
        printf("\nUnable to create temporary file!");
        fclose(fp);
        return;
    }

    while (fread(&s, sizeof(Student), 1, fp) == 1) {
        if (s.id == searchID) {
            found = 1;

            printf("\nStudent Found!");
            printf("\nName: %s", s.name);
            printf("\nCourse: %s", s.course);

            printf("\nAre you sure you want to delete? (Y/N): ");
            scanf(" %c", &confirm);
            clearInput();

            if (confirm == 'Y' || confirm == 'y') {
                printf("\nStudent deleted successfully!");
            }
            else {
                fwrite(&s, sizeof(Student), 1, temp);
                printf("\nDeletion cancelled!");
            }
        }
        else {
            fwrite(&s, sizeof(Student), 1, temp);
        }
    }

    fclose(fp);
    fclose(temp);

    if (!found) {
        printf("\nStudent not found!");
    }

    if (remove("students.dat") != 0) {
        printf("\nError updating student records!");
        return;
    }

    if (rename("temp.dat", "students.dat") != 0) {
        printf("\nError saving updated records!");
        return;
    }
}
int studentExists(int id)
{
    Student s;
    FILE *fp = fopen("students.dat", "rb");

    if (fp == NULL)
        return 0;

    while (fread(&s, sizeof(Student), 1, fp) == 1)
    {
        if (s.id == id)
        {
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

void issueBook()
{
    Book b;
    Issue issue;
    FILE *fp;
    FILE *issueFP;
    int studentID, bookID, found = 0;

    printf("\n===== ISSUE BOOK =====");

    printf("\nEnter Student ID: ");
    if (scanf("%d", &studentID) != 1) {
        printf("\nInvalid Student ID!");
        clearInput();
        return;
    }
    clearInput();

    if (!studentExists(studentID)) {
        printf("\nStudent not found!");
        return;
    }

    printf("\nEnter Book ID: ");
    if (scanf("%d", &bookID) != 1) {
        printf("\nInvalid Book ID!");
        clearInput();
        return;
    }
    clearInput();

    fp = fopen(FILE_NAME, "rb+");

    if (fp == NULL) {
        printf("\nNo books available!");
        return;
    }

    while (fread(&b, sizeof(Book), 1, fp) == 1) {
        if (b.id == bookID) {
            found = 1;

            if (b.quantity <= 0) {
                printf("\nBook not available!");
                break;
            }

            issue.studentID = studentID;
            issue.bookID = bookID;
            issue.isReturned = 0;

            time_t now = time(NULL);
            struct tm *date = localtime(&now);

            if (date == NULL) {
                printf("\nDate error!");
                break;
            }

            strftime(issue.issueDate, sizeof(issue.issueDate),
                     "%d-%m-%Y", date);

            issueFP = fopen("issues.dat", "ab");

            if (issueFP == NULL) {
                printf("\nIssue file error!");
                break;
            }

            if (fwrite(&issue, sizeof(Issue), 1, issueFP) != 1) {
                printf("\nIssue record saving failed!");
                fclose(issueFP);
                break;
            }

            fclose(issueFP);

            b.quantity--;

            fseek(fp, -(long)sizeof(Book), SEEK_CUR);

            if (fwrite(&b, sizeof(Book), 1, fp) == 1) {
                printf("\nBook issued successfully!");
                printf("\nIssue Date: %s", issue.issueDate);
                printf("\nRemaining Quantity: %d", b.quantity);
            }
            else {
                printf("\nQuantity update failed!");
            }

            break;
        }
    }

    if (!found) {
        printf("\nBook not found!");
    }

    fclose(fp);
}
void returnBook()
{
    Issue issue;
    Book b;
    FILE *issueFP;
    FILE *bookFP;
    int studentID, bookID, found = 0;

    printf("\n===== RETURN BOOK =====");

    printf("\nEnter Student ID: ");
    if (scanf("%d", &studentID) != 1) {
        printf("\nInvalid Student ID!");
        clearInput();
        return;
    }
    clearInput();

    printf("\nEnter Book ID: ");
    if (scanf("%d", &bookID) != 1) {
        printf("\nInvalid Book ID!");
        clearInput();
        return;
    }
    clearInput();

    issueFP = fopen("issues.dat", "rb+");

    if (issueFP == NULL) {
        printf("\nNo issue records found!");
        return;
    }

    while (fread(&issue, sizeof(Issue), 1, issueFP) == 1) {

        if (issue.studentID == studentID &&
            issue.bookID == bookID &&
            issue.isReturned == 0) {

            found = 1;

            bookFP = fopen(FILE_NAME, "rb+");

            if (bookFP == NULL) {
                printf("\nBook file error!");
                fclose(issueFP);
                return;
            }

            while (fread(&b, sizeof(Book), 1, bookFP) == 1) {

                if (b.id == bookID) {

                    b.quantity++;

                    fseek(bookFP, -(long)sizeof(Book), SEEK_CUR);

                    if (fwrite(&b, sizeof(Book), 1, bookFP) != 1) {
                        printf("\nQuantity update failed!");
                        fclose(bookFP);
                        fclose(issueFP);
                        return;
                    }

                    break;
                }
            }

            fclose(bookFP);

            issue.isReturned = 1;

            fseek(issueFP, -(long)sizeof(Issue), SEEK_CUR);

            if (fwrite(&issue, sizeof(Issue), 1, issueFP) == 1) {
                printf("\nBook returned successfully!");
                printf("\nUpdated Quantity: %d", b.quantity);
            }
            else {
                printf("\nReturn record update failed!");
            }

            break;
        }
    }

    if (!found) {
        printf("\nNo matching active issue record found!");
    }

    fclose(issueFP);
}void viewIssueRecords()
{
    Issue issue;
    FILE *fp;

    fp = fopen("issues.dat", "rb");

    if (fp == NULL) {
        printf("\nNo issue records found!");
        return;
    }

    printf("\n========== ISSUE RECORDS ==========\n");

    printf("\n%-12s %-12s %-15s %-15s\n",
           "STUDENT ID", "BOOK ID", "ISSUE DATE", "STATUS");

    printf("----------------------------------------------------------\n");

    while (fread(&issue, sizeof(Issue), 1, fp) == 1)
    {
        printf("%-12d %-12d %-15s ",
               issue.studentID,
               issue.bookID,
               issue.issueDate);

        if (issue.isReturned == 0)
            printf("%-15s\n", "Pending");
        else
            printf("%-15s\n", "Returned");
    }

    fclose(fp);
}