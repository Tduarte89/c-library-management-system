/* ============================================================================
 *  LIBRARY MANAGEMENT SYSTEM - C LANGUAGE
 *  Console-based relational CRUD with binary file persistence & loan tracking
 *
 *  Author: Thiago A. Duarte
 *
 *  Features:
 *  - Full CRUD operations for Authors, Publishers, Books, and Users
 *  - Loan & Return transaction processing with automatic inventory tracking
 *  - Relational integrity constraints (prevents orphan records)
 *  - Safe input parsing (buffer overflow and non-numeric validation)
 *  - Binary serialization (.dat files) for cross-session state persistence
 * ========================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ----------------------------------------------------------------------
 *  SYSTEM CONSTANTS & STORAGE CONFIGURATION
 * ---------------------------------------------------------------------- */
#define MAX_BOOKS        200
#define MAX_AUTHORS      100
#define MAX_PUBLISHERS   100
#define MAX_USERS        200
#define MAX_LOANS        500

#define FILE_BOOKS       "books.dat"
#define FILE_AUTHORS     "authors.dat"
#define FILE_PUBLISHERS  "publishers.dat"
#define FILE_USERS       "users.dat"
#define FILE_LOANS       "loans.dat"

#define STATUS_BORROWED  0   /* Active loan, item currently checked out */
#define STATUS_RETURNED  1   /* Completed loan, item returned           */

/* ----------------------------------------------------------------------
 *  DATA STRUCTURES (ENTITIES)
 * ---------------------------------------------------------------------- */
struct Book {
    int id;
    char title[100];
    int authorId;
    int publisherId;
    int year;
    int totalCopies;
    int availableCopies;
};

struct Author {
    int id;
    char name[100];
    char nationality[50];
};

struct Publisher {
    int id;
    char name[100];
    char city[50];
};

struct User {
    int id;
    char name[100];
    char documentId[20];
    char phone[20];
};

struct Loan {
    int id;
    int userId;
    int bookId;
    char checkoutDate[11];   /* DD/MM/YYYY + '\0' */
    char returnDate[11];     /* DD/MM/YYYY + '\0' */
    int status;              /* STATUS_BORROWED or STATUS_RETURNED */
};

/* ----------------------------------------------------------------------
 *  GLOBAL IN-MEMORY STATE
 * ---------------------------------------------------------------------- */
struct Book books[MAX_BOOKS];
int totalBooks = 0;

struct Author authors[MAX_AUTHORS];
int totalAuthors = 0;

struct Publisher publishers[MAX_PUBLISHERS];
int totalPublishers = 0;

struct User users[MAX_USERS];
int totalUsers = 0;

struct Loan loans[MAX_LOANS];
int totalLoans = 0;

/* ----------------------------------------------------------------------
 *  FUNCTION PROTOTYPES
 * ---------------------------------------------------------------------- */
/* Utility & Input Sanitization */
void readLine(char *dest, int size);
int readInt(const char *prompt);
void clearScreen(void);
void pauseScreen(void);
void getCurrentDate(char *dest);

/* Search / Index Lookup (returns array index or -1 if not found) */
int findAuthorById(int id);
int findPublisherById(int id);
int findBookById(int id);
int findUserById(int id);

/* Display Tables */
void displayAuthors(void);
void displayPublishers(void);
void displayBooks(void);
void displayUsers(void);
void displayLoans(void);

/* CRUD Operations - Author */
void createAuthor(void);
void listAuthors(void);
void updateAuthor(void);
void deleteAuthor(void);

/* CRUD Operations - Publisher */
void createPublisher(void);
void listPublishers(void);
void updatePublisher(void);
void deletePublisher(void);

/* CRUD Operations - Book */
void createBook(void);
void listBooks(void);
void updateBook(void);
void deleteBook(void);

/* CRUD Operations - User */
void createUser(void);
void listUsers(void);
void updateUser(void);
void deleteUser(void);

/* Loan & Return Management */
void registerLoan(void);
void registerReturn(void);
void listLoans(void);

/* Persistence (Binary File I/O) */
void saveAuthors(void);
void loadAuthors(void);
void savePublishers(void);
void loadPublishers(void);
void saveBooks(void);
void loadBooks(void);
void saveUsers(void);
void loadUsers(void);
void saveLoans(void);
void loadLoans(void);
void loadAllData(void);

/* Navigation Menus */
void mainMenu(void);
void booksMenu(void);
void authorsMenu(void);
void publishersMenu(void);
void usersMenu(void);
void loansMenu(void);

/* ============================================================================
 *  MAIN ENTRY POINT
 * ========================================================================== */
