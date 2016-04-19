/******************************************************************************
 * Copyright (c) 2015, AllSeen Alliance. All rights reserved.
 *
 *    Permission to use, copy, modify, and/or distribute this software for any
 *    purpose with or without fee is hereby granted, provided that the above
 *    copyright notice and this permission notice appear in all copies.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 ******************************************************************************/
#include "LsInclude.h"


using namespace std;
using namespace qcc;
using namespace ajn;

static bool useBundledRouter = false;
static volatile sig_atomic_t appQuit = false;
static void SigIntHandler(int sig)
{
    appQuit = true;
}

int main(int argc, char** argv) 
{
    /* Look at the command line options. */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-r") == 0) {
            useBundledRouter = true;
        }
    }

    /* Handle required initialization. */
#ifdef NEW_VERSION
    // Initialize alljoyn - this is only for versions newer than 14.12.
    if (AllJoynInit() != ER_OK) {
        return 1;
    }
    if (useBundledRouter) {
        if (AllJoynRouterInit() != ER_OK) {
            AllJoynShutdown();
            return 1;
        }
    }
#endif

    /* Setup debug log information. */
    QCC_SetDebugLevel(QCC_MODULE, 7);
    QCC_DbgPrintf(("AllJoyn Library Version: %s\n", ajn::GetVersion()));
    QCC_DbgPrintf(("AllJoyn Library Build Info: %s\n", ajn::GetBuildInfo()));

    /* Install SIGINT handler */
    signal(SIGINT, SigIntHandler);

    /* Create the message bus. */
    ajn::BusAttachment* msgBus = new BusAttachment(argv[0], true, 8);

    /* Start the API. */
    LsManager *lsManager = new LsManager(msgBus );
    QStatus status = (lsManager != NULL) ? lsManager->StartApi() : ER_FAIL;
    
    /* Run until we get a SIGINT. */
    appQuit = (status != ER_OK);
    while (!appQuit) {
        lsManager->CheckApi();
        usleep(500 * 1000);
    }

    /* Cleanup. */
    delete lsManager;
    delete msgBus;

#ifdef NEW_VERSION
    if (useBundledRouter) {
        AllJoynRouterShutdown();
    }
    AllJoynShutdown();
#endif

    return(0);
}

