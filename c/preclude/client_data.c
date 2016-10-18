#include <string.h>

#include "client_data.h"
#include "const.h"

void initClientData(client_data* client) {
    client->status = OFFLINE;
    memset(client->password, 0, PASSWORD_SIZE);
    return;
}