#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_BOOKS 50000
#define DASHBOARD_WIDTH 72

static int debugMode = 0;

void printFinalAnalysis(double linearTime, int linearComparisons, double binaryTime, int binaryComparisons, double sortTime);

// Print a repeated character line for dashboard formatting.
void printDashboardLine(char ch) {
    for (int i = 0; i < DASHBOARD_WIDTH; ++i) {
        putchar(ch);
    }
    putchar('\n');
}

// Print a console dashboard summarizing all search operations.
void printDashboard(const char *title,
                    const char *dataMode,
                    int n,
                    int key,
                    int linearUsed,
                    int linearFound,
                    int linearPos,
                    int linearComparisons,
                    double linearTime,
                    int binaryUsed,
                    int binaryFound,
                    int binaryPos,
                    int binaryComparisons,
                    double binaryTime,
                    int sortPerformed,
                    int sortComparisons,
                    double sortTime) {
    printDashboardLine('=');
    printf("SMART LIBRARY SEARCH DASHBOARD - %s\n", title);
    printDashboardLine('=');
    printf("Dataset mode       : %s\n", dataMode);
    printf("Book count         : %d\n", n);
    printf("Search key         : %d\n", key);
    printf("Debug logs enabled : %s\n", debugMode ? "Yes" : "No");
    printf("Sorted data used   : %s\n", binaryUsed ? "Yes" : "No");
    printDashboardLine('-');

    printf("LINEAR SEARCH\n");
    if (!linearUsed) {
        printf("  Status          : Not performed\n");
    } else {
        printf("  Found           : %s\n", linearFound ? "Yes" : "No");
        if (linearFound) {
            printf("  Position        : %d\n", linearPos + 1);
        }
        printf("  Comparisons     : %d\n", linearComparisons);
        printf("  Time            : %.6f sec\n", linearTime);
    }
    printDashboardLine('-');

    if (!binaryUsed) {
        printf("BINARY SEARCH\n");
        printf("  Status          : Not performed\n");
        printDashboardLine('-');
    } else {
        printf("BINARY SEARCH\n");
        printf("  Found           : %s\n", binaryFound ? "Yes" : "No");
        if (binaryFound) {
            printf("  Position        : %d\n", binaryPos + 1);
        }
        printf("  Comparisons     : %d\n", binaryComparisons);
        printf("  Time            : %.6f sec\n", binaryTime);
        printDashboardLine('-');
    }

    if (sortPerformed) {
        printf("SORTING OVERHEAD\n");
        printf("  Time            : %.6f sec\n", sortTime);
        printDashboardLine('-');
    }

    if (linearUsed && binaryUsed) {
        printFinalAnalysis(linearTime, linearComparisons, binaryTime, binaryComparisons, sortTime);
    } else if (binaryUsed) {
        printf("\n=== FINAL ANALYSIS ===\n");
        printf("Binary search completed without linear comparison in this mode.\n");
        printf("Binary Search: %.6f sec, %d comparisons\n", binaryTime, binaryComparisons);
        if (sortPerformed) {
            printf("Sorting overhead for Binary Search: %.6f sec\n", sortTime);
        }
        printf("Recommendation: Binary Search is best when data is already sorted or searched repeatedly.\n");
    } else if (linearUsed) {
        printf("\n=== FINAL ANALYSIS ===\n");
        printf("Linear search completed without binary comparison in this mode.\n");
        printf("Linear Search: %.6f sec, %d comparisons\n", linearTime, linearComparisons);
        printf("Recommendation: Linear Search is practical for unsorted or frequently changing data.\n");
    } else {
        printf("Final Analysis: Binary Search and sorting were not performed in this mode.\n");
    }
    printDashboardLine('=');
}

// Read a validated integer from stdin with range enforcement.
int readInt(const char *prompt, int minValue, int maxValue) {
    char buffer[128];
    long value;
    char *endPtr;

    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("Input error. Please try again.\n");
            continue;
        }

        value = strtol(buffer, &endPtr, 10);
        if (endPtr == buffer || *endPtr != '\n') {
            printf("Please enter a valid integer.\n");
            continue;
        }

        if (value < minValue || value > maxValue) {
            printf("Enter a value between %d and %d.\n", minValue, maxValue);
            continue;
        }

        return (int)value;
    }
}

