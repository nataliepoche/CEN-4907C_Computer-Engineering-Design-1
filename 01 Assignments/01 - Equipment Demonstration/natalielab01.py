# Low=pass filter frequency response
# DG1032 FUNCTION GENERATOR + MSO2302A OSCILLOSCOPE
# ONLY CHAN1 IS USED

# Imports
import numpy as np
import pyvisa
import math
import time
import csv
import matplotlib.pyplot as plt

#User settings
GEN_ADDRESS = "TCPIP::169.254.213.175::INSTR"      # DG1032, function generator
SCOPE_ADDRESS = "TCPIP::169.254.43.160::INSTR"     # MSO2302A, oscilloscope

# Frequency sweep
START_FREQ = 100 # Starting frequency of the sweep
STOP_FREQ = 10000 # Enfing frequency of the sweep
NUM_PTS = 32 # Number of frequency measurements to collect

# Function generator
VOLTAGE_AMPLITUDE = 2.0       # VPP, amplitude of sine wave produced by the functiongenerator in VPP
DC_OFFSET = 0.0               # V, DC offset of the function generator in volts

# Theoretical circuit values
R = 1000          # Resistance in ohms
C =  0.1e-6         # Capacitance in farads

#Theoretical cutoff
theoretical_cutoff = 1 / (2 * math.pi * R * C)

# Print theoretical cutoff
print(
    f"\nTheoretical cutoff frequency = "
    f"{theoretical_cutoff:.2f} Hz"
)

# VISA setup
rm = pyvisa.ResourceManager('@py') # Creates a PyVISA resource manager using the Python VISA backend

generator = rm.open_resource(GEN_ADDRESS) # Opens communication with the function generator
scope = rm.open_resource(SCOPE_ADDRESS) # Opens communcation with the oscilloscope

generator.timeout = 5000 # Sets function generator communication timeout to 5000 ms
scope.timeout = 5000 # Sets the oscilloscope communcation timeout to 5000 ms

# Function generator setup
generator.write("*RST") # Resets function generator to default settings

time.sleep(0.5) # Waits for 0.5 seconds for function generator to reset

# Configure:
generator.write(
    f"APPL:SIN " # Sine wave
    f"{START_FREQ}," # 100 Hz initial frequency
    f"{VOLTAGE_AMPLITUDE}," # 2 VPP, sets the output amplitude
    f"{DC_OFFSET}") # 0V DC offset

time.sleep(0.5) # Waits for setting to take effect


# Turn output ON
generator.write("OUTP ON")

time.sleep(0.5) # Gives generator time to stabilize

# Oscilloscope set uscope# only CH1
scope.write("*RST") # Resets oscilloscope

time.sleep(0.5) # Waits for it to reset

# Enable CH1
scope.write(":CHAN1:DISP ON") # Turns CH1 on

scope.write(":CHAN1:COUP DC") # Sets CH1 to DC coupling

# Vertical scale
scope.write(":CHAN1:SCAL 0.05") # Gives better vertical resolution

scope.write(":CHAN1:OFFS 0") # Sets vertical offset to 0 V

# Trigger
scope.write(":TRIG:EDGE:SOUR CHAN1") # Sets CH1 as trigger source

scope.write(":TRIG:EDGE:SLOP POS") # Trigger slope to positive

scope.write(":TRIG:EDGE:LEV 0") # Sets trigger level to 0 V

scope.write(":MEAS:CLE") # Clears old measurements

time.sleep(0.5) # Gives time to update

# Creates 32 data points
frequencies = np.logspace(
    np.log10(START_FREQ),
    np.log10(STOP_FREQ),
    NUM_PTS
)

# Arrays for measurements
valid_freqs = []

vout_values = []

# Frequency sweep
print("\n======================================================")

print("MEASUREMENTS")

print("======================================================")

print(
    "Frequency (Hz) | Vout (VPP)"
)

print("------------------------------------------------------")


