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
#ifndef POSITION_CONTRIBUTOR_INTERFACE_H_
#define POSITION_CONTRIBUTOR_INTERFACE_H_

namespace org_alljoyn_locationservices { namespace position { namespace Contributor {
const unsigned short int INTERFACE_VERSION = 1;

const char* const INTERFACE_NAME = 
    "org.alljoyn.locationservices.position.Contributor";

const char* const INTERFACE_PATH = 
    "/Locationservices/Position/Contributor";

const char* const INTERFACE_DESCRIPTION = 
    "Location Services Position Contributor";

const char* const INTERFACE_XML =
    "<interface name=\"org.alljoyn.locationservices.position.Contributor\">"
    "  <description language=\"en\">"
    "    Interface to add position information to provider services" 
    "  </description>"
    "  <property name=\"Version\" type=\"q\" access=\"read\"/>"
    "  <method name=\"UpdatePositionInfo\">"
    "    <annotation name=\"org.freedesktop.DBus.Method.NoReply\" value=\"true\"/>"
    "    <description language=\"en\">"
    "      Inject entity information into the provider"
    "    </description>"
    "    <arg name=\"contributor\" type=\"(ss)\" direction=\"in\"/>"
    "    <arg name=\"entity\" type=\"(ss)\" direction=\"in\"/>"
    "    <arg name=\"position\" type=\"(sdddd)\" direction=\"in\"/>"
    "  </method>"
    "</interface>";
}}}

#endif /* POSITION_CONTRIBUTOR_INTERFACE_H_ */