# org.alljoyn.locationservices.Presence.Presence version 1
## Specification
The Presence interface is used by applications to retrieve presence data from the presence service.
An application can query the service using a PresenceFilter and recieve zero or more entities that
match the filter. Alternativly an application can subscribe to the presence service using a filter. 
The service will return a PresenceTracker object.
The service will signal the application using the PresenceTracker object when the presence of an 
entity that matches the filter changes.

 

|                       |                                                                       |
|-----------------------|-----------------------------------------------------------------------|
| Version               | 1                                                                     |
| Annotation            | org.alljoyn.Bus.Secure = true                                         |


### Methods

#### PresenceSubscribe(filter) -> (presenceTracker)

|                       |                                             |
|-----------------------|---------------------------------------------|


This method returns a tracker. 
If a tracker exists that matches the filter it is returned,
otherwise a new tracker object is created.
Trackers have a lifetime and will be reaped after the lifetime is exceeded.
Input arguments:

  * **filter** --- PresenceFilter --- A filter to match entities

Output arguments:

  * **presenceTracker** --- PresenceTracker --- a PresenceTracker object that is on the AllJoyn Bus.

Errors raised by this method:

 * org.alljoyn.example.Foo.Bar.Error.BazNotHome --- with some explanation if needed

#### DoNothing()

|                       |                                             |
|-----------------------|---------------------------------------------|
| Introduced in version | n (only add this line for n > 1)            |
| Annotation            | org.freedesktop.DBus.Method.NoReply = true  |

Description of the method. It does nothing, there are no inputs, no replies and
no errors. So we can leave out all those sections below.

### Signals

#### SomethingHasBeenDone -> (what)

|                       |                                   |
|-----------------------|-----------------------------------|
| Introduced in version | n (only add this line for n > 1)  |
| Signal Type           | sessioncast                       |

(possible signal types are: unicast, sessioncast, sessionless, local broadcast,
global broadcast. As per the Interface Design guidelines, the default signal
type is sessioncast. If this signal is not sessioncast, make sure to explain
the rationale.)

Description of the signal.

Output arguments:

  * **what** --- string --- textual description of what happened


#### struct PresenceEntity

Entities are objects that are tracked by the location services.
An entity may or may not be a member of the AllJoyn bus. 
An entity may be known or anonymous. 
A known entity is registered with the service by an application. 
Entities that are discovered, and not pre-registered are designated as unknown entities.
An entity is comprised of two strings, a unique identifier and a media specific identifier. 
The unique identifier is a human readable string.
The media specific identifier will normally be the MAC address. 
Location Services uses both the unique identifier and the media specific identifier when matching 
entities. Anonymous entities will have a unique identifier of 00000000-0000-0000-0000-000000000000
and a media specific identifier of their MAC address.

A PresenceEntity is a representation of a device in the presence service. 
It contains an entity (a human readable description and a unique id of the device) and a boolean
representing wether the device is present. 

  * **entityDescriptor** --- string --- A human readable description of the entity
  * **entityMac** --- array of bytes --- the device specific ID (usually a MAC address)
  * **present** ---boolean--- Wether the device is present or not.

#### struct PresenceFilter

A PresenceFilter is used to query or subscribe to information held in the presence service.
The filter contains a regular expression to match against the entityDescriptor and a regular expression to match against the entityMac of a PresenceEntity.

  * **descriptorParser** --- string --- A regular expression used to match the entityDescriptor 
  * **macParser** --- string --- A regular expression used to match the entityMac


## References

  * The xml definition of the [Presence service] (Presence-v1.xml)
  * The [Theory of Operations] (theory-of-operations)
  * The [PresenceTracker] (PresenceTracker-v1)
  * the [PresenceContributor] (PresenceContributor)