for f in frequencies:

    # Generator Frequency
    # Sets the current  frequency to function generator
    generator.write(
        f"FREQ {f}"
    )

    # Give generator a short time to settle
    time.sleep(0.20)

    # Set Oscilloscope time scale
    # Approximately two periods across the screen
    time_per_div = max(
        0.2 / f,
        5e-6
    )

    scope.write(
        f":TIM:SCAL {time_per_div}" # Sends time scale to o
    )
    
    # Starts waveform acquisition
    scope.write(":RUN")

    time.sleep(0.25) # Waits for waveform to update


    # --------------------------------------------------------
    # First measurement
    #
    # Discard the first measurement after changing frequency
    # so that the scope has time to update its waveform.
    # --------------------------------------------------------

    try:

        scope.query(
            ":MEASure:VPP? CHANnel1"
        )

    # Handles timeout from 
    except pyvisa.VisaIOError:

        print(
            f"{f:10.2f} Hz | "
            f"Measurement timeout"
        )

        continue
    
    # Attempts acutal VPP measurement
    try:

        # Requests CH1 peak-to-peak voltage
        vout_string = scope.query(
            ":MEASure:VPP? CHANnel1"
        ).strip()

        vout = float(vout_string)

    # Handles communcation errors
    except (pyvisa.VisaIOError, ValueError):

        print(
            f"{f:10.2f} Hz | "
            f"Measurement failed"
        )

        continue
    
    # Chack Measurement# Checks if the measured voltage is valid
    if vout <= 0 or vout >= 1e30:

        print(
            f"{f:10.2f} Hz | "
            f"Invalid Vout = {vout}"
        )

        continue
    
    # Store valid measurements
    valid_freqs.append(f)

    vout_values.append(vout)

# Convert to numpy arrays
valid_freqs = np.array(valid_freqs)

vout_values = np.array(vout_values)

# Checks data was collected
if len(valid_freqs) < 3:

    print(
        "\nERROR: Not enough valid measurements."
    )

    generator.write("OUTP OFF") # Turns generator output off

    generator.close() # Closes generator connection

    scope.close() # Closes oscilloscope connecttion

    rm.close() # Closes VISA resource manager

    raise SystemExit # Stops program


# Determine low frequency passband reference
# The first point in your previous data was an outlier.
#
# Therefore, don't use the first point to establish the
# passband reference.
#
# Use the next several low-frequency points.

# Determines how many low-frequency measurements to Use
# A maximum of 10 measurements are used
reference_points = min(
    10,
    len(vout_values) - 1
)

# Calculate median Vout of the low-frequency points
# These points represent the filter's passband
reference_vout = np.median(
    vout_values[
        1:reference_points + 1
    ]
)

#Calculate experiemental gain
gain_db = 20 * np.log10(
    vout_values / reference_vout
)

# Print complete results
print("\n======================================================")

print("LOW-PASS FILTER RESULTS")

print("======================================================")

print(
    f"Passband reference = "
    f"{reference_vout:.4f} VPP"
)

print(
    f"Theoretical cutoff = "
    f"{theoretical_cutoff:.2f} Hz"
)

print("------------------------------------------------------")

print(
    "Frequency (Hz) | "
    "Vout (VPP) | "
    "Gain (dB)"
)

print("------------------------------------------------------")


for f, v, g in zip(
    valid_freqs,
    vout_values,
    gain_db
):

    # Gain is included on the same result line

    print(
        f"{f:14.2f} | "
        f"{v:10.4f} | "
        f"{g:8.2f}"
    )

# Find experimental -3 dB cuttoff
cutoff_gain = -3.0

experimental_cutoff = None # Assume no cuttoff found

# Loop through neighboring gain emasurements
for i in range(
    len(gain_db) - 1
):

    g1 = gain_db[i] # Gets current gain measurement

    g2 = gain_db[i + 1] # Gets next gain


    # Check for crossing of -3 dB
    if (
        g1 >= cutoff_gain
        and
        g2 <= cutoff_gain
    ):

        f1 = valid_freqs[i] # Gets first frequency around crossing

        f2 = valid_freqs[i + 1] # Gets second frequency around crossing
        
        # Log-frequency interpolation, converts to log scale
        log_f1 = np.log10(f1)

        log_f2 = np.log10(f2)
        
        # Calcualtes how far between 2 points the -3dB level occurs
        fraction = (
            (cutoff_gain - g1)
            /
            (g2 - g1)
        )
        
        # Interpolates between the two logarithms
        log_fc = (
            log_f1
            +
            fraction *
            (log_f2 - log_f1)
        )
        
        # Converts interpolated logarothsm back into Hz
        experimental_cutoff = (
            10 ** log_fc
        )
        
        break

