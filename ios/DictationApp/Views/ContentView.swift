import SwiftUI

/// Main content view composing all app components
struct ContentView: View {
    
    @StateObject private var viewModel = DictationViewModel()
    
    var body: some View {
        NavigationView {
            ScrollView {
                VStack(spacing: 24) {
                    // Connection status section
                    ConnectionStatusView(
                        connectionState: viewModel.connectionState,
                        isTyping: viewModel.isTyping,
                        typingProgress: viewModel.typingProgress,
                        typingStatus: viewModel.typingStatus,
                        onConnect: viewModel.connect,
                        onDisconnect: viewModel.disconnect
                    )
                    
                    // Speed selector section
                    SpeedSelectorView(
                        selectedSpeed: $viewModel.selectedSpeed,
                        isDisabled: !viewModel.isConnected || viewModel.isTyping
                    )
                    
                    // Text input section
                    TextInputView(
                        text: $viewModel.inputText,
                        characterCount: viewModel.characterCount,
                        canSend: viewModel.canSend,
                        canStop: viewModel.canStop,
                        isConnected: viewModel.isConnected,
                        onSend: viewModel.sendText,
                        onStop: viewModel.stopTyping,
                        onClear: viewModel.clearText
                    )
                    
                    // Error display
                    if let error = viewModel.lastError {
                        ErrorBanner(message: error) {
                            viewModel.clearError()
                        }
                    }
                    
                    Spacer(minLength: 40)
                }
                .padding()
            }
            .navigationTitle("Dictation")
            .navigationBarTitleDisplayMode(.large)
        }
    }
}

/// Error banner component
private struct ErrorBanner: View {
    let message: String
    let onDismiss: () -> Void
    
    var body: some View {
        HStack {
            Image(systemName: "exclamationmark.triangle.fill")
                .foregroundColor(.orange)
            
            Text(message)
                .font(.subheadline)
                .foregroundColor(.primary)
            
            Spacer()
            
            Button(action: onDismiss) {
                Image(systemName: "xmark")
                    .font(.caption)
                    .foregroundColor(.secondary)
            }
        }
        .padding()
        .background(Color.orange.opacity(0.1))
        .cornerRadius(8)
    }
}