import SwiftUI

/// View displaying BLE connection status
struct ConnectionStatusView: View {
    
    let connectionState: BLEConnectionState
    let isTyping: Bool
    let typingProgress: Int
    let typingStatus: TypingStatus
    let onConnect: () -> Void
    let onDisconnect: () -> Void
    
    var body: some View {
        VStack(spacing: 12) {
            // Connection status row
            HStack {
                Circle()
                    .fill(connectionColor)
                    .frame(width: 12, height: 12)
                
                Text(connectionState.description)
                    .font(.subheadline)
                    .foregroundColor(.secondary)
                
                Spacer()
                
                connectionButton
            }
            
            // Typing status (shown when connected)
            if connectionState == .connected {
                HStack {
                    Image(systemName: statusIcon)
                        .foregroundColor(statusColor)
                        .font(.system(size: 14))
                    
                    Text(typingStatus.description)
                        .font(.subheadline)
                        .foregroundColor(statusColor)
                    
                    if isTyping {
                        Spacer()
                        
                        // Progress bar
                        ProgressView(value: Double(typingProgress), total: 100)
                            .progressViewStyle(LinearProgressViewStyle())
                            .frame(width: 80)
                        
                        Text("\(typingProgress)%")
                            .font(.caption)
                            .foregroundColor(.secondary)
                            .frame(width: 40, alignment: .trailing)
                    }
                    
                    Spacer()
                }
                .padding(.vertical, 8)
                .padding(.horizontal, 12)
                .background(Color(.systemGray6))
                .cornerRadius(8)
            }
        }
    }
    
    // MARK: - Computed Properties
    
    private var connectionColor: Color {
        switch connectionState {
        case .connected:
            return .green
        case .connecting, .scanning:
            return .yellow
        case .disconnecting:
            return .orange
        case .disconnected:
            return .gray
        }
    }
    
    private var statusColor: Color {
        switch typingStatus {
        case .idle:
            return .gray
        case .typing:
            return .blue
        case .completed:
            return .green
        case .stopped:
            return .orange
        case .error:
            return .red
        }
    }
    
    private var statusIcon: String {
        switch typingStatus {
        case .idle:
            return "keyboard"
        case .typing:
            return "keyboard.and.key"
        case .completed:
            return "checkmark.circle"
        case .stopped:
            return "stop.circle"
        case .error:
            return "exclamationmark.triangle"
        }
    }
    
    @ViewBuilder
    private var connectionButton: some View {
        switch connectionState {
        case .connected:
            Button(action: onDisconnect) {
                Text("Disconnect")
                    .font(.caption)
                    .foregroundColor(.red)
            }
        case .disconnected:
            Button(action: onConnect) {
                Text("Connect")
                    .font(.caption)
                    .foregroundColor(.blue)
            }
        case .scanning, .connecting, .disconnecting:
            ProgressView()
                .scaleEffect(0.8)
        }
    }
}