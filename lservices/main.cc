/******************************************************************************
 *    Copyright (c) Open Connectivity Foundation (OCF) and AllJoyn Open
 *    Source Project (AJOSP) Contributors and others.
 *
 *    SPDX-License-Identifier: Apache-2.0
 *
 *    All rights reserved. This program and the accompanying materials are
 *    made available under the terms of the Apache License, Version 2.0
 *    which accompanies this distribution, and is available at
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Copyright (c) Open Connectivity Foundation and Contributors to AllSeen
 *    Alliance. All rights reserved.
 *
 *    Permission to use, copy, modify, and/or distribute this software for
 *    any purpose with or without fee is hereby granted, provided that the
 *    above copyright notice and this permission notice appear in all
 *    copies.
 *
 *     THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 *     WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 *     WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 *     AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 *     DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 *     PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 *     TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 *     PERFORMANCE OF THIS SOFTWARE.
 ******************************************************************************/
#include "Constants.h"
#include "ServiceDatabase.h"
#include "ServiceObject.h"
#include "ServiceManager.h"


using namespace std;
using namespace qcc;
using namespace ajn;

//qcc::String foo = "*";

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

        } else if (strcmp(argv[i], "-h") == 0) {
            printf("lservices\n\n");
            printf("-r  use embedded router\n");
            exit(0);
        }
    }

    /* Handle required initialization. */
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

    /* Setup debug log information. */
    QCC_SetDebugLevel(QCC_MODULE, 7);
    QCC_DbgPrintf(("%s\n", ajn::GetBuildInfo()));

    /* Install SIGINT handler */
    signal(SIGINT, SigIntHandler);

    /* Create the message bus. */
    ajn::BusAttachment* msgBus = new BusAttachment(argv[0], true, 16);

    /* Start the API. */
    ServiceManager *svcManager = new ServiceManager(msgBus);
    QStatus status = (svcManager != NULL) ? svcManager->StartApi() : ER_FAIL;
    
    /* Run until we get a SIGINT. */
    appQuit = (status != ER_OK);
    while (!appQuit) {
        svcManager->CheckApi();
        usleep(500 * 1000);
    }

    /* Cleanup. */
    delete svcManager;
    delete msgBus;

    /* Shutdown. */
    if (useBundledRouter) {
        AllJoynRouterShutdown();
    }
    AllJoynShutdown();

    return(0);
}
