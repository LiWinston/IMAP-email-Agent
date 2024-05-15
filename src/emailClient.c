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
__attribute__((unused)) static char *
strstr_case_insensitive(char *haystack, const char *needle);
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

// Removing any CRLF that is immediately followed by WSP
static void unfold(char *s) {
    int n = strlen(s);
    int j = 0;
    for (int i = 0; i < n; i++, j++) {
        if (i < n - 2 && s[i] == '\r' && s[i + 1] == '\n' && (s[i + 2] == ' ' || s[i + 2] == '\t')) {
            i += 2;
        }
        s[j] = s[i];
    }
    s[j] = '\0';
}

static int login() {
    char msg[1024];
    sprintf(msg, "A%d LOGIN \"%s\" \"%s\"\r\n", ++tagNum, arg.username,
            arg.password);

    HANDLE_ERR(n_send(msg));

    while (true) {
        HANDLE_ERR(n_readLine());
        if (byteList.bytes[0] != '*') {
            break;
        }
    }

    sprintf(msg, "A%d OK ", tagNum);
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
        HANDLE_ERR(n_readLine())
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
    HANDLE_ERR(n_readLine())
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
    HANDLE_ERR(n_readLine())
    // Read last line
    HANDLE_ERR(n_readLine())
    // This error may not specific in paper
    sprintf(msg, "A%d OK ", tagNum);
    if (strncasecmp(byteList.bytes, msg, strlen(msg)) != 0) {
        printf("Message not found\n");
        return 3;
    }
    return 0;
}

static int parse() {
    char msg[1024];
    // Need four request to get all info, inefficient but easy for string parse,
    // and I don't think assignment ask for speed.
    char *headers[4] = {"From", "To", "Date", "Subject"};
    for (int hd = 0; hd < 4; hd++) {
        sprintf(msg, "A%d FETCH %d BODY.PEEK[HEADER.FIELDS (%s)]\r\n", ++tagNum,
                arg.messageNum, headers[hd]);

        HANDLE_ERR(n_send(msg))

        while (true) {
            HANDLE_ERR(n_readLine())
            toUpper(byteList.bytes);

            if (byteList.bytes[0] != '*') {
                break;
            }

            int messageNum, bodyLen;
            if (sscanf(byteList.bytes,
                       "* %d FETCH (BODY[HEADER.FIELDS (%[^)])] {%d}\r\n",
                       &messageNum, msg, &bodyLen) != 3) {
                // TODO: unkonwn error, how to handle?
                return 3;
            }

            HANDLE_ERR(n_readBytes(bodyLen))
            unfold(byteList.bytes);

            int i = strlen(byteList.bytes) - 4;
            // Tmp fix, shit code for tutor
            if (i == -2) {
                if (hd == 0 || hd == 2) {
                    // TODO: unkonwn error, how to handle?
                    return 3;
                }
                if (hd == 3) {
                    printf("Subject: <No subject>\n");

                } else {
                    printf("To:\n");
                }
                HANDLE_ERR(n_readLine())
                continue;
            } else {
                byteList.bytes[i] = '\0';
            }

            printf("%s: %s\n", headers[hd],
                   byteList.bytes + strlen(headers[hd]) + 2);
            // Read remain line
            HANDLE_ERR(n_readLine())
        }

        sprintf(msg, "A%d OK ", tagNum);
        if (strncasecmp(byteList.bytes, msg, strlen(msg)) != 0) {
            printf("Message not found\n");
            return 3;
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

    while (true) {
        // Read first line
        HANDLE_ERR(n_readLine())
        if (byteList.bytes[0] != '*') {
            break;
        }
        toUpper(byteList.bytes);

        // Extract subject from the response
        int messageNum, bodyLen;
        if (sscanf(byteList.bytes,
                   "* %d FETCH (BODY[HEADER.FIELDS (SUBJECT)] {%d}\r\n",
                   &messageNum, &bodyLen) != 2) {
            return 3;
        }
        // Read body
        HANDLE_ERR(n_readBytes(bodyLen));
        unfold(byteList.bytes);

        char *subject = "<No subject>";
        // No empty subject
        if (strlen(byteList.bytes) >= 10) {
            subject = byteList.bytes + 9;
            // Tmp fix, will remove after ci fix:
            // https://edstem.org/au/courses/15616/discussion/1944410
            int i = strlen(byteList.bytes) - 4;
            byteList.bytes[i] = '\0';
            while (byteList.bytes[--i] == ' ') {
                byteList.bytes[i] = '\0';
            }
        }
        printf("%d: %s\n", messageNum, subject);
        // Read remain line
        HANDLE_ERR(n_readLine())
    }

    // TODO: handle error? cannot find details in pdf

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