# nRF5340 BLE Environmental Sensor Node

A production-grade embedded C++ project targeting the **Nordic nRF5340 DK**, combining a
custom AHT10 Zephyr driver, BLE with encryption, an LVGL-based display interface, and a
polymorphic State-pattern FSM. WiFi provisioning and MQTT publishing are scoped as
post-milestone stretch goals.

---

## Hardware Bill of Materials

| Component | Part | Interface | Notes |
|-----------|------|-----------|-------|
| MCU Board | Nordic nRF5340 DK | — | Dual-core: app + net |
| Display | 3.5" ILI9486 SPI TFT | SPI + GPIO (CS, DC, RST) | 480×320, 16-bit color |
| Sensor | AHT10 Temperature & Humidity | I2C | Address 0x38 |
| Buttons | SW0, SW1 (on-board) | GPIO | Active-low |
| LEDs | LED0–LED3 (on-board) | GPIO | Active-high |

> **Pin mapping** lives in `boards/nrf5340dk_nrf5340_cpuapp.overlay`.
> Never hardcode pin numbers in application code.

---

## Software Stack

| Layer | Technology | Version target |
|-------|-----------|----------------|
| SDK | nRF Connect SDK | v3.2.1 |
| RTOS | Zephyr RTOS | NCS-bundled (v3.7.x) |
| BLE host | Zephyr BT (SoftDevice Controller) | NCS-bundled |
| Display framework | LVGL | v8 (NCS-bundled) |
| Language | C++17 | `CONFIG_CPP=y`, `CONFIG_STD_CPP17=y` |
| Build system | west + CMake | NCS-bundled west |

---

## C++ Design Constraints

These are non-negotiable throughout the codebase. Every review checkpoint verifies them.

- **No dynamic allocation after boot** — use Zephyr memory slabs, fixed pools, or
  placement-new into stack/static storage. `new`/`delete` are forbidden in application code.
- **RAII everywhere** — every resource that requires cleanup (GPIO, I2C device handle,
  BLE connection, LVGL object) must be owned by a class whose destructor releases it.
- **Namespaces** — all application code lives under `sensor_node::`. Sub-namespaces:
  `sensor_node::ble`, `sensor_node::ui`, `sensor_node::sensor`, `sensor_node::led`.
- **Lambdas** — prefer lambdas for one-shot Zephyr work queue submissions, timer callbacks,
  and LVGL event handlers. Capture by reference only when lifetime is guaranteed.
- **No raw pointers for ownership** — use references or `std::array`/`std::span` where
  possible; raw pointers are permitted only for non-owning observation.

---

## Architecture Overview

```
┌─────────────────────────────────────────────────────────┐
│                     Application Core                    │
│                                                         │
│  ┌──────────────┐   events    ┌────────────────────┐    │
│  │  AppFSM      │◄────────────│  EventDispatcher   │    │
│  │  (State pat.)│             │  (k_msgq-backed)   │    │
│  └──────┬───────┘             └────────────────────┘    │
│         │ owns / transitions                            │
│  ┌──────▼───────────────────────────────────────┐       │
│  │          Concrete States                     │       │
│  │  IdleState  SamplingState  ConnectedState    │       │
│  │  AdvertisingState  ProvisioningState         │       │
│  └──────────────────────────────────────────────┘       │
│                                                         │
│  ┌─────────────┐ ┌──────────┐ ┌──────────────────────┐  │
│  │ SensorMgr   │ │  LedMgr  │ │     DisplayMgr       │  │
│  │ (AHT10 drv) │ │  (RAII)  │ │  (LVGL, double-buf)  │  │
│  └─────────────┘ └──────────┘ └──────────────────────┘  │
│                                                         │
│  ┌──────────────────────────────────────────────────┐   │
│  │              BleManager                          │   │
│  │  EnvService (GATT)  +  ProvService (GATT)        │   │
│  │  Pairing / Bonding / Encryption                  │   │
│  └──────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────┘
              │  OpenAMP / RPMsg IPC
┌─────────────▼───────────────────────────────────────────┐
│                     Network Core                        │
│              Zephyr BT Controller                       │
└─────────────────────────────────────────────────────────┘
```

### State Machine

The top-level `AppFSM` owns a pointer to the current `IState`. State transitions are
triggered by `IEvent` subclasses dispatched through a Zephyr message queue.

