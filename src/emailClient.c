#include "emailClient.h"
#include "macros.h"
#include "network.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

Arguments arg;

static int tagNum;

static int login() {
    int err = 0;
    char msg[1024];
    sprintf(msg, "a%d LOGIN \"%s\" \"%s\"\r\n", ++tagNum, arg.username,
            arg.password);

    err = n_send(msg);
    RETURN_ERR

    while (true) {
        err = n_readline();
        RETURN_ERR
        if (byteList.bytes[0] != '*') {
            break;
        }
    }

    sprintf(msg, "a%d OK ", tagNum);
    if (strncasecmp(byteList.bytes, msg, strlen(msg)) != 0) {
        printf("Login failure\n");
        return 3;
    }

    return 0;
}

static int selectFolder() {
    int err = 0;
    char msg[1024];
    if (arg.folder == NULL) {
        arg.folder = "INBOX";
    }
    sprintf(msg, "a%d SELECT \"%s\"\r\n", ++tagNum, arg.folder);

    err = n_send(msg);
    RETURN_ERR

    while (true) {
        err = n_readline();
        RETURN_ERR
        int maxMessageNum;
        if (arg.messageNum == 0 &&
            sscanf(byteList.bytes, "* %d EXISTS\r\n", &maxMessageNum) == 1) {
            arg.messageNum = maxMessageNum;
        }

        if (byteList.bytes[0] != '*') {
            break;
        }
    }

    sprintf(msg, "a%d OK ", tagNum);
    if (strncasecmp(byteList.bytes, msg, strlen(msg)) != 0) {
        printf("Folder not found\n");
        return 3;
    }

    return 0;
}

static int retrieve() {
    int err = 0;
    char msg[1024];
    sprintf(msg, "a%d FETCH %d BODY.PEEK[]\r\n", ++tagNum, arg.messageNum);

    err = n_send(msg);
    RETURN_ERR

    // Answer format: * messageNum FETCH (BODY[] {bodyLength}body)
    err = n_readline();
    RETURN_ERR

    int messageNum, bodyLength;
    if (sscanf(byteList.bytes, "* %d FETCH (BODY[] {%d}\r\n", &messageNum,
               &bodyLength) != 2) {
        printf("Message not found\n");
        return 3;
    }
    err = n_readBytes(bodyLength);
    // Print body
    printf("%s", byteList.bytes);
    // Read )/r/n
    err = n_readline();
    RETURN_ERR
    // Read last line
    err = n_readline();
    RETURN_ERR

    // This error may not specific in paper
    sprintf(msg, "a%d OK ", tagNum);
    if (strncasecmp(byteList.bytes, msg, strlen(msg)) != 0) {
        printf("Message not found\n");
        return 3;
    }
    return 0;
}

char *strstr_case_insensitive(const char *haystack, const char *needle);
static int parse() {
    char msg[1024];
    sprintf(msg,
            "a%d FETCH %d BODY.PEEK[HEADER.FIELDS (FROM TO DATE SUBJECT)]\r\n",
            ++tagNum, arg.messageNum);

    n_send(msg);

    while (true) {
        n_readline();
        printf("%s", byteList.bytes);
        if (strstr_case_insensitive(byteList.bytes, "OK Fetch completed") != NULL) {
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
    sprintf(msg, "a%d FETCH 1:* BODY.PEEK[HEADER.FIELDS (SUBJECT)]\r\n", ++tagNum);
    n_send(msg);

    int messageNum = 0;
    bool fetchCompleted = false;
    while (!fetchCompleted) {
        n_readline();
        if (strstr_case_insensitive(byteList.bytes, "OK Fetch completed") != NULL) {
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
                while (end > subject && (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n')) {
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
    int err = 0;
    tagNum = 0;

    err = n_connect(arg.server_name, arg.tls_flag);
    RETURN_ERR

    err = login();
    RETURN_ERR

    err = selectFolder();
    RETURN_ERR

    if (strcasecmp(arg.command, "retrieve") == 0) {
        err = retrieve();
    } else if (strcasecmp(arg.command, "parse") == 0) {
        err = parse();
    } else if (strcasecmp(arg.command, "mime") == 0) {
        err = mime();
    } else if (strcasecmp(arg.command, "list") == 0) {
        err = list();
    } else {
        fprintf(stderr, "Unknown command %s\n", arg.command);
        return 1;
    }
    RETURN_ERR

    return 0;
}

void c_free() {
    n_free();
}

char *strstr_case_insensitive(const char *haystack, const char *needle) {
    // Case-insensitive version of strstr
    size_t needle_len = strlen(needle);
    if (needle_len == 0) {
        return (char *)haystack;
    }

    while (*haystack) {
        if (strncasecmp(haystack, needle, needle_len) == 0) {
            return (char *)haystack;
        }
        haystack++;
    }

    return NULL;
}