// Check whether a value already exists in the array.
int containsValue(int arr[], int n, int value) {
    for (int i = 0; i < n; ++i) {
        if (arr[i] == value) {
            return 1;
        }
    }
    return 0;
}

// Read book accession numbers from the user and avoid duplicates.
void inputArray(int arr[], int n) {
    for (int i = 0; i < n; ++i) {
        while (1) {
            int accession = readInt("Enter accession number: ", 1, INT_MAX);
            if (containsValue(arr, i, accession)) {
                printf("Duplicate accession number detected. Please enter a unique value.\n");
                continue;
            }
            arr[i] = accession;
            break;
        }
    }
}

// Generate a fixed dataset of accession numbers for fast testing.
void generateFixedBooks(int arr[], int n) {
    for (int i = 0; i < n; ++i) {
        arr[i] = i + 1;
    }
}

// Print the array of accession numbers.
void printArray(const int arr[], int n) {
    if (n <= 20) {
        printf("[");
        for (int i = 0; i < n; ++i) {
            printf("%d", arr[i]);
            if (i < n - 1) {
                printf(", ");
            }
        }
        printf("]\n");
        return;
    }

    printf("[");
    for (int i = 0; i < 10; ++i) {
        printf("%d", arr[i]);
        printf(", ");
    }
    printf("... ");
    for (int i = n - 10; i < n; ++i) {
        printf("%d", arr[i]);
        if (i < n - 1) {
            printf(", ");
        }
    }
    printf("] (%d elements)\n", n);
}

// Linear search on an unsorted array with comparison counting.
int linearSearch(int arr[], int n, int key, int *comparisons) {
    *comparisons = 0;
    for (int index = 0; index < n; ++index) {
        (*comparisons)++;
        if (arr[index] == key) {
            return index;
        }
    }
    return -1;
}

// Binary search on a sorted array with comparison counting.
int binarySearch(int arr[], int n, int key, int *comparisons) {
    *comparisons = 0;
    int left = 0;
    int right = n - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;
        (*comparisons)++;

        if (arr[mid] == key) {
            return mid;
        }
        if (arr[mid] < key) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return -1;
}

// Bubble sort implementation that counts comparisons and measures time.
void sortArray(int arr[], int n, int *comparisons, double *timeTaken) {
    *comparisons = 0;
    clock_t start = clock();

    for (int i = 0; i < n - 1; ++i) {
        int swapped = 0;
        for (int j = 0; j < n - i - 1; ++j) {
            (*comparisons)++;
            if (arr[j] > arr[j + 1]) {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
                swapped = 1;
            }
        }
        if (!swapped) {
            break;
        }
    }

    clock_t end = clock();
    *timeTaken = (double)(end - start) / CLOCKS_PER_SEC;
}

// Print a final conclusion comparing linear and binary search.
void printFinalAnalysis(double linearTime, int linearComparisons, double binaryTime, int binaryComparisons, double sortTime) {
    printf("\n=== FINAL ANALYSIS ===\n");
    printf("Linear Search: %.6f sec, %d comparisons\n", linearTime, linearComparisons);
    printf("Binary Search: %.6f sec, %d comparisons\n", binaryTime, binaryComparisons);
    printf("Sorting overhead for Binary Search: %.6f sec\n", sortTime);

    if ((binaryTime + sortTime) < linearTime) {
        printf("Conclusion: Binary Search is faster for repeated searches on sorted data.\n");
    } else {
        printf("Conclusion: Linear Search can be faster when dataset is small or when sorting overhead is not justified.\n");
    }

    printf("When to use each algorithm:\n");
    printf("- Linear Search: Use when data is unsorted, dataset is small, or insert/delete operations happen frequently.\n");
    printf("- Binary Search: Use when data is sorted and you need many searches, especially on larger datasets.\n");
    printf("- Binary Search is less appropriate for dynamic datasets unless you maintain a sorted structure.\n");

    printf("Static vs Dynamic dataset:\n");
    printf("- Static dataset: content is mostly fixed; invest in sorting once and use Binary Search repeatedly.\n");
    printf("- Dynamic dataset: data changes frequently; Linear Search avoids repeated sorting overhead and is simpler to maintain.\n");
}

