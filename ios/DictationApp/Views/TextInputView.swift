import SwiftUI

/// View for text input with send functionality
struct TextInputView: View {
    
    @Binding var text: String
    let characterCount: Int
    let canSend: Bool
    let canStop: Bool
    let isConnected: Bool
    let onSend: () -> Void
    let onStop: () -> Void
    let onClear: () -> Void
    
    @FocusState private var isFocused: Bool
    
    var body: some View {
        VStack(alignment: .leading, spacing: 12) {
            // Header
            HStack {
                Text("Text to Type")
                    .font(.subheadline)
                    .foregroundColor(.secondary)
                
                Spacer()
                
                if characterCount > 0 {
                    Text("\(characterCount) characters")
                        .font(.caption)
                        .foregroundColor(.secondary)
                }
            }
            
            // Text editor
            ZStack(alignment: .topLeading) {
                TextEditor(text: $text)
                    .focused($isFocused)
                    .frame(minHeight: 150, maxHeight: 250)
                    .padding(8)
                    .background(Color(.systemGray6))
                    .cornerRadius(12)
                    .disabled(!isConnected)
                
                // Placeholder
                if text.isEmpty && isConnected {
                    Text("Enter or paste text here...")
                        .foregroundColor(.secondary)
                        .padding(.horizontal, 12)
                        .padding(.vertical, 16)
                        .allowsHitTesting(false)
                }
                
                // Disabled overlay
                if !isConnected {
                    Color(.systemGray5).opacity(0.7)
                        .cornerRadius(12)
                    
                    VStack {
                        Spacer()
                        Text("Connect to Flipper to type text")
                            .font(.subheadline)
                            .foregroundColor(.secondary)
                        Spacer()
                    }
                    .frame(maxWidth: .infinity)
                }
            }
            
            // Action buttons
            HStack(spacing: 12) {
                // Clear button
                if !text.isEmpty && isConnected && !canStop {
                    Button(action: onClear) {
                        Label("Clear", systemImage: "xmark.circle")
                            .font(.subheadline)
                    }
                    .buttonStyle(.bordered)
                }
                
                Spacer()
                
                // Stop button (shown while typing)
                if canStop {
                    Button(action: onStop) {
                        Label("Stop", systemImage: "stop.fill")
                            .font(.subheadline)
                            .foregroundColor(.white)
                    }
                    .buttonStyle(.borderedProminent)
                    .tint(.red)
                }
                
                // Send button
                if isConnected && !canStop {
                    Button(action: onSend) {
                        Label("Send & Type", systemImage: "paperplane.fill")
                            .font(.subheadline)
                            .foregroundColor(.white)
                    }
                    .buttonStyle(.borderedProminent)
                    .tint(.blue)
                    .disabled(!canSend)
                    .opacity(canSend ? 1.0 : 0.5)
                }
            }
        }
    }
}