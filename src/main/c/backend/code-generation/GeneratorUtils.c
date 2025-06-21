#include <stdio.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include "../../shared/Environment.h"
#include "../../shared/String.h"
#include "../../shared/Logger.h"

static FILE   *asmFile   = NULL;
static Logger *outLogger = NULL;





static void initOutputFileOnce(void) {
    if (asmFile) return;
    if (!outLogger) outLogger = createLogger("GeneratorUtils");

    const char *dir = "/home/ubuntu/Flex-Bison-Compiler/src/test/asm/output";

    if (mkdir(dir, 0755) == -1 && errno != EEXIST) {
        LogWarning(outLogger, "mkdir(%s) failed: %s", dir, strerror(errno));
        return;
    }

    const char *fileName = getStringOrDefault("OUTPUT_FILE_NAME", "OUTPUT.asm");
    size_t pathLen = strlen(dir) + 1 + strlen(fileName) + 1;
    char *fullPath = malloc(pathLen);
    if (!fullPath) {
        LogWarning(outLogger, "malloc(%zu) failed", pathLen);
        return;
    }
    snprintf(fullPath, pathLen, "%s/%s", dir, fileName);

    asmFile = fopen(fullPath, "w");
    if (!asmFile) LogWarning(outLogger, "fopen(%s) failed: %s", fullPath, strerror(errno));
    free(fullPath);
}

static char *pad(unsigned n) { return indentation(' ', n, 4); }

void out(unsigned n, const char *fmt, ...)
{
    va_list ap, ap_copy;
    va_start(ap, fmt);
    va_copy(ap_copy, ap);

    char *p  = pad(n);
    char *ef = concatenate(2, p, fmt);

    vfprintf(stdout, ef, ap);
    fflush(stdout);

    initOutputFileOnce();
    if (asmFile)
        vfprintf(asmFile, ef, ap_copy);

    if (!asmFile)
        LogWarning(outLogger, "ASM file not available; output lost");

    fflush(asmFile);
    free(ef);
    free(p);
    va_end(ap_copy);
    va_end(ap);
}




void shutdownGeneratorUtils(void) {
    if (asmFile) {
        fflush(asmFile);
        fclose(asmFile);
        asmFile = NULL;
    }
    if (outLogger) {
        destroyLogger(outLogger);
        outLogger = NULL;
    }
}
