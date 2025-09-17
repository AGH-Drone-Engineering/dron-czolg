SBUS Data Frame Structure
| Byte Index | Purpose |
| ---------- | ------- |
| **Byte 0** | Start byte — always `0x0F`. Marks the beginning of a frame. |
| **Bytes 1-22** | Packed data for 16 RC control channels. Each channel has **11 bits**, so total = 16 × 11 = 176 bits. These are packed across these 22 bytes, least significant bits (LSBs) first. |
| **Byte 23** | Flag byte: includes additional / digital channels (17 & 18), `frame_lost` flag, `failsafe` flag. |
| **Byte 24** | End byte — always `0x00`. Marks the end of the frame. |

Useful link: https://uwarg-docs.atlassian.net/wiki/spaces/ZP/pages/2238283817/SBUS+Protocol

16 Channels meaning
| **Channel** | **Typical use** |
| ----------- | --------------------------------------------------------------- |
| CH1 | Roll (Aileron stick, left/right) |
| CH2 | Pitch (Elevator stick, forward/back) |
| CH3 | Throttle (up/down stick) |
| CH4 | Yaw (Rudder stick, left/right) |
| CH5 | Arm switch (2-position switch) |
| CH6 | Flight mode switch (3-position switch) |
| CH7 | Auxiliary switch (e.g. beeper, lights) |
| CH8 | Auxiliary switch |
| CH9–16 | More aux functions (camera control, gimbal, custom mixes, etc.) |
CH17 - CH18 | Digital (on/off switches). Stored in flags byte |

Values:
Value ranges:

- The analog channels (ch-1 to ch-16) typically return values in the range ~ 172 to 1811.
- With extended limits (or special configurations) you might see the full 11-bit range (0-2047).

For more information use README from this repo: https://github.com/bolderflight/sbus
