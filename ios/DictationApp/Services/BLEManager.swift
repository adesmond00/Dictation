import Foundation
import CoreBluetooth
import Combine

/// BLE Manager for communicating with Flipper Zero
final class BLEManager: NSObject, ObservableObject {
    
    // MARK: - Singleton
    
    static let shared = BLEManager()
    
    // MARK: - Published Properties
    
    @Published private(set) var connectionState: BLEConnectionState = .disconnected
    @Published private(set) var connectedPeripheral: CBPeripheral?
    @Published private(set) var typingStatus: TypingStatus = .idle
    @Published private(set) var typingProgress: Int = 0
    @Published private(set) var lastError: String?
    
    // MARK: - Private Properties
    
    private var centralManager: CBCentralManager!
    private var discoveredPeripheral: CBPeripheral?
    
    // Characteristics
    private var txCharacteristic: CBCharacteristic?
    private var rxCharacteristic: CBCharacteristic?
    private var statusCharacteristic: CBCharacteristic?
    private var configCharacteristic: CBCharacteristic?
    
    // Characteristic UUIDs set
    private let characteristicUUIDs: Set<CBUUID> = [
        BLEConstants.charTXUUID,
        BLEConstants.charRXUUID,
        BLEConstants.charStatusUUID,
        BLEConstants.charConfigUUID
    ]
    
    // MARK: - Initialization
    
    private override init() {
        super.init()
        centralManager = CBCentralManager(delegate: self, queue: nil)
    }
    
    // MARK: - Public Methods
    
    /// Start scanning for Flipper Zero device
    func startScanning() {
        guard centralManager.state == .poweredOn else {
            lastError = "Bluetooth is not available"
            return
        }
        
        connectionState = .scanning
        lastError = nil
        
        centralManager.scanForPeripherals(
            withServices: [BLEConstants.serviceUUID],
            options: [CBCentralManagerScanOptionAllowDuplicatesKey: false]
        )
        
        // Also try scanning by device UUID as backup
        centralManager.scanForPeripherals(
            withServices: nil,
            options: [CBCentralManagerScanOptionAllowDuplicatesKey: false]
        )
    }
    
    /// Stop scanning for devices
    func stopScanning() {
        centralManager.stopScan()
        if connectionState == .scanning {
            connectionState = .disconnected
        }
    }
    
    /// Connect to a discovered peripheral
    func connect(to peripheral: CBPeripheral) {
        stopScanning()
        connectionState = .connecting
        discoveredPeripheral = peripheral
        centralManager.connect(peripheral, options: nil)
    }
    
    /// Disconnect from current peripheral
    func disconnect() {
        guard let peripheral = connectedPeripheral else { return }
        connectionState = .disconnecting
        centralManager.cancelPeripheralConnection(peripheral)
    }
    
    /// Send text to Flipper Zero to type
    func sendText(_ text: String, speed: SpeedPreset) {
        guard let txChar = txCharacteristic,
              let peripheral = connectedPeripheral,
              !text.isEmpty else {
            lastError = "Not connected or empty text"
            return
        }
        
        // Build command: [CMD_START_TYPING][SPEED][TEXT]
        var command = Data()
        command.append(BLEConstants.cmdStartTyping)
        command.append(speed.byteValue)
        command.append(contentsOf: text.utf8)
        
        // Send as write without response for streaming
        peripheral.writeValue(command, for: txChar, type: .withoutResponse)
        
        typingStatus = .typing
    }
    
    /// Send stop command to Flipper Zero
    func sendStop() {
        guard let txChar = txCharacteristic,
              let peripheral = connectedPeripheral else {
            lastError = "Not connected"
            return
        }
        
        var command = Data()
        command.append(BLEConstants.cmdStopTyping)
        
        peripheral.writeValue(command, for: txChar, type: .withoutResponse)
        typingStatus = .stopped
    }
    
    /// Change typing speed
    func changeSpeed(_ speed: SpeedPreset) {
        guard let configChar = configCharacteristic,
              let peripheral = connectedPeripheral else {
            lastError = "Not connected"
            return
        }
        
        var command = Data()
        command.append(BLEConstants.cmdChangeSpeed)
        command.append(speed.byteValue)
        
        peripheral.writeValue(command, for: configChar, type: .withResponse)
    }
    
    /// Request current status from Flipper
    func requestStatus() {
        guard let txChar = txCharacteristic,
              let peripheral = connectedPeripheral else {
            return
        }
        
        var command = Data()
        command.append(BLEConstants.cmdGetStatus)
        
        peripheral.writeValue(command, for: txChar, type: .withResponse)
    }
    
    // MARK: - Private Methods
    
    private func clearCharacteristics() {
        txCharacteristic = nil
        rxCharacteristic = nil
        statusCharacteristic = nil
        configCharacteristic = nil
    }
}

// MARK: - CBCentralManagerDelegate

extension BLEManager: CBCentralManagerDelegate {
    
