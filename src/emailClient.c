#include "emailClient.h"
#include "macros.h"
#include "network.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Arguments arg;

static int tagNum;

// Case-insensitive version of strstr
static char *strstr_case_insensitive(char *haystack, const char *needle) {
    size_t needle_len = strlen(needle);
    while (*haystack) {
        if (strncasecmp(haystack, needle, needle_len) == 0) {
            return haystack;
        }
        haystack++;
    }

    return NULL;
}

// Convert string to upper case
static void toUpper(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
}

static int login() {
    char msg[1024];
    sprintf(msg, "A%d LOGIN \"%s\" \"%s\"\r\n", ++tagNum, arg.username,
            arg.password);

    HANDLE_ERR(n_send(msg));

    while (true) {
        HANDLE_ERR(n_readline());
        if (byteList.bytes[0] != '*') {
            break;
        }
    }

    sprintf(msg, "A%d OK ", tagNum);
    toUpper(byteList.bytes);
    if (strncasecmp(byteList.bytes, msg, strlen(msg)) != 0) {
        printf("Login failure\n");
        return 3;
    }

    return 0;
}

static int selectFolder() {
    char msg[1024];
    if (arg.folder == NULL) {
        arg.folder = "INBOX";
    }
    sprintf(msg, "A%d SELECT \"%s\"\r\n", ++tagNum, arg.folder);

    HANDLE_ERR(n_send(msg))

    int maxMessageNum;
    while (true) {
        HANDLE_ERR(n_readline())
        toUpper(byteList.bytes);
        if (arg.messageNum == 0 &&
            sscanf(byteList.bytes, "* %d EXISTS\r\n", &maxMessageNum) == 1) {
            arg.messageNum = maxMessageNum;
        }

        if (byteList.bytes[0] != '*') {
            break;
        }
    }

    sprintf(msg, "A%d OK ", tagNum);
    if (strncasecmp(byteList.bytes, msg, strlen(msg)) != 0) {
        printf("Folder not found\n");
        return 3;
    }

    return 0;
}

static int retrieve() {
    char msg[1024];
    sprintf(msg, "A%d FETCH %d BODY.PEEK[]\r\n", ++tagNum, arg.messageNum);

    HANDLE_ERR(n_send(msg))

    // Answer format: * messageNum FETCH (BODY[] {bodyLength}body)
    HANDLE_ERR(n_readline())
    toUpper(byteList.bytes);

    int messageNum, bodyLength;
    if (sscanf(byteList.bytes, "* %d FETCH (BODY[] {%d}\r\n", &messageNum,
               &bodyLength) != 2) {
        printf("Message not found\n");
        return 3;
    }
    HANDLE_ERR(n_readBytes(bodyLength))
    // Print body
    printf("%s", byteList.bytes);
    // Read )/r/n
    HANDLE_ERR(n_readline())
    // Read last line
    HANDLE_ERR(n_readline())
    // This error may not specific in paper
    sprintf(msg, "A%d OK ", tagNum);
    toUpper(byteList.bytes);
    if (strncasecmp(byteList.bytes, msg, strlen(msg)) != 0) {
        printf("Message not found\n");
        return 3;
    }
    return 0;
}

static int parse() {
    char msg[1024];
    sprintf(msg,
            "A%d FETCH %d BODY.PEEK[HEADER.FIELDS (FROM TO DATE SUBJECT)]\r\n",
            ++tagNum, arg.messageNum);

    HANDLE_ERR(n_send(msg))

    while (true) {
        HANDLE_ERR(n_readline())
        printf("%s", byteList.bytes);
        if (strstr_case_insensitive(byteList.bytes, "OK Fetch completed") !=
            NULL) {
            break;
        }
    }

    return 0;
}

static int mime() {
    return 0;
}

static int list() {
    char msg[1024];
    sprintf(msg, "A%d FETCH 1:* BODY.PEEK[HEADER.FIELDS (SUBJECT)]\r\n",
            ++tagNum);

    HANDLE_ERR(n_send(msg))

    int messageNum = 0;
    bool fetchCompleted = false;
    while (!fetchCompleted) {
        HANDLE_ERR(n_readline())
        if (strstr_case_insensitive(byteList.bytes, "OK Fetch completed") !=
            NULL) {
            fetchCompleted = true;
            break;
        }

        // Extract subject from the response
        if (strstr_case_insensitive(byteList.bytes, "Subject:") != NULL) {
            char *subject = strstr_case_insensitive(byteList.bytes, "Subject:");
            if (subject != NULL) {
                subject += strlen("Subject:");
                // Remove leading and trailing whitespace
                while (*subject && (*subject == ' ' || *subject == '\t')) {
                    subject++;
                }
                char *end = subject + strlen(subject) - 1;
                while (end > subject && (*end == ' ' || *end == '\t' ||
                                         *end == '\r' || *end == '\n')) {
                    *end-- = '\0';
                }

                // printf("%d: %s\n", ++messageNum, subject);
                // if(strlen(subject) > 0 && *subject != '\0') {
                if (strlen(subject) - 1 > 0) {
                    // printf("Subject len %lu\n", strlen(subject));
                    printf("%d: %s\n", ++messageNum, subject);
                } else {
                    printf("%d: <No subject>\n", ++messageNum);
                }
            } else {
                // If subject is not found, print "<No subject>"
                printf("%d: <No subject>\n", ++messageNum);
            }
        }
    }

    if (messageNum == 0) {
        printf("Mailbox is empty\n");
    }

    return 0;
}

int c_run() {
    tagNum = 0;

    HANDLE_ERR(n_connect(arg.server_name, arg.tls_flag))

    HANDLE_ERR(login())

    HANDLE_ERR(selectFolder())

    if (strcmp(arg.command, "retrieve") == 0) {
        HANDLE_ERR(retrieve())
    } else if (strcmp(arg.command, "parse") == 0) {
        HANDLE_ERR(parse())
    } else if (strcmp(arg.command, "mime") == 0) {
        HANDLE_ERR(mime())
    } else if (strcmp(arg.command, "list") == 0) {
        HANDLE_ERR(list())
    } else {
        fprintf(stderr, "Unknown command %s\n", arg.command);
        return 1;
    }

    return 0;
}

void c_free() {
    n_free();
}