int main(void) {
    loadAllData();
    clearScreen();
    printf("======================================================\n");
    printf("     WELCOME TO THE LIBRARY MANAGEMENT SYSTEM         \n");
    printf("======================================================\n\n");
    printf("Loaded records from persistent storage:\n");
    printf("  - %d book(s)\n", totalBooks);
    printf("  - %d author(s)\n", totalAuthors);
    printf("  - %d publisher(s)\n", totalPublishers);
    printf("  - %d user(s)\n", totalUsers);
    printf("  - %d loan transaction(s)\n", totalLoans);
    pauseScreen();

    mainMenu();

    printf("\nGoodbye! All records successfully saved.\n");
    return 0;
}

/* ============================================================================
 *  UTILITY & INPUT SANITIZATION
 * ========================================================================== */
void readLine(char *dest, int size) {
    if (fgets(dest, size, stdin) == NULL) {
        dest[0] = '\0';
        return;
    }
    int len = (int)strlen(dest);
    if (len > 0 && dest[len - 1] == '\n') {
        dest[len - 1] = '\0';
    } else if (len == size - 1) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {
            /* flush excess stream bytes */
        }
    }
}

int readInt(const char *prompt) {
    char buffer[64];
    char *endptr;
    long val;

    while (1) {
        printf("%s", prompt);
        readLine(buffer, sizeof(buffer));
        val = strtol(buffer, &endptr, 10);
        if (endptr != buffer && *endptr == '\0') {
            return (int)val;
        }
        printf("  >> Invalid input. Please enter numbers only.\n");
    }
}

void clearScreen(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pauseScreen(void) {
    printf("\nPress ENTER to continue...");
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        /* flush */
    }
}

void getCurrentDate(char *dest) {
    time_t now = time(NULL);
    struct tm *local = localtime(&now);
    strftime(dest, 11, "%d/%m/%Y", local);
}

/* ============================================================================
 *  SEARCH FUNCTIONS
 * ========================================================================== */
int findAuthorById(int id) {
    for (int i = 0; i < totalAuthors; i++) {
        if (authors[i].id == id) return i;
    }
    return -1;
}

int findPublisherById(int id) {
    for (int i = 0; i < totalPublishers; i++) {
        if (publishers[i].id == id) return i;
    }
    return -1;
}

int findBookById(int id) {
    for (int i = 0; i < totalBooks; i++) {
        if (books[i].id == id) return i;
    }
    return -1;
}

int findUserById(int id) {
    for (int i = 0; i < totalUsers; i++) {
        if (users[i].id == id) return i;
    }
    return -1;
}

/* ============================================================================
 *  DISPLAY TABLES
 * ========================================================================== */
void displayAuthors(void) {
    if (totalAuthors == 0) {
        printf("No authors registered.\n");
        return;
    }
    printf("%-6s %-30s %-20s\n", "ID", "Name", "Nationality");
    printf("---------------------------------------------------------\n");
    for (int i = 0; i < totalAuthors; i++) {
        printf("%-6d %-30s %-20s\n", authors[i].id, authors[i].name, authors[i].nationality);
    }
}

void displayPublishers(void) {
    if (totalPublishers == 0) {
        printf("No publishers registered.\n");
        return;
    }
    printf("%-6s %-30s %-20s\n", "ID", "Name", "City");
    printf("---------------------------------------------------------\n");
    for (int i = 0; i < totalPublishers; i++) {
        printf("%-6d %-30s %-20s\n", publishers[i].id, publishers[i].name, publishers[i].city);
    }
}

void displayBooks(void) {
    if (totalBooks == 0) {
        printf("No books registered.\n");
        return;
    }
    printf("%-6s %-30s %-6s %-6s %-6s\n", "ID", "Title", "Year", "Total", "Avail");
    printf("-------------------------------------------------------------\n");
    for (int i = 0; i < totalBooks; i++) {
        printf("%-6d %-30s %-6d %-6d %-6d\n",
               books[i].id, books[i].title, books[i].year,
               books[i].totalCopies, books[i].availableCopies);
    }
}

void displayUsers(void) {
    if (totalUsers == 0) {
        printf("No users registered.\n");
        return;
    }
    printf("%-6s %-30s %-16s %-16s\n", "ID", "Name", "Document/ID", "Phone");
    printf("-------------------------------------------------------------------\n");
    for (int i = 0; i < totalUsers; i++) {
        printf("%-6d %-30s %-16s %-16s\n",
               users[i].id, users[i].name, users[i].documentId, users[i].phone);
    }
}

