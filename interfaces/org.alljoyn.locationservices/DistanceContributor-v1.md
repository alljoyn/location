# org.alljoyn.locationservices.Distance.Contributor version 1
## Specification
The Contributor interface allows external devices (called contributing devices) to inject distance
information into the distance service.
These devices can be sensors, cell phones or any AllJoyn device.

|                       |                                                                       |
|-----------------------|-----------------------------------------------------------------------|
| Version               | 1                                                                     |
| Annotation            | org.alljoyn.Bus.Secure = true                                         |


### Methods

#### UpdateDistanceInfo(contributor, entity, distance, distanceMultiplier, accuracy, accuracyMultiplier)

This method is called by the contributing device to add the entity to the distance service or
update the state of the entity in the distance service.

Input arguments:

 * **contributor** --- Entity --- The entity representing the contributing device
 * **entity** --- Entity --- The entity to update
 * **distance** --- double --- integer representing the distance of the device from a reference entity  
 * **distanceMultiplier** --- uint32 --- distance multiplier  
 * **accuracy** --- uint32 --- integer representing the accuracy of the measurment  
 * **accuracyMultiplier** --- uint32 --- accuracy multiplier
 
  
### Named Types

#### struct Entity

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

 * **entityDescriptor** --- string --- A human readable description of the entity
 * **entityMac** --- array of bytes --- the device specific ID (usually a MAC address)
