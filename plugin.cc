#include "../cracker/cracker-plugin.h"

/** Any initialization the plugin needs; should set data to zero if no
 *  data is needed to be passed around, otw. to an opaque handle
 * */
extern "C" void crackerPluginInit(void ** const data) {
}

/** Loads a file by full path, so we can try to decrypt it
 * @arg fullPath is a file name with path
 * @data is whatever is set by crackerPluginInit
 * @error is set to null (0) or an error string if there is an error
 * @code is set to zero or an error code if there is an error
 * @returns true iff file was loaded
 * */
extern "C" bool crackerPluginLoadFile(const char * const fullPath, void * const data, const char ** const error, int * const code) {
    if (error) *error = "unimplemented";
    if (code) *code = 501;
    return false;
}

/** Attempts to decrypt the loaded data file with the given password.
 *  @returns true iff decryption successful
*/
extern "C" bool crackerPluginDecrypt(const char * const pass, void * const data) {
    return false;
}

/** Handles any cleanup that's needed when we're done using this plugin
 *  @returns true iff it succeeds
*/
extern "C" bool crackerPluginFinalize(void * const data) {
    return true;
}