void displayLoans(void) {
    if (totalLoans == 0) {
        printf("No loan transactions found.\n");
        return;
    }
    printf("%-6s %-20s %-20s %-12s %-12s %-10s\n",
           "ID", "User", "Book", "Checkout", "Return", "Status");
    printf("---------------------------------------------------------------------------------\n");
    for (int i = 0; i < totalLoans; i++) {
        int posU = findUserById(loans[i].userId);
        int posB = findBookById(loans[i].bookId);

        const char *userName = (posU != -1) ? users[posU].name : "(Deleted)";
        const char *bookTitle = (posB != -1) ? books[posB].title : "(Deleted)";
        const char *statusLabel = (loans[i].status == STATUS_BORROWED) ? "Borrowed" : "Returned";
        const char *returnDate = (strlen(loans[i].returnDate) > 0) ? loans[i].returnDate : "--";

        printf("%-6d %-20s %-20s %-12s %-12s %-10s\n",
               loans[i].id, userName, bookTitle,
               loans[i].checkoutDate, returnDate, statusLabel);
    }
}

/* ============================================================================
 *  CRUD - AUTHOR
 * ========================================================================== */
void createAuthor(void) {
    clearScreen();
    printf("===== REGISTER AUTHOR =====\n\n");
    if (totalAuthors >= MAX_AUTHORS) {
        printf("Maximum author limit reached!\n");
        pauseScreen();
        return;
    }

    struct Author newAuthor;
    int maxId = 0;
    for (int i = 0; i < totalAuthors; i++) {
        if (authors[i].id > maxId) maxId = authors[i].id;
    }
    newAuthor.id = maxId + 1;

    printf("Generated ID: %d\n\n", newAuthor.id);
    printf("Author Name: ");
    readLine(newAuthor.name, sizeof(newAuthor.name));
    printf("Nationality: ");
    readLine(newAuthor.nationality, sizeof(newAuthor.nationality));

    authors[totalAuthors] = newAuthor;
    totalAuthors++;
    saveAuthors();

    printf("\nAuthor registered successfully!\n");
    pauseScreen();
}

void listAuthors(void) {
    clearScreen();
    printf("===== AUTHORS LIST =====\n\n");
    displayAuthors();
    pauseScreen();
}

void updateAuthor(void) {
    clearScreen();
    printf("===== UPDATE AUTHOR =====\n\n");
    if (totalAuthors == 0) {
        printf("No authors registered.\n");
        pauseScreen();
        return;
    }

    displayAuthors();
    int id = readInt("\nEnter Author ID to update (0 to cancel): ");
    if (id == 0) return;

    int pos = findAuthorById(id);
    if (pos == -1) {
        printf("\nAuthor not found!\n");
        pauseScreen();
        return;
    }

    printf("\nCurrent details -> Name: %s | Nationality: %s\n",
           authors[pos].name, authors[pos].nationality);

    printf("New Name: ");
    readLine(authors[pos].name, sizeof(authors[pos].name));
    printf("New Nationality: ");
    readLine(authors[pos].nationality, sizeof(authors[pos].nationality));

    saveAuthors();
    printf("\nAuthor updated successfully!\n");
    pauseScreen();
}

void deleteAuthor(void) {
    clearScreen();
    printf("===== DELETE AUTHOR =====\n\n");
    if (totalAuthors == 0) {
        printf("No authors registered.\n");
        pauseScreen();
        return;
    }

    displayAuthors();
    int id = readInt("\nEnter Author ID to delete (0 to cancel): ");
    if (id == 0) return;

    int pos = findAuthorById(id);
    if (pos == -1) {
        printf("\nAuthor not found!\n");
        pauseScreen();
        return;
    }

    /* Integrity check: verify if any book is referencing this author */
    for (int i = 0; i < totalBooks; i++) {
        if (books[i].authorId == id) {
            printf("\nCannot delete: One or more books are associated with this author.\n");
            pauseScreen();
            return;
        }
    }

    for (int i = pos; i < totalAuthors - 1; i++) {
        authors[i] = authors[i + 1];
    }
    totalAuthors--;
    saveAuthors();

    printf("\nAuthor deleted successfully!\n");
    pauseScreen();
}

/* ============================================================================
 *  CRUD - PUBLISHER
 * ========================================================================== */
void createPublisher(void) {
    clearScreen();
    printf("===== REGISTER PUBLISHER =====\n\n");
    if (totalPublishers >= MAX_PUBLISHERS) {
        printf("Maximum publisher limit reached!\n");
        pauseScreen();
        return;
    }

    struct Publisher newPub;
    int maxId = 0;
    for (int i = 0; i < totalPublishers; i++) {
        if (publishers[i].id > maxId) maxId = publishers[i].id;
    }
    newPub.id = maxId + 1;

    printf("Generated ID: %d\n\n", newPub.id);
    printf("Publisher Name: ");
    readLine(newPub.name, sizeof(newPub.name));
    printf("City: ");
    readLine(newPub.city, sizeof(newPub.city));

    publishers[totalPublishers] = newPub;
    totalPublishers++;
    savePublishers();

    printf("\nPublisher registered successfully!\n");
    pauseScreen();
}

