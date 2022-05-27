#ifndef SENDFILE_H
#define SENDFILE_H

#include <stdio.h>
#include <sys/sendfile.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>

void CopyFile(const char* from, const char* to);

#endif // SENDFILE_H