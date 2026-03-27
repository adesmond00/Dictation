import Foundation

/// Speed preset options for typing
enum SpeedPreset: Int, CaseIterable, Identifiable {
    /// Slow speed - 150ms delay per character
    case slow = 1
    
    /// Normal speed - 75ms delay per character
    case normal = 2
    
    /// Fast speed - 30ms delay per character
    case fast = 3
    
    /// Turbo speed - 10ms delay per character
    case turbo = 4
    
    var id: Int { rawValue }
    
    /// Human-readable name
    var name: String {
        switch self {
        case .slow:
            return "Slow"
        case .normal:
            return "Normal"
        case .fast:
            return "Fast"
        case .turbo:
            return "Turbo"
        }
    }
    
    /// Description with delay info
    var description: String {
        switch self {
        case .slow:
            return "Slow (150ms)"
        case .normal:
            return "Normal (75ms)"
        case .fast:
            return "Fast (30ms)"
        case .turbo:
            return "Turbo (10ms)"
        }
    }
    
    /// Delay in milliseconds per character
    var delayMs: Int {
        switch self {
        case .slow:
            return 150
        case .normal:
            return 75
        case .fast:
            return 30
        case .turbo:
            return 10
        }
    }
    
    /// Byte value for BLE command
    var byteValue: UInt8 {
        return UInt8(rawValue)
    }
    
    /// Initialize from byte value
    init?(byteValue: UInt8) {
        switch byteValue {
        case 0x01:
            self = .slow
        case 0x02:
            self = .normal
        case 0x03:
            self = .fast
        case 0x04:
            self = .turbo
        default:
            return nil
        }
    }
}