void listPublishers(void) {
    clearScreen();
    printf("===== PUBLISHERS LIST =====\n\n");
    displayPublishers();
    pauseScreen();
}

void updatePublisher(void) {
    clearScreen();
    printf("===== UPDATE PUBLISHER =====\n\n");
    if (totalPublishers == 0) {
        printf("No publishers registered.\n");
        pauseScreen();
        return;
    }

    displayPublishers();
    int id = readInt("\nEnter Publisher ID to update (0 to cancel): ");
    if (id == 0) return;

    int pos = findPublisherById(id);
    if (pos == -1) {
        printf("\nPublisher not found!\n");
        pauseScreen();
        return;
    }

    printf("\nCurrent details -> Name: %s | City: %s\n",
           publishers[pos].name, publishers[pos].city);

    printf("New Name: ");
    readLine(publishers[pos].name, sizeof(publishers[pos].name));
    printf("New City: ");
    readLine(publishers[pos].city, sizeof(publishers[pos].city));

    savePublishers();
    printf("\nPublisher updated successfully!\n");
    pauseScreen();
}

void deletePublisher(void) {
    clearScreen();
    printf("===== DELETE PUBLISHER =====\n\n");
    if (totalPublishers == 0) {
        printf("No publishers registered.\n");
        pauseScreen();
        return;
    }

    displayPublishers();
    int id = readInt("\nEnter Publisher ID to delete (0 to cancel): ");
    if (id == 0) return;

    int pos = findPublisherById(id);
    if (pos == -1) {
        printf("\nPublisher not found!\n");
        pauseScreen();
        return;
    }

    /* Integrity check */
    for (int i = 0; i < totalBooks; i++) {
        if (books[i].publisherId == id) {
            printf("\nCannot delete: One or more books are associated with this publisher.\n");
            pauseScreen();
            return;
        }
    }

    for (int i = pos; i < totalPublishers - 1; i++) {
        publishers[i] = publishers[i + 1];
    }
    totalPublishers--;
    savePublishers();

    printf("\nPublisher deleted successfully!\n");
    pauseScreen();
}

/* ============================================================================
 *  CRUD - BOOK
 * ========================================================================== */
void createBook(void) {
    clearScreen();
    printf("===== REGISTER BOOK =====\n\n");
    if (totalBooks >= MAX_BOOKS) {
        printf("Maximum book capacity reached!\n");
        pauseScreen();
        return;
    }
    if (totalAuthors == 0) {
        printf("Please register at least one author before creating a book.\n");
        pauseScreen();
        return;
    }
    if (totalPublishers == 0) {
        printf("Please register at least one publisher before creating a book.\n");
        pauseScreen();
        return;
    }

    struct Book newBook;
    int maxId = 0;
    for (int i = 0; i < totalBooks; i++) {
        if (books[i].id > maxId) maxId = books[i].id;
    }
    newBook.id = maxId + 1;

    printf("Generated ID: %d\n\n", newBook.id);
    printf("Book Title: ");
    readLine(newBook.title, sizeof(newBook.title));

    printf("\nAvailable Authors:\n");
    displayAuthors();
    int aId;
    while (1) {
        aId = readInt("\nAuthor ID for this book (0 to cancel): ");
        if (aId == 0) {
            printf("\nRegistration cancelled.\n");
            pauseScreen();
            return;
        }
        if (findAuthorById(aId) != -1) break;
        printf("  >> Invalid Author ID, please try again.\n");
    }
    newBook.authorId = aId;

    printf("\nAvailable Publishers:\n");
    displayPublishers();
    int pId;
    while (1) {
        pId = readInt("\nPublisher ID for this book (0 to cancel): ");
        if (pId == 0) {
            printf("\nRegistration cancelled.\n");
            pauseScreen();
            return;
        }
        if (findPublisherById(pId) != -1) break;
        printf("  >> Invalid Publisher ID, please try again.\n");
    }
    newBook.publisherId = pId;

    newBook.year = readInt("Publication Year: ");
    int qty;
    do {
        qty = readInt("Total Copies: ");
        if (qty <= 0) printf("  >> Total copies must be greater than zero.\n");
    } while (qty <= 0);

    newBook.totalCopies = qty;
    newBook.availableCopies = qty;

    books[totalBooks] = newBook;
    totalBooks++;
    saveBooks();

    printf("\nBook registered successfully!\n");
    pauseScreen();
}

