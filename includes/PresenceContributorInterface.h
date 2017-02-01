/******************************************************************************
 *    Copyright (c) Open Connectivity Foundation (OCF), AllJoyn Open Source
 *    Project (AJOSP) Contributors and others.
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
 *    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 *    WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 *    WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 *    AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 *    DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 *    PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 *    TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 *    PERFORMANCE OF THIS SOFTWARE.
******************************************************************************/
#ifndef PRESENCE_CONTRIBUTOR_INTERFACE_H_
#define PRESENCE_CONTRIBUTOR_INTERFACE_H_

namespace org_alljoyn_locationservices { namespace presence { namespace Contributor {
const unsigned short int INTERFACE_VERSION = 1;

const char* const INTERFACE_NAME = 
    "org.alljoyn.locationservices.presence.Contributor";

const char* const INTERFACE_PATH = 
    "/Locationservices/Presence/Contributor";

const char* const INTERFACE_DESCRIPTION = 
    "Location Services Presence Contributor";

const char* const INTERFACE_XML =
    "<interface name=\"org.alljoyn.locationservices.presence.Contributor\">"
    "  <description language=\"en\">"
    "    Interface to add presence information to provider services" 
    "  </description>"
    "  <property name=\"Version\" type=\"q\" access=\"read\"/>"
    "  <method name=\"UpdatePresenceInfo\">"
    "    <annotation name=\"org.freedesktop.DBus.Method.NoReply\" value=\"true\"/>"
    "    <description language=\"en\">"
    "      Inject entity information into the provider"
    "    </description>"
    "    <arg name=\"contributor\" type=\"(ss)\" direction=\"in\"/>"
    "    <arg name=\"entity\" type=\"(ss)\" direction=\"in\"/>"
    "    <arg name=\"isPresent\" type=\"b\" direction=\"in\"/>"
    "  </method>"
    "  <method name=\"LimitContribution\">"
    "    <annotation name=\"org.freedesktop.DBus.Method.NoReply\" value=\"true\"/>"
    "    <description language=\"en\">"
    "      Limit the entities reported on"
    "    </description>"
    "    <arg name=\"presenceFilter\" type=\"((ss)a(ss))\" direction=\"in\"/>"
    "  </method>"
    "</interface>";
}}}

#endif /* PRESENCE_CONTRIBUTOR_INTERFACE_H_ */