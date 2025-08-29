RTC initialization:

 	Reset RTC

 		(Register 0x2) = 1 // Resets

 	Release Reset (release built into reset function)

 		(Register 0x2) = 0 // Normal



 	========================================================



 	Set Seconds

 		(Register 0x8) = BCD(seconds)
 	Set Minutes

 		(Register 0x9) = BCD(minutes)

 	Set Hours (plus format)

 		(Register 0xA) = if (format == HOUR24) {

 				 	regs->hours\_reg.bcd\_24hr.value = BIN2BCD(time->tm\_hour);

        				regs->hours\_reg.bits\_24hr.f\_24\_12 = HOUR24;

 				 } else if (format == HOUR12) {

        				uint8\_t hr\_12, pm;

        				to\_12hr(time->tm\_hour, \&hr\_12, \&pm);

        				regs->hours\_reg.bcd\_12hr.value = BIN2BCD(hr\_12);

        				regs->hours\_reg.bits\_12hr.f\_24\_12 = HOUR12;

        				regs->hours\_reg.bits\_12hr.am\_pm = pm;

    				} else {

      					pr\_err("Invalid Hour Format!");

        				return MAX3133X\_INVALID\_TIME\_ERR;

    				}

 	Set Days...Year

 		(Register 0xB-E) = BCD(...)

 

 	!!! Calculate 7 bytes (seconds to year) in one go and transfer data once

 	========================================================

 

 	Read Same registers to verify RTC Works as expected

 

 	!!! Read back 7 bytes (seconds to year) in one read command

 	========================================================

RTC Calibration:

 	Enable uncalibrated CLKO at 32kHz:

 		(Register 0x4) = 0b111 (uncorrected 32kHz on INTB)



 	========================================================



 	Measure freq using logic analyzer or oscilloscope



 	========================================================



 	Apply offset:
ACCuracy = ((Meas - 32,768) \* 10^6 ) / 32,768

 		Offset  = int{ACC/0.477}

 		Offset (Regs 0x1D,1E) = offset{15:8}, offset{7:0}

 

 	========================================================

Enable Interrupts:

 	Enable CLKO on INTB (timer and all alarms move to INTA)

 		(Register 0x4) = 0b100 (Corrected 1Hz on INTB)

 

 	========================================================



 	Configure Timer

 		(Register 0x6) = 1010 (64Hz timer that loops back to initial value once it reaches zero)

 		(Register 0x19) = 1 (timer is has initial value of 1)

 		(Register 0x1) = 0b100 (enables timer interrupt)

 

 			(Read 0x0) should be 0b100 for timer

 			This clears interrupt status after each interrupt

 

 	========================================================



Update time:

 	Every interrupt, read status bit