void listBooks(void) {
    clearScreen();
    printf("===== BOOKS LIST =====\n\n");
    displayBooks();
    if (totalBooks == 0) {
        pauseScreen();
        return;
    }

    int id = readInt("\nEnter Book ID to inspect details (0 to return): ");
    if (id == 0) return;

    int pos = findBookById(id);
    if (pos == -1) {
        printf("\nBook not found!\n");
    } else {
        int posAuthor = findAuthorById(books[pos].authorId);
        int posPub = findPublisherById(books[pos].publisherId);

        printf("\n----- BOOK DETAILS -----\n");
        printf("ID............: %d\n", books[pos].id);
        printf("Title.........: %s\n", books[pos].title);
        printf("Author........: %s\n", (posAuthor != -1) ? authors[posAuthor].name : "(Unknown)");
        printf("Publisher.....: %s\n", (posPub != -1) ? publishers[posPub].name : "(Unknown)");
        printf("Year..........: %d\n", books[pos].year);
        printf("Total Copies..: %d (Available: %d)\n",
               books[pos].totalCopies, books[pos].availableCopies);
    }
    pauseScreen();
}

void updateBook(void) {
    clearScreen();
    printf("===== UPDATE BOOK =====\n\n");
    if (totalBooks == 0) {
        printf("No books registered.\n");
        pauseScreen();
        return;
    }

    displayBooks();
    int id = readInt("\nEnter Book ID to update (0 to cancel): ");
    if (id == 0) return;

    int pos = findBookById(id);
    if (pos == -1) {
        printf("\nBook not found!\n");
        pauseScreen();
        return;
    }

    printf("\nCurrent Title: %s\n", books[pos].title);
    printf("New Title: ");
    readLine(books[pos].title, sizeof(books[pos].title));

    printf("\nCurrent Year: %d\n", books[pos].year);
    books[pos].year = readInt("New Year: ");

    printf("\nCurrent Copies: %d (Available: %d)\n",
           books[pos].totalCopies, books[pos].availableCopies);

    int newQty;
    do {
        newQty = readInt("New Total Copies: ");
        if (newQty <= 0) printf("  >> Must be greater than zero.\n");
    } while (newQty <= 0);

    int diff = newQty - books[pos].totalCopies;
    books[pos].availableCopies += diff;
    if (books[pos].availableCopies < 0) books[pos].availableCopies = 0;
    if (books[pos].availableCopies > newQty) books[pos].availableCopies = newQty;
    books[pos].totalCopies = newQty;

    saveBooks();
    printf("\nBook updated successfully!\n");
    pauseScreen();
}

void deleteBook(void) {
    clearScreen();
    printf("===== DELETE BOOK =====\n\n");
    if (totalBooks == 0) {
        printf("No books registered.\n");
        pauseScreen();
        return;
    }

    displayBooks();
    int id = readInt("\nEnter Book ID to delete (0 to cancel): ");
    if (id == 0) return;

    int pos = findBookById(id);
    if (pos == -1) {
        printf("\nBook not found!\n");
        pauseScreen();
        return;
    }

    /* Integrity check: cannot delete book with active loans */
    for (int i = 0; i < totalLoans; i++) {
        if (loans[i].bookId == id && loans[i].status == STATUS_BORROWED) {
            printf("\nCannot delete: This book currently has active loans checked out.\n");
            pauseScreen();
            return;
        }
    }

    for (int i = pos; i < totalBooks - 1; i++) {
        books[i] = books[i + 1];
    }
    totalBooks--;
    saveBooks();

    printf("\nBook deleted successfully!\n");
    pauseScreen();
}

/* ============================================================================
 *  CRUD - USER
 * ========================================================================== */
void createUser(void) {
    clearScreen();
    printf("===== REGISTER USER =====\n\n");
    if (totalUsers >= MAX_USERS) {
        printf("Maximum user limit reached!\n");
        pauseScreen();
        return;
    }

    struct User newUser;
    int maxId = 0;
    for (int i = 0; i < totalUsers; i++) {
        if (users[i].id > maxId) maxId = users[i].id;
    }
    newUser.id = maxId + 1;

    printf("Generated ID: %d\n\n", newUser.id);
    printf("User Name: ");
    readLine(newUser.name, sizeof(newUser.name));
    printf("Document / ID: ");
    readLine(newUser.documentId, sizeof(newUser.documentId));
    printf("Phone: ");
    readLine(newUser.phone, sizeof(newUser.phone));

    users[totalUsers] = newUser;
    totalUsers++;
    saveUsers();

    printf("\nUser registered successfully!\n");
    pauseScreen();
}

void listUsers(void) {
    clearScreen();
    printf("===== USERS LIST =====\n\n");
    displayUsers();
    pauseScreen();
}