```
                ┌───────────────────────────────────┐
                │                                   │
     PowerOn    ▼                   BtnPress        │
  ──────────► Idle ─────────────────────────────► Sampling
                │                                   │
                │ BleStartReq                       │ SampleReady
                ▼                                   ▼
           Advertising ◄──────── Disconnected   (data posted
                │                    ▲          to BleManager)
                │ Connected          │
                ▼                    │
           Connected ────────────────┘
                │
                │ ProvisioningReq (stretch goal)
                ▼
           Provisioning
```

**State base interface:**

```cpp
namespace sensor_node {

struct IEvent { virtual ~IEvent() = default; };
struct IState {
    virtual void on_entry(AppFSM&)         = 0;
    virtual void handle(AppFSM&, IEvent&)  = 0;
    virtual void on_exit(AppFSM&)          = 0;
    virtual ~IState()                      = default;
};

} // namespace sensor_node
```

Each concrete state (e.g. `SamplingState`) is a class implementing `IState`. The FSM
stores a fixed-size state pool and uses placement-new to transition without heap
allocation.

---

## Repository Layout

```
sensor-node/
├── app/
│   ├── src/
│   │   ├── main.cpp
│   │   ├── fsm/
│   │   │   ├── app_fsm.hpp / app_fsm.cpp
│   │   │   ├── states/
│   │   │   │   ├── idle_state.hpp / idle_state.cpp
│   │   │   │   ├── sampling_state.hpp / sampling_state.cpp
│   │   │   │   ├── advertising_state.hpp / advertising_state.cpp
│   │   │   │   └── connected_state.hpp / connected_state.cpp
│   │   │   └── events.hpp
│   │   ├── ble/
│   │   │   ├── ble_manager.hpp / ble_manager.cpp
│   │   │   ├── env_service.hpp / env_service.cpp
│   │   │   └── prov_service.hpp / prov_service.cpp  (stretch)
│   │   ├── sensor/
│   │   │   └── sensor_manager.hpp / sensor_manager.cpp
│   │   ├── ui/
│   │   │   ├── display_manager.hpp / display_manager.cpp
│   │   │   └── screens/
│   │   │       ├── dashboard_screen.hpp / dashboard_screen.cpp
│   │   │       └── status_screen.hpp / status_screen.cpp
│   │   └── led/
│   │       └── led_manager.hpp / led_manager.cpp
│   ├── CMakeLists.txt
│   └── prj.conf
├── drivers/
│   └── sensor/
│       └── aht10/
│           ├── aht10.c
│           ├── aht10.h
│           ├── CMakeLists.txt
│           └── Kconfig
├── boards/
│   └── nrf5340dk_nrf5340_cpuapp.overlay
├── west.yml
└── README.md
```

---

## Milestones and Acceptance Criteria

### Phase 0 — Project Skeleton

**Goal:** Reproducible build, boots to a shell log, CI green.

Acceptance criteria:
- `west build -b nrf5340dk/nrf5340/cpuapp` succeeds from a clean checkout
- Board overlay compiles without warnings
- `CONFIG_CPP=y`, `CONFIG_STD_CPP17=y` confirmed in `.config`
- A `UART_0` log line "sensor-node booting" appears over RTT/UART
- GitHub Actions (or equivalent) runs `west build` on every push

### Phase 1 — AHT10 Custom Zephyr Driver

**Goal:** A Zephyr sensor subsystem driver for the AHT10 that exposes temperature and
humidity via the standard `sensor_channel` API.

Acceptance criteria:
- Driver registered under `CONFIG_AHT10` Kconfig symbol
- `sensor_sample_fetch()` and `sensor_channel_get(SENSOR_CHAN_AMBIENT_TEMP)` /
  `sensor_channel_get(SENSOR_CHAN_HUMIDITY)` return valid readings
- Readings within ±0.5 °C / ±2 %RH of a reference measurement
- Driver handles I2C NACK and bus timeout gracefully (logged, not crashed)
- `SensorManager` C++ class wraps the driver with RAII; destructor releases the device
  handle cleanly
- Unit test (Zephyr `ztest`) exercises the driver on the host using a fake I2C controller

### Phase 2 — BLE with Encryption

**Goal:** A BLE peripheral advertising a custom Environmental Sensing profile, requiring
encryption before characteristic reads.

Acceptance criteria:
- Device advertises as `SensorNode` with a custom 128-bit service UUID
- `EnvService` exposes two notify characteristics: Temperature (int16, 0.01 °C units)
  and Humidity (uint16, 0.01 %RH units)
- Central cannot read/subscribe without pairing; unauthenticated reads return
  `BT_ATT_ERR_AUTHENTICATION`
