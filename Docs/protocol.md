# NEWorld Client/Server Network Protocol

## Basic structure
The basic structure used by all valid NEWorld packets.

|  Name  |     Type     | Description                                       |
| :--: | :--------: | :--------------------------------------- |
|  Length  |  uint32  | The length of the payload                                   |
|  Identifier  | Identifier | Identifier showing the type of the packet. See [payload types](#type-of-payload) | 
|  Payload  | raw | Payload contained by the packet. Varies by the packet type. | 

## Type of Payload

## Serverbound packets

### Login (0x01)

This verifies that the client is compatible with the server and authenticates the credentials.

|  Name     |   Type      | Description |
|   :--:    | :------:    | :--------   |
|  username |   string    | Username    |
|  password |   string    | Password    |
|  version  | uint16    | Version     |

### GetAvailableWorldId (0x02)

This requests all available world IDs. Return packet: [GetAvailableWorldIdResult](#GetAvailableWorldIdResult-0x03). This packet has no payload.

### GetWorldInfo (0x04)

This requests basic world information related to a world. Return packet: [GetWorldInfoResult](#GetWorldInfo-0x05).

|  Name     |   Type      | Description |
|   :--:    | :------:    | :--------   |
|  id      |   uint32    | the world id   |

### GetChunk (0x06)

This requests a specific chunk. Return packet: [GetChunk](#GetChunkResult-0x07).

|  Name    |   Type      | Description |
|   :--:   | :------:    | :--------   |
|  id      |   uint32    | the world id   |
|  x       |   int32     | chunk x position  |
|  y       |   int32     | chunk y position  |
|  z       |   int32     | chunk z position  |

### PickBlock (0x08)

This requests a specific block to be removed. Return packet: [GeneralOperationResult](#GeneralOperationResult-0x09).

|  Name    |   Type      | Description |
|   :--:   | :------:    | :--------   |
|  id      |   uint32    | the world id   |
|  x       |   int32     | block x position  |
|  y       |   int32     | block y position  |
|  z       |   int32     | block z position  |

## Clientbound packets

### GetAvailableWorldIdResult (0x03)

|  Name     |   Type      | Description |
|   :--:    | :------:    | :--------   |
|  ids      |   array<uint32>    | all available world ids    |

### GetWorldInfo (0x05)

This packet has a map structure.

|  Key     |   Type      |   Value     |   Type      |Description |
|   :--:    | :------:    | :------:   | :------:   | :--------   |
|  name     |  string  |  name |  string   | The name of the world.    |


### GetChunkResult (0x07)

|  Name     |   Type      | Description |
|   :--:    | :------:    | :--------   |
|  chunk_data   |   bin   | chunk data    |


### GeneralOperationResult (0x09)

|  Name     |   Type      | Description |
|   :--:    | :------:    | :--------   |
|  success   |   bool   | whether the operation is successful   |
|  reason   |   string   | (only when `success == false`) the reason for failure   |

