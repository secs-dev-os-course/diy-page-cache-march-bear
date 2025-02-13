#pragma once

#define BLOCK_SIZE 2 * 1024
#define IEXIT_CODE_WRONG_ARGC -1
#define IEXIT_CODE_WRONG_ARG_FORMAT -2
#define IEXIT_CODE_CANNOT_OPEN_FILE -3
#define IEXIT_CODE_WRITING_ERROR -4
#define FILL_CH 'A'

int do_io_lat_write(long rep, char* filename);