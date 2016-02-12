# Communication between Client and Server
Server communicates with Client by sending string commands. Once client accepts command, it sends back an binary representation of object `LRCDATA`. All data blocks in `LRCDATA` object are represented in big endian.

## LRCData

`LRCData` object

| Type | Name |
| ---- | --- |
| Header | *header* |
| Data | *data* |

## Header

Header of `LRCData` object

| Type | Name | Offset | Description |
| ---- | --- | --- | --- |
| U16 | *signature* | 0x0000 | Signature of LRCDATA file (always should be 0x539) |
| U8 | *version* | 0x0002 | Version of LRCDATA file (current is 0x1) |
| S8[64] | *id* | 0x0003 | Unique identifier of user |
| S8 | *type* | 0x0043 | Type of data (0x0 - 0x2) |
| S32 | *length* | 0x0043 | Length of DATA block in bytes |

## DATA

`DATA` block of `LRCDATA` object

```
DATA
{
  U32       count       // Number of parts
  PART[]    parts       // Array of different data types
}
```

## PART
Data inside `PART` block depends on `type` field inside `HEADER`.

#### Part types:

###### Keyboard (type - 0x1):
```
PART
{
  U8        subtype     // Subtype of part
  
  if (subtype == 0x1)
  {
    VKINFO  vkInfo    // Information about virtual-key
  }
  
  if (subtype == 0x2)
  {
    WNDINFO wndInfo   // Information about window
  }
}
```

###### Clipboard (type - 0x2):
```
PART
{
  U32       time        // Time of copying in buffer
  WNDINFO   wndInfo     // Information about window from where text was copied
  STRING    data        // Unicode clipboard data
}
```

## VKINFO
Virtual-Key information
```
VKINFO
{
  U32       keyCode     // Virtual-Key code
  U16       lang        // Language code
  U8        flags       // Caps Lock / Shift flags
}
```

## WNDINFO
Window information
```
WNDINFO
{
  STRING    process     // Process name
  STRING    title       // Window title
}
```

## STRING
Sequence of unicode characters (without '\0')
```
STRING
{
  U32       length        // Length of string in bytes
  S8[]      text          // Unicode text
}
```
