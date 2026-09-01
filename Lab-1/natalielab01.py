# ============================================================
# LOW-PASS FILTER FREQUENCY RESPONSE
# DG1032 FUNCTION GENERATOR + MSO2302A OSCILLOSCOPE
#
# ONLY CHAN1 IS USED
# ============================================================


# --- IMPORTS --- #

import numpy as np
import pyvisa
import math
import time
import csv
import matplotlib.pyplot as plt


# ============================================================
# USER SETTINGS
# ============================================================ #

GEN_ADDRESS = "TCPIP::169.254.213.175::INSTR"      # DG1032
SCOPE_ADDRESS = "TCPIP::169.254.43.160::INSTR"     # MSO2302A


# Frequency sweep
START_FREQ = 100
STOP_FREQ = 10000
NUM_PTS = 32


# Function generator
VOLTAGE_AMPLITUDE = 2.0       # VPP
DC_OFFSET = 0.0               # V


# ============================================================
# CIRCUIT VALUES
# CHANGE THESE TO YOUR ACTUAL VALUES
# ============================================================ #

R = 1000          # Resistance in ohms
C =  0.1e-6         # Capacitance in farads


# ============================================================
# THEORETICAL CUTOFF FREQUENCY
# ============================================================ #

theoretical_cutoff = 1 / (2 * math.pi * R * C)


# Print theoretical cutoff
print(
    f"\nTheoretical cutoff frequency = "
    f"{theoretical_cutoff:.2f} Hz"
)


# ============================================================
# VISA SETUP
# ============================================================ #

rm = pyvisa.ResourceManager('@py')

generator = rm.open_resource(GEN_ADDRESS)
scope = rm.open_resource(SCOPE_ADDRESS)

generator.timeout = 5000
scope.timeout = 5000


# ============================================================
# FUNCTION GENERATOR SETUP
# ============================================================ #

generator.write("*RST")

time.sleep(0.5)


# Configure:
# Sine wave
# 100 Hz initial frequency
# 2 VPP
# 0 V DC offset

generator.write(
    f"APPL:SIN "
    f"{START_FREQ},"
    f"{VOLTAGE_AMPLITUDE},"
    f"{DC_OFFSET}"
)

time.sleep(0.5)


# Turn output ON
generator.write("OUTP ON")

time.sleep(0.5)


# ============================================================
# OSCILLOSCOPE SETUP
# ONLY CHAN1
# ============================================================ #

scope.write("*RST")

time.sleep(0.5)


# ------------------------------------------------------------
# Enable CHAN1
# ------------------------------------------------------------ #

scope.write(":CHAN1:DISP ON")

scope.write(":CHAN1:COUP DC")


# ------------------------------------------------------------
# Vertical scale
#
# Your measured Vout was approximately 0.12-0.28 VPP.
# 0.05 V/div gives better vertical resolution.
# ------------------------------------------------------------ #

scope.write(":CHAN1:SCAL 0.05")

scope.write(":CHAN1:OFFS 0")


# ------------------------------------------------------------
# Trigger
# ------------------------------------------------------------ #

scope.write(":TRIG:EDGE:SOUR CHAN1")

scope.write(":TRIG:EDGE:SLOP POS")

scope.write(":TRIG:EDGE:LEV 0")


# ------------------------------------------------------------
# Clear old measurements
# ------------------------------------------------------------ #

scope.write(":MEAS:CLE")

time.sleep(0.5)


# ============================================================
# CREATE 32 LOGARITHMIC FREQUENCY POINTS
# ============================================================ #

frequencies = np.logspace(
    np.log10(START_FREQ),
    np.log10(STOP_FREQ),
    NUM_PTS
)


# ============================================================
# ARRAYS FOR MEASUREMENTS
# ============================================================ #

valid_freqs = []

vout_values = []


# ============================================================
# FREQUENCY SWEEP
# ============================================================ #

print("\n======================================================")

print("MEASUREMENTS")

print("======================================================")

print(
    "Frequency (Hz) | Vout (VPP)"
)

print("------------------------------------------------------")


for f in frequencies:

    # --------------------------------------------------------
    # Set generator frequency
    # --------------------------------------------------------

    generator.write(
        f"FREQ {f}"
    )


    # Give generator a short time to settle
    time.sleep(0.20)


    # --------------------------------------------------------
    # Set oscilloscope time scale
    # --------------------------------------------------------

    # Approximately two periods across the screen

    time_per_div = max(
        0.2 / f,
        5e-6
    )

    scope.write(
        f":TIM:SCAL {time_per_div}"
    )


    # --------------------------------------------------------
    # Start acquisition
    # --------------------------------------------------------

    scope.write(":RUN")

    time.sleep(0.25)


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

    except pyvisa.VisaIOError:

        print(
            f"{f:10.2f} Hz | "
            f"Measurement timeout"
        )

        continue


    # --------------------------------------------------------
    # Second measurement
    #
    # This is the value we keep.
    # --------------------------------------------------------

    try:

        vout_string = scope.query(
            ":MEASure:VPP? CHANnel1"
        ).strip()

        vout = float(vout_string)

    except (pyvisa.VisaIOError, ValueError):

        print(
            f"{f:10.2f} Hz | "
            f"Measurement failed"
        )

        continue


    # --------------------------------------------------------
    # Check measurement
    # --------------------------------------------------------

    if vout <= 0 or vout >= 1e30:

        print(
            f"{f:10.2f} Hz | "
            f"Invalid Vout = {vout}"
        )

        continue


    # --------------------------------------------------------
    # Store valid measurement
    # --------------------------------------------------------

    valid_freqs.append(f)

    vout_values.append(vout)