void updateUser(void) {
    clearScreen();
    printf("===== UPDATE USER =====\n\n");
    if (totalUsers == 0) {
        printf("No users registered.\n");
        pauseScreen();
        return;
    }

    displayUsers();
    int id = readInt("\nEnter User ID to update (0 to cancel): ");
    if (id == 0) return;

    int pos = findUserById(id);
    if (pos == -1) {
        printf("\nUser not found!\n");
        pauseScreen();
        return;
    }

    printf("\nCurrent details -> Name: %s | Document: %s | Phone: %s\n",
           users[pos].name, users[pos].documentId, users[pos].phone);

    printf("New Name: ");
    readLine(users[pos].name, sizeof(users[pos].name));
    printf("New Document: ");
    readLine(users[pos].documentId, sizeof(users[pos].documentId));
    printf("New Phone: ");
    readLine(users[pos].phone, sizeof(users[pos].phone));

    saveUsers();
    printf("\nUser updated successfully!\n");
    pauseScreen();
}

void deleteUser(void) {
    clearScreen();
    printf("===== DELETE USER =====\n\n");
    if (totalUsers == 0) {
        printf("No users registered.\n");
        pauseScreen();
        return;
    }

    displayUsers();
    int id = readInt("\nEnter User ID to delete (0 to cancel): ");
    if (id == 0) return;

    int pos = findUserById(id);
    if (pos == -1) {
        printf("\nUser not found!\n");
        pauseScreen();
        return;
    }

    /* Integrity check */
    for (int i = 0; i < totalLoans; i++) {
        if (loans[i].userId == id && loans[i].status == STATUS_BORROWED) {
            printf("\nCannot delete: This user currently has active loans pending return.\n");
            pauseScreen();
            return;
        }
    }

    for (int i = pos; i < totalUsers - 1; i++) {
        users[i] = users[i + 1];
    }
    totalUsers--;
    saveUsers();

    printf("\nUser deleted successfully!\n");
    pauseScreen();
}

/* ============================================================================
 *  LOAN & RETURN MANAGEMENT
 * ========================================================================== */
void registerLoan(void) {
    clearScreen();
    printf("===== REGISTER LOAN =====\n\n");
    if (totalLoans >= MAX_LOANS) {
        printf("Maximum loan capacity reached!\n");
        pauseScreen();
        return;
    }
    if (totalUsers == 0) {
        printf("No registered users available.\n");
        pauseScreen();
        return;
    }
    if (totalBooks == 0) {
        printf("No registered books available.\n");
        pauseScreen();
        return;
    }

    printf("Registered Users:\n");
    displayUsers();
    int uId;
    while (1) {
        uId = readInt("\nUser ID (0 to cancel): ");
        if (uId == 0) {
            printf("\nOperation cancelled.\n");
            pauseScreen();
            return;
        }
        if (findUserById(uId) != -1) break;
        printf("  >> User not found, please try again.\n");
    }

    printf("\nRegistered Books:\n");
    displayBooks();
    int bId, posBook = -1;
    while (1) {
        bId = readInt("\nBook ID (0 to cancel): ");
        if (bId == 0) {
            printf("\nOperation cancelled.\n");
            pauseScreen();
            return;
        }
        posBook = findBookById(bId);
        if (posBook == -1) {
            printf("  >> Book not found, please try again.\n");
            continue;
        }
        if (books[posBook].availableCopies <= 0) {
            printf("  >> No copies available for this book at the moment.\n");
            continue;
        }
        break;
    }

    struct Loan newLoan;
    int maxId = 0;
    for (int i = 0; i < totalLoans; i++) {
        if (loans[i].id > maxId) maxId = loans[i].id;
    }
    newLoan.id = maxId + 1;
    newLoan.userId = uId;
    newLoan.bookId = bId;
    getCurrentDate(newLoan.checkoutDate);
    newLoan.returnDate[0] = '\0';
    newLoan.status = STATUS_BORROWED;

    loans[totalLoans] = newLoan;
    totalLoans++;
    books[posBook].availableCopies--;

    saveLoans();
    saveBooks();

    printf("\nLoan #%d registered successfully on %s!\n", newLoan.id, newLoan.checkoutDate);
    pauseScreen();
}

