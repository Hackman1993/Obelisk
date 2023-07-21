//
// Created by Hackman.Lo on 7/21/2023.
//

#include "database.h"

rosetta::database_pool g_pool(10, "127.0.0.1", 3306, "root", "hl97005497--", "go_member_sys");