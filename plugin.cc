#include <gpgme.h>

#include "../cracker/cracker-plugin.h"
#include "./logging.hh"

// maintains state of plugin
typedef struct {
    
} State;

/** Any initialization the plugin needs; returns opaque state variable
 * 
 *  The only arg currently is the <encrypted filename>.
 * */
extern "C" void *crackerPluginInit(const char ** const plugin_args) {
    return new State();
}

/** Attempts to decrypt the loaded data file with the given password.
 *  @returns true iff decryption successful
*/
extern "C" bool crackerPluginDecrypt(const char * const pass, void * const state) {
    return false;
}

/** Handles any cleanup that's needed when we're done using this plugin
 *  @returns true iff it succeeds
*/
extern "C" bool crackerPluginFinalize(void * const state) {
    delete reinterpret_cast<State *>(state);
    return true;
}