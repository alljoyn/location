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

Request presence tracking based on the supplied filter
This method returns a tracker. 
If a tracker exists that matches the filter it is returned,
otherwise a new tracker object is created.
Trackers have a lifetime and will be reaped after the lifetime is exceeded.

Input arguments:

  * **presenceFilter** --- PresenceFilter --- A filter to match entities

Output arguments:

  * **presenceTracker** --- PresenceTracker --- a PresenceTracker object that is on the AllJoyn Bus.

Errors raised by this method:

#### QueryPresence(filter) -> (presenceEntity)
|                       |                                             |
|-----------------------|---------------------------------------------|

Input Arguments:

* **presenceFilter** --- PresenceFilter --- The filter to be used to select the presence entities to return 

Output Arguments:

* **presenceEntities** --- a[PresenceEntity] --- An array of presence entities that match the presenceFilter


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

  * The xml definition of the [Presence service] (Presence-v1.xml)
  * The [Theory of Operations] (theory-of-operations)
  * The [PresenceTracker] (PresenceTracker-v1)
  * the [PresenceContributor] (PresenceContributor)