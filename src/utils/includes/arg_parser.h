#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <getopt.h>
#include <bits/getopt_core.h>
#include "../../includes/types.h"



void print_helper();
int parse_args(int ac, char **av, arg_parser_t *args);
void print_version();