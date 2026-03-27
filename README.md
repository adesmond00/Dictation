# Dictation



Device UUID (Peripheral)
E1C27A89-BF45-7A7F-0AE8-07E4B4F9D289

Custom Service UUID
0x8FE5B3D5-2E7F-4A98-2A48-7ACC60FE0000

Characteristics under that service:

0x19ED82AE-ED21-4C9D-4145-228E62FE0000 — Read, Write, Write without Response
0x19ED82AE-ED21-4C9D-4145-228E61FE0000 — Indicate, Read
0x19ED82AE-ED21-4C9D-4145-228E63FE0000 — Notify, Read
0x19ED82AE-ED21-4C9D-4145-228E64FE0000 — Notify, Read, Write





I have a Flipper Zero and I want to be able to connect it to a PC using its USB-C port and have it act as a HID keyboard. Additionally, I want to be able to connect it to my iPhone over BLE and I want to be able to send text to the Flipper Zero over BLE. Once the Flipper Zero receives that text, it should then automatically type the text as if it’s a HID keyboard. this should include formatting such as spaces, new lines, and symbols.

This will require an iOS companion app to achieve.   

The Flipper 0 uses Unleashed Firmware. 

The iOS should have a UI to connect to the flipper 0. Once connected I should see a text box, where I can type or paste content into it, then click send. Once set, the flipper 0 should start typing. While typing, I should be able to stop it early if needed either via the Flipper 0 itself, or the app. 

Typing speed should be configurable via the iPhone app. 

Device UUID (Peripheral)
E1C27A89-BF45-7A7F-0AE8-07E4B4F9D289

Custom Service UUID
0x8FE5B3D5-2E7F-4A98-2A48-7ACC60FE0000

Characteristics under that service:

0x19ED82AE-ED21-4C9D-4145-228E62FE0000 — Read, Write, Write without Response
0x19ED82AE-ED21-4C9D-4145-228E61FE0000 — Indicate, Read
0x19ED82AE-ED21-4C9D-4145-228E63FE0000 — Notify, Read
0x19ED82AE-ED21-4C9D-4145-228E64FE0000 — Notify, Read, Write

