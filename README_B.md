### UsbAmps (revision B) ###

UsbAmps is a small USB pass-through device that measures current, voltage and
power used by whatever is connected on its output.

By default it shows current value but you can also recall minimum and maximum.

![UsbAmps, revision B board](Wiki/B/Picture.jpg)


#### Usage ####

Two bears are actually touch-sensitive buttons. Outside button determines what
are you measuring (current, voltage, or power) and inner button determines
whether you will see current value or minimum/maximum.

* Button 1: Current (A) / Voltage (V) / Power (W)
* Button 2: Actual / Maximum / Minimum
* Button 1&2: Resets minimum/maximum.


#### Schematics ####

![UsbAmps, revision B schematics](Wiki/B/Schema.png)


#### Specifications ####

|         | Input ratings            |
|---------|--------------------------|
| Voltage | 5 ±10% V                 |
| Current | 10 mA (typical)          |

|         | Output ratings           |
|---------|--------------------------|
| Current | 2 A (maximum)            |


##### ADC resolution #####

10-bit ADC enables 1024 distinct measurement steps. Since 2.048 V is used as
voltage reference this brings us to resolution of 2 mA for current measurement.
Voltage has an additional divider and thus has resolution of 6.4 mV.

Since device averages multiple measurements you can also expect values
in-between.


##### Display resolution #####

Display will show resolution of 1 mA up to 100 mA. All other measurements are
shown with resolution of 10 mA/mV/mW (e.g. 5.10 V).


##### Accuracy #####

Accuracy is given as ±(percent of reading + [counts](http://www.youtube.com/watch?v=U4JFeU-o2kc)
of least significant digit).

Due to averaging, it will usually do better than this, but don't count on it as
a precision device.


###### DC current ######

| Range          | Resolution | Accuracy |
|:--------------:|:----------:|:--------:|
| 0 - 99 mA      | 1 mA       | 1% + 6   |
| 0.10 - 2.00 A  | 0.01 A     | 1% + 4   |


###### DC voltage ######

| Range          | Resolution | Accuracy |
|:--------------:|:----------:|:--------:|
| 5.00 ±10% V    | 0.01 V     | 1% + 2   |


#### FAQ ####

##### Why is Rout for current measurement 1kO? #####

Since sense resistor is 100mO, having 1kO gives us nice 1V/1A. Maximum current
that we actually have to measure is 1.5A (USB battery specification). Having
2.048V as a voltage reference means that current measurements are restricted to
2A which gives us some headroom. Yes, measurements is mA range are quite
imprecise but when was last time you saw USB device that pulls only 10 mA?


##### Why is 554 mA shown as 0.55 A? #####

Since device is intended for measuring up to 1.5 A (USB battery specification)
and decimal point is rather small, there is a chance that you can mistake
1.23 A for 123 mA when looking from a far. If your vision is better than mine
or you don't deal much with higher currents, you can download source and change
`THREE_DIGIT_MILLIAMPS` define in `lcd.c` to `true`.

Notice that this means your accuracy is 1% + 6 in that range (same as for
0 - 99 mA range).


##### Device shows some mA value even when not plugged in. #####

You can calibrate device to show 0 when there is no current passing through.

While plugging device in hold both buttons together, releasing outside one
first. Device will then display CAL on screen and do 0-point calibration.

Nothing else should be connected to UsbAmps at this time.


#### Other projects ####

You can check my other projects at [www.jmedved.com](http://www.jmedved.com/electronics/).
