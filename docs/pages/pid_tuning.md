# PID Tuning {#pid_tuning}

## Overview

The Libopenpresso PID temperature controller is designed specifically for espresso machine boilers, which are characterized by high thermal inertia and significant delay between applying power and seeing a temperature change.

To handle these specific challenges, the controller extends the standard PID algorithm with:
*   **Relaxed D-term**: Reduces derivative kick and allows faster approach by lessening damping when far from the setpoint.
*   **Relaxed I-term**: Prevents integral windup during large temperature changes (like initial heat-up) without complex conditional logic.
*   **Feedforward (F & W)**: Proactively adds power based on water flow to compensate for the massive cooling effect of injecting cold water into the boiler.

## Configuration Parameters

The controller is configured via the `PidSettings` struct. Detailed unit descriptions are available in the [API Reference](\ref libopenpresso::PidSettings), but here is a functional overview:

### Standard Terms
*   **`p` (Proportional)**: The primary driving force. Reacts to the immediate error.
*   **`i` (Integral)**: Corrects steady-state error (long-term drift).
*   **`d` (Derivative)**: Dampens the system to prevent overshoot and oscillation.

### Advanced Stabilizing Terms
*   **`dFilterTime`**: Low-pass filter time constant for the D-term. Essential because raw derivative terms amplify sensor noise.
*   **`dTermRelax`**: Reduces the D-term's impact when the error is large.
    *   *Why?* A strong D-term resists changes. We want strong damping *near* the setpoint to stop at the right time, but we don't want to fight the P-term when we are cold and need to heat up fast.
*   **`iTermRelax`**: Reduces the I-term accumulation when the total output is already saturated.
    *   *Why?* Prevents "windup" where the I-term builds up massively during heat-up, causing a huge overshoot later.

### Feedforward Terms (Brewing Compensation)
*   **`f` (Flow Rate)**: Adds power proportional to 'grams per second' of water flow.
    *   *Goal:* compensate for the immediate cooling effect of cold water entering the boiler *while the pump is running*.
*   **`w` (Weight)**: Adds power proportional to the *total grams* of water added.
    *   *Goal:* Compensate for the thermal mass of the added cold water *after the pump stops*.
*   **`wDecay`**: Slowly reduces the `w` term's influence as temperature recovers.
    *   *Goal:* Prevent the boiler from overheating after the recovery phase is done.

## Tuning Procedure

### Prerequisites
1.  Ensure your **heating element**, **SSR**, and **temperature sensor** are working correctly.
2.  Enable `enablePidStateDump` in your `TemperaturePidControllerConfig`. This allows you to graph the P, I, D, and Feedforward terms separately to see what the controller is "thinking".

### Phase 1: Stabilization (Static Heating)

Start with all settings except `p` set to 0.

1.  **Tune P (Proportional)**
    *   Good starting point is to make controller hold 100% power while error is more than 20°C (`p = 1.0 / 20`).
    *   Increase `p` until the system reaches the setpoint reasonably fast.
    *   Some oscillation around the setpoint is expected and acceptable at this stage.
    *   If it oscillates wildly, reduce `p`.
    
2.  **Tune D (Derivative) & Filter**
    *   Set `dFilterTime` to approx 5-10x your sensor update period (e.g., if sensor updates every 100ms, set filter to 500ms-1s). You can reduce this timing while you don't see significant noise in D-term. 
    *   Good starting point is to make controller reduce power by 10% per 1 °C/s temperature rise (`d = 0.1`).
    *   Increase `d` to dampen the oscillations from the P-term.
    *   After introducing D-term you may need to increase P-term again. 
    *   Try to achieve as fast as possible set-point reaching without overshoot.
    *   Temperature may still drift after reaching the set point because I-term is inactive.

4.  **Tune I (Integral) & I-Relax**
    *   As a starting point set `iTermRelax` to 1.0 what will mean that I-term starts to impact when P+D 
    don't drive heater to full power.
    *   Add `i` to eliminate constant error like heat loss to surrounding.
    *   Increase per `0.0005f` until you see that temperature doesn't drift and stabilizes after at max one full oscillation after passing the set point.
    *   Now you can try to play with all three coefficients to find the best possible balance.

5.  **Tune D-Relax**
    *   Sometimes to achieve better results we need to make D-term a bit more agressive near the set point.
    *   While increasing `d` you can also add `dTermRelax` to make the D-term "fade out" when the temperature is far from the target.
    *   This allows the P-term to drive full power during heat-up (faster rise time) while technically keeping a high `d` value for stability when you finally reach the target.

### Phase 2: Disturbance Rejection (Brewing)

Now that the machine holds a steady idle temperature, tune it to handle the shock of brewing.

1.  **Tune F (Flow Feedforward)**
    *   Start a simulated brew (pump on).
    *   Increase `f` until the temperature drop during the shot is minimized.
    *   **Rule of thumb**: A good starting point is a value that contributes ~50% of your heater's max power at maximum flow rate.
    *   **Stop** increasing if the temperature starts to *rise* above the setpoint while the pump is running.

2.  **Tune W (Weight) & Decay**
    *   After the pump stops, you might see the temperature drop further or struggle to recover because the boiler is now full of colder water.
    *   Good starting point is to set `w=0.0005` and `wDecay` about 4 times more than `i`.
    *   Increase `w` to provide a "post-shot boost" based on how much water was dispensed.
    *   **Crucial**: You **must** tune `wDecay` simultaneously.
        *   The `w` term adds a constant power offset.
        *   `wDecay` reduces this offset as the temperature rises above the setpoint.
        *   Without `wDecay`, the machine will overheat after the shot because the `w` boost never goes away.

## Tune for steam mode

When you already have well tuned controller for brewing temperature, you can use these settings as
a starting point to control steam temperature.  
This scenario doesn't need any feed-forward features, so you can set `f`, `w` and `wDecay` to 0. `flowCounter` field also can be empty.  
Standard `p`, `i` and `d` can be increased slightly for the controller that holds steam temperature in idle state, and increased about 2 times for the controller that will handle active steaming.
Steam controller will switch two temperature controllers automatically, which is especially helpful to prevent I-term overshoot after steam valve was closed.
