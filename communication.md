# Communication between Client and Server
Server communicates with Client by sending string commands. Once client accepts command, it sends back an binary representation of object `LRCData`. All data blocks in `LRCData` object are represented in big endian.

## LRCData

| Type | Name |
| ---- | --- |
| Header | *header* |
| Data | *data* |

## Header

| Type | Name | Offset | Description |
| ---- | --- | --- | --- |
| U16 | *signature* | 0x0000 | Signature of LRCDATA file (always should be 0x539) |
| U8 | *version* | 0x0002 | Version of LRCDATA file (current is 0x1) |
| S8[64] | *id* | 0x0003 | Unique identifier of user |
| S8 | *type* | 0x0043 | Type of data (0x0 - 0x2) |
| S32 | *length* | 0x0043 | Length of DATA block in bytes |

## Data

Data inside this block depends on `type` parameter of `header`.

#### Types of Data block:

###### 0x01 - Keyboard:

| Type | Name | Description |
| --- | --- | --- |
| U32 | *count* | Number of items |
| Keyboard[count] | *items* | Array of `Keyboard` items |

###### 0x02 - Clipboard:

| Type | Name | Description |
| --- | --- | --- |
| U32 | *count* | Number of items |
| Clipboard[count] | *items* | Array of `Clipboard` items |

## Keyboard

| Type | Name | Description |
| --- | --- | --- |
| U8 | *subtype* | Subtype of `Keyboard` item |

###### If subtype == 0x01:

| Type | Name | Description |
| --- | --- | --- |
| VKInfo | *vkInfo* | Virtual-Key information |

###### If subtype == 0x02:

| Type | Name | Description |
| --- | --- | --- |
| WNDInfo | *wndInfo* | Information about window |

## Clipboard

| Type | Name | Description |
| --- | --- | --- |
| U32 | *time* | Time of copying in buffer |
| WNDINFO | *wndInfo* | Information about window from where text was copied |
| STRING | *data* | Unicode clipboard data |

## VKInfo

Virtual-Key information

| Type | Name | Description |
| --- | --- | --- |
| U32 | *keyCode* | Virtual-Key code |
| U16 | *lang* | Language code |
| U8 | *flags* | Caps Lock / Shift flags |

## WNDInfo

Information about window

| Type | Name | Description |
| --- | --- | --- |
| String | *process* | Process name |
| String | *title* | Window title |

## String

Sequence of unicode characters (without '\0')

| Type | Name | Description |
| --- | --- | --- |
| U32 | *length* | Length of string in bytes |
| S8[] | *text* | Unicode text |
