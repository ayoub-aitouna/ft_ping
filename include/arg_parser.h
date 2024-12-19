#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <getopt.h>
#include <bits/getopt_core.h>
#include "./types.h"
#include "./utils.h"

void print_helper();
short get_address_type(char *hostname);
int parse_args(int ac, char **av, ping_config_t *args);
void print_version();
int validate_int_value(char *str);