// Perform automatic test cases for best, average, and worst scenarios.
void runAutomaticTests(int arr[], int n, const char *dataMode) {
    if (n <= 0) {
        printf("No books available to test.\n");
        return;
    }

    int *sortedBooks = malloc((size_t)n * sizeof(int));
    if (sortedBooks == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }
    memcpy(sortedBooks, arr, (size_t)n * sizeof(int));

    int sortComparisons = 0;
    double sortTime = 0.0;
    sortArray(sortedBooks, n, &sortComparisons, &sortTime);

    int bestKey = arr[0];
    int averageKey = arr[n / 2];
    int worstKey = arr[n - 1] + 1;

    printf("\n=== AUTOMATIC TEST CASES ===\n");
    printf("Best case search key: %d\n", bestKey);
    printf("Average case search key: %d\n", averageKey);
    printf("Worst case search key (not found): %d\n", worstKey);

    int testKeys[3] = {bestKey, averageKey, worstKey};
    const char *testNames[3] = {"Best Case", "Average Case", "Worst Case"};

    for (int i = 0; i < 3; ++i) {
        printf("\n--- %s ---\n", testNames[i]);
        int key = testKeys[i];

        int linearComparisons = 0;
        clock_t linearStart = clock();
        int linearPosition = linearSearch(arr, n, key, &linearComparisons);
        double linearTime = (double)(clock() - linearStart) / CLOCKS_PER_SEC;
        if (linearPosition >= 0) {
            printf("Linear Search found key at position %d.\n", linearPosition + 1);
        } else {
            printf("Linear Search did not find the key.\n");
        }
        printf("Linear comparisons: %d, time: %.6f sec\n", linearComparisons, linearTime);

        int binaryComparisons = 0;
        clock_t binaryStart = clock();
        int binaryPosition = binarySearch(sortedBooks, n, key, &binaryComparisons);
        double binaryTime = (double)(clock() - binaryStart) / CLOCKS_PER_SEC;
        if (binaryPosition >= 0) {
            printf("Binary Search found key at position %d in sorted data.\n", binaryPosition + 1);
        } else {
            printf("Binary Search did not find the key.\n");
        }
        printf("Binary comparisons: %d, time: %.6f sec\n", binaryComparisons, binaryTime);
    }

    free(sortedBooks);
    printf("\nSorting overhead for Binary Search: time=%.6f sec\n", sortTime);
}

