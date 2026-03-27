import Foundation
import Combine

/// Main ViewModel for Dictation app
@MainActor
final class DictationViewModel: ObservableObject {
    
    // MARK: - Published Properties
    
    @Published var inputText: String = ""
    @Published var selectedSpeed: SpeedPreset = .normal
    @Published private(set) var isTyping: Bool = false
    @Published private(set) var typingProgress: Int = 0
    @Published private(set) var typingStatus: TypingStatus = .idle
    @Published private(set) var connectionState: BLEConnectionState = .disconnected
    @Published private(set) var lastError: String?
    
    // MARK: - Private Properties
    
    private let bleManager = BLEManager.shared
    private var cancellables = Set<AnyCancellable>()
    
    // MARK: - Computed Properties
    
    var isConnected: Bool {
        connectionState == .connected
    }
    
    var canSend: Bool {
        isConnected && !inputText.isEmpty && !isTyping
    }
    
    var canStop: Bool {
        isConnected && isTyping
    }
    
    var characterCount: Int {
        inputText.count
    }
    
    // MARK: - Initialization
    
    init() {
        setupBindings()
    }
    
    // MARK: - Public Methods
    
    /// Connect to Flipper Zero
    func connect() {
        bleManager.startScanning()
    }
    
    /// Disconnect from Flipper Zero
    func disconnect() {
        bleManager.disconnect()
    }
    
    /// Send text to Flipper Zero for typing
    func sendText() {
        guard canSend else { return }
        
        let text = inputText
        let speed = selectedSpeed
        
        bleManager.sendText(text, speed: speed)
        inputText = ""
    }
    
    /// Stop typing immediately
    func stopTyping() {
        guard canStop else { return }
        bleManager.sendStop()
    }
    
    /// Clear input text
    func clearText() {
        inputText = ""
    }
    
    /// Clear error message
    func clearError() {
        lastError = nil
    }
    
    // MARK: - Private Methods
    
    private func setupBindings() {
        // Bind BLE manager states to view model
        bleManager.$connectionState
            .receive(on: DispatchQueue.main)
            .sink { [weak self] state in
                self?.connectionState = state
            }
            .store(in: &cancellables)
        
        bleManager.$typingStatus
            .receive(on: DispatchQueue.main)
            .sink { [weak self] status in
                self?.typingStatus = status
                self?.isTyping = status.isActive
            }
            .store(in: &cancellables)
        
        bleManager.$typingProgress
            .receive(on: DispatchQueue.main)
            .sink { [weak self] progress in
                self?.typingProgress = progress
            }
            .store(in: &cancellables)
        
        bleManager.$lastError
            .receive(on: DispatchQueue.main)
            .sink { [weak self] error in
                self?.lastError = error
            }
            .store(in: &cancellables)
    }
}