void registerReturn(void) {
    clearScreen();
    printf("===== REGISTER RETURN =====\n\n");
    if (totalLoans == 0) {
        printf("No loan transactions recorded.\n");
        pauseScreen();
        return;
    }

    printf("Active Loans Pending Return:\n\n");
    int activeCount = 0;
    for (int i = 0; i < totalLoans; i++) {
        if (loans[i].status == STATUS_BORROWED) {
            int posBook = findBookById(loans[i].bookId);
            printf("Loan ID: %-4d | Book: %-30s | Date: %s\n",
                   loans[i].id,
                   (posBook != -1) ? books[posBook].title : "(Deleted)",
                   loans[i].checkoutDate);
            activeCount++;
        }
    }

    if (activeCount == 0) {
        printf("No active loans pending return at this time.\n");
        pauseScreen();
        return;
    }

    int loanId = readInt("\nEnter Loan ID to return (0 to cancel): ");
    if (loanId == 0) return;

    int pos = -1;
    for (int i = 0; i < totalLoans; i++) {
        if (loans[i].id == loanId) {
            pos = i;
            break;
        }
    }

    if (pos == -1) {
        printf("\nLoan record not found!\n");
        pauseScreen();
        return;
    }
    if (loans[pos].status == STATUS_RETURNED) {
        printf("\nThis loan has already been marked as returned.\n");
        pauseScreen();
        return;
    }

    getCurrentDate(loans[pos].returnDate);
    loans[pos].status = STATUS_RETURNED;

    int posBook = findBookById(loans[pos].bookId);
    if (posBook != -1) {
        books[posBook].availableCopies++;
        if (books[posBook].availableCopies > books[posBook].totalCopies) {
            books[posBook].availableCopies = books[posBook].totalCopies;
        }
    }

    saveLoans();
    saveBooks();

    printf("\nReturn registered successfully on %s!\n", loans[pos].returnDate);
    pauseScreen();
}

void listLoans(void) {
    clearScreen();
    printf("===== LOAN HISTORY =====\n\n");
    displayLoans();
    pauseScreen();
}

/* ============================================================================
 *  PERSISTENCE (BINARY FILE I/O)
 * ========================================================================== */
void saveAuthors(void) {
    FILE *f = fopen(FILE_AUTHORS, "wb");
    if (!f) return;
    fwrite(&totalAuthors, sizeof(int), 1, f);
    fwrite(authors, sizeof(struct Author), totalAuthors, f);
    fclose(f);
}

void loadAuthors(void) {
    FILE *f = fopen(FILE_AUTHORS, "rb");
    if (!f) { totalAuthors = 0; return; }
    fread(&totalAuthors, sizeof(int), 1, f);
    if (totalAuthors < 0 || totalAuthors > MAX_AUTHORS) totalAuthors = 0;
    else fread(authors, sizeof(struct Author), totalAuthors, f);
    fclose(f);
}

void savePublishers(void) {
    FILE *f = fopen(FILE_PUBLISHERS, "wb");
    if (!f) return;
    fwrite(&totalPublishers, sizeof(int), 1, f);
    fwrite(publishers, sizeof(struct Publisher), totalPublishers, f);
    fclose(f);
}

void loadPublishers(void) {
    FILE *f = fopen(FILE_PUBLISHERS, "rb");
    if (!f) { totalPublishers = 0; return; }
    fread(&totalPublishers, sizeof(int), 1, f);
    if (totalPublishers < 0 || totalPublishers > MAX_PUBLISHERS) totalPublishers = 0;
    else fread(publishers, sizeof(struct Publisher), totalPublishers, f);
    fclose(f);
}

void saveBooks(void) {
    FILE *f = fopen(FILE_BOOKS, "wb");
    if (!f) return;
    fwrite(&totalBooks, sizeof(int), 1, f);
    fwrite(books, sizeof(struct Book), totalBooks, f);
    fclose(f);
}

void loadBooks(void) {
    FILE *f = fopen(FILE_BOOKS, "rb");
    if (!f) { totalBooks = 0; return; }
    fread(&totalBooks, sizeof(int), 1, f);
    if (totalBooks < 0 || totalBooks > MAX_BOOKS) totalBooks = 0;
    else fread(books, sizeof(struct Book), totalBooks, f);
    fclose(f);
}

void saveUsers(void) {
    FILE *f = fopen(FILE_USERS, "wb");
    if (!f) return;
    fwrite(&totalUsers, sizeof(int), 1, f);
    fwrite(users, sizeof(struct User), totalUsers, f);
    fclose(f);
}

void loadUsers(void) {
    FILE *f = fopen(FILE_USERS, "rb");
    if (!f) { totalUsers = 0; return; }
    fread(&totalUsers, sizeof(int), 1, f);
    if (totalUsers < 0 || totalUsers > MAX_USERS) totalUsers = 0;
    else fread(users, sizeof(struct User), totalUsers, f);
    fclose(f);
}

void saveLoans(void) {
    FILE *f = fopen(FILE_LOANS, "wb");
    if (!f) return;
    fwrite(&totalLoans, sizeof(int), 1, f);
    fwrite(loans, sizeof(struct Loan), totalLoans, f);
    fclose(f);
}

void loadLoans(void) {
    FILE *f = fopen(FILE_LOANS, "rb");
    if (!f) { totalLoans = 0; return; }
    fread(&totalLoans, sizeof(int), 1, f);
    if (totalLoans < 0 || totalLoans > MAX_LOANS) totalLoans = 0;
    else fread(loans, sizeof(struct Loan), totalLoans, f);
    fclose(f);
}

