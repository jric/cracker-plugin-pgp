// attempt to use cracker with the gpg plugin to crack open the test file

#include <string>
#include <vector>
#include <cstdlib>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h> // for O_CLOEXEC
#include <boost/process.hpp>
#include <iostream>
#include <algorithm> // for copy

#include "../logging.hh"

using namespace std;
namespace bp = boost::process;

#define ASSERT_NOT(x, y) { if (x == y) ABORT(x) }
#define ASSERT_IS(x, y) { if (x != y) ABORT(x << " != " << y) }

// foo is the real pwd; off by one to see if it can crack it!
#define SEED_PWD "foo"
// set distance to one when it's time to crack it
#define DISTANCE "0"

const char *CRACKER_CMD = "cracker";
vector<string> plugin_args = {
    "--checker",
    "./data/encrypted.txt.gpg",
    "--distance",
    DISTANCE,
    "--plugin",
    "../libcrackerplugin_gpg.so.1.0.0"
};

inline std::ostream& operator <<(std::ostream& os, char * const *str) {
    for (int i = 0; str[i] != nullptr; ++i) {
        os << str[i];
        if (str[i + 1] != nullptr) {
            os << ", ";
        }
    }
    return os;    
}

/* Output each string in the array, separated by commas */
inline std::ostream& operator<<(std::ostream& os, char** str) {
    return os << const_cast<char * const *>(str);
}

/* Copy input stream to output stream */
std::ostream & operator<<(std::ostream& os, std::istream& is) {
    is >> std::noskipws;

    copy(std::istream_iterator<char>(is), 
              std::istream_iterator<char>(), 
              std::ostream_iterator<char>(os));
    return os;
}

/* output vector to stream */
template <typename T>
std::ostream & operator<<(std::ostream & os, std::vector<T>& v) {
    os << '[';
    typename vector<T>::const_iterator vit = v.cbegin();
    if (vit < v.cend()) os << *vit;
    while (++vit < v.cend()) {
        os << ", " << *vit;
    }
    os << ']';

    return os;
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
    if (!is_executable_in_path(CRACKER_CMD)) {
        ERR(CRACKER_CMD << " not in path, " << getenv("PATH"));
        return -2;
    }

    setenv("SEED_PWD", SEED_PWD, 1 /* overwrite */);

    bp::ipstream err_stream;
    try {
        bp::child c(bp::search_path(CRACKER_CMD), plugin_args, bp::std_err > err_stream);

        c.wait();

        if (c.exit_code() == 0)
        {
            INFO("Test passed!");
        }
        else
        {
            INFO("Command failed with exit code " << c.exit_code());
            INFO("Error output: " << err_stream);
        }
    } catch (const bp::process_error& e)
    {
        ERR("Error executing command: " << e.what() << "; command was: " << CRACKER_CMD << " " << plugin_args);
        return -1;
    }

    return 0;
}