# ============================================================
# CONVERT TO NUMPY ARRAYS
# ============================================================ #

valid_freqs = np.array(valid_freqs)

vout_values = np.array(vout_values)


# ============================================================
# CHECK THAT DATA WAS COLLECTED
# ============================================================ #

if len(valid_freqs) < 3:

    print(
        "\nERROR: Not enough valid measurements."
    )

    generator.write("OUTP OFF")

    generator.close()

    scope.close()

    rm.close()

    raise SystemExit


# ============================================================
# DETERMINE LOW-FREQUENCY PASSBAND REFERENCE
# ============================================================ #

# The first point in your previous data was an outlier.
#
# Therefore, don't use the first point to establish the
# passband reference.
#
# Use the next several low-frequency points.

reference_points = min(
    10,
    len(vout_values) - 1
)


reference_vout = np.median(
    vout_values[
        1:reference_points + 1
    ]
)


# ============================================================
# CALCULATE EXPERIMENTAL GAIN
# ============================================================ #

gain_db = 20 * np.log10(
    vout_values / reference_vout
)


# ============================================================
# PRINT COMPLETE RESULTS
# ============================================================ #

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


# ============================================================
# FIND EXPERIMENTAL -3 dB CUTOFF
# ============================================================ #

cutoff_gain = -3.0

experimental_cutoff = None


for i in range(
    len(gain_db) - 1
):

    g1 = gain_db[i]

    g2 = gain_db[i + 1]


    # Check for crossing of -3 dB

    if (
        g1 >= cutoff_gain
        and
        g2 <= cutoff_gain
    ):

        f1 = valid_freqs[i]

        f2 = valid_freqs[i + 1]


        # ----------------------------------------------------
        # Log-frequency interpolation
        # ----------------------------------------------------

        log_f1 = np.log10(f1)

        log_f2 = np.log10(f2)


        fraction = (
            (cutoff_gain - g1)
            /
            (g2 - g1)
        )


        log_fc = (
            log_f1
            +
            fraction *
            (log_f2 - log_f1)
        )


        experimental_cutoff = (
            10 ** log_fc
        )


        break


# ============================================================
# PRINT CUTOFF RESULTS
# ============================================================ #

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


# ============================================================
# SAVE CSV
# ============================================================ #

with open(
    "lowpass_data.csv",
    mode="w",
    newline=""
) as file:

    writer = csv.writer(file)


    writer.writerow(
        [
            "Frequency (Hz)",
            "Vout (VPP)",
            "Gain (dB)"
        ]
    )


    for f, v, g in zip(
        valid_freqs,
        vout_values,
        gain_db
    ):

        writer.writerow(
            [
                f,
                v,
                g
            ]
        )


# ============================================================
# CREATE IDEAL THEORETICAL FREQUENCY RESPONSE
# ============================================================ #

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


# ------------------------------------------------------------
# Convert ideal magnitude to dB
# ------------------------------------------------------------ #

theoretical_gain_db = (
    20 *
    np.log10(
        theoretical_magnitude
    )
)


# ============================================================
# BODE PLOT
# ============================================================ #

plt.figure(
    figsize=(9, 6)
)


# ------------------------------------------------------------
# Experimental response
# ------------------------------------------------------------ #

plt.semilogx(
    valid_freqs,
    gain_db,
    marker='o',
    linestyle='-',
    label="Experimental"
)


# ------------------------------------------------------------
# Ideal theoretical response
# ------------------------------------------------------------ #

plt.semilogx(
    theoretical_freqs,
    theoretical_gain_db,
    linestyle='--',
    label="Ideal Theoretical"
)


# ------------------------------------------------------------
# -3 dB reference
# ------------------------------------------------------------ #

plt.axhline(
    y=-3,
    linestyle=':',
    label="-3 dB"
)


# ------------------------------------------------------------
# THEORETICAL CUTOFF
# ------------------------------------------------------------ #

plt.axvline(
    x=theoretical_cutoff,
    linestyle='--',
    label=(
        f"Theoretical cutoff = "
        f"{theoretical_cutoff:.1f} Hz"
    )
)


# ------------------------------------------------------------
# EXPERIMENTAL CUTOFF
# ------------------------------------------------------------ #

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


# ------------------------------------------------------------
# PLOT LABELS
# ------------------------------------------------------------ #

plt.xlabel(
    "Frequency (Hz)"
)

plt.ylabel(
    "Gain (dB)"
)

plt.title(
    "Low-Pass Filter Frequency Response"
)


# ------------------------------------------------------------
# Grid
# ------------------------------------------------------------ #

plt.grid(
    True,
    which="both"
)


# ------------------------------------------------------------
# Legend
# ------------------------------------------------------------ #

plt.legend()


# ------------------------------------------------------------
# Layout
# ------------------------------------------------------------ #

plt.tight_layout()


# ------------------------------------------------------------
# Display
# ------------------------------------------------------------ #

plt.show()


# ============================================================
# TURN OFF AND CLOSE INSTRUMENTS
# ============================================================ #

generator.write("OUTP OFF")

generator.close()

scope.close()

rm.close()