void loadAllData(void) {
    loadAuthors();
    loadPublishers();
    loadBooks();
    loadUsers();
    loadLoans();
}

/* ============================================================================
 *  NAVIGATION MENUS
 * ========================================================================== */
void mainMenu(void) {
    int opt;
    do {
        clearScreen();
        printf("======================================================\n");
        printf("              LIBRARY MANAGEMENT SYSTEM               \n");
        printf("======================================================\n\n");
        printf("  1 - Manage Books\n");
        printf("  2 - Manage Authors\n");
        printf("  3 - Manage Publishers\n");
        printf("  4 - Manage Users\n");
        printf("  5 - Loan & Return Transactions\n");
        printf("  0 - Exit System\n");
        printf("------------------------------------------------------\n");
        opt = readInt("Select an option: ");

        switch (opt) {
            case 1: booksMenu(); break;
            case 2: authorsMenu(); break;
            case 3: publishersMenu(); break;
            case 4: usersMenu(); break;
            case 5: loansMenu(); break;
            case 0: break;
            default:
                printf("\nInvalid option!\n");
                pauseScreen();
        }
    } while (opt != 0);
}

void booksMenu(void) {
    int opt;
    do {
        clearScreen();
        printf("===== MANAGE BOOKS =====\n\n");
        printf("  1 - Register Book\n");
        printf("  2 - List Books / Inspect Details\n");
        printf("  3 - Update Book\n");
        printf("  4 - Delete Book\n");
        printf("  0 - Return to Main Menu\n");
        printf("------------------------------------------------------\n");
        opt = readInt("Select an option: ");

        switch (opt) {
            case 1: createBook(); break;
            case 2: listBooks(); break;
            case 3: updateBook(); break;
            case 4: deleteBook(); break;
            case 0: break;
            default:
                printf("\nInvalid option!\n");
                pauseScreen();
        }
    } while (opt != 0);
}

void authorsMenu(void) {
    int opt;
    do {
        clearScreen();
        printf("===== MANAGE AUTHORS =====\n\n");
        printf("  1 - Register Author\n");
        printf("  2 - List Authors\n");
        printf("  3 - Update Author\n");
        printf("  4 - Delete Author\n");
        printf("  0 - Return to Main Menu\n");
        printf("------------------------------------------------------\n");
        opt = readInt("Select an option: ");

        switch (opt) {
            case 1: createAuthor(); break;
            case 2: listAuthors(); break;
            case 3: updateAuthor(); break;
            case 4: deleteAuthor(); break;
            case 0: break;
            default:
                printf("\nInvalid option!\n");
                pauseScreen();
        }
    } while (opt != 0);
}

void publishersMenu(void) {
    int opt;
    do {
        clearScreen();
        printf("===== MANAGE PUBLISHERS =====\n\n");
        printf("  1 - Register Publisher\n");
        printf("  2 - List Publishers\n");
        printf("  3 - Update Publisher\n");
        printf("  4 - Delete Publisher\n");
        printf("  0 - Return to Main Menu\n");
        printf("------------------------------------------------------\n");
        opt = readInt("Select an option: ");

        switch (opt) {
            case 1: createPublisher(); break;
            case 2: listPublishers(); break;
            case 3: updatePublisher(); break;
            case 4: deletePublisher(); break;
            case 0: break;
            default:
                printf("\nInvalid option!\n");
                pauseScreen();
        }
    } while (opt != 0);
}

void usersMenu(void) {
    int opt;
    do {
        clearScreen();
        printf("===== MANAGE USERS =====\n\n");
        printf("  1 - Register User\n");
        printf("  2 - List Users\n");
        printf("  3 - Update User\n");
        printf("  4 - Delete User\n");
        printf("  0 - Return to Main Menu\n");
        printf("------------------------------------------------------\n");
        opt = readInt("Select an option: ");

        switch (opt) {
            case 1: createUser(); break;
            case 2: listUsers(); break;
            case 3: updateUser(); break;
            case 4: deleteUser(); break;
            case 0: break;
            default:
                printf("\nInvalid option!\n");
                pauseScreen();
        }
    } while (opt != 0);
}

void loansMenu(void) {
    int opt;
    do {
        clearScreen();
        printf("===== LOAN & RETURN TRANSACTIONS =====\n\n");
        printf("  1 - Register Loan (Check Out)\n");
        printf("  2 - Register Return (Check In)\n");
        printf("  3 - View Loan History\n");
        printf("  0 - Return to Main Menu\n");
        printf("------------------------------------------------------\n");
        opt = readInt("Select an option: ");

        switch (opt) {
            case 1: registerLoan(); break;
            case 2: registerReturn(); break;
            case 3: listLoans(); break;
            case 0: break;
            default:
                printf("\nInvalid option!\n");
                pauseScreen();
        }
    } while (opt != 0);
}