- Pairing uses LE Secure Connections (LESC) with `CONFIG_BT_SMP_SC_ONLY=y`
- Bonding data persists across power cycles (`CONFIG_BT_SETTINGS=y`)
- `BleManager` C++ class owns the connection handle; `on_connected` / `on_disconnected`
  callbacks dispatch `IEvent` subclasses to the FSM via `EventDispatcher`
- Verified with nRF Connect mobile app: notifications received, values match sensor output

### Phase 3 — LED Status Manager

**Goal:** LEDs communicate system state without requiring the display.

| LED | Meaning |
|-----|---------|
| LED0 | Heartbeat (1 Hz blink = system alive) |
| LED1 | BLE advertising (fast blink 4 Hz) / connected (steady) |
| LED2 | Sensor sampling in progress |
| LED3 | Error (any subsystem fault) |

Acceptance criteria:
- `LedManager` is the sole owner of GPIO LED handles; uses RAII `gpio_dt_spec` wrapper
- LED states driven by FSM state transitions, not scattered `gpio_pin_set` calls
- Heartbeat runs on a Zephyr timer, not `k_sleep` in main thread
- All four patterns verified manually and by code review

### Phase 4 — LVGL Display Interface

**Goal:** A live dashboard showing temperature, humidity, timestamp, and BLE connection status.

Acceptance criteria:
- ILI9486 initialises over SPI; display shows content within 2 seconds of boot
- Dashboard screen renders: temperature value, humidity value, last-update time, BLE
  connection indicator (advertising / connected / disconnected states)
- Values refresh automatically when `DisplayManager::update(SensorData)` is called
- LVGL tick integration is correct (no animation drift over 60 seconds)
- Screen transitions (dashboard ↔ status) triggered by SW1 button press
- `DisplayManager` is the sole owner of all LVGL objects; no LVGL handles leak outside
  the class
- No screen tearing under continuous updates at the sensor poll rate

### Phase 5 — Integration and Application FSM

**Goal:** All subsystems coordinated by the `AppFSM`. End-to-end data flow from sensor
to BLE notification to display update, governed by the State pattern.

Acceptance criteria:
- Five concrete states implemented: `IdleState`, `SamplingState`, `AdvertisingState`,
  `ConnectedState`, `ErrorState`
- Every `IState` subclass has a unit test verifying `on_entry` / `handle` / `on_exit`
  for all expected events
- FSM transitions with no heap allocation (placement-new into static state pool confirmed
  by `CONFIG_HEAP_MEM_POOL_SIZE=0` build)
- Full flow demo: power on → idle → button press → sampling → BLE advertising → phone
  connects → live notifications → phone disconnects → advertising resumes
- No state transition occurs outside the FSM (no cross-module direct state mutation)
- System recovers from simulated I2C fault (sensor removed) by entering `ErrorState`,
  displaying error screen, and blinking LED3; recovers to `SamplingState` on sensor
  reconnect without reboot

---

## Stretch Goals (Post-Milestone, Not in Acceptance Criteria)

These are explicitly out of scope for the milestones above. They are documented here for
reference and future phases.

**WiFi Credential Provisioning via BLE**
Requires nRF7002 expansion board. A `ProvService` GATT service accepts SSID and
passphrase writes over an encrypted BLE link. Credentials are stored in Zephyr settings
NVS. This is the standard Nordic Wi-Fi Provisioning Service pattern.

**MQTT Publishing**
After WiFi is provisioned, the node connects to a broker (e.g. Mosquitto) and publishes
JSON payloads to `sensor-node/env` at the configured poll rate. Uses Zephyr's MQTT
library with TLS (`CONFIG_MQTT_LIB_TLS=y`). Retain flag set so the last reading
survives broker restart.

---

## Key Zephyr Concepts per Phase

| Phase | Zephyr APIs / Subsystems |
|-------|--------------------------|
| P0 | `west`, `CMakeLists.txt`, `prj.conf`, `devicetree` overlays |
| P1 | Sensor subsystem, I2C API, `ztest`, Kconfig, `DT_NODELABEL` |
| P2 | `bt_enable`, GATT service macros, SMP, `bt_conn_cb`, settings NVS |
| P3 | GPIO API, `gpio_dt_spec`, `k_timer`, Zephyr work queues |
| P4 | LVGL port, SPI API, `k_work`, display driver binding, `lv_timer` |
| P5 | `k_msgq`, OpenAMP/RPMsg (awareness), `k_mem_slab`, integration testing |

---

## References

### Nordic / Zephyr

