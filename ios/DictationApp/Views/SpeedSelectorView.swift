import SwiftUI

/// View for selecting typing speed preset
struct SpeedSelectorView: View {
    
    @Binding var selectedSpeed: SpeedPreset
    let isDisabled: Bool
    
    var body: some View {
        VStack(alignment: .leading, spacing: 8) {
            Text("Typing Speed")
                .font(.subheadline)
                .foregroundColor(.secondary)
            
            HStack(spacing: 8) {
                ForEach(SpeedPreset.allCases) { preset in
                    SpeedButton(
                        preset: preset,
                        isSelected: selectedSpeed == preset,
                        isDisabled: isDisabled
                    ) {
                        selectedSpeed = preset
                    }
                }
            }
        }
    }
}

/// Individual speed selection button
private struct SpeedButton: View {
    
    let preset: SpeedPreset
    let isSelected: Bool
    let isDisabled: Bool
    let action: () -> Void
    
    var body: some View {
        Button(action: action) {
            VStack(spacing: 4) {
                Text(preset.name)
                    .font(.caption)
                    .fontWeight(isSelected ? .semibold : .regular)
                
                Text("\(preset.delayMs)ms")
                    .font(.caption2)
                    .foregroundColor(.secondary)
            }
            .frame(maxWidth: .infinity)
            .padding(.vertical, 10)
            .background(
                RoundedRectangle(cornerRadius: 8)
                    .fill(isSelected ? Color.blue : Color(.systemGray5))
            )
            .foregroundColor(isSelected ? .white : .primary)
        }
        .disabled(isDisabled)
        .opacity(isDisabled ? 0.5 : 1.0)
    }
}