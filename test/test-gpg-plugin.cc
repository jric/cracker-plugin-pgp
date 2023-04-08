// attempt to use cracker with the gpg plugin to crack open the test file

#include <string>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h> // for O_CLOEXEC


#include "logging.hh"

using namespace std;

#define ASSERT_NOT(x, y) { if (x == y) ABORT(x) }
#define ASSERT_IS(x, y) { if (x != y) ABORT(x << " != " << y) }

enum PIPE_FILE_DESCRIPTERS
{
  READ_FD  = 0,
  WRITE_FD = 1
};

typedef struct _ChildDescriptor {
    int read_fd;
    int write_fd;
    pid_t pid;
    int err_no;
} ChildDescriptor;

const char *CRACKER_CMD = "cracker";
const char *PLUGIN_CMD[] = {
    CRACKER_CMD,
    "--checker",
    "./data/encrypted.txt.gpg",
    "--plugin",
    "libcrackerplugin_gpg.so.1.0.0",
    NULL
};

inline std::ostream& operator<<(std::ostream& os, char** str) {
    // Output each string in the array, separated by commas
    for (int i = 0; str[i] != nullptr; ++i) {
        os << str[i];
        if (str[i + 1] != nullptr) {
            os << ", ";
        }
    }
    return os;
}

// Executes the given command and returns descriptor that can be used to interact with it.
// Stdout and stderr are collected in the same file descriptor.
// @param commandAndArgs should have a null pointer to indicate no more args
// @param sendData should be true if a second pipe is needed to write to STDIN on the subprocess
// Make sure to read and write from/to the file descriptors at the right times to avoid deadlocking, else use
//   non-blocking calls (select?)
// Make sure to close the file descriptors when done with them so the pipes behave correctly.
// Make sure to free the descriptor object when done with it to avoid memory leak.
ChildDescriptor *execute(char * const *commandAndArgs, bool sendData) {
    ChildDescriptor *ret = (ChildDescriptor *)malloc(sizeof(ChildDescriptor));
    ASSERT_NOT(nullptr, ret);
    int parentToChild[2]; // parent writes to [1], child reads from [0]
    int childToParent[2]; // child writes to [1], parent reads from [0]
    int execCheck[2]; // pipe descriptors to be auto-closed on successful exec, get error otherwise

    if (sendData)
        ASSERT_IS(0, pipe(parentToChild));
    ASSERT_IS(0, pipe(childToParent));
    ASSERT_IS(0, pipe(execCheck));
    ASSERT_NOT(-1, fcntl(execCheck[0], F_SETFD, FD_CLOEXEC));
    ASSERT_NOT(-1, fcntl(execCheck[1], F_SETFD, FD_CLOEXEC));

    ret->read_fd = childToParent[READ_FD];
    ret->write_fd = sendData ? parentToChild[WRITE_FD] : -1;
    ret->err_no = 0;

    switch (ret->pid = fork()) {
        case -1: ABORT("failed to fork")
        case 0: // child
            if (sendData)
                ASSERT_IS(0, close(parentToChild [WRITE_FD]));
            ASSERT_IS(0, close(childToParent [READ_FD]));
            ASSERT_IS(0, close(execCheck[READ_FD]));
            if (sendData)
                ASSERT_NOT(-1, dup2(parentToChild[READ_FD], STDIN_FILENO));
            ASSERT_NOT(-1, dup2(childToParent[WRITE_FD], STDOUT_FILENO));
            ASSERT_NOT(-1, dup2(childToParent[WRITE_FD], STDERR_FILENO));
            execv(commandAndArgs[0], commandAndArgs);
            write(execCheck[WRITE_FD], &errno, sizeof(errno)); // let parent know what error
            ABORT("failed to exec")
        default: // parent
            if (sendData)
                ASSERT_IS(0, close(parentToChild [READ_FD]));
            ASSERT_IS(0, close(childToParent [WRITE_FD]));
            ASSERT_IS(0, close(execCheck[WRITE_FD]));
            read(execCheck[READ_FD], &ret->err_no, sizeof(ret->err_no));
            ASSERT_IS(0, close(execCheck[READ_FD]));
    }

    return ret;
}

bool is_executable_in_path(const string &executable_name) {
    const char *path_env = getenv("PATH");

    if (!path_env) {
        WARN("PATH environment variable is not set")
        return false;
    }
    char *path_env_copy = strdup(path_env);
    char *path_dir = strtok(path_env_copy, ":");
    while (path_dir) {
        string path_entry = path_dir;
        path_entry += '/';
        path_entry += executable_name;

        if (access(path_entry.c_str(), X_OK) == 0) {
            DBG1(executable_name << " exists and is executable")
            free(path_env_copy);
            return true;
        }
        path_dir = strtok(NULL, ":");
    }
    free(path_env_copy);

    return false;
}

int main(int argc, const char **argv) {
    char **plugin_cmd = static_cast<char **>(calloc(sizeof(PLUGIN_CMD), sizeof(const char *)));
    char **i = plugin_cmd;
    const char **j = PLUGIN_CMD;
    for (; *j; i++, j++) {
        *i = strdup(*j);
    }
    ChildDescriptor *cd = execute(plugin_cmd, false /* sendDat */);

    if (cd->err_no) {
        ERR("Non-zero return code " << cd->err_no << " when executing " << plugin_cmd);
        return -1;
    }

    close(cd->read_fd);
    free(cd);
    for (i = plugin_cmd; *i; i++)
        free(*i);

    return 0;
}