// Run the selected search analysis for the user.
void analyzePerformance(int books[], int n, int option, const char *dataMode) {
    if (n <= 0) {
        printf("No data to analyze.\n");
        return;
    }

    int *sortedBooks = malloc((size_t)n * sizeof(int));
    if (sortedBooks == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    printf("\nOriginal accession numbers: ");
    printArray(books, n);
    memcpy(sortedBooks, books, (size_t)n * sizeof(int));

    int key = readInt("Enter accession number to search: ", 1, INT_MAX);
    printf("\n");

    int linearComparisons = 0;
    double linearTime = 0.0;
    int linearPosition = -1;
    int binaryComparisons = 0;
    double binaryTime = 0.0;
    int binaryPosition = -1;
    int sortComparisons = 0;
    double sortTime = 0.0;
    int sortPerformed = 0;
    int binaryUsed = 0;

    if (option == 1 || option == 3) {
        clock_t linearStart = clock();
        linearPosition = linearSearch(books, n, key, &linearComparisons);
        linearTime = (double)(clock() - linearStart) / CLOCKS_PER_SEC;

        if (option == 1) {
            printf("Linear Search: %s\n", linearPosition >= 0 ? "Found key in unsorted array." : "Key not found in unsorted array.");
            printf("Linear comparisons: %d\n", linearComparisons);
            printf("Linear search time: %.6f seconds\n", linearTime);
        }

        if (option == 3) {
            sortPerformed = 1;
            memcpy(sortedBooks, books, (size_t)n * sizeof(int));
            sortArray(sortedBooks, n, &sortComparisons, &sortTime);
            binaryUsed = 1;
            clock_t binaryStart = clock();
            binaryPosition = binarySearch(sortedBooks, n, key, &binaryComparisons);
            binaryTime = (double)(clock() - binaryStart) / CLOCKS_PER_SEC;

            printf("Sorted accession numbers: ");
            printArray(sortedBooks, n);
            printf("Sorting time: %.6f seconds\n", sortTime);
            printf("Binary Search: %s\n", binaryPosition >= 0 ? "Found key in sorted array." : "Key not found in sorted array.");
            printf("Binary comparisons: %d\n", binaryComparisons);
            printf("Binary search time: %.6f seconds\n", binaryTime);
        }
    } else if (option == 2) {
        sortPerformed = 1;
        memcpy(sortedBooks, books, (size_t)n * sizeof(int));
        sortArray(sortedBooks, n, &sortComparisons, &sortTime);
        binaryUsed = 1;
        clock_t binaryStart = clock();
        binaryPosition = binarySearch(sortedBooks, n, key, &binaryComparisons);
        binaryTime = (double)(clock() - binaryStart) / CLOCKS_PER_SEC;

        printf("Sorted accession numbers: ");
        printArray(sortedBooks, n);
        printf("Sorting time: %.6f seconds\n", sortTime);
        printf("Binary Search: %s\n", binaryPosition >= 0 ? "Found key in sorted array." : "Key not found in sorted array.");
        printf("Binary comparisons: %d\n", binaryComparisons);
        printf("Binary search time: %.6f seconds\n", binaryTime);
    }

    printDashboard("Search Summary", dataMode, n, key,
                   (option == 1 || option == 3), linearPosition >= 0, linearPosition, linearComparisons, linearTime,
                   binaryUsed, binaryPosition >= 0, binaryPosition, binaryComparisons, binaryTime,
                   sortPerformed, sortComparisons, sortTime);

    free(sortedBooks);
}

int main(void) {
    printf("Smart Library Search Optimization\n");
    printf("--------------------------------\n");

    printf("Debug mode flag is available (detailed comparison logs are disabled).\n");
    int debugChoice = readInt("Enable debug mode? (1 = yes, 0 = no): ", 0, 1);
    debugMode = debugChoice;

    int dataChoice = readInt("Choose data mode (1 = manual entry, 2 = fixed 50000-book dataset): ", 1, 2);
    int numberOfBooks;
    int *books;

    const char *dataMode;

    if (dataChoice == 1) {
        numberOfBooks = readInt("Enter the number of books: ", 1, MAX_BOOKS);
        books = malloc((size_t)numberOfBooks * sizeof(int));
        if (books == NULL) {
            printf("Memory allocation failed.\n");
            return EXIT_FAILURE;
        }

        printf("Enter unique accession numbers for each book.\n");
        inputArray(books, numberOfBooks);
        dataMode = "Manual entry";
    } else {
        numberOfBooks = MAX_BOOKS;
        books = malloc((size_t)numberOfBooks * sizeof(int));
        if (books == NULL) {
            printf("Memory allocation failed.\n");
            return EXIT_FAILURE;
        }

        generateFixedBooks(books, numberOfBooks);
        printf("Generated fixed dataset of %d books.\n", numberOfBooks);
        dataMode = "Fixed 50000-book dataset";
    }

    while (1) {
        printf("\nMenu:\n");
        printf("1. Linear Search\n");
        printf("2. Binary Search\n");
        printf("3. Compare Both\n");
        printf("4. Run Automatic Test Cases\n");
        printf("5. Exit\n");

        int choice = readInt("Choose an option: ", 1, 5);
        if (choice == 5) {
            break;
        }

        switch (choice) {
            case 1:
                analyzePerformance(books, numberOfBooks, 1, dataMode);
                break;
            case 2:
                analyzePerformance(books, numberOfBooks, 2, dataMode);
                break;
            case 3:
                analyzePerformance(books, numberOfBooks, 3, dataMode);
                break;
            case 4:
                runAutomaticTests(books, numberOfBooks, dataMode);
                break;
            default:
                printf("Invalid option. Please choose from the menu.\n");
                break;
        }
    }

    free(books);
    printf("Program terminated.\n");
    return EXIT_SUCCESS;
}
