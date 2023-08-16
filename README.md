# Stepper CBK

This program controls stepper motor via Arduino program. After asking about current position and 
target position it sends information to go to that position.  
The velocity grows gradually (100 steps/second to 500 steps/second).


Included devices:
- Stepper motor
- Arduino Leonardo
- controller Tic 36v4
- 2x Limit Switch 
- LCD screen (20, 4)
- USB cable
 

The program calculate the distance("zmiana") as a difference between "zadana_pos" and "obecna_pos".  
Then it calculate the distance ("odl") in steps. "odl" = "zmiana" 2 / 3 * 100.  
The next step is to calculate time ("czekanie") while we wait for stepper to achieve target position. "czekanie" = (abs("odl")) * 4 + 5000.  

You have to type the position between (0 - 500).  
Watch out for putting the incorrect position.  
If the position you put is not from the scope and either of Limit Switch was touched the stepper motor should stop instantly.
