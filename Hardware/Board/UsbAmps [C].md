### UsbAmps ###

54.0 x 23.0 mm ~ 1.9 sqin


#### Parts ####

|  # | Part name                        | RefDes  | DigiKey                    | DigiKey (alternative)      |
|---:|----------------------------------|---------|----------------------------|----------------------------|
|  3 | C 100nF, X7R (0805)              | C1-C3   | 399-1170-1-ND              |                            |
|  1 | DS LCD 3 digits (LCD-S301C31TF)  | DS1     | 67-1799-ND                 |                            |
|  2 | DS LED R/A green (1206) OPTIONAL | DS2-DS3 | 160-2028-1-ND              |                            |
|  1 | J USB A, receptable, horizontal  | J1      | ED2990-ND                  | ED2989-ND                  |
|  1 | P USB A, plug, horizontal        | P1      | WM17118-ND                 |                            |
|  1 | R .1, 1%, 1W (2512)              | R1      | CRA2512-FZ-R100ELFCT-ND    |                            |
|  3 | R 1K 1%, 0.125W (0805)           | R2-R4   | RMCF0805FT1K00CT-ND        |                            |
|  3 | R 2.2K 1%, 0.125W (0805)         | R5-R7   | RMCF0805FT2K20CT-ND        |                            |
|  2 | R 330, 0.125W (0805) OPTIONAL    | R8-R9   | RMCF0805FT330RCT-ND        |                            |
|  1 | Q MOSFET-N, logic-level (SOT23)  | Q1      | DMN65D8L-7DICT-ND          |                            |
|  1 | U PIC16F1934-I/PT (TQFP-44)      | U1      | PIC16F1934-I/PT-ND         |                            |
|  1 | U ZXCT1009 (SOT23)               | U2      | ZXCT1009FCT-ND             |                            |


##### LCD #####

Transflective display is used in UsbAmps in order to allow for backlight. If
backlight is not required, it is possible to use higher clarity reflective LCD.

| Part name                        | RefDes  | DigiKey                    |
|----------------------------------|---------|----------------------------|
| DS LCD 3 digits (LCD-S301C31TR)  | DS1     | 67-1788-ND                 |


#### Boards ####

There are two board versions: Left and Right. Left version is intended for USB
located on the left side of laptop. Likewise Right version is intended for USB
located on the right side. Only difference is in the display orientation. All
other properties are identical.

Board version is denoted in second letter of board code; e.g. board named
CL1452 would be Left version while CR1452 would be Right version.
