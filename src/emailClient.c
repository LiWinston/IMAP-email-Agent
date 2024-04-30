#include "emailClient.h"
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
    char msg[1024];
    sprintf(msg, "a%d LOGIN %s %s\r\n", ++tagNum, arg.username, arg.password);

    n_send(msg);

    while (true) {
        n_readline();
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
    char msg[1024];
    if (arg.folder == NULL) {
        arg.folder = "INBOX";
    }
    sprintf(msg, "a%d SELECT %s\r\n", ++tagNum, arg.folder);

    n_send(msg);

    while (true) {
        n_readline();
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
    char msg[1024];
    sprintf(msg, "a%d FETCH %d BODY.PEEK[]\r\n", ++tagNum, arg.messageNum);

    n_send(msg);

    // Answer format: * messageNum FETCH (BODY[] {bodyLength}body)
    n_readline();
    int messageNum, bodyLength;
    if (sscanf(byteList.bytes, "* %d FETCH (BODY[] {%d}\r\n", &messageNum,
               &bodyLength) != 2) {
        printf("Message not found\n");
        return 3;
    }
    n_readBytes(bodyLength);
    // Print body
    printf("%s", byteList.bytes);
    // Read )/r/n
    n_readline();
    // Read last line
    n_readline();

    // This error may not specific in paper
    sprintf(msg, "a%d OK ", tagNum);
    if (strncasecmp(byteList.bytes, msg, strlen(msg)) != 0) {
        printf("Message not found\n");
        return 3;
    }
    return 0;
}

static int parse() {
    char msg[1024];
    sprintf(msg,
            "a%d FETCH %d BODY.PEEK[HEADER.FIELDS (FROM TO DATE SUBJECT)]\r\n",
            ++tagNum, arg.messageNum);

    n_send(msg);

    while (true) {
        n_readline();
        printf("%s", byteList.bytes);
        if (strstr(byteList.bytes, "OK Fetch completed") != NULL) {
            break;
        }
    }

    return 0;
}

static int mime() {
    return 0;
}

static int list() {
    return 0;
}

int runClient() {
    tagNum = 0;

    n_connect(arg.server_name, arg.tls_flag);

    int err = login();
    if (err) {
        return err;
    }

    err = selectFolder();
    if (err) {
        return err;
    }

    if (strcmp(arg.command, "retrieve") == 0) {
        retrieve();
    } else if (strcmp(arg.command, "parse") == 0) {
        parse();
    } else if (strcmp(arg.command, "mime") == 0) {
        mime();
    } else if (strcmp(arg.command, "list") == 0) {
        list();
    } else {
        fprintf(stderr, "Unknown command %s\n", arg.command);
        return 1;
    }

    return 0;
}

void killClient() {
    n_free();
}