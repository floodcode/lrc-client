# Overview
Server communicates with Client by sending string commands. Once client accepts command, it sends back an binary representation of object `LRCData`. All data blocks in `LRCData` object are represented in big endian.

## LRCData file

| Type | Name | Offset |
| ---- | --- | --- |
| [Header](#header) | *header* | 0x0000 |
| [Data](#data) | *data* | 0x0049 |

## Header

| Type | Name | Offset | Description |
| ---- | --- | --- | --- |
| [U16](#primitive-types) | *signature* | 0x0000 | Signature of LRCDATA file (always should be 0x539) |
| [U8](#primitive-types) | *version* | 0x0002 | Version of LRCDATA file (current is 0x1) |
| [S8\[64\]](#primitive-types) | *id* | 0x0003 | Unique identifier of user |
| [S8](#primitive-types) | *type* | 0x0043 | Type of data (0x0 - 0x2) |
| [U32](#primitive-types) | *length* | 0x0044 | Length of DATA block in bytes |

## Data

Data inside this block depends on `type` parameter of `header`.

#### Types of Data block:

###### 0x00 - Error message

| Type | Name | Description |
| --- | --- | --- |
| [U32](#primitive-types) | *code* | Error code |
| [String](#string) | *message* | Error message |

###### 0x01 - Keyboard:

| Type | Name | Description |
| --- | --- | --- |
| [U32](#primitive-types) | *count* | Number of items |
| [Keyboard[count]](#keyboard) | *items* | Array of `Keyboard` items |

###### 0x02 - Clipboard:

| Type | Name | Description |
| --- | --- | --- |
| [U32](#primitive-types) | *count* | Number of items |
| [Clipboard[count]](#clipboard) | *items* | Array of `Clipboard` items |

## Keyboard

| Type | Name | Description | Condition |
| --- | --- | --- | :---: |
| [U8](#primitive-types) | *subtype* | Subtype of `Keyboard` item | `true` |
| [VKInfo](#vkinfo) | *vkInfo* | Virtual-Key information | `subtype == 0x1` |
| [WNDInfo](#wndinfo) | *wndInfo* | Information about window | `subtype == 0x2` |

## Clipboard

| Type | Name | Description |
| --- | --- | --- |
| [U32](#primitive-types) | *time* | Time of copying in buffer |
| [WNDInfo](#wndinfo) | *wndInfo* | Information about window from where text was copied |
| [String](#string) | *data* | Unicode clipboard data |

## VKInfo

Virtual-Key information

| Type | Name | Description |
| --- | --- | --- |
| [U32](#primitive-types) | *keyCode* | Virtual-Key code |
| [U16](#primitive-types) | *lang* | Language code |
| [U8](#primitive-types) | *flags* | Caps Lock / Shift flags |

## WNDInfo

Information about window

| Type | Name | Description |
| --- | --- | --- |
| [String](#string) | *process* | Process name |
| [String](#string) | *title* | Window title |

## String

Sequence of unicode characters (without '\0')

| Type | Name | Description |
| --- | --- | --- |
| [U32](#primitive-types) | *length* | Length of string in bytes |
| [S8\[length\]](#primitive-types) | *text* | Unicode text |

# Primitive types

| Type | Size in bytes | Description |
| --- | --- | --- |
| S8 | 1 | Signed 8-bit integer |
| S16 | 2 | Signed 16-bit integer |
| S32 | 4 | Signed 32-bit integer |
| U8 | 1 | Unsigned 8-bit integer |
| U16 | 2 | Unsigned 16-bit integer |
| U32 | 4 | Unsigned 32-bit integer |
