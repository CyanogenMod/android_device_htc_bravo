/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/** Bluetooth configuration for Bravo (debug only)  */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/uio.h>
#include <unistd.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/sco.h>

#include <bluedroid/bluetooth.h>

static void usage(void);

int vendor_sleep(int fd) {
    unsigned char hci_sleep_cmd[] = {
        0x01,               // HCI command packet
        0x27, 0xfc,         // HCI_Set_Sleep_Mode_Param
        0x0c,               // 12 arguments
        0x01,               // ??
        0x01,               // idle threshold Host (x300ms)
        0x01,               // idle threadhold HC (x300ms)
        0x01,               // WAKE active high
        0x01,               // HOST_WAKE active high
        0x01,               // Allow host sleep during SCO
        0x01,               // Combine sleep mode and LPM
        0x00,               // Enable tristate control of uart TX
        0x00, 0x00, 0x00, 0x00,
    };

    int ret = write(fd, hci_sleep_cmd, sizeof(hci_sleep_cmd));
    if (ret < 0) {
        printf("write(): %s (%d)]\n", strerror(errno), errno);
        return -1;
    } else if (ret != sizeof(hci_sleep_cmd)) {
        printf("write(): unexpected length %d\n", ret);
        return -1;
    }
    return 0;
}

int vendor_high_priority(int fd, unsigned char acl) {
    unsigned char hci_sleep_cmd[] = {
        0x01,               // HCI command packet
        0x57, 0xfc,         // HCI_Write_High_Priority_Connection
        0x02,               // Length
        0x00, 0x00          // Handle
    };

    hci_sleep_cmd[4] = acl;

    int ret = write(fd, hci_sleep_cmd, sizeof(hci_sleep_cmd));
    if (ret < 0) {
        printf("write(): %s (%d)]\n", strerror(errno), errno);
        return -1;
    } else if (ret != sizeof(hci_sleep_cmd)) {
        printf("write(): unexpected length %d\n", ret);
        return -1;
    }
    return 0;
}

int get_hci_sock() {
    int sock = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI);
    struct sockaddr_hci addr;
    int opt;

    if(sock < 0) {
        printf("Can't create raw socket!\n");
        return -1;
    }

    opt = 1;
    printf("Setting data direction.\n");
    if (setsockopt(sock, SOL_HCI, HCI_DATA_DIR, &opt, sizeof(opt)) < 0) {
        printf("Error setting data direction\n");
        return -1;
    }

    /* Bind socket to the HCI device */
    addr.hci_family = AF_BLUETOOTH;
    addr.hci_dev = 0;  // hci0
    printf("Binding to HCI device.\n");
    if(bind(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        printf("Can't attach to device hci0. %s(%d)\n",
             strerror(errno),
             errno);
        return -1;
    }
    return sock;
}

static int get_acl_handle(int fd, bdaddr_t bdaddr) {
    int i;
    int ret = -1;
    struct hci_conn_list_req *conn_list;
    struct hci_conn_info *conn_info;
    int max_conn = 10;

    conn_list = malloc(max_conn * (
            sizeof(struct hci_conn_list_req) + sizeof(struct hci_conn_info)));
    if (!conn_list) {
        printf("Out of memory in %s\n", __FUNCTION__);
        return -1;
    }

    conn_list->dev_id = 0;  /* hardcoded to HCI device 0 */
    conn_list->conn_num = max_conn;

    if (ioctl(fd, HCIGETCONNLIST, (void *)conn_list)) {
        printf("Failed to get connection list\n");
        goto out;
    }

    for (i=0; i < conn_list->conn_num; i++) {
        conn_info = &conn_list->conn_info[i];
        if (conn_info->type == ACL_LINK &&
                !memcmp((void *)&conn_info->bdaddr, (void *)&bdaddr,
                sizeof(bdaddr_t))) {
            ret = conn_info->handle;
            goto out;
        }
    }
    ret = 0;

out:
    free(conn_list);
    return ret;
}

static int do_sleep(int argc, char **argv) {
    int ret;
    int sock = get_hci_sock();

    if (sock < 0)
        return sock;

    ret = vendor_sleep(sock);
    close(sock);

    return ret;
}

static int do_high_priority(int argc, char **argv) {
    int ret;
    int sock = get_hci_sock();
    unsigned char acl;

    if (sock < 0)
        return sock;

    if (argc != 1) {
        usage();
        return -1;
    }

    acl = (unsigned char)atoi(argv[0]);

    ret = vendor_high_priority(sock, acl);
    close(sock);

    return ret;
}

static int do_high_priority_address(int argc, char **argv) {
    int ret;
    int sock = get_hci_sock();
    unsigned char acl;
    bdaddr_t bdaddr;

    if (sock < 0)
        return sock;

    if (argc != 1) {
        usage();
        return -1;
    }

    str2ba(argv[0], &bdaddr);

    ret = get_acl_handle(sock, bdaddr);
    if (ret < 0) goto out;

    ret = vendor_high_priority(sock, ret);

out:
    close(sock);

    return ret;
}

struct {
    char *name;
    int (*ptr)(int argc, char **argv);
} function_table[]  = {
    {"sleep", do_sleep},
    {"pri", do_high_priority},
    {"pri_addr", do_high_priority_address},
    {"", do_sleep},
    {NULL, NULL},
};

static void usage() {
    int i;

    printf("Usage:\n");
    for (i = 0; function_table[i].name; i++) {
        printf("\tbtconfig %s\n", function_table[i].name);
    }
}

int main(int argc, char **argv) {
    int i;

    if (argc < 2) {
        usage();
        return -1;
    }
    for (i = 0; function_table[i].name; i++) {
        if (!strcmp(argv[1], function_table[i].name)) {
            printf("%s\n", function_table[i].name);
            return (*function_table[i].ptr)(argc - 2, &argv[2]);
        }
    }
    usage();
    return -1;
}
