# Internal Flow Tuning

This guide explains how to tune the **Vibro Pump Flow Sensor** parameters in Libopenpresso to achieve accurate flow estimation.

## How it Works

The flow sensor estimates water mass flow based on the current pressure and pump characteristics. It uses the following formula:

\f[ \text{mass} = \left(1 - \min\left(\frac{P}{P_{\text{stall}}}, 1\right)\right) \times V_{\text{pulse}} \times N_{\text{pulses}} \f]

Where:
 * \f$P\f$ is the current pressure (in millibars)
 * \f$P_{\text{stall}}\f$ is the \ref libopenpresso::VibroPumpFlowSensor::pumpStallPressure "VibroPumpFlowSensor::pumpStallPressure"
 * \f$V_{\text{pulse}}\f$ is the \ref libopenpresso::VibroPumpFlowSensor::volumePerPulse "VibroPumpFlowSensor::volumePerPulse"
 * \f$N_{\text{pulses}}\f$ is the number of pump pulses since the last callback

Essentially, the flow is highest when pressure is low and decreases linearly as pressure increases, reaching zero at the stall pressure.  

## Configuration Parameters

You need to tune two key parameters in your machine configuration:

1.  **`pumpStallPressure`**: The virtual pressure value where the flow formula calculates zero flow. 
    *   *Note:* This is a theoretical value where the pump plunger effectively stops moving or moves a negligible volume. It might differ slightly from the actual maximum physical pressure your pump can reach.
2.  **`volumePerPulse`**: The theoretical volume of water moved by a single pump stroke when there is **zero backpressure** (0 bar).

## Tuning Procedure

Tuning involves two phases: first establishing a baseline at zero pressure, and then refining the behavior under high pressure.

### Phase 1: Zero Backpressure Tuning

**Step 1: Estimate `volumePerPulse`**

1.  Remove the portafilter so water can flow freely (0 bar condition). 
2.  Place a container on a scale under the group head.
3.  Run the pump for a fixed duration (e.g., 10 seconds) and multiply it by your AC mains frequency to get total pulses count.
4.  Weigh the dispensed water.
5.  Calculate the initial value:
    \f[ V_{\text{pulse}} = \frac{\text{Total Weight (µg)}}{\text{Total Pulses}} \f]
    *(Or Total Weight / (Time * Frequency) if pulses aren't counted directly)*
6.  Update your `volumePerPulse` configuration with this value.

**Step 2: Fine-tune `volumePerPulse`**

1.  Repeat the free flow test with the new setting.
2.  Compare the mass reported by \ref libopenpresso::VirtualWeightSensorConfig "virtual weight sensor" against the actual weight on the scale. Adjust volumePerPulse with the following formula:
    \f[ V_{\text{pulse new}} = V_{\text{pulse old}} \times \frac{\text{Real Weight}}{\text{Virtual Sensor Weight}} \f]
3.  Repeat until they match closely.

### Phase 2: High Pressure Tuning

**Step 3: Estimate `pumpStallPressure`**

1.  Insert a blind filter into the portafilter and lock it in.
2.  Run the pump at 100% power.
3.  Record the maximum pressure reading from your pressure sensor. If it's maxing out the transducer, just use this maximum value as a starting point, we will correct it on the next step.
4.  Set `pumpStallPressure` to this value as a starting point.
    *   *Important:* The "formula" stall pressure required for accurate estimation often differs from the physical maximum pressure.

**Step 4: Fine-tune `pumpStallPressure`**

1.  Prepare a setup that generates high pressure but allows some flow. Use **portafilter with a coffee pack** or a flow restrictor if available. Make a runs to fully saturate the coffee or fill the portafilter with water.
2.  Without removing portafilter after saturating run, perform another one with \ref libopenpresso::VirtualWeightSensorConfig "virtual weight sensor".
3. Compare the mass reported by \ref libopenpresso::VirtualWeightSensorConfig "virtual weight sensor" against the actual weight on the scale. Adjust pumpStallPressure with the following formula:
    \f[ P_{\text{stall new}} = P_{\text{stall old}} \times \frac{\text{Real Weight}}{\text{Virtual Sensor Weight}} \f]
4.  Repeat until they match closely.

## Accuracy and precision

This is the simplest mathematical bilinear model. It is quite far from the real process physics where
it can be considered linear very very approximately.
Given that, precision will vary when you move away from pulse density or pressure which
was used during tuning.
This model should be used for scenarios where we need to know how much water comes into the boiler,
but not comes out of the group head: feed-forward term of the temperature controller and boiler refill during steaming. 
It also can be used to automatically stop brewing at specific espresso shot weight, but in this case an approximate water mass needed to saturate the coffee pack should be added to target weight.

## See Also

- [Device Configuration](device_config.md) — how to configure the flow sensor in the components map
- [PID Tuning Guide](pid_tuning.md) — tuning temperature controllers which may use flow feedforward
- \ref libopenpresso::VibroPumpFlowSensor "VibroPumpFlowSensor" — API reference
- \ref libopenpresso::VirtualWeightSensorConfig "VirtualWeightSensorConfig" — API reference
