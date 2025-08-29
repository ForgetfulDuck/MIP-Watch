# STM32 Fitness Tracker with MIP Display
A simple fitness tracker based on the STM32U0 microcontroller that utilizes low-power modes, DMA-based I2C data transfers, and an ultra-low-power MIP display to maximize battery life.
___
### Hardware Components
| Description | Part |
| :------- | -------:|
| Arm Cortex-M0+ MCU  | [STM32U0](/Datasheets/stm32u031f4.pdf) |
| Accelerometer with built-in pedometer  | [LSM6DSR](/Datasheets/LSM6DSR.pdf) |
| RTC  | [MAX31331](/Datasheets/MAX31331.pdf) |
| MIP display  | [TN0104ANVAANN](/Datasheets/TN0104ANVAANN-GN00_Spec.pdf) |
| Coin Cell  (4.2-3.6V, 120mAh )  | LIR2450 |

### Software + Tools
- PCB Design: KiCAD (Schematic and 4-layer board design)
- Firmware Development: STM32CubeIDE with HAL drivers
- Programming: ST-Link V2 debugger/programmer
- Version Control: Git with structured project management
___
### PCB Design
**4-Layer Stackup: Signal - GND - Power - Signal**:
|  | Image | Contents |
| :-------:  | :-------: | :-------: |
|Schematic| ![Schematic](/img/Schematic_v2.1.png)|  |
|Signal| ![PCB](/img/PCB_sig_v2.png)| Short traces, Decoupling caps, Copper pours, Test pads |
|GND| ![PCB](/img/PCB_gnd_v2.png)| A continuous ground plane to maximize signal integrity and minimize EMI |
|Power| ![PCB](/img/PCB_pwr_v2.png)|  A clean power plane for efficient power distribution with minimal voltage drop |
|DFM | ![DFM](/img/DFM_v2.png)| Designed around JLCPCB's design constraints, with design-for-testability (DFT) considerations, including test points and adequate component spacing. |
___

### System Architecture

```mermaid
---
config:
  theme: base
  themeVariables:
    primaryColor: "#2b3137"
    primaryTextColor: "#fafbfc"
    primaryBorderColor: "#fafbfc"
    lineColor: "#fafbfc"
    secondaryColor: "#24292e"
    tertiaryColor: "#0D1117"
    fontSize: "16px"
    fontFamily: "Segoe UI, Helvetica, Arial, sans-serif"
  nodeSpacing: 30
  rankSpacing: 30
---
flowchart TD
   subgraph SystemArchitecture [ ]
      A[Power On Reset] --> B[MCU Core Initialization<br>HAL, Clock, I2C Bus Clear]
      B --> C[Peripheral Setup<br>GPIO, DMA, I2C,<br>RTC, IMU, Interrupts]
      
      C --> D[Main Control Loop]
      
      D --> E{Job in Queue?}
      E --> F[No] --> G[Enter STOP Mode]
      G --> H[Wait for Sensor Interrupt]
      
      E --> I[Yes] --> J{DMA Available?}
      J --> K[No] --> L[Enter SLEEP Mode]
      L --> M[Wait for DMA Line]
      
      J --> N[Yes] --> O[Start DMA Transfer] --> L
      
      subgraph InterruptSources [Interrupt Handler]
      style InterruptSources fill:#2dba4e,stroke:#ffffff,stroke-width:2px,color:#ffffff
         P[IMU INT1: Step Detection] --> Q[Enqueue Step<br>Job]
         R[IMU INT2: Tap Detection] --> S[Enqueue Tap<br>Job]
         T[RTC INT: Timer Tick] --> U[Enqueue Time<br>Job]
         V[DMA Complete] --> W[Process Sensor Data<br>&<br>Free DMA Resources]
      end
      
      Q --> D
      S --> D
      U --> D
      W --> D
   end
```
___
### Key Features
- **Interrupt-driven firmware:** Interrupts from sensors queue jobs, preventing the need to continuously poll for updated data.
   - The IMU handles step/tap detection and only wakes the MCU when a new event occurs.
   - The RTC generates periodic interrupts for timekeeping and display refreshes.
- **Custom RTC Driver:** Wrote a platform-independent driver for the MAX31331 that provides:
   - Time/date get + set functions
   - Timer configuration + clock calibration
   - Interrupt setup
- **Direct Memory Access (DMA):** I²C data from the sensors is transferred in the background, while the CPU is in SLEEP mode.
- **Dynamic Power Management:** The firmware enters STOP mode when idle and SLEEP mode during DMA operations, waking only to process data from the sensors.
- **Memory-in-Pixel (MIP) display:** The display only consumes energy when the content is updated.

### Build & Flash
- Open project in STM32CubeIDE.
- Build firmware (`Project → Build Project`).
- Connect ST-Link V2 over SWD
   - SWDIO → SWDIO
   - SWCLK → SWCLK
   - VCC   → 3.6-5V
   - GND   → GND
- Flash firmware (`Run -> Run/Debug`)

### Debugging
   - Arm Cortex-M0+ MCUs do not support SWV, so SWO debug output is unavailable.
   - Use CubeIDE debugger with:
      - Breakpoints for interrupt handling.
      - Live Expressions to monitor variables (e.g., step counts, clock).
___
### Roadmap
- **User Input:** Add tactile buttons for UI interaction.
- **Power System:** Integrate a coin-cell holder with a BMS for charging support.
- **Peripheral Control:** Dedicate pins on the MCU to enable/disable sensors for proper power sequencing.
- **UART:** Add UART RX/TX headers for logging & debug.
- **Future Plans:** BLE, heart-rate sensor.