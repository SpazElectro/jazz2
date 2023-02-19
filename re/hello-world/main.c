#include <stdio.h>
#include <stdlib.h>

int main() {
    char password[20];
    int i, len;

    len = sprintf(password, "mypassword");

    for (i = 0; i < len; i++) {
        password[i] ^= 0x1F;
    }

    printf("Enter password: ");
    scanf("%s", password);

    for (i = 0; i < len; i++) {
        password[i] ^= 0x1F;
    }

    if (strcmp(password, "mypassword") == 0) {
        printf("Access granted.\n");
    } else {
        printf("Access denied.\n");
    }

    return 0;
}