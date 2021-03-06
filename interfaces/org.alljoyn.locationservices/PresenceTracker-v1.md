# org.alljoyn.locationservices.Presence.Tracker version 1
## Specification

PresenceTrackers have a lifetime threshold and a lifetime. 
When an instance of a PresenceTracker lifetime crosses the lifetime threshold, the service signals
the application via the PresenceTracker using the EndofLifeThreasholdReached. 
If the application calls the KeepAlive method, the lifetime threshold and the lifetime is reset.
If a KeepAlive is not recieved before the lifetime expires, the PreseneceTracker is reaped.

|                       |                                                                       |
|-----------------------|-----------------------------------------------------------------------|
| Version               | 1                                                                     |
| Annotation            | org.alljoyn.Bus.Secure = true                                         |

### Properties


#### Filter
|                       |                                                                       |
|-----------------------|-----------------------------------------------------------------------|
| Type                  | PresenceFilter                                                        |
| Access                | read-only                                                             |
| Annotation            | org.freedesktop.DBus.Property.EmitsChangedSignal = true               |

This is the filter that created the PresenceTracker.

### Methods

#### Matches() -> presenceEntity[]
|                       |                                                                       |
|-----------------------|-----------------------------------------------------------------------|
This returns the list of entities that match the filter that created the PresenceTracker.


Output Arguments:

* **presenceEntity** --- PresenceEntity[] --- An array of presence entities that match the presenceFilter


#### KeepAlive()
|                       |                                             |
|-----------------------|---------------------------------------------|
| Annotation            | org.freedesktop.DBus.Method.NoReply = true  |

### Signals

#### EntityChanged
|                       |                                   |
|-----------------------|-----------------------------------|
| Signal Type           | sessioncast                       |


This signal is emmited when an entity that matches the filter presence changes

Output arguments:

  * **entity** --- PresenceEntity --- The entity whose state has changed

#### EndOfLifeThresholdReached

|                       |                                   |
|-----------------------|-----------------------------------|
| Signal Type           | sessioncast                       |

Output arguments:

  * **secondsRemaining** --- uint32 --- The number of seconds remaining before the tracker is destroyed

This signal is emitted when the EndOfLife threshold is crossed.
Applications must call the KeepAlive method to reset the EndOfLife timer

### Named Types

#### struct Entity

Entity is a generic representation of a device
Entities are objects that are tracked by the location services.
An entity may or may not be a member of the AllJoyn bus. 
An entity may be known or anonymous. 
A known entity is registered with the service by an application. 
Entities that are discovered, and not pre-registered are designated as unknown entities.
An entity is comprised of two strings, a unique identifier and a media specific identifier. 
The unique identifier is a human readable string.
The media specific identifier is a unique descriptive string or it can be the MAC address if available. 
Location Services uses both the unique identifier and the media specific identifier when matching 
entities. Anonymous entities will have a unique identifier of 00000000-0000-0000-0000-000000000000
and a media specific identifier.

  * **entityDescriptor** --- string --- A human readable description of the entity
  * **entityMac** --- array of bytes --- the device specific ID (usually a MAC address)
  
#### struct PresenceEntity

A PresenceEntity is a representation of a device in the presence service. 
It contains an entity (a human readable description and a unique id of the device) and a boolean
representing wether the device is present. 

  * **entity** -- Entity -- The entity for the device
  * **present** --- boolean --- Wether the device is present or not.

#### struct PresenceFilter

A PresenceFilter is used to query or subscribe to information held in the presence service.
The filter contains Service settings along with selection criteria for specifying entities of interest.

  * **entityParser** --- Entity --- The entity parser is an entity filed with regular expressions for matching
  * **entityList** --- a[Entity] --- The entity list is a list of entities for matching


## References
  * theroy-of-operations
