
## Intellichlor IC-40 messages:

C++ program to communicate with a Pentair Intellichlor sanitizer; such as an IC40. 

Requirements:

	Boost PropertyTree

	Boost ASIO

Build with cmake.

Refer to -h options output for options. Note that all verbose numeric output is in hex.

Requires an USB-RS485 converter or other appropriate RS485 interface.

The IntelliChlor IC40 puts messages on the bus that begin with a 0x10 0x02 (DLE/STX) sequence, a data field, a single byte checksum and end with a 0x10 0x03 (DLE/ETX)trailer sequence... The checksum is the unsigned sum over the data field plus the DLE/STX header bytes.

Known Intellichlor Requests/Responses


<table>
  <tr>
   <td><strong>IC40 Requests</strong>
   </td>
   <td>Value
   </td>
   <td>Startup response
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>IC40_Status
   </td>
   <td>0x00
   </td>
   <td>yes
   </td>
   <td>Simple status request.
   </td>
  </tr>
  <tr>
   <td>Set Sanitizer output percent
   </td>
   <td>0x11
   </td>
   <td>no
   </td>
   <td>Set the Sanitizer Output level. The percent level is indicated on the control panel lights.
   </td>
  </tr>
  <tr>
   <td>Get Sanitizer output percent
   </td>
   <td>0x15
   </td>
   <td>yes
   </td>
   <td>Requests the sanitizer percent output from the cell. Additional data returned is not identified.
   </td>
  </tr>
  <tr>
   <td>Model/Version Request
   </td>
   <td>0x14
   </td>
   <td>yes
   </td>
   <td>Requests model and version information from cell.
   </td>
  </tr>
  <tr>
   <td><strong>IC40 Responses</strong>
   </td>
   <td>
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>IC40_status response
   </td>
   <td>0x01
   </td>
   <td>
   </td>
   <td>Not much info returned. Likely used as a polling message to validate RS485 connection and cell presence/power on.
   </td>
  </tr>
  <tr>
   <td>Set Sanitizer Output response
   </td>
   <td>0x12
   </td>
   <td>
   </td>
   <td>Returns measured salinity in PPM, cell status as indicated by Salt Level and Status lights. Additional returned data has not been identified.
   </td>
  </tr>
  <tr>
   <td>Get Sanitizer output  response
   </td>
   <td>0x16
   </td>
   <td>
   </td>
   <td>Returns Sanitizer Output level and additional data that has not been identified.
   </td>
  </tr>
  <tr>
   <td>Model/Version Response
   </td>
   <td>0x03
   </td>
   <td>
   </td>
   <td>Returns salinity in PPM and the 
   </td>
  </tr>
</table>



## IC-40 Requests/Responses

Note: All packet data is displayed in hex.


### Status Request Msg ID: 0x00


```
 0  1  2  3  4  5  6  7
10 02 50 00 00 cs 10 03

Byte 3: Msg ID get status
Byte 4: ??
```



### Status Response Msg ID: 0x01


```
 0  1  2  3  4  5  6  7  8
10 02 00 01 00 00 cs 10 03

Byte 3: 1 == OK or Status response msg-id?
Byte 4-5: ?? Seem to be always 0.
```



### Set Sanitizer Level (i.e. 0% to 100%) Msg ID: 0x11


```
 0  1  2  3  4  5  6  7
10 02 50 11 32 A5 10 03
```


Byte 4 is Percent: Acceptable value are 0..100. In the above 0x32 is 50%.

In this packet the checksum 0xA5 in byte 5. The check sum includes the DLE/STX header bytes. Byte 2 is the destination which is 0x50 for the Chlorinator.

Note: When the cell is in starting state(Good/Low Salt Level lights flashing) any SetSanitzer command does not receive a response. Although the Sanitizer Output lights reflect the new level setting.


### SET Sanitizer Percent Level Response Msg ID: 0x12


```
 0  1  2  3  4  5  6  7
10 02 50 11 64 d7 10 3 
10 02 00 12 00 84 a8 10 3

Response:
Byte 3: Response Code 0x12.
Byte 4: Measured Salinity level. (0x00 on test bench)
Byte 5: IC40 Status byte:Bits  7654 3210
				Value:   1000 0100
		Bit 7:
		Bit 2:
    Bit 0: Flow (1 no flow, 0 flow)
    Possible status associated with status byte
    Low Salt:  Salt Level LOW light RED. GOOD light off.
    High Salt: Salt level GOOD light Green and flashing.
    Cold Water: Status COLD WATER light RED.
    OK: Status CELL light on.
    INSPECT Cell: Status CELL light flashing. 



```


This works on my IC40 built in 2013. May not work on later models????


### GET Sanitizer Setting Msg ID: 0x15


```
 0  1  2  3  4  5  6  7
10 02 50 15 00 77 10 03 
10 02 00 16 4b 64 03 00 da 10 3

Response:
Byte 3: Sanitizer setting response 0x16.
Byte 4: 0x4b Temperture F.
Byte 5: Sanitizer level % 0x00..0x64
Byte 6: 0x03 ??
Byte 7: 0x00 ?? 
```



### Get IC40 version and salinity Msg ID: 0x14


```
 0  1  2  3  4  5  6  7
10 02 50 14 02 cs 10 03
```



### Version and salinity response Msg ID: 0x03


```
 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 
10 02 00 03 00 49 6e 74 65 6c 6c 69 63 68 6c 6f 72 2d 2d 34 30 cs 10 03
```


Byte 2 is destination

Byte 3 is msg id.

Byte 4 is Salt PPM ??? 00?

Bytes 5..20 “Intellichlor--40”


## IC40 Bench Test 

Specified power requirements: 22-39 VDC 7 Amp.

For testing and discovery I used a salvaged IC40 that blows the power supply fuse when the chlorine generation kicks on. For bench testing I used a Stihl battery to supply 36V to power the unit. The control panel lights, Sanitizer Output buttons, and flow-switch works fine. 

With just the control panel powered- no chlorine generation- the IC40 draws 11-12 milliAmp.


## Acknowledgements:

Additional info here:

[https://github.com/tagyoureit/nodejs-poolController/wiki/Chlorinator](https://github.com/tagyoureit/nodejs-poolController/wiki/Chlorinator)

Note: My experimentation suggests that some data at this link is incorrect. 
