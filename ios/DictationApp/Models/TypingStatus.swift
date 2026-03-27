import Foundation

/// Represents the typing status received from Flipper Zero
enum TypingStatus: UInt8, CaseIterable {
    /// Typing is idle/not started
    case idle = 0x00
    
    /// Typing is in progress
    case typing = 0x01
    
    /// Typing completed successfully
    case completed = 0x02
    
    /// Typing was stopped/interrupted
    case stopped = 0x03
    
    /// An error occurred
    case error = 0xFF
    
    /// Human-readable description
    var description: String {
        switch self {
        case .idle:
            return "Idle"
        case .typing:
            return "Typing..."
        case .completed:
            return "Completed"
        case .stopped:
            return "Stopped"
        case .error:
            return "Error"
        }
    }
    
    /// Whether typing is currently active
    var isActive: Bool {
        return self == .typing
    }
    
    /// Whether the status indicates an end state
    var isFinal: Bool {
        return self == .completed || self == .stopped || self == .error
    }
}

/// Connection state for BLE
enum BLEConnectionState {
    case disconnected
    case scanning
    case connecting
    case connected
    case disconnecting
    
    var description: String {
        switch self {
        case .disconnected:
            return "Disconnected"
        case .scanning:
            return "Scanning..."
        case .connecting:
            return "Connecting..."
        case .connected:
            return "Connected"
        case .disconnecting:
            return "Disconnecting..."
        }
    }
}