- [nRF Connect SDK v3.2.1 Documentation](https://docs.nordicsemi.com/bundle/ncs-3.2.1/page/nrf/index.html)
- [Zephyr Project Documentation (NCS-pinned)](https://docs.nordicsemi.com/bundle/ncs-3.2.1/page/zephyr/index.html)
- [Zephyr Sensor Subsystem — Writing a Driver](https://docs.nordicsemi.com/bundle/ncs-3.2.1/page/zephyr/hardware/peripherals/sensor.html)
- [Zephyr Bluetooth — Peripheral Sample](https://docs.nordicsemi.com/bundle/ncs-3.2.1/page/zephyr/samples/bluetooth/peripheral/README.html)
- [Zephyr BLE Security — SMP / Pairing](https://docs.nordicsemi.com/bundle/ncs-3.2.1/page/zephyr/connectivity/bluetooth/api/security.html)
- [Nordic DevAcademy — Bluetooth LE Fundamentals](https://academy.nordicsemi.com/courses/bluetooth-low-energy-fundamentals/)
  *(you have already completed this — use it as a reference, not re-study)*
- [nRF5340 Product Specification](https://infocenter.nordicsemi.com/topic/ps_nrf5340/)
- [nRF5340 DK User Guide](https://infocenter.nordicsemi.com/topic/ug_nrf5340_dk/)
- [Nordic Wi-Fi Provisioning Service (stretch)](https://docs.nordicsemi.com/bundle/ncs-3.2.1/page/nrf/libraries/networking/wifi_prov.html)

### LVGL

- [LVGL Documentation v8](https://docs.lvgl.io/8.3/)
- [LVGL Zephyr Integration Guide](https://docs.zephyrproject.org/latest/services/display/lvgl.html)
- [LVGL C++ Wrapper (community)](https://github.com/lvgl/lvgl/tree/master/examples) — read the
  C++ object-wrapping examples even if you write your own wrappers

### ILI9486 / Display

- [ILI9486 Datasheet](https://www.displayfuture.com/Display/datasheet/controller/ILI9486.pdf)
- Zephyr `ili9xxx` driver family — see `drivers/display/display_ili9486.c` in the Zephyr tree

### AHT10

- [AHT10 Datasheet](https://asairsensors.com/wp-content/uploads/2021/09/Data-Sheet-AHT10-ASAIR-V2.31.pdf)
- Study `drivers/sensor/shtcx/` in the Zephyr tree as a reference implementation for a
  similar I2C humidity sensor — same init-then-measure pattern

### C++ in Embedded Systems

- **"Embedded Systems: A Practical Transition from C to Modern C++"** — your current book;
  refer back to the RAII and resource-management chapters when writing Phase 1 and Phase 4
- [C++ Core Guidelines — relevant sections](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines):
  R (resource management), C (classes), E (error handling)
- Miro Samek, *"Practical UML Statecharts in C/C++"* — the canonical embedded FSM
  reference; Chapter 3 covers the State pattern specifically
- [Jason Turner — C++ Weekly (YouTube)](https://www.youtube.com/@cppweekly) — short
  focused episodes; episodes on `[[nodiscard]]`, `constexpr`, and RAII are directly
  applicable here

### BLE Security

- Bluetooth Core Specification 5.4, Vol 3, Part H (Security Manager Protocol) — for
  LESC internals
- [NimBLE Security Docs](https://mynewt.apache.org/latest/network/docs/ble_sec.html) —
  readable overview of the pairing flow even if you use Zephyr's BT stack

### Tooling

- [nRF Connect for VS Code](https://nrfconnect.github.io/vscode-nrf-connect/) — recommended IDE integration
- [nRF Connect mobile app](https://www.nordicsemi.com/Products/Development-tools/nRF-Connect-for-mobile) — BLE debugging
- [Wireshark + nRF Sniffer](https://infocenter.nordicsemi.com/topic/ug_sniffer_ble/) — essential for verifying encryption and pairing

---

## Development Workflow

1. One branch per phase (`phase/p0-skeleton`, `phase/p1-aht10-driver`, …)
2. Acceptance criteria from the relevant phase section serve as the PR checklist
3. `west build` must succeed with `CONFIG_COMPILER_WARNINGS_AS_ERRORS=y` before merge
4. Add a `ztest` suite for every new module; suites run in CI via `west twister`
5. Commit messages: `[P1] feat: add AHT10 I2C init sequence` — phase tag + conventional
   commit prefix

---

*Last updated: project inception. Update this README at the start of each phase to
reflect any scope changes agreed in review.


