# Bright Breeze
## Embedded Systems Design Laboratory Capstone
EE 459 Spring 2023
Anjali Gopinathan, Riyana Gobin, Kimberle Sotelo

A SMART Home Ventilation and Lighting Optimizer

## Introduction
Heating, ventilation, and air conditioning (HVAC) consume approximately 40% of a building's electricity. Many air conditioning units stay on all day despite having ideal temperature conditions outdoors and sometimes are often left on even when someone leaves the house. Heating and cooling efficiency is the most effective way that people can decrease their overall energy consumption. This proposed Smart HVAC System uses smart devices and sensors in your household that allows your system to automatically adjust itself, based on a number of sensory inputs, custom settings and schedules. Being an environmentally conscious consumer-centric system, it can not only improve a building’s energy efficiency, but can also provide an occupant with optimal comfort and convenience by automatically optimizing the temperature conditions whilst requiring limited human interaction. It saves energy by adjusting to the ideal settings based on factors such as sunset time, sunrise time, user’s bedtime and wake up time. The system controls the lights, window and air conditioning system within a home according to the outdoor temperatures and preset inputs. The HVAC system would not be required to expend as much energy if the outdoor temperatures are close to the ideal temperature. In turn, use of the system will cut energy consumption and costs. In a residential context, residents would not have to worry about adjusting the temperatures or opening the windows whilst at home. By automatically adjusting indoor lighting, temperature and ventilation according to current occupancy, weather conditions and more in real-time, this SMART system can reduce wasteful energy consumption to consume no more energy resources than needed. 

## Overview

This product is a device that optimizes the lighting, temperature, and ventilation in a home to reduce energy consumption. It takes into account the brightness inside and outside the house, the temperatures inside and outside the home, and the time of day. This information combined can help the product make SMART decisions to optimize the lights, windows, and AC systems in a home. This product aims to automatically adjust the lighting inside a home, whether or not the windows and blinds should be opened, and whether the AC system should be turned on. Users can set their bedtime and wake-up time for the system to execute various actions. In Section IV, we have outlined the SMART decisions that the product makes.

## Block Diagram
![Block diagram](./img/blockDiagram.jpg "Block Diagram")

## State Machine

There are various SMART decisions that the product makes in order to achieve the goals of both home automation and saving energy consumption. 

### Light Conditions
When it is dark inside the home but bright outside the home, then the device should turn off the lights and open the blind. Conversely, if the inside is either brighter or the same as outside, then we want to turn the lights on and close the blinds. 
### Fan Conditions
If the indoor temperature is both greater than the user-set target temperature and less than 80 degrees Fahrenheit, then we want to reach our target temperature by turning the fan off and closing the windows. If both the inside and outside temperatures are over 80 degrees Fahrenheit, then the fan should stay on and the windows closed. Finally, if the outside temperature is cooler than the inside temperature and below the target temperature, then the window should open and the fan should turn off. 
### Motion Conditions
If there hasn’t been any motion indoors for over 30 minutes, then the device assumes that no one is inside and turns the lights and fan off and closes the windows and blinds.
### Sunset Conditions
If the current time is after sunset, the device assumes it is now dark outside and turns the lights on inside while closing the blinds. The lights will remain on as long as there hasn’t been a motion time out or it is the end of the day.
### End of Day Condition
If the current time is after the user’s set bedtime and before their wakeup time, then the device turns off the lights and fan and closes the windows and blinds. This is essentially shutting off the device until the current time reaches the wakeup time set by the user. Once it is wakeup time, then the device will power back on and continue to function as described above.

The state transition conditions are outlined in more detail below:

```c
Fan_On = (inside_temp >= 80) && (outside_temp >= 80) && (time_without_motion <= 30)
End_Of_Day = (wakeup_time > current_time) && (bedtime < current_time)
Window_Open = (outside_temp < inside_temp) && (time_without_motion <= 30) && (outside_temp >= 65)
Light_On = ((outside_brightness < inside_brightness) || (current_time > sunset_time)) && (time_without_motion <= 30)
Blind_Open = (outside_brightness >= inside_brightness) && (time_without_motion <= 30)
```
