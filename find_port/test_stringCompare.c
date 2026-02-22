#include <stdio.h>
#include <string.h>

/* Your stringCompare function here - paste the cleaned version */
int stringCompare(const char* string1, const char* string2, int maxSize) {
    /* Error: NULL string or invalid maxSize */
    if (!string1 || !string2 || maxSize < 1) {
        return maxSize < 1 ? -3 : -2;
    }

    int i;
    for (i = 0; i < maxSize; i++) {
        if (string1[i] != string2[i]) {
            return i;  /* Mismatch at position i */
        }
        if (string1[i] == '\0') {
            return 0;  /* Equal up to EOS */
        }
    }
    return -1;  /* maxSize reached without EOS */
} /* stringCompare */

int main(void) {
    char buf1[20] = "abc";
    char buf2[20] = "abc";
    char buf3[20] = "abd";
    char empty[1] = "";
    int result;

    printf("stringCompare Test Harness\n");
    printf("==========================\n\n");

    /* Test 1: Equal strings with EOS */
    result = stringCompare(buf1, buf2, 10);
    printf("Test 1 - Equal EOS: %d ", result);
    printf("%s\n", result == 0 ? "PASS" : "FAIL");

    /* Test 2: Mismatch at position 2 */
    result = stringCompare(buf1, buf3, 10);
    printf("Test 2 - Mismatch pos2: %d ", result);
    printf("%s\n", result == 2 ? "PASS" : "FAIL");

    /* Test 3: maxSize reached, no EOS */
    result = stringCompare(buf1, buf2, 2);
    printf("Test 3 - maxSize 2: %d ", result);
    printf("%s\n", result == -1 ? "PASS" : "FAIL");

    /* Test 4: NULL pointer */
    result = stringCompare(NULL, buf2, 5);
    printf("Test 4 - NULL arg: %d ", result);
    printf("%s\n", result == -2 ? "PASS" : "FAIL");

    /* Test 5: maxSize < 1 */
    result = stringCompare(buf1, buf2, 0);
    printf("Test 5 - maxSize 0: %d ", result);
    printf("%s\n", result == -3 ? "PASS" : "FAIL");

    /* Test 6: Empty strings */
    result = stringCompare(empty, empty, 1);
    printf("Test 6 - Empty EOS: %d ", result);
    printf("%s\n", result == 0 ? "PASS" : "FAIL");

    /* Test 7: Longer strings, EOS within maxSize */
    strcpy(buf1, "ab\0def");  /* EOS at 2 */
    strcpy(buf2, "ab\0ghi");
    result = stringCompare(buf1, buf2, 10);
    printf("Test 7 - EOS at 2: %d ", result);
    printf("%s\n", result == 0 ? "PASS" : "FAIL");

    printf("\nAll tests complete.\n");
    return 0;
} /* main */