    func centralManagerDidUpdateState(_ central: CBCentralManager) {
        switch central.state {
        case .poweredOn:
            lastError = nil
        case .poweredOff:
            lastError = "Bluetooth is turned off"
            connectionState = .disconnected
        case .unsupported:
            lastError = "Bluetooth is not supported"
        case .unauthorized:
            lastError = "Bluetooth is not authorized"
        case .resetting:
            lastError = "Bluetooth is resetting"
        case .unknown:
            lastError = "Bluetooth state is unknown"
        @unknown default:
            lastError = "Unknown Bluetooth state"
        }
    }
    
    func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral,
                        advertisementData: [String: Any], rssi RSSI: NSNumber) {
        
        // Check if this is our target device
        if let name = peripheral.name, name.contains("Flipper") {
            discoveredPeripheral = peripheral
            stopScanning()
            connect(to: peripheral)
        }
    }
    
    func centralManager(_ central: CBCentralManager, didConnect peripheral: CBPeripheral) {
        connectedPeripheral = peripheral
        peripheral.delegate = self
        connectionState = .connected
        clearCharacteristics()
        
        // Discover services
        peripheral.discoverServices([BLEConstants.serviceUUID])
    }
    
    func centralManager(_ central: CBCentralManager, didFailToConnect peripheral: CBPeripheral, error: Error?) {
        connectionState = .disconnected
        lastError = error?.localizedDescription ?? "Failed to connect"
        discoveredPeripheral = nil
    }
    
    func centralManager(_ central: CBCentralManager, didDisconnectPeripheral peripheral: CBPeripheral, error: Error?) {
        connectionState = .disconnected
        connectedPeripheral = nil
        clearCharacteristics()
        
        if let error = error {
            lastError = "Disconnected: \(error.localizedDescription)"
        }
    }
}

// MARK: - CBPeripheralDelegate

extension BLEManager: CBPeripheralDelegate {
    
    func peripheral(_ peripheral: CBPeripheral, didDiscoverServices error: Error?) {
        if let error = error {
            lastError = "Service discovery failed: \(error.localizedDescription)"
            return
        }
        
        guard let services = peripheral.services else { return }
        
        for service in services {
            if service.uuid == BLEConstants.serviceUUID {
                peripheral.discoverCharacteristics(
                    Array(characteristicUUIDs),
                    for: service
                )
            }
        }
    }
    
    func peripheral(_ peripheral: CBPeripheral, didDiscoverCharacteristicsFor service: CBService, error: Error?) {
        if let error = error {
            lastError = "Characteristic discovery failed: \(error.localizedDescription)"
            return
        }
        
        guard let characteristics = service.characteristics else { return }
        
        for characteristic in characteristics {
            switch characteristic.uuid {
            case BLEConstants.charTXUUID:
                txCharacteristic = characteristic
            case BLEConstants.charRXUUID:
                rxCharacteristic = characteristic
                // Subscribe for indications
                peripheral.setNotifyValue(true, for: characteristic)
            case BLEConstants.charStatusUUID:
                statusCharacteristic = characteristic
                // Subscribe for notifications
                peripheral.setNotifyValue(true, for: characteristic)
            case BLEConstants.charConfigUUID:
                configCharacteristic = characteristic
                // Subscribe for notifications
                peripheral.setNotifyValue(true, for: characteristic)
            default:
                break
            }
        }
    }
    
    func peripheral(_ peripheral: CBPeripheral, didUpdateValueFor characteristic: CBCharacteristic, error: Error?) {
        if let error = error {
            lastError = "Read failed: \(error.localizedDescription)"
            return
        }
        
        guard let data = characteristic.value else { return }
        
        switch characteristic.uuid {
        case BLEConstants.charStatusUUID:
            // Status notification: [STATUS][PROGRESS]
            if data.count >= 2 {
                let statusByte = data[0]
                let progressByte = data[1]
                
                if let status = TypingStatus(rawValue: statusByte) {
                    typingStatus = status
                    typingProgress = Int(progressByte)
                }
            } else if data.count >= 1 {
                if let status = TypingStatus(rawValue: data[0]) {
                    typingStatus = status
                }
            }
            
        case BLEConstants.charRXUUID:
            // RX indication (acknowledgments)
            if data.count >= 1 {
                let cmd = data[0]
                // Handle acknowledgment for commands
                if cmd == BLEConstants.cmdChangeSpeed {
                    // Speed change acknowledged
                }
            }
            
        case BLEConstants.charConfigUUID:
            // Config notification
            break
            
        default:
            break
        }
    }
    
    func peripheral(_ peripheral: CBPeripheral, didWriteValueFor characteristic: CBCharacteristic, error: Error?) {
        if let error = error {
            lastError = "Write failed: \(error.localizedDescription)"
        }
    }
    
    func peripheral(_ peripheral: CBPeripheral, didUpdateNotificationStateFor characteristic: CBCharacteristic, error: Error?) {
        if let error = error {
            lastError = "Notification setup failed: \(error.localizedDescription)"
        }
    }
}