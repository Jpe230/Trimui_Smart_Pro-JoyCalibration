// Copyright 2024 Jose Pablo Ramirez (@Jpe230)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

//#define DBG

void INFO(const char * MSG)
{
#ifndef DBG
    return;
#endif
#ifdef DBG
    FILE *fp = fopen("./tsp_logs.txt", "a");
    if(fp==NULL) {
        perror("Error opening file.");
        return;
    }
    fprintf(fp, "DBG: %s\n", MSG);
    fclose(fp);
#endif
}