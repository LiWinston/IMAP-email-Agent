#include "emailClient.h"
#include "macros.h"
#include "network.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK_OK(__msg)                                                               \
    sprintf(msg, "A%d OK ", tagNum);                                           \
    if (strncasecmp(byteList.bytes, msg, strlen(msg)) != 0) {                  \
        printf(__msg);                                             \
        return 3;                                                              \
    }

Arguments arg;

static int tagNum;

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
        if (i < n - 2 && s[i] == '\r' && s[i + 1] == '\n' &&
            (s[i + 2] == ' ' || s[i + 2] == '\t')) {
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

    CHECK_OK("Login failure\n")

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
        if (arg.messageNum == -1000000007 &&
            sscanf(byteList.bytes, "* %d EXISTS\r\n", &maxMessageNum) == 1) {
            arg.messageNum = maxMessageNum;
        }

        if (byteList.bytes[0] != '*') {
            break;
        }
    }

    CHECK_OK("Folder not found\n")

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
    CHECK_OK("Message not found\n")
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

        CHECK_OK("Message not found\n")
    }

    return 0;
}

// sprintf(msg, "A%d FETCH %d BODY\r\n", ++tagNum, arg.messageNum);
// This is just a placeholder, and shoud parse content of the respond of fetch
// BODY and find the first UTF-8 text/plain body.
static int mime() {
    char msg[1024];
    sprintf(msg, "A%d FETCH %d BODY\r\n", ++tagNum, arg.messageNum);

    HANDLE_ERR(n_send(msg))

    int body_part = -1, cur = 0, qc = 0, messageNum;
    HANDLE_ERR(n_readLine())
    toUpper(byteList.bytes);
    if (sscanf(byteList.bytes, "* %d FETCH (BODY", &messageNum) != 1) {
        printf("%s", "Message not found\n");
        return 3;
    }
    char *requirement[6] = {"TEXT", "PLAIN", "UTF-8", "QUOTED-PRINTABLE",
                            "7BIT", "8BIT"};
    // Parse body structure
    char mate_data[10 * 1024];
    int md_c = 0;
    for (int part = 1; byteList.bytes[cur] || qc; cur++) {
        if (byteList.bytes[cur] == '\0') {
            HANDLE_ERR(n_readLine())
            toUpper(byteList.bytes);
            cur = 0;
            continue;
        }
        if (qc >= 3) {
            mate_data[md_c] = byteList.bytes[cur];
            md_c++;
        }
        if (byteList.bytes[cur] == '(') {
            qc++;
        } else if (byteList.bytes[cur] == ')') {
            qc--;
            if (qc == 2) {
                mate_data[md_c] = '\0';
                // Match the requirement
                bool ok1 = true, ok2 = false;
                for (int i = 0; i < 3; i++) {
                    if (strstr(mate_data, requirement[i]) == NULL) {
                        ok1 = false;
                    }
                }
                for (int i = 3; i < 6; i++) {
                    if (strstr(mate_data, requirement[i]) != NULL) {
                        ok2 = true;
                    }
                }
                if (ok2 && ok1) {
                    body_part = part;
                    break;
                }
                part++;
                md_c = 0;
            }
        }
    }
    if (body_part == -1) {
        printf("%s", "mime error: can not find mime that fit requirement");
        return 4;
    }
    HANDLE_ERR(n_readLine())
    toUpper(byteList.bytes);
    CHECK_OK("Message not found\n")

    sprintf(msg, "A%d FETCH %d BODY.PEEK[%d]\r\n", ++tagNum, arg.messageNum,
            body_part);

    HANDLE_ERR(n_send(msg))

    // Answer format: * messageNum FETCH (BODY[] {bodyLength}body)
    HANDLE_ERR(n_readLine())
    toUpper(byteList.bytes);

    int bodyLength;
    if (sscanf(byteList.bytes, "* %d FETCH (BODY[%d] {%d}\r\n", &messageNum,
               &body_part, &bodyLength) != 3) {
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
    CHECK_OK("Message not found\n")
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

    HANDLE_ERR(n_connect(arg.server_name, arg.tls_flag));

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