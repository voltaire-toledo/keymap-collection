import Foundation
import IOKit.hid

class HIDProfiler {
    var keyStartTimes: [UInt32: Double] = [:]
    var tapDurations: [UInt32: [Double]] = [:]
    var totalSamples = 0
    let targetSamples = 150
    
    let homeRowCodes: [UInt32: String] = [
        0x04: "A", 0x16: "S", 0x07: "D", 0x09: "F",
        0x0D: "J", 0x0E: "K", 0x0F: "L", 0x33: ";"
    ]

    func start() {
        let manager = IOHIDManagerCreate(kCFAllocatorDefault, 0)
        IOHIDManagerSetDeviceMatching(manager, nil)
        IOHIDManagerOpen(manager, 0)

        IOHIDManagerRegisterInputValueCallback(manager, { (context, result, sender, value) in
            let element = IOHIDValueGetElement(value)
            let usagePage = IOHIDElementGetUsagePage(element)
            let usage = IOHIDElementGetUsage(element)
            
            if usagePage == 0x07 {
                let instance = Unmanaged<HIDProfiler>.fromOpaque(context!).takeUnretainedValue()
                let state = IOHIDValueGetIntegerValue(value)
                let now = Date().timeIntervalSince1970
                
                if state == 1 { // Key Down
                    // Only record the FIRST down event (ignore repeats)
                    if instance.keyStartTimes[usage] == nil {
                        instance.keyStartTimes[usage] = now
                    }
                } else if state == 0 { // Key Up
                    if let start = instance.keyStartTimes[usage] {
                        let duration = now - start
                        if instance.homeRowCodes.keys.contains(usage) {
                            // Ignore impossible durations < 10ms
                            if duration > 0.01 {
                                instance.tapDurations[usage, default: []].append(duration)
                                instance.totalSamples += 1
                                print("\rCollected \(instance.totalSamples)/\(instance.targetSamples) samples...", terminator: "")
                                fflush(stdout)
                                
                                if instance.totalSamples >= instance.targetSamples {
                                    instance.printResults()
                                    exit(0)
                                }
                            }
                        }
                        instance.keyStartTimes.removeValue(forKey: usage)
                    }
                }
            }
        }, UnsafeMutableRawPointer(Unmanaged.passUnretained(self).toOpaque()))

        IOHIDManagerScheduleWithRunLoop(manager, CFRunLoopGetCurrent(), CFRunLoopMode.defaultMode.rawValue)
        
        print("\n--- Home Row Profiler ---")
        print("Type naturally. The script will auto-exit once finished.\n")
        
        CFRunLoopRun()
    }

    func printResults() {
        print("\n\n--- PROFILING RESULTS ---")
        let order = ["A", "S", "D", "F", "J", "K", "L", ";"]
        let results = homeRowCodes.map { ($1, $0) }.sorted { a, b in
            (order.firstIndex(of: a.0) ?? 99) < (order.firstIndex(of: b.0) ?? 99)
        }
        
        for (name, usage) in results {
            if let durations = tapDurations[usage], !durations.isEmpty {
                let avg = (durations.reduce(0, +) / Double(durations.count)) * 1000
                let minDur = (durations.min() ?? 0) * 1000
                let maxDur = (durations.max() ?? 0) * 1000
                print("\(name): Avg \(Int(avg))ms (Range: \(Int(minDur))-\(Int(maxDur))ms, Samples: \(durations.count))")
            }
        }
        print("------------------------\n")
    }
}

let profiler = HIDProfiler()
profiler.start()
