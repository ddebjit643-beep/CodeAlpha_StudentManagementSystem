/*
 * CodeAlpha C Programming Internship - Task 3
 * Student Management System
 * Features: Add, Delete, Update, Search, Display
 * Uses structures + file handling (binary file) for permanent storage.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_NAME "students.dat"
#define TEMP_FILE "temp.dat"
#define NAME_LEN  50

typedef struct {
    int   roll;
    char  name[NAME_LEN];
    float marks;
} Student;

/* ---------- Input helpers ---------- */

void clearInput(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int readInt(const char *prompt) {
    int value;
    while (1) {
        printf("%s", prompt);
        int r = scanf("%d", &value);
        if (r == EOF) exit(0);
        clearInput();
        if (r == 1) return value;
        printf("Invalid input. Please enter a whole number.\n");
    }
}

float readFloat(const char *prompt) {
    float value;
    while (1) {
        printf("%s", prompt);
        int r = scanf("%f", &value);
        if (r == EOF) exit(0);
        clearInput();
        if (r == 1) return value;
        printf("Invalid input. Please enter a number.\n");
    }
}

void readName(char *buf, int size) {
    while (1) {
        printf("Enter name: ");
        if (fgets(buf, size, stdin) == NULL) exit(0);
        buf[strcspn(buf, "\n")] = '\0';
        if (strlen(buf) > 0) return;
        printf("Name cannot be empty.\n");
    }
}

/* ---------- File helpers ---------- */

int rollExists(int roll) {
    FILE *fp = fopen(FILE_NAME, "rb");
    Student s;
    if (fp == NULL) return 0;
    while (fread(&s, sizeof(Student), 1, fp) == 1) {
        if (s.roll == roll) {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

void printHeader(void) {
    printf("\n%-10s %-30s %-10s\n", "Roll No", "Name", "Marks");
    printf("------------------------------------------------------\n");
}

void printStudent(const Student *s) {
    printf("%-10d %-30s %-10.2f\n", s->roll, s->name, s->marks);
}

/* ---------- Features ---------- */

void addStudent(void) {
    Student s;
    FILE *fp;

    s.roll = readInt("Enter roll number: ");
    if (rollExists(s.roll)) {
        printf("A student with roll number %d already exists.\n", s.roll);
        return;
    }
    readName(s.name, NAME_LEN);
    do {
        s.marks = readFloat("Enter marks (0-100): ");
        if (s.marks < 0 || s.marks > 100)
            printf("Marks must be between 0 and 100.\n");
    } while (s.marks < 0 || s.marks > 100);

    fp = fopen(FILE_NAME, "ab");
    if (fp == NULL) {
        printf("Error: could not open file.\n");
        return;
    }
    fwrite(&s, sizeof(Student), 1, fp);
    fclose(fp);
    printf("Student record added successfully.\n");
}

void displayStudents(void) {
    FILE *fp = fopen(FILE_NAME, "rb");
    Student s;
    int count = 0;

    if (fp == NULL) {
        printf("No records found.\n");
        return;
    }
    printHeader();
    while (fread(&s, sizeof(Student), 1, fp) == 1) {
        printStudent(&s);
        count++;
    }
    fclose(fp);
    if (count == 0)
        printf("No records found.\n");
    else
        printf("\nTotal records: %d\n", count);
}

void searchStudent(void) {
    FILE *fp = fopen(FILE_NAME, "rb");
    Student s;
    int roll, found = 0;

    if (fp == NULL) {
        printf("No records found.\n");
        return;
    }
    roll = readInt("Enter roll number to search: ");
    while (fread(&s, sizeof(Student), 1, fp) == 1) {
        if (s.roll == roll) {
            printHeader();
            printStudent(&s);
            found = 1;
            break;
        }
    }
    fclose(fp);
    if (!found)
        printf("Student with roll number %d not found.\n", roll);
}

void updateStudent(void) {
    FILE *fp = fopen(FILE_NAME, "rb+");
    Student s;
    int roll, found = 0;

    if (fp == NULL) {
        printf("No records found.\n");
        return;
    }
    roll = readInt("Enter roll number to update: ");
    while (fread(&s, sizeof(Student), 1, fp) == 1) {
        if (s.roll == roll) {
            printf("Current record:");
            printHeader();
            printStudent(&s);
            printf("\nEnter new details:\n");
            readName(s.name, NAME_LEN);
            do {
                s.marks = readFloat("Enter marks (0-100): ");
                if (s.marks < 0 || s.marks > 100)
                    printf("Marks must be between 0 and 100.\n");
            } while (s.marks < 0 || s.marks > 100);

            fseek(fp, -(long)sizeof(Student), SEEK_CUR);
            fwrite(&s, sizeof(Student), 1, fp);
            found = 1;
            break;
        }
    }
    fclose(fp);
    if (found)
        printf("Record updated successfully.\n");
    else
        printf("Student with roll number %d not found.\n", roll);
}

void deleteStudent(void) {
    FILE *fp = fopen(FILE_NAME, "rb");
    FILE *tmp;
    Student s;
    int roll, found = 0;

    if (fp == NULL) {
        printf("No records found.\n");
        return;
    }
    tmp = fopen(TEMP_FILE, "wb");
    if (tmp == NULL) {
        printf("Error: could not create temporary file.\n");
        fclose(fp);
        return;
    }
    roll = readInt("Enter roll number to delete: ");
    while (fread(&s, sizeof(Student), 1, fp) == 1) {
        if (s.roll == roll)
            found = 1;
        else
            fwrite(&s, sizeof(Student), 1, tmp);
    }
    fclose(fp);
    fclose(tmp);

    if (found) {
        remove(FILE_NAME);
        rename(TEMP_FILE, FILE_NAME);
        printf("Record deleted successfully.\n");
    } else {
        remove(TEMP_FILE);
        printf("Student with roll number %d not found.\n", roll);
    }
}

/* ---------- Main menu ---------- */

int main(void) {
    int choice;

    do {
        printf("\n===== STUDENT MANAGEMENT SYSTEM =====\n");
        printf("1. Add Student\n");
        printf("2. Display All Students\n");
        printf("3. Search Student\n");
        printf("4. Update Student\n");
        printf("5. Delete Student\n");
        printf("6. Exit\n");
        choice = readInt("Enter your choice: ");

        switch (choice) {
            case 1: addStudent();      break;
            case 2: displayStudents(); break;
            case 3: searchStudent();   break;
            case 4: updateStudent();   break;
            case 5: deleteStudent();   break;
            case 6: printf("Goodbye!\n"); break;
            default: printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 6);

    return 0;
}