print("\n======================================================")

print("CUTOFF FREQUENCIES")

print("======================================================")

print(
    f"Theoretical cutoff frequency = "
    f"{theoretical_cutoff:.2f} Hz"
)


if experimental_cutoff is not None:

    print(
        f"Experimental cutoff frequency = "
        f"{experimental_cutoff:.2f} Hz"
    )


    # Calculate percent difference
    percent_difference = (
        abs(
            experimental_cutoff
            -
            theoretical_cutoff
        )
        /
        theoretical_cutoff
        * 100
    )


    print(
        f"Percent difference = "
        f"{percent_difference:.2f}%"
    )

else:

    print(
        "Experimental cutoff frequency "
        "could not be determined."
    )


print("======================================================")


# Save CSV
# Opens/Creates CSV file
with open(
    "lowpass_data.csv",
    mode="w",
    newline=""
) as file:

    writer = csv.writer(file) # Creates a CSV filter
    
    # Write column headings
    writer.writerow(
        [
            "Frequency (Hz)",
            "Vout (VPP)",
            "Gain (dB)"
        ]
    )
    
    # Loop through measured data
    for f, v, g in zip(
        valid_freqs,
        vout_values,
        gain_db
    ):
        
        # Write one measurement row to CSV
        writer.writerow(
            [
                f,
                v,
                g
            ]
        )


# Create Ideal Theoretical Frequency response
theoretical_freqs = np.logspace(
    np.log10(START_FREQ),
    np.log10(STOP_FREQ),
    500
)


# ------------------------------------------------------------
# Ideal RC low-pass magnitude
#
# |H(f)| =
#
#       1
# ----------------
# sqrt(1 + (f/fc)^2)
# ------------------------------------------------------------ #

theoretical_magnitude = (
    1 /
    np.sqrt(
        1 +
        (
            theoretical_freqs
            /
            theoretical_cutoff
        ) ** 2
    )
)


# Convert ideal magnitude to dB
theoretical_gain_db = (
    20 *
    np.log10(
        theoretical_magnitude
    )
)

# Bode plot
plt.figure(
    figsize=(9, 6)
)

# Experimental response
plt.semilogx(
    valid_freqs,
    gain_db,
    marker='o',
    linestyle='-',
    label="Experimental"
)

# Ideal theoretical rsponse
plt.semilogx(
    theoretical_freqs,
    theoretical_gain_db,
    linestyle='--',
    label="Ideal Theoretical"
)

# -3 dB reference
plt.axhline(
    y=-3,
    linestyle=':',
    label="-3 dB"
)

# Theoretical cutoff
plt.axvline(
    x=theoretical_cutoff,
    linestyle='--',
    label=(
        f"Theoretical cutoff = "
        f"{theoretical_cutoff:.1f} Hz"
    )
)

# Experiemental cutoff
if experimental_cutoff is not None:

    plt.axvline(
        x=experimental_cutoff,
        linestyle=':',
        label=(
            f"Experimental cutoff = "
            f"{experimental_cutoff:.1f} Hz"
        )
    )


    # Mark experimental -3 dB point

    plt.plot(
        experimental_cutoff,
        -3,
        marker='o',
        markersize=8
    )

# Plot labels
plt.xlabel(
    "Frequency (Hz)"
)

plt.ylabel(
    "Gain (dB)"
)

plt.title(
    "Low-Pass Filter Frequency Response"
)

# Grid
plt.grid(
    True,
    which="both"
)

# Legend
plt.legend()

# Layout
plt.tight_layout()

# Display
plt.show()

# Turn off and close instruments
generator.write("OUTP OFF")

generator.close()

scope.close()

rm.close()