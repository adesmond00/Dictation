import Foundation
import CoreBluetooth

/// BLE UUID constants for Dictation app communication with Flipper Zero
enum BLEConstants {
    
    // MARK: - Device UUID
    
    /// Device UUID (Peripheral) for Flipper Zero
    static let deviceUUID = UUID(uuidString: "E1C27A89-BF45-7A7F-0AE8-07E4B4F9D289")!
    
    // MARK: - Service UUID
    
    /// Custom Service UUID for Dictation
    static let serviceUUID = CBUUID(string: "8FE5B3D5-2E7F-4A98-2A48-7ACC60FE0000")
    
    // MARK: - Characteristic UUIDs
    
    /// TX Characteristic - Write, Write Without Response, Read
    /// Used by app to send commands/text to Flipper
    static let charTXUUID = CBUUID(string: "19ED82AE-ED21-4C9D-4145-228E62FE0000")
    
    /// RX Characteristic - Indicate, Read
    /// Used by Flipper to send confirmations to app
    static let charRXUUID = CBUUID(string: "19ED82AE-ED21-4C9D-4145-228E61FE0000")
    
    /// Status Characteristic - Notify, Read
    /// Used by Flipper to send typing progress/status to app
    static let charStatusUUID = CBUUID(string: "19ED82AE-ED21-4C9D-4145-228E63FE0000")
    
    /// Config Characteristic - Notify, Read, Write
    /// Used for speed settings and control
    static let charConfigUUID = CBUUID(string: "19ED82AE-ED21-4C9D-4145-228E64FE0000")
    
    // MARK: - Command Bytes
    
    /// Command: Start typing text at specified speed
    static let cmdStartTyping: UInt8 = 0x01
    
    /// Command: Immediate stop and clear buffer
    static let cmdStopTyping: UInt8 = 0x02
    
    /// Command: Update typing speed only
    static let cmdChangeSpeed: UInt8 = 0x03
    
    /// Command: Get current status
    static let cmdGetStatus: UInt8 = 0x04
    
    // MARK: - Status Codes
    
    /// Status: Idle
    static let statusIdle: UInt8 = 0x00
    
    /// Status: Typing in progress
    static let statusTyping: UInt8 = 0x01
    
    /// Status: Typing completed
    static let statusCompleted: UInt8 = 0x02
    
    /// Status: Stopped/Interrupted
    static let statusStopped: UInt8 = 0x03
    
    /// Status: Error
    static let statusError: UInt8 = 0xFF
    
    // MARK: - Speed Presets
    
    /// Speed value for Slow (150ms delay)
    static let speedSlow: UInt8 = 0x01
    
    /// Speed value for Normal (75ms delay)
    static let speedNormal: UInt8 = 0x02
    
    /// Speed value for Fast (30ms delay)
    static let speedFast: UInt8 = 0x03
    
    /// Speed value for Turbo (10ms delay)
    static let speedTurbo: UInt8 = 0x04
}