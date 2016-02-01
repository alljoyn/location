# org.alljoyn.locationservices.Position.Contributor version 1
## Specification
The Contributor interface allows external devices (called contributing devices) to inject position
information into the position service.
These devices can be sensors, cell phones or any AllJoyn device.

|                       |                                                                       |
|-----------------------|-----------------------------------------------------------------------|
| Version               | 1                                                                     |
| Annotation            | org.alljoyn.Bus.Secure = true                                         |


### Methods

#### UpdatePositionInfo(contributor, entity, position)

This method is called by the contributing device to add Inject entity information into the provider 

Input arguments:

 * **contributor** --- Entity --- The entity representing the contributing device
 * **entity** --- Entity --- The entity to update
 * **position** --- PositionData --- Representation of the position data  
 
  
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


#### struct PositionTime

The representation of time for the Position Service

  * **year** --- uint16 --- Representation for year
  * **month** --- byte --- Representation for month
  * **day** --- byte --- Representation for day
  * **hour** --- byte --- Representation for hour
  * **second** --- byte --- Representation for second
  * **millisecond** --- uint16 --- Representation for millisecond

 
#### struct PositionData

The representation of position data. 
The field origin defines the reference point (e.g. wgs84)

  * **origin** -- string -- Text representation fo the origin reference point
  * **x** --- uint32 --- Representation of position on x-axis
  * **y** --- uint32 --- Representation of position on y-axis
  * **z** --- uint32 --- Representation of position on z-axis
  * **horizontalAccuracy** --- uint32 --- Representation of the horizontal accuracy
  * **horizontalAccuracyMultiplier** --- uint32 --- Representation of horizontal accuracy units
  * **verticalAccuracy** --- uint32 --- Representation of the vertical accuracy
  * **verticalAccuracyMultiplier** --- uint32 --- Representation of vertical accuracy units
  * **time** --- PositionTime --- Representation of the time the position was recorded
