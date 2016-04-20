/******************************************************************************
 * Copyright (c) 2016 Open Connectivity Foundation (OCF) and AllJoyn Open
 *    Source Project (AJOSP) Contributors and others.
 *
 *    SPDX-License-Identifier: Apache-2.0
 *
 *    All rights reserved. This program and the accompanying materials are
 *    made available under the terms of the Apache License, Version 2.0
 *    which accompanies this distribution, and is available at
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Copyright 2016 Open Connectivity Foundation and Contributors to
 *    AllSeen Alliance. All rights reserved.
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

using namespace ajn;
using namespace std;
using namespace qcc;
     

ServiceObject::ServiceObject(const char* path, ajn::BusAttachment* msgBus, ServiceDatabase* svcDb)
        : BusObject(path), msgBus(msgBus), svcDb(svcDb), svcIntf(NULL)
{
    QCC_DbgPrintf(("ServiceObject"));

    return;
}

ServiceObject::~ServiceObject()
{
    QCC_DbgPrintf(("~ServiceObject"));
    return;
}

QStatus ServiceObject::Start() 
{
    QCC_DbgPrintf(("Start (without arg)"));
    return ER_OK;
}

QStatus ServiceObject::Start(ajn::Message& msg) {
    QCC_DbgPrintf(("Start (with arg)"));
    return ER_OK;
}

void ServiceObject::Check() 
{
    //QCC_DbgPrintf(("Check"));
    return;
}

qcc::String ServiceObject::Lowercase(qcc::String myString) {
    for( size_t pos = 0; pos < myString.length(); pos++ ) {
        myString[pos] = std::tolower(myString[pos]);
    }
    return(myString);
}  

QStatus ServiceObject::ConfigureFilter(int entityParserMember,
                                       int entityListMember,
                                       const ajn::MsgArg& filterArg,
                                       DbFilter& dbFilter) 
{
    QStatus status = ER_OK;

    qcc::String uid = Lowercase(filterArg.v_struct.members[entityParserMember].v_struct.members[0].v_string.str);
    qcc::String descriptor = Lowercase(filterArg.v_struct.members[entityParserMember].v_struct.members[1].v_string.str);

    int listNum = (int)(filterArg.v_struct.members[entityListMember].v_array.GetNumElements());
    const MsgArg* listArgs = filterArg.v_struct.members[entityListMember].v_array.GetElements();
            
    dbFilter.AddParser(uid, descriptor);
    for (int i = 0; i < listNum; i++) {
        dbFilter.AddParser(Lowercase(listArgs[i].v_struct.members[0].v_string.str),
                           Lowercase(listArgs[i].v_struct.members[1].v_string.str));
    } 